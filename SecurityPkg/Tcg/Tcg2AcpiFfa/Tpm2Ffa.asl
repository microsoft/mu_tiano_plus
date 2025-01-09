/** @file

The TPM2 definition block in ACPI table for TCG2 physical presence.

Copyright (c) Microsoft Corporation.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "Tpm.aml",
  "SSDT",
  2,
  "INTEL ",
  "Tpm2Tabl",
  0x1000
  )
{
  Scope (_SB) {
    Device(TPM0) {
      Name (_HID, "NNNN0000")
      Name (_CID, "MSFT0101")

      //
      // Operational region for TPM access
      //
      OperationRegion (TPMR, SystemMemory, FixedPcdGet64 (PcdTpmBaseAddress), FixedPcdGet32 (PcdTpmCrbRegionSize))
      Field (TPMR, AnyAcc, NoLock, Preserve)
      {
        ACC0, 8,  // TPM_ACCESS_0
        Offset(0x8),
        INTE, 32, // TPM_INT_ENABLE_0
        INTV, 8,  // TPM_INT_VECTOR_0
        Offset(0x10),
        INTS, 32, // TPM_INT_STATUS_0
        INTF, 32, // TPM_INTF_CAPABILITY_0
        STS0, 32, // TPM_STS_0
        Offset(0x24),
        FIFO, 32, // TPM_DATA_FIFO_0
        Offset(0x30),
        TID0, 32, // TPM_INTERFACE_ID_0
                  // ignore the rest
      }

      Method (_CRS, 0x0, Serialized) {
        Name (RBUF, ResourceTemplate ()
        {
          QWordMemory (
            ResourceConsumer, 
            PosDecode, 
            MinFixed, 
            MaxFixed, 
            Cacheable,
            ReadWrite, 
            0x0, 
            FixedPcdGet64 (PcdTpmBaseAddress), 
            FixedPcdGet64 (PcdTpmMaxAddress), 
            0x0, 
            FixedPcdGet32 (PcdTpmCrbRegionSize))
        })
        Return (RBUF)
      }

      Method (_STR, 0) {
        Return (Unicode ("TPM 2.0 Device"))
      }

      Method (_STA, 0)
      {
        if (LEqual (ACC0, 0xff))
        {
            Return (0)
        }
        Return (0x0f)
      }

      Name(TPR2, Package (0x02){
        0x0,        // Function Return Code - Success
        0x0         // Pending operation requested by the OS - None
      })

      Name(TPR3, Package (0x03){
        0x0,        // Function Return Code - Success
        0x0,        // Most recent operation request - None
        0x0         // Response to the most recent operation request - Success
      })

      //
      // TCG Physical Presence Interface
      //
      Method (TPPI, 2, Serialized, 0, {BuffObj, PkgObj, IntObj, StrObj}, {UnknownObj, UnknownObj}) // IntObj, PkgObj
      {
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg0))
        {
          Case (0)
          {
            //
            // Standard query, no PPI functions supported
            //
            Return (Buffer () {0x0})
          }
          Case (1)
          {
            //
            // a) Get Physical Presence Interface Version
            //
            Return ("$PV")
          }

          Default {BreakPoint}
        }
        Return (1)
      }


      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {
        //
        // TCG Physical Presence Interface
        //
        If(LEqual(Arg0, ToUUID ("3dddfaa6-361b-4eb4-a424-8d10089d1653")))
        {
          Return (TPPI (Arg2, Arg3))
        }

        Return (Buffer () {0})
      }
    }
  } // Scope(_SB)
}
