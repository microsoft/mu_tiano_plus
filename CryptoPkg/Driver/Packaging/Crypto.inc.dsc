# ****************************************************************************
# AUTOGENERATED BY CryptoPkg\Driver\Packaging\generate_cryptodriver.py
# AUTOGENED AS Crypto.inc.dsc
# DO NOT MODIFY
# GENERATED ON: 2020-03-27 23:51:38.652968

[Defines]
!ifndef CRYPTO_SERVICES
 DEFINE CRYPTO_SERVICES = PACKAGE
!endif
!if $(CRYPTO_SERVICES) IN "ALL NONE PACKAGE TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD"
 # we don't have a problem
!else
 !error CRYPTO_SERVICES must be set to one of ALL NONE PACKAGE TINY_SHA MINIMAL_SHA_SM3 SMALL_SHA_RSA STANDARD.
!endif

  DEFINE PEI_CRYPTO_DRIVER_FILE_GUID = d6f4500f-ad73-4368-9149-842c49f3aa00
  DEFINE DXE_CRYPTO_DRIVER_FILE_GUID = 254e0f83-c675-4578-bc16-d44111c34e01
  DEFINE SMM_CRYPTO_DRIVER_FILE_GUID = be5b74af-e07f-456b-a9e4-296c8fee9502

!if $(CRYPTO_SERVICES) == TINY_SHA
  DEFINE PEI_CRYPTO_DRIVER_FILE_GUID = e6ed744a-8db0-42b8-a507-8909782ed201
  DEFINE DXE_CRYPTO_DRIVER_FILE_GUID = e6ed744a-8db0-42b8-a507-8909782ed202
  DEFINE SMM_CRYPTO_DRIVER_FILE_GUID = e6ed744a-8db0-42b8-a507-8909782ed203
!endif

!if $(CRYPTO_SERVICES) == MINIMAL_SHA_SM3
  DEFINE PEI_CRYPTO_DRIVER_FILE_GUID = 6d653b3b-0654-4eec-8ab3-183a3e061401
  DEFINE DXE_CRYPTO_DRIVER_FILE_GUID = 6d653b3b-0654-4eec-8ab3-183a3e061402
  DEFINE SMM_CRYPTO_DRIVER_FILE_GUID = 6d653b3b-0654-4eec-8ab3-183a3e061403
!endif

!if $(CRYPTO_SERVICES) == SMALL_SHA_RSA
  DEFINE PEI_CRYPTO_DRIVER_FILE_GUID = d9a75606-caba-4aa0-80a6-591852335401
  DEFINE DXE_CRYPTO_DRIVER_FILE_GUID = d9a75606-caba-4aa0-80a6-591852335402
  DEFINE SMM_CRYPTO_DRIVER_FILE_GUID = d9a75606-caba-4aa0-80a6-591852335403
!endif

!if $(CRYPTO_SERVICES) == STANDARD
  DEFINE PEI_CRYPTO_DRIVER_FILE_GUID = bdee011f-87f2-4a7f-bc5e-44b6b61fef01
  DEFINE DXE_CRYPTO_DRIVER_FILE_GUID = bdee011f-87f2-4a7f-bc5e-44b6b61fef02
  DEFINE SMM_CRYPTO_DRIVER_FILE_GUID = bdee011f-87f2-4a7f-bc5e-44b6b61fef03
!endif

[PcdsFixedAtBuild]
!if $(CRYPTO_SERVICES) IN "PACKAGE ALL"
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4GetContextSize         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4Init                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4Duplicate              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4Update                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4Final                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd4HashAll                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5GetContextSize         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5Init                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5Duplicate              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5Update                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5Final                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceMd5HashAll                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1GetContextSize        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1Init                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1Duplicate             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1Update                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1Final                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha1HashAll               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256GetContextSize      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256Init                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256Duplicate           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256Update              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256Final               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha256HashAll             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384GetContextSize      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384Init                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384Duplicate           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384Update              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384Final               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha384HashAll             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512GetContextSize      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512Init                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512Duplicate           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512Update              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512Final               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSha512HashAll             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3GetContextSize         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3Init                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3Duplicate              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3Update                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3Final                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceSm3HashAll                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5New                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5Free               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5SetKey             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5Duplicate          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5Update             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacMd5Final              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1New               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1Free              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1SetKey            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1Duplicate         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1Update            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha1Final             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256New             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256Free            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256SetKey          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256Duplicate       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256Update          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHmacSha256Final           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesGetContextSize        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesInit                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesEcbEncrypt            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesEcbDecrypt            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesCbcEncrypt            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTdesCbcDecrypt            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesGetContextSize         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesInit                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesEcbEncrypt             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesEcbDecrypt             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesCbcEncrypt             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAesCbcDecrypt             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceArc4GetContextSize        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceArc4Init                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceArc4Encrypt               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceArc4Decrypt               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceArc4Reset                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaNew                    | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaFree                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaSetKey                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaGetKey                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaGenerateKey            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaCheckKey               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaPkcs1Sign              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaPkcs1Verify            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaGetPrivateKeyFromPem   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRsaGetPublicKeyFromX509   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509GetSubjectName        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509GetCommonName         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509GetOrganizationName   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509VerifyCert            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509ConstructCertificate  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509ConstructCertificateStackV| TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509ConstructCertificateStack| TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509Free                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509StackFree             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceX509GetTBSCert            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs5HashPassword         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs1v2Encrypt            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7GetSigners           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7FreeSigners          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7GetCertificatesList  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7Sign                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7Verify               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceVerifyEKUsInPkcs7Signature| TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServicePkcs7GetAttachedContent   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceAuthenticodeVerify        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceImageTimestampVerify      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhNew                     | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhFree                    | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhGenerateParameter       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhSetParameter            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhGenerateKey             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceDhComputeKey              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRandomSeed                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceRandomBytes               | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceHkdfSha256ExtractAndExpand| TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsInitialize             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsCtxFree                | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsCtxNew                 | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsFree                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsNew                    | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsInHandshake            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsDoHandshake            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsHandleAlert            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsCloseNotify            | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsCtrlTrafficOut         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsCtrlTrafficIn          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsRead                   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsWrite                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetVersion             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetConnectionEnd       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetCipherList          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetCompressionMethod   | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetVerify              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetVerifyHost          | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetSessionId           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetCaCertificate       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetHostPublicCert      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetHostPrivateKey      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsSetCertRevocationList  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetVersion             | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetConnectionEnd       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetCurrentCipher       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetCurrentCompressionId| TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetVerify              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetSessionId           | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetClientRandom        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetServerRandom        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetKeyMaterial         | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetCaCertificate       | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetHostPublicCert      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetHostPrivateKey      | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceTlsGetCertRevocationList  | TRUE
!endif

!if $(CRYPTO_SERVICES) == TINY_SHA

!include CryptoPkg/Driver/Packaging/Crypto.pcd.TINY_SHA.inc.dsc
!endif

!if $(CRYPTO_SERVICES) == MINIMAL_SHA_SM3

!include CryptoPkg/Driver/Packaging/Crypto.pcd.MINIMAL_SHA_SM3.inc.dsc
!endif

!if $(CRYPTO_SERVICES) == SMALL_SHA_RSA

!include CryptoPkg/Driver/Packaging/Crypto.pcd.SMALL_SHA_RSA.inc.dsc
!endif

!if $(CRYPTO_SERVICES) == STANDARD

!include CryptoPkg/Driver/Packaging/Crypto.pcd.STANDARD.inc.dsc
!endif
