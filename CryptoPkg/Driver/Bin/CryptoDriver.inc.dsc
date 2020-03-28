# ****************************************************************************
# AUTOGENERATED BY Packaging\generate_cryptodriver.py
# AUTOGENED AS CryptoDriver.inc.dsc
# DO NOT MODIFY
# GENERATED ON: 2020-03-27 23:57:58.113124

# this is to be included by a platform :)
[Defines]
!ifndef PEI_CRYPTO_SERVICES
 DEFINE PEI_CRYPTO_SERVICES = ALL
!endif
!if $(PEI_CRYPTO_SERVICES) IN "ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD"
 # we don't have a problem
!else
 !error CRYPTO_SERVICES must be set to one of ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD.
!endif
!ifndef PEI_CRYPTO_ARCH
 DEFINE PEI_CRYPTO_ARCH = IA32
!endif

!ifndef DXE_CRYPTO_SERVICES
 DEFINE DXE_CRYPTO_SERVICES = ALL
!endif
!if $(DXE_CRYPTO_SERVICES) IN "ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD"
 # we don't have a problem
!else
 !error CRYPTO_SERVICES must be set to one of ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD.
!endif
!ifndef DXE_CRYPTO_ARCH
 DEFINE DXE_CRYPTO_ARCH = X64
!endif

!ifndef SMM_CRYPTO_SERVICES
 DEFINE SMM_CRYPTO_SERVICES = ALL
!endif
!if $(SMM_CRYPTO_SERVICES) IN "ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD"
 # we don't have a problem
!else
 !error CRYPTO_SERVICES must be set to one of ALL TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD.
!endif
!ifndef SMM_CRYPTO_ARCH
 DEFINE SMM_CRYPTO_ARCH = X64
!endif

!if $(PEI_CRYPTO_SERVICES) == TINY_SHA
 !if $(PEI_CRYPTO_ARCH) == X64
  [Components.X64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == ARM
  [Components.ARM.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == IA32
  [Components.IA32.PEIM]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_TINY_SHA_Pei_$(TARGET).inf 

 [Components.PEIM]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.TINY_SHA.inc.dsc
    }
!endif

!if $(DXE_CRYPTO_SERVICES) == TINY_SHA
 !if $(DXE_CRYPTO_ARCH) == X64
  [Components.X64.DXE_DRIVER, Components.X64.UEFI_DRIVER, Components.X64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_DRIVER, Components.ARM.UEFI_DRIVER, Components.ARM.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_DRIVER, Components.AARCH64.UEFI_DRIVER, Components.AARCH64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_DRIVER, Components.IA32.UEFI_DRIVER, Components.IA32.UEFI_APPLICATION]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_TINY_SHA_Dxe_$(TARGET).inf 

 [Components.DXE_DRIVER, Components.UEFI_DRIVER, Components.UEFI_APPLICATION]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.TINY_SHA.inc.dsc
    }
!endif

!if $(SMM_CRYPTO_SERVICES) == TINY_SHA
 !if $(SMM_CRYPTO_ARCH) == X64
  [Components.X64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_SMM_DRIVER]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_TINY_SHA_Smm_$(TARGET).inf 

 [Components.DXE_SMM_DRIVER]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.TINY_SHA.inc.dsc
    }
!endif

!if $(PEI_CRYPTO_SERVICES) == MINIMAL_SHA_SM3
 !if $(PEI_CRYPTO_ARCH) == X64
  [Components.X64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == ARM
  [Components.ARM.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == IA32
  [Components.IA32.PEIM]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_MINIMAL_SHA_SM3_Pei_$(TARGET).inf 

 [Components.PEIM]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.MINIMAL_SHA_SM3.inc.dsc
    }
!endif

!if $(DXE_CRYPTO_SERVICES) == MINIMAL_SHA_SM3
 !if $(DXE_CRYPTO_ARCH) == X64
  [Components.X64.DXE_DRIVER, Components.X64.UEFI_DRIVER, Components.X64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_DRIVER, Components.ARM.UEFI_DRIVER, Components.ARM.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_DRIVER, Components.AARCH64.UEFI_DRIVER, Components.AARCH64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_DRIVER, Components.IA32.UEFI_DRIVER, Components.IA32.UEFI_APPLICATION]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_MINIMAL_SHA_SM3_Dxe_$(TARGET).inf 

 [Components.DXE_DRIVER, Components.UEFI_DRIVER, Components.UEFI_APPLICATION]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.MINIMAL_SHA_SM3.inc.dsc
    }
!endif

!if $(SMM_CRYPTO_SERVICES) == MINIMAL_SHA_SM3
 !if $(SMM_CRYPTO_ARCH) == X64
  [Components.X64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_SMM_DRIVER]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_MINIMAL_SHA_SM3_Smm_$(TARGET).inf 

 [Components.DXE_SMM_DRIVER]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.MINIMAL_SHA_SM3.inc.dsc
    }
!endif

!if $(PEI_CRYPTO_SERVICES) == SMALL_SHA_RSA
 !if $(PEI_CRYPTO_ARCH) == X64
  [Components.X64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == ARM
  [Components.ARM.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == IA32
  [Components.IA32.PEIM]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_SMALL_SHA_RSA_Pei_$(TARGET).inf 

 [Components.PEIM]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.SMALL_SHA_RSA.inc.dsc
    }
!endif

!if $(DXE_CRYPTO_SERVICES) == SMALL_SHA_RSA
 !if $(DXE_CRYPTO_ARCH) == X64
  [Components.X64.DXE_DRIVER, Components.X64.UEFI_DRIVER, Components.X64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_DRIVER, Components.ARM.UEFI_DRIVER, Components.ARM.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_DRIVER, Components.AARCH64.UEFI_DRIVER, Components.AARCH64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_DRIVER, Components.IA32.UEFI_DRIVER, Components.IA32.UEFI_APPLICATION]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_SMALL_SHA_RSA_Dxe_$(TARGET).inf 

 [Components.DXE_DRIVER, Components.UEFI_DRIVER, Components.UEFI_APPLICATION]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.SMALL_SHA_RSA.inc.dsc
    }
!endif

!if $(SMM_CRYPTO_SERVICES) == SMALL_SHA_RSA
 !if $(SMM_CRYPTO_ARCH) == X64
  [Components.X64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_SMM_DRIVER]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_SMALL_SHA_RSA_Smm_$(TARGET).inf 

 [Components.DXE_SMM_DRIVER]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.SMALL_SHA_RSA.inc.dsc
    }
!endif

!if $(PEI_CRYPTO_SERVICES) == STANDARD
 !if $(PEI_CRYPTO_ARCH) == X64
  [Components.X64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == ARM
  [Components.ARM.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.PEIM]
 !endif
 !if $(PEI_CRYPTO_ARCH) == IA32
  [Components.IA32.PEIM]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_STANDARD_Pei_$(TARGET).inf 

 [Components.PEIM]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.STANDARD.inc.dsc
    }
!endif

!if $(DXE_CRYPTO_SERVICES) == STANDARD
 !if $(DXE_CRYPTO_ARCH) == X64
  [Components.X64.DXE_DRIVER, Components.X64.UEFI_DRIVER, Components.X64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_DRIVER, Components.ARM.UEFI_DRIVER, Components.ARM.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_DRIVER, Components.AARCH64.UEFI_DRIVER, Components.AARCH64.UEFI_APPLICATION]
 !endif
 !if $(DXE_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_DRIVER, Components.IA32.UEFI_DRIVER, Components.IA32.UEFI_APPLICATION]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_STANDARD_Dxe_$(TARGET).inf 

 [Components.DXE_DRIVER, Components.UEFI_DRIVER, Components.UEFI_APPLICATION]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.STANDARD.inc.dsc
    }
!endif

!if $(SMM_CRYPTO_SERVICES) == STANDARD
 !if $(SMM_CRYPTO_ARCH) == X64
  [Components.X64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == ARM
  [Components.ARM.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == AARCH64
  [Components.AARCH64.DXE_SMM_DRIVER]
 !endif
 !if $(SMM_CRYPTO_ARCH) == IA32
  [Components.IA32.DXE_SMM_DRIVER]
 !endif
  CryptoPkg/Driver/Bin/CryptoDriverBin_STANDARD_Smm_$(TARGET).inf 

 [Components.DXE_SMM_DRIVER]
   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf {
     <PcdsFixedAtBuild>
      !include CryptoPkg/Driver/Packaging/Crypto.pcd.STANDARD.inc.dsc
    }
!endif

