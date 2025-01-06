/** @file
  This driver implements TPM 2.0 definition block in ACPI table and
  populates registered SMI callback functions for Tcg2 physical presence
  and MemoryClear to handle the requests for ACPI method. It needs to be
  used together with Tcg2 MM drivers to exchange information on registered
  SwSmiValue and allocated NVS region address.

Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <IndustryStandard/Tpm2Acpi.h>

#include <Guid/TpmInstance.h>
#include <Guid/TpmNvsMm.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/Tcg2Protocol.h>
#include <Protocol/MmCommunication.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/TpmMeasurementLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/UefiLib.h>

//
// Physical Presence Interface Version supported by Platform
//
#define PHYSICAL_PRESENCE_VERSION_TAG   "$PV"
#define PHYSICAL_PRESENCE_VERSION_SIZE  4

//
// PNP _HID for TPM2 device
//
#define TPM_HID_TAG        "NNNN0000"
#define TPM_HID_PNP_SIZE   8
#define TPM_HID_ACPI_SIZE  9

#define TPM_PRS_RESL           "RESL"
#define TPM_PRS_RESS           "RESS"
#define TPM_PRS_RES_NAME_SIZE  4
//
// Minimum PRS resource template size
//  1 byte    for  BufferOp
//  1 byte    for  PkgLength
//  2 bytes   for  BufferSize
//  12 bytes  for  Memory32Fixed descriptor
//  5 bytes   for  Interrupt descriptor
//  2 bytes   for  END Tag
//
#define TPM_POS_RES_TEMPLATE_MIN_SIZE  (1 + 1 + 2 + 12 + 5 + 2)

//
// Max Interrupt buffer size for PRS interrupt resource
// Now support 15 interrupts in maxmum
//
#define MAX_PRS_INT_BUF_SIZE  (15*4)

#pragma pack(1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  // Flags field is replaced in version 4 and above
  //    BIT0~15:  PlatformClass      This field is only valid for version 4 and above
  //    BIT16~31: Reserved
  UINT32                         Flags;
  UINT64                         AddressOfControlArea;
  UINT32                         StartMethod;
  UINT8                          PlatformSpecificParameters[12]; // size up to 12
  UINT32                         Laml;                           // Optional
  UINT64                         Lasa;                           // Optional
} EFI_TPM2_ACPI_TABLE_V4;

#pragma pack()

EFI_TPM2_ACPI_TABLE_V4  mTpm2AcpiTemplate = {
  {
    EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE,
    sizeof (mTpm2AcpiTemplate),
    EFI_TPM2_ACPI_TABLE_REVISION,
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  0, // BIT0~15:  PlatformClass
     // BIT16~31: Reserved
  0,                                    // Control Area
  EFI_TPM2_ACPI_TABLE_START_METHOD_TIS, // StartMethod
};

/**
  Patch version string of Physical Presence interface supported by platform. The initial string tag in TPM
ACPI table is "$PV".

  @param[in, out] Table          The TPM item in ACPI table.
  @param[in]      PPVer          Version string of Physical Presence interface supported by platform.

  @return                        The allocated address for the found region.

**/
EFI_STATUS
UpdatePPVersion (
  EFI_ACPI_DESCRIPTION_HEADER  *Table,
  CHAR8                        *PPVer
  )
{
  EFI_STATUS  Status;
  UINT8       *DataPtr;

  //
  // Patch some pointers for the ASL code before loading the SSDT.
  //
  for (DataPtr  = (UINT8 *)(Table + 1);
       DataPtr <= (UINT8 *)((UINT8 *)Table + Table->Length - PHYSICAL_PRESENCE_VERSION_SIZE);
       DataPtr += 1)
  {
    if (AsciiStrCmp ((CHAR8 *)DataPtr, PHYSICAL_PRESENCE_VERSION_TAG) == 0) {
      Status = AsciiStrCpyS ((CHAR8 *)DataPtr, PHYSICAL_PRESENCE_VERSION_SIZE, PPVer);
      DEBUG ((DEBUG_INFO, "TPM2 Physical Presence Interface Version update status 0x%x\n", Status));
      return Status;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Patch interrupt resources returned by TPM _PRS. ResourceTemplate to patch is determined by input
  interrupt buffer size. BufferSize, PkgLength and interrupt descriptor in ByteList need to be patched

  @param[in, out] Table            The TPM item in ACPI table.
  @param[in]      IrqBuffer        Input new IRQ buffer.
  @param[in]      IrqBuffserSize   Input new IRQ buffer size.
  @param[out]     IsShortFormPkgLength   If _PRS returns Short length Package(ACPI spec 20.2.4).

  @return                          patch status.

**/
EFI_STATUS
UpdatePossibleResource (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN      UINT32                       *IrqBuffer,
  IN      UINT32                       IrqBuffserSize,
  OUT     BOOLEAN                      *IsShortFormPkgLength
  )
{
  UINT8   *DataPtr;
  UINT8   *DataEndPtr;
  UINT32  NewPkgLength;
  UINT32  OriginalPkgLength;

  NewPkgLength      = 0;
  OriginalPkgLength = 0;
  DataEndPtr        = NULL;

  //
  // Follow ACPI spec
  //           6.4.3   Extend Interrupt Descriptor.
  //           19.3.3 ASL Resource Template
  //           20      AML specification
  // to patch TPM ACPI object _PRS returned ResourceTemplate() containing 2 resource descriptors and an auto appended End Tag
  //
  //  AML data is organized by following rule.
  //  Code need to patch BufferSize and PkgLength and interrupt descriptor in ByteList
  //
  // =============  Buffer ====================
  //           DefBuffer := BufferOp PkgLength BufferSize ByteList
  //            BufferOp := 0x11
  //
  // ==============PkgLength==================
  //          PkgLength := PkgLeadByte |
  //                              <PkgLeadByte ByteData> |
  //                              <PkgLeadByte ByteData ByteData> |
  //                              <PkgLeadByte ByteData ByteData ByteData>
  //
  //       PkgLeadByte := <bit 7-6: ByteData count that follows (0-3)>
  //                               <bit 5-4: Only used if PkgLength <= 63 >
  //                               <bit 3-0: Least significant package length nybble>
  //
  // ==============BufferSize==================
  //        BufferSize := Integer
  //           Integer := ByteConst|WordConst|DwordConst....
  //
  //           ByteConst := BytePrefix ByteData
  //
  // ==============ByteList===================
  //          ByteList := ByteData ByteList
  //
  // =========================================

  //
  // 1. Check TPM_PRS_RESS with PkgLength <=63 can hold the input interrupt number buffer for patching
  //
  for (DataPtr  = (UINT8 *)(Table + 1);
       DataPtr < (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE));
       DataPtr += 1)
  {
    if (CompareMem (DataPtr, TPM_PRS_RESS, TPM_PRS_RES_NAME_SIZE) == 0) {
      //
      // Jump over object name & BufferOp
      //
      DataPtr += TPM_PRS_RES_NAME_SIZE + 1;

      if ((*DataPtr & (BIT7|BIT6)) == 0) {
        OriginalPkgLength = (UINT32)*DataPtr;
        DataEndPtr        = DataPtr + OriginalPkgLength;

        //
        // Jump over PkgLength = PkgLeadByte only
        //
        NewPkgLength++;

        //
        // Jump over BufferSize
        //
        if (*(DataPtr + 1) == AML_BYTE_PREFIX) {
          NewPkgLength += 2;
        } else if (*(DataPtr + 1) == AML_WORD_PREFIX) {
          NewPkgLength += 3;
        } else if (*(DataPtr + 1) == AML_DWORD_PREFIX) {
          NewPkgLength += 5;
        } else {
          ASSERT (FALSE);
          return EFI_UNSUPPORTED;
        }
      } else {
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
      }

      //
      // Include Memory32Fixed Descriptor (12 Bytes) + Interrupt Descriptor header(5 Bytes) + End Tag(2 Bytes)
      //
      NewPkgLength += 19 + IrqBuffserSize;
      if (NewPkgLength > 63) {
        break;
      }

      if (NewPkgLength > OriginalPkgLength) {
        ASSERT (FALSE);
        return EFI_INVALID_PARAMETER;
      }

      //
      // 1.1 Patch PkgLength
      //
      *DataPtr = (UINT8)NewPkgLength;

      //
      // 1.2 Patch BufferSize = sizeof(Memory32Fixed Descriptor + Interrupt Descriptor + End Tag).
      //      It is Little endian. So only patch lowest byte of BufferSize due to current interrupt number limit.
      //
      *(DataPtr + 2) = (UINT8)(IrqBuffserSize + 19);

      //
      // Notify _PRS to report short formed ResourceTemplate
      //
      *IsShortFormPkgLength = TRUE;

      break;
    }
  }

  //
  // 2. Use TPM_PRS_RESL with PkgLength > 63 to hold longer input interrupt number buffer for patching
  //
  if (NewPkgLength > 63) {
    NewPkgLength      = 0;
    OriginalPkgLength = 0;
    for (DataPtr  = (UINT8 *)(Table + 1);
         DataPtr < (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE));
         DataPtr += 1)
    {
      if (CompareMem (DataPtr, TPM_PRS_RESL, TPM_PRS_RES_NAME_SIZE) == 0) {
        //
        // Jump over object name & BufferOp
        //
        DataPtr += TPM_PRS_RES_NAME_SIZE + 1;

        if ((*DataPtr & (BIT7|BIT6)) != 0) {
          OriginalPkgLength = (UINT32)(*(DataPtr + 1) << 4) + (*DataPtr & 0x0F);
          DataEndPtr        = DataPtr + OriginalPkgLength;
          //
          // Jump over PkgLength = PkgLeadByte + ByteData length
          //
          NewPkgLength += 1 + ((*DataPtr & (BIT7|BIT6)) >> 6);

          //
          // Jump over BufferSize
          //
          if (*(DataPtr + NewPkgLength) == AML_BYTE_PREFIX) {
            NewPkgLength += 2;
          } else if (*(DataPtr + NewPkgLength) == AML_WORD_PREFIX) {
            NewPkgLength += 3;
          } else if (*(DataPtr + NewPkgLength) == AML_DWORD_PREFIX) {
            NewPkgLength += 5;
          } else {
            ASSERT (FALSE);
            return EFI_UNSUPPORTED;
          }
        } else {
          ASSERT (FALSE);
          return EFI_UNSUPPORTED;
        }

        //
        // Include Memory32Fixed Descriptor (12 Bytes) + Interrupt Descriptor header(5 Bytes) + End Tag(2  Bytes)
        //
        NewPkgLength += 19 + IrqBuffserSize;

        if (NewPkgLength > OriginalPkgLength) {
          ASSERT (FALSE);
          return EFI_INVALID_PARAMETER;
        }

        //
        // 2.1 Patch PkgLength. Only patch PkgLeadByte and first ByteData
        //
        *DataPtr       = (UINT8)((*DataPtr) & 0xF0) | (NewPkgLength & 0x0F);
        *(DataPtr + 1) = (UINT8)((NewPkgLength & 0xFF0) >> 4);

        //
        // 2.2 Patch BufferSize = sizeof(Memory32Fixed Descriptor + Interrupt Descriptor + End Tag).
        //     It is Little endian. Only patch lowest byte of BufferSize due to current interrupt number limit.
        //
        *(DataPtr + 2 + ((*DataPtr & (BIT7|BIT6)) >> 6)) = (UINT8)(IrqBuffserSize + 19);

        //
        // Notify _PRS to report long formed ResourceTemplate
        //
        *IsShortFormPkgLength = FALSE;
        break;
      }
    }
  }

  if (DataPtr >= (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE))) {
    return EFI_NOT_FOUND;
  }

  //
  // 3. Move DataPtr to Interrupt descriptor header and patch interrupt descriptor.
  //     5 bytes for interrupt descriptor header, 2 bytes for End Tag
  //
  DataPtr += NewPkgLength - (5 + IrqBuffserSize + 2);
  //
  //   3.1 Patch Length bit[7:0] of Interrupt descriptor patch interrupt descriptor
  //
  *(DataPtr + 1) = (UINT8)(2 + IrqBuffserSize);
  //
  //   3.2 Patch Interrupt Table Length
  //
  *(DataPtr + 4) = (UINT8)(IrqBuffserSize / sizeof (UINT32));
  //
  //   3.3 Copy patched InterruptNumBuffer
  //
  CopyMem (DataPtr + 5, IrqBuffer, IrqBuffserSize);

  //
  // 4. Jump over Interrupt descriptor and Patch END Tag, set Checksum field to 0
  //
  DataPtr       += 5 + IrqBuffserSize;
  *DataPtr       = ACPI_END_TAG_DESCRIPTOR;
  *(DataPtr + 1) = 0;

  //
  // 5. Jump over new ResourceTemplate. Stuff rest bytes to NOOP
  //
  DataPtr += 2;
  if (DataPtr < DataEndPtr) {
    SetMem (DataPtr, (UINTN)DataEndPtr - (UINTN)DataPtr, AML_NOOP_OP);
  }

  return EFI_SUCCESS;
}

/**
  Patch TPM2 device HID string.  The initial string tag in TPM2 ACPI table is "NNN0000".

  @param[in, out] Table          The TPM2 SSDT ACPI table.

  @return                               HID Update status.

**/
EFI_STATUS
UpdateHID (
  EFI_ACPI_DESCRIPTION_HEADER  *Table
  )
{
  EFI_STATUS  Status;
  UINT8       *DataPtr;
  CHAR8       Hid[TPM_HID_ACPI_SIZE];
  UINT32      ManufacturerID;
  UINT32      FirmwareVersion1;
  UINT32      FirmwareVersion2;
  BOOLEAN     PnpHID;

  PnpHID = TRUE;

  //
  // Initialize HID with Default PNP string
  //
  ZeroMem (Hid, TPM_HID_ACPI_SIZE);

  //
  // Get Manufacturer ID
  //
  Status = Tpm2GetCapabilityManufactureID (&ManufacturerID);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TPM_PT_MANUFACTURER 0x%08x\n", ManufacturerID));
    //
    // ManufacturerID defined in TCG Vendor ID Registry
    // may tailed with 0x00 or 0x20
    //
    if (((ManufacturerID >> 24) == 0x00) || ((ManufacturerID >> 24) == 0x20)) {
      //
      //  HID containing PNP ID "NNN####"
      //   NNN is uppercase letter for Vendor ID specified by manufacturer
      //
      CopyMem (Hid, &ManufacturerID, 3);
    } else {
      //
      //  HID containing ACP ID "NNNN####"
      //   NNNN is uppercase letter for Vendor ID specified by manufacturer
      //
      CopyMem (Hid, &ManufacturerID, 4);
      PnpHID = FALSE;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Get TPM_PT_MANUFACTURER failed %x!\n", Status));
    ASSERT (FALSE);
    return Status;
  }

  Status = Tpm2GetCapabilityFirmwareVersion (&FirmwareVersion1, &FirmwareVersion2);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TPM_PT_FIRMWARE_VERSION_1 0x%x\n", FirmwareVersion1));
    DEBUG ((DEBUG_INFO, "TPM_PT_FIRMWARE_VERSION_2 0x%x\n", FirmwareVersion2));
    //
    //   #### is Firmware Version 1
    //
    if (PnpHID) {
      AsciiSPrint (Hid + 3, TPM_HID_PNP_SIZE - 3, "%02d%02d", ((FirmwareVersion1 & 0xFFFF0000) >> 16), (FirmwareVersion1 & 0x0000FFFF));
    } else {
      AsciiSPrint (Hid + 4, TPM_HID_ACPI_SIZE - 4, "%02d%02d", ((FirmwareVersion1 & 0xFFFF0000) >> 16), (FirmwareVersion1 & 0x0000FFFF));
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Get TPM_PT_FIRMWARE_VERSION_X failed %x!\n", Status));
    ASSERT (FALSE);
    return Status;
  }

  //
  // Patch HID in ASL code before loading the SSDT.
  //
  for (DataPtr  = (UINT8 *)(Table + 1);
       DataPtr <= (UINT8 *)((UINT8 *)Table + Table->Length - TPM_HID_PNP_SIZE);
       DataPtr += 1)
  {
    if (AsciiStrCmp ((CHAR8 *)DataPtr, TPM_HID_TAG) == 0) {
      if (PnpHID) {
        CopyMem (DataPtr, Hid, TPM_HID_PNP_SIZE);
        //
        // if HID is PNP ID, patch the last byte in HID TAG to Noop
        //
        *(DataPtr + TPM_HID_PNP_SIZE) = AML_NOOP_OP;
      } else {
        CopyMem (DataPtr, Hid, TPM_HID_ACPI_SIZE);
      }

      DEBUG ((DEBUG_INFO, "TPM2 ACPI _HID is patched to %a\n", Hid));

      return Status;
    }
  }

  DEBUG ((DEBUG_ERROR, "TPM2 ACPI HID TAG for patch not found!\n"));
  return EFI_NOT_FOUND;
}

/**
  Initialize and publish TPM items in ACPI table.

  @retval   EFI_SUCCESS     The TCG ACPI table is published successfully.
  @retval   Others          The TCG ACPI table is not published.

**/
EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_ACPI_TABLE_PROTOCOL      *AcpiTable;
  UINTN                        TableKey;
  EFI_ACPI_DESCRIPTION_HEADER  *Table;
  UINTN                        TableSize;
  UINT32                       *PossibleIrqNumBuf;
  UINT32                       PossibleIrqNumBufSize;
  BOOLEAN                      IsShortFormPkgLength;

  IsShortFormPkgLength = FALSE;

  Status = GetSectionFromFv (
             &gEfiCallerIdGuid,
             EFI_SECTION_RAW,
             0,
             (VOID **)&Table,
             &TableSize
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Measure to PCR[0] with event EV_POST_CODE ACPI DATA.
  // The measurement has to be done before any update.
  // Otherwise, the PCR record would be different after TPM FW update
  // or the PCD configuration change.
  //
  // MU_CHANGE [BEGIN]
  // Allow a platform to drop TCG ACPI measurements until we have a chance to make them more
  // consistent and functional.
  if (!FixedPcdGetBool (PcdSkipTcgSmmAcpiMeasurements)) {
    TpmMeasureAndLogData (
      0,
      EV_POST_CODE,
      EV_POSTCODE_INFO_ACPI_DATA,
      ACPI_DATA_LEN,
      Table,
      TableSize
      );
  }

  // MU_CHANGE [END]

  //
  // Update Table version before measuring it to PCR
  //
  Status = UpdatePPVersion (Table, (CHAR8 *)PcdGetPtr (PcdTcgPhysicalPresenceInterfaceVer));
  ASSERT_EFI_ERROR (Status);

  DEBUG ((
    DEBUG_INFO,
    "Current physical presence interface version - %a\n",
    (CHAR8 *)PcdGetPtr (PcdTcgPhysicalPresenceInterfaceVer)
    ));

  //
  // Update TPM2 HID after measuring it to PCR
  //
  Status = UpdateHID (Table);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (PcdGet32 (PcdTpm2CurrentIrqNum) != 0) {
    //
    // Patch _PRS interrupt resource only when TPM interrupt is supported
    //
    PossibleIrqNumBuf     = (UINT32 *)PcdGetPtr (PcdTpm2PossibleIrqNumBuf);
    PossibleIrqNumBufSize = (UINT32)PcdGetSize (PcdTpm2PossibleIrqNumBuf);

    if ((PossibleIrqNumBufSize <= MAX_PRS_INT_BUF_SIZE) && ((PossibleIrqNumBufSize % sizeof (UINT32)) == 0)) {
      Status = UpdatePossibleResource (Table, PossibleIrqNumBuf, PossibleIrqNumBufSize, &IsShortFormPkgLength);
      DEBUG ((
        DEBUG_INFO,
        "UpdatePossibleResource status - %x. TPM2 service may not ready in OS.\n",
        Status
        ));
    } else {
      DEBUG ((
        DEBUG_INFO,
        "PcdTpm2PossibleIrqNumBuf size %x is not correct. TPM2 service may not ready in OS.\n",
        PossibleIrqNumBufSize
        ));
    }
  }

  //
  // Publish the TPM ACPI table. Table is re-checksummed.
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  ASSERT_EFI_ERROR (Status);

  TableKey = 0;
  Status   = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          Table,
                          TableSize,
                          &TableKey
                          );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Publish TPM2 ACPI table

  @retval   EFI_SUCCESS     The TPM2 ACPI table is published successfully.
  @retval   Others          The TPM2 ACPI table is not published.

**/
EFI_STATUS
PublishTpm2 (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_TABLE_PROTOCOL     *AcpiTable;
  UINTN                       TableKey;
  UINT64                      OemTableId;
  EFI_TPM2_ACPI_CONTROL_AREA  *ControlArea;
  TPM2_PTP_INTERFACE_TYPE     InterfaceType;
  UINT64                      PartitionId;

  // Allow a platform to drop TCG ACPI measurements until we have a chance to make them more
  // consistent and functional.
  if (!FixedPcdGetBool (PcdSkipTcgSmmAcpiMeasurements)) {
    TpmMeasureAndLogData (
      0,
      EV_POST_CODE,
      EV_POSTCODE_INFO_ACPI_DATA,
      ACPI_DATA_LEN,
      &mTpm2AcpiTemplate,
      mTpm2AcpiTemplate.Header.Length
      );
  }

  mTpm2AcpiTemplate.Header.Revision = PcdGet8 (PcdTpm2AcpiTableRev);
  DEBUG ((DEBUG_INFO, "Tpm2 ACPI table revision is %d\n", mTpm2AcpiTemplate.Header.Revision));

  if (mTpm2AcpiTemplate.Header.Revision >= EFI_TPM2_ACPI_TABLE_REVISION_4) {
    mTpm2AcpiTemplate.Flags = (mTpm2AcpiTemplate.Flags & 0xFFFF0000) | PcdGet8 (PcdTpmPlatformClass);
    DEBUG ((DEBUG_INFO, "Tpm2 ACPI table PlatformClass is %d\n", (mTpm2AcpiTemplate.Flags & 0x0000FFFF)));
  }

  mTpm2AcpiTemplate.Laml = PcdGet32 (PcdTpm2AcpiTableLaml);
  mTpm2AcpiTemplate.Lasa = PcdGet64 (PcdTpm2AcpiTableLasa);
  if ((mTpm2AcpiTemplate.Header.Revision < EFI_TPM2_ACPI_TABLE_REVISION_4) ||
      (mTpm2AcpiTemplate.Laml == 0) || (mTpm2AcpiTemplate.Lasa == 0))
  {
    //
    // If version is smaller than 4 or Laml/Lasa is not valid, rollback to original Length.
    //
    mTpm2AcpiTemplate.Header.Length = sizeof (EFI_TPM2_ACPI_TABLE);
  }

  InterfaceType = PcdGet8 (PcdActiveTpmInterfaceType);
  DEBUG ((DEBUG_INFO, "Tpm Active Interface Type %d\n", InterfaceType));

  PartitionId = PcdGet16 (PcdTpmServiceFfaPartitionId);
  ASSERT (PartitionId != 0);
  if (InterfaceType == Tpm2PtpInterfaceCrb) {
    mTpm2AcpiTemplate.StartMethod                   = EFI_TPM2_ACPI_TABLE_START_METHOD_COMMAND_RESPONSE_BUFFER_INTERFACE_WITH_FFA;
    mTpm2AcpiTemplate.AddressOfControlArea          = PcdGet64 (PcdTpmBaseAddress) + 0x40;
    mTpm2AcpiTemplate.PlatformSpecificParameters[0] = 0x00;                           // Notifications Not Supported
    mTpm2AcpiTemplate.PlatformSpecificParameters[1] = 0x00;                           // CRB 4KiB size, Not Cacheable
    mTpm2AcpiTemplate.PlatformSpecificParameters[2] = (PartitionId >> 8) & MAX_UINT8; // HI Byte of Partition ID
    mTpm2AcpiTemplate.PlatformSpecificParameters[3] = (PartitionId) & MAX_UINT8;      // LO Byte of Partition ID
    ControlArea                                     = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN)mTpm2AcpiTemplate.AddressOfControlArea;
    ControlArea->CommandSize                        = 0xF80;
    ControlArea->ResponseSize                       = 0xF80;
    ControlArea->Command                            = PcdGet64 (PcdTpmBaseAddress) + 0x80;
    ControlArea->Response                           = PcdGet64 (PcdTpmBaseAddress) + 0x80;
  } else {
    DEBUG ((DEBUG_ERROR, "TPM2 InterfaceType get error! %d\n", InterfaceType));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Tpm2 ACPI table size %d\n", mTpm2AcpiTemplate.Header.Length));

  CopyMem (mTpm2AcpiTemplate.Header.OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (mTpm2AcpiTemplate.Header.OemId));
  OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem (&mTpm2AcpiTemplate.Header.OemTableId, &OemTableId, sizeof (UINT64));
  mTpm2AcpiTemplate.Header.OemRevision     = PcdGet32 (PcdAcpiDefaultOemRevision);
  mTpm2AcpiTemplate.Header.CreatorId       = PcdGet32 (PcdAcpiDefaultCreatorId);
  mTpm2AcpiTemplate.Header.CreatorRevision = PcdGet32 (PcdAcpiDefaultCreatorRevision);

  //
  // Construct ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  ASSERT_EFI_ERROR (Status);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &mTpm2AcpiTemplate,
                        mTpm2AcpiTemplate.Header.Length,
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  The driver's entry point.

  It patches and installs ACPI tables used for handling TPM physical presence
  and Memory Clear requests through ACPI method.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeTcgAcpiFfa (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "TCG ACPI FFA Entry Point!\n"));

  if (!CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid)) {
    DEBUG ((DEBUG_ERROR, "No TPM2 DTPM instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  Status = PublishAcpiTable ();
  ASSERT_EFI_ERROR (Status);

  //
  // Set TPM2 ACPI table
  //
  Status = PublishTpm2 ();
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
