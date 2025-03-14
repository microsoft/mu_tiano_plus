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
  Scope (_SB_) {
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

      //
      // Operational region for TPM support, TPM Physical Presence and TPM Memory Clear
      // Region Offset 0xFFFFFFFFFFFF0000 and Length 0xF0 will be fixed in C code.
      //
      OperationRegion (TNVS, SystemMemory, 0xFFFFFFFFFFFF0000, 0xF0)
      Field (TNVS, AnyAcc, NoLock, Preserve)
      {
        PPIN,   8,  //   Software SMI for Physical Presence Interface
        PPIP,   32, //   Used for save physical presence parameter
        PPRP,   32, //   Physical Presence request operation response
        PPRQ,   32, //   Physical Presence request operation
        PPRM,   32, //   Physical Presence request operation parameter
        LPPR,   32, //   Last Physical Presence request operation
        FRET,   32, //   Physical Presence function return code
        MCIN,   8,  //   Software SMI for Memory Clear Interface
        MCIP,   32, //   Used for save the Mor parameter
        MORD,   32, //   Memory Overwrite Request Data
        MRET,   32, //   Memory Overwrite function return code
        UCRQ,   32, //   Physical Presence request operation to Get User Confirmation Status
        IRQN,   32, //   IRQ Number for _CRS
        SFRB,   8   //   Is shortformed Pkglength for resource buffer
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

      Method(_EST, 0x0, NotSerialized) {
        FDR2 ()
      }

      Method (_STA, 0)
      {
        if (LEqual (ACC0, 0xff))
        {
            Return (0)
        }
        Return (0x0f)
      }

      Name(TPM2, Package (0x02){
        0x0,        // Function Return Code - Success
        0x0         // Pending operation requested by the OS - None
      })

      Name(TPM3, Package (0x03){
        0x0,        // Function Return Code - Success
        0x0,        // Most recent operation request - None
        0x0         // Response to the most recent operation request - Success
      })

      Name(BUFF, Buffer(50){})   // Create buffer for send/recv data

      //
      // FFA Direct Req2 Wrapper
      //
      OperationRegion(AFFH, FFixedHw, 4, 144)
      Field(AFFH, BufferAcc, NoLock, Preserve) { AccessAs(BufferAcc, 0x1), FFAC, 1152 }
      Method (FDR2, 0, Serialized) {
        CreateByteField(BUFF,0,STAT) // Out - Status for req/rsp
        CreateByteField(BUFF,1,LENG) // In/Out - Bytes in req, updates bytes returned
        CreateField(BUFF,16,128,UUID) // UUID of service

        Store(0x20, LENG)
        // Service UUID from the input
        Store(ToUUID("3dddfaa6-361b-4eb4-a424-8d10089d1653"), UUID)
        Store(Store(BUFF, \_SB_.TPM0.FFAC), BUFF)
      }

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
            // Standard query, supports function 1-8
            //
            Return (Buffer () {0xFF, 0x01})
          }
          Case (1)
          {
            //
            // a) Get Physical Presence Interface Version
            //
            Return ("$PV")
          }
          Case (2)
          {
            //
            // b) Submit TPM Operation Request to Pre-OS Environment
            //

            Store (DerefOf (Index (Arg1, 0x00)), PPRQ)
            Store (0, PPRM)
            Store (0x02, PPIP)

            //
            // Trigger the FFA direct req2
            //
            FDR2 ()
            Return (FRET)


          }
          Case (3)
          {
            //
            // c) Get Pending TPM Operation Requested By the OS
            //

            Store (PPRQ, Index (TPM2, 0x01))
            Return (TPM2)
          }
          Case (4)
          {
            //
            // d) Get Platform-Specific Action to Transition to Pre-OS Environment
            //
            Return (2)
          }
          Case (5)
          {
            //
            // e) Return TPM Operation Response to OS Environment
            //
            Store (0x05, PPIP)

            //
            // Trigger the FFA direct req2
            //
            FDR2 ()

            Store (LPPR, Index (TPM3, 0x01))
            Store (PPRP, Index (TPM3, 0x02))

            Return (TPM3)
          }
          Case (6)
          {

            //
            // f) Submit preferred user language (Not implemented)
            //

            Return (3)

          }
          Case (7)
          {
            //
            // g) Submit TPM Operation Request to Pre-OS Environment 2
            //
            Store (7, PPIP)
            Store (DerefOf (Index (Arg1, 0x00)), PPRQ)
            Store (0, PPRM)
            If (LEqual (PPRQ, 23)) {
              Store (DerefOf (Index (Arg1, 0x01)), PPRM)
            }

            //
            // Trigger the FFA direct req2
            //
            FDR2 ()
            Return (FRET)
          }
          Case (8)
          {
            //
            // e) Get User Confirmation Status for Operation
            //
            Store (8, PPIP)
            Store (DerefOf (Index (Arg1, 0x00)), UCRQ)

            //
            // Trigger the FFA direct req2
            //
            FDR2 ()

            Return (FRET)
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
  } // Scope(_SB_)
}
