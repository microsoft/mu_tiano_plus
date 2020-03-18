# this is meant to be included in a DSC that will enable the correct behavior for a given platform. 
# Unfortunately, it cannot also be as easy for an FDF. You'll need to include the relevant logic in your FDF.

[Defines]

!ifndef SHARED_CRYPTO_ENABLE
DEFINE SHARED_CRYPTO_ENABLE = FALSE
!endif

[LibraryClasses.Common]
  !if $(SHARED_CRYPTO_ENABLE) == FALSE
    OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
    TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf
    IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  !else
    TlsLib|CryptoPkg/Library/TlsLibNull/TlsLibNull.inf
    BaseCryptLib|CryptoPkg/Library/BaseCryptLibNull/BaseCryptLibNull.inf
  !endif


[LibraryClasses.common.PEIM]
  !if $(SHARED_CRYPTO_ENABLE) == TRUE
    BaseCryptLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf
    TlsLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/PeiCryptLib.inf
  !else
    BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  !endif

[LibraryClasses.common.DXE_DRIVER, LibraryClasses.common.UEFI_DRIVER, LibraryClasses.common.UEFI_APPLICATION]
  !if $(SHARED_CRYPTO_ENABLE) == TRUE
    BaseCryptLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf
    TlsLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf
  !else
    BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  !endif

[LibraryClasses.common.DXE_SMM_DRIVER]
  !if $(SHARED_CRYPTO_ENABLE) == TRUE
    BaseCryptLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf
    TlsLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/SmmCryptLib.inf
  !else
    BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
  !endif
