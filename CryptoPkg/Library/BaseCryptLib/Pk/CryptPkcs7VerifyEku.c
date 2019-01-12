/** @file
 * 
    This module verifies that Enhanced Key Usages (EKU's) are present within
    a PKCS7 signature blob using OpenSSL.  


    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
    THE POSSIBILITY OF SUCH DAMAGE.

    
    Copyright (C) Microsoft Corporation. All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

**/

#include <Base.h> // MS_CHANGE
#include "InternalCryptLib.h"

#include <openssl/x509v3.h>
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <internal/x509_int.h>
#include <openssl/pkcs7.h>
#include <openssl/bn.h>
#include <openssl/x509_vfy.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <internal/asn1_int.h>

/**
  GetSignerCertificate()
  
  This function will return the leaf signer certificate in a chain.  This is 
  required because certificate chains are not guaranteed to have the 
  certificates in the order that they were issued. 

  A typical certificate chain looks like this:

  
                 ----------------------------
                |            Root            |
                 ----------------------------
                               ^
                               |
                 ----------------------------
                |          Policy CA         | <-- Typical Trust Anchor.
                 ----------------------------
                               ^
                               |
                 ----------------------------
                |         Issuing CA         |
                 ----------------------------
                               ^
                               |
                 -----------------------------
                /  End-Entity (leaf) signer  / <-- Bottom certificate.
                -----------------------------  EKU: "1.3.6.1.4.1.311.76.9.21.1" 
                                                    (Firmware Signing)


  @param[in]   CertChain  - Certificate chain.
 
  @param[out]  SignerCert - Last certificate in the chain.  For PKCS7 signatures,
                            this will be the end-entity (leaf) signer cert.
 
  @retval EFI_SUCCESS             - The required EKUs were found in the signature.
  @retval EFI_INVALID_PARAMETER   - A parameter was invalid.
  @retval EFI_NOT_FOUND           - The number of signers found was not 1.
 
**/
EFI_STATUS
GetSignerCertificate (
  IN CONST PKCS7 *CertChain,
  OUT X509       **SignerCert
  )
{
  EFI_STATUS      Status         = EFI_SUCCESS;
  STACK_OF(X509)* Signers        = NULL;
  INT32           NumberSigners  = 0;

  if (CertChain == NULL || SignerCert == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get the signers from the chain.
  //
  Signers = PKCS7_get0_signers((PKCS7*) CertChain, NULL, PKCS7_BINARY);
  if (Signers == NULL) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR:  Getting signers from PKCS7 failed.\n",
            __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // There should only be one signer in the PKCS7 stack.
  //
  NumberSigners = sk_X509_num(Signers);
  if (NumberSigners != 1) {
    DEBUG((DEBUG_ERROR,
      "%a - ERROR:  The number of signers should have "
      "been 1, but was %d\n", __FUNCTION__, NumberSigners));
    Status = EFI_NOT_FOUND;
    goto Exit;
  }

  *SignerCert = sk_X509_value(Signers, 0);

Exit:
  //
  // Cleanup...
  //
  if (Signers) {
    sk_X509_free(Signers);
  }

  return Status;
}//GetSignerCertificate()


/**
  IsEkuInCertificate()

  Determines if the specified EKU represented in ASN1 form is present
  in a given certificate.
 
  @param[in]  Cert        - The certificate to check.
 
  @param[in]  Asn1ToFind  - The EKU to look for.
 
  @retval EFI_SUCCESS             - We successfully identified the signing type.
  @retval EFI_INVALID_PARAMETER   - A parameter was invalid.
  @retval EFI_NOT_FOUND           - One or more EKU's were not found in the signature.
**/
EFI_STATUS
IsEkuInCertificate (
  IN CONST X509  *Cert,
  IN ASN1_OBJECT *Asn1ToFind
  )
{
  EFI_STATUS          Status          = EFI_NOT_FOUND; // Assume failure.
  X509*               ClonedCert      = NULL;
  X509_EXTENSION     *Extension       = NULL;
  EXTENDED_KEY_USAGE *EKU             = NULL;
  int                 ExtensionIndex  = -1;
  int                 NumExtensions   = 0;
  ASN1_OBJECT        *Asn1InCert      = NULL;


  if (Cert == NULL || Asn1ToFind == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Clone the certificate.  This is required because the Extension API's
  // only work once per instance of an X509 object.
  //
  ClonedCert = X509_dup((X509*)Cert);
  if (ClonedCert == NULL) {
    DEBUG((DEBUG_ERROR,
          "%a - ERROR:  Could not duplicate cert\n",
          __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Look for the extended key usage.
  //
  ExtensionIndex = X509_get_ext_by_NID(ClonedCert, NID_ext_key_usage, -1);

  if (ExtensionIndex < 0) {
    DEBUG((DEBUG_INFO,
           "%a - Did not find 'NID_ext_key_usage' in Cert\n",
           __FUNCTION__));
    goto Exit;
  }

  Extension = X509_get_ext(ClonedCert, ExtensionIndex);
  if (Extension == NULL) {
    DEBUG((DEBUG_INFO,
           "%a - Could not Get Extension from cert\n",
           __FUNCTION__));
    goto Exit;
  }

  EKU = (EXTENDED_KEY_USAGE*)X509V3_EXT_d2i(Extension);
  if (EKU == NULL) {
    DEBUG((DEBUG_ERROR,
           "%a - Error:  Could not get EKU from extension\n",
            __FUNCTION__));
    goto Exit;
  }
  
  NumExtensions = sk_ASN1_OBJECT_num(EKU);
  DEBUG((DEBUG_INFO,
         "%a - Number of extensions in cert: %d\n",
         __FUNCTION__, NumExtensions));

  //
  // Now loop through the extensions, looking for the specified EKU.
  //
  for (int i = 0; i < NumExtensions; i++)
  {
    Asn1InCert = sk_ASN1_OBJECT_value(EKU, i);
    if (Asn1InCert == NULL) {
      DEBUG((DEBUG_ERROR,
             "%a - ERROR:  Could not get ASN object from EKU\n",
             __FUNCTION__));
      goto Exit;
    }
    DEBUG((DEBUG_INFO,
           "%a - Raw ASN1 EKU length in Cert: %d\n",
             __FUNCTION__, Asn1InCert->length));

    if (Asn1InCert->length == Asn1ToFind->length &&
        CompareMem(Asn1InCert->data, 
                   Asn1ToFind->data, 
                   Asn1InCert->length) == 0) {
      DEBUG((DEBUG_INFO,
             "%a - Found EKU in certificate\n",
             __FUNCTION__));
      Status = EFI_SUCCESS;
      goto Exit;
    }
  }

Exit:
  
  //
  // Cleanup...
  //
  if (ClonedCert) {
    X509_free(ClonedCert);
  }

  if (EKU) {
    sk_ASN1_OBJECT_pop_free(EKU, ASN1_OBJECT_free);
  }

  return Status;
}//IsEkuInCertificate()


/**
  CheckEKUs()

  Determines if the specified EKUs are present in a signing certificate.
 
  @param[in]  SignerCert        - The certificate to check.
  @param[in]  RequiredEKUs      - The EKUs to look for.
  @param[in]  RequiredEKUsSise  - The number of EKUs
  @param[in]  RequireAllPresent - If TRUE, then all the specified EKUs
                                  must be present in the certificate.
 
  @retval EFI_SUCCESS             - We successfully identified the signing type.
  @retval EFI_INVALID_PARAMETER   - A parameter was invalid.
  @retval EFI_NOT_FOUND           - One or more EKU's were not found in the signature.
**/
EFI_STATUS
CheckEKUs(
  IN CONST X509   *SignerCert,
  IN CONST CHAR8  *RequiredEKUs[],
  IN CONST UINT32  RequiredEKUsSize,
  IN BOOLEAN       RequireAllPresent
  )
{
  EFI_STATUS   Status       = EFI_SUCCESS;
  ASN1_OBJECT *Asn1ToFind   = NULL;
  UINT32       NumEkusFound = 0;
  
  if (SignerCert == NULL || RequiredEKUs == NULL || RequiredEKUsSize == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  

  for (UINT32 i = 0; i < RequiredEKUsSize; i++)
  {
    DEBUG((DEBUG_INFO,
           "%a - Looking for EKU: '%a' in cert\n",
           __FUNCTION__, RequiredEKUs[i]));
    if (Asn1ToFind) {
      ASN1_OBJECT_free(Asn1ToFind);
      Asn1ToFind = NULL;
    }

    Asn1ToFind = OBJ_txt2obj(RequiredEKUs[i], 0);
    if (!Asn1ToFind) {
      DEBUG((DEBUG_ERROR,
             "%a - Error, could not convert EKU '%a' to ASN1\n",
             __FUNCTION__, RequiredEKUs[i]));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
  
    Status = IsEkuInCertificate(SignerCert, Asn1ToFind);
    if (Status == EFI_SUCCESS) {
      NumEkusFound++;
      if (!RequireAllPresent) {
        //
        // Found at least one, so we are done.
        //
        goto Exit;
      }
    }
  }

Exit:

  if (Asn1ToFind) {
    ASN1_OBJECT_free(Asn1ToFind);
  }

  if (RequireAllPresent && 
      NumEkusFound == RequiredEKUsSize) {
    DEBUG((DEBUG_INFO,
           "%a - Found all required EKUs in certificate\n",
           __FUNCTION__));
    Status = EFI_SUCCESS;
  }

  if (Status != EFI_SUCCESS) {
    DEBUG((DEBUG_INFO,
          "%a - Warning, did not find all required EKUs in certificate.\n",
          __FUNCTION__));
  }
  
  return Status;
}//CheckEKUs()

/**
  VerifyEKUsInPkcs7Signature()

  This function receives a PKCS#7 formatted signature blob,
  looks for the EKU SEQUENCE blob, and if found then looks
  for all the required EKUs.  This function was created so that
  the Surface team can cut down on the number of Certificate
  Authorities (CA's) by checking EKU's on leaf signers for
  a specific product.  This prevents one product's certificate
  from signing another product's firmware or unlock blobs.

  Note that this function does not validate the certificate chain.
  That needs to be done before using this function.

 
  @param[in]  Pkcs7Signature    -  The PKCS#7 signed information content block. An array
                                   containing the content block with both the signature,
                                   the signer's certificate, and any necessary intermediate
                                   certificates.
 
  @param[in]  Pkcs7SignatureSize - Number of bytes in pPkcs7Signature.
 
  @param[in]  RequiredEKUs       - Array of null-terminated strings listing OIDs of
                                   required EKUs that must be present in the signature.
                                   All specified EKU's must be present in order to
                                   succeed.
 
  @param[in]  RequiredEKUsSize   - Number of elements in the rgRequiredEKUs string.
                                   This parameter has a maximum of MAX_EKU_SEARCH.

  @param[in]  RequireAllPresent - If this is TRUE, then all of the specified EKU's
                                  must be present in the leaf signer.  If it is
                                  FALSE, then we will succeed if we find any
                                  of the specified EKU's.

  @retval EFI_SUCCESS             - The required EKUs were found in the signature.
  @retval EFI_INVALID_PARAMETER   - A parameter was invalid.
  @retval EFI_NOT_FOUND           - One or more EKU's were not found in the signature.
**/
EFI_STATUS
EFIAPI
VerifyEKUsInPkcs7Signature (
  IN CONST UINT8  *Pkcs7Signature,
  IN CONST UINT32  SignatureSize,
  IN CONST CHAR8  *RequiredEKUs[],
  IN CONST UINT32  RequiredEKUsSize,
  IN BOOLEAN       RequireAllPresent
  )
{

  EFI_STATUS      Status                 = EFI_SUCCESS;
  PKCS7          *Pkcs7                  = NULL;
  STACK_OF(X509) *CertChain              = NULL;
  INT32           SignatureType          = 0;
  INT32           NumberCertsInSignature = 0;
  X509           *SignerCert             = NULL;
  UINT8          *SignedData             = NULL;
  UINT8          *Temp                   = NULL;
  UINTN           SignedDataSize         = 0;
  BOOLEAN         IsWrapped              = FALSE;
  BOOLEAN         Ok                     = FALSE;

  //
  // Validate the input parameters.
  //
  if (Pkcs7Signature   == NULL ||
      SignatureSize    == 0    ||
      RequiredEKUs     == NULL ||
      RequiredEKUsSize == 0    ) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR:  Invalid parameter!\n",
           __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (RequiredEKUsSize == 1) {
    RequireAllPresent = TRUE;
  }

  //
  // Wrap the PKCS7 data if needed.
  //
  Ok = WrapPkcs7Data(Pkcs7Signature,
                     SignatureSize, 
                     &IsWrapped, 
                     &SignedData,  
                     &SignedDataSize);
  if (!Ok) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR:  WrapPkcs7Data failed.\n",
           __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  Temp = SignedData;

  //
  // Create the PKCS7 object.
  //
  // MS_CHANGE: fix the cast to be UINT8 instead of CHAR8
  Pkcs7 = d2i_PKCS7(NULL, (CONST UINT8 **)&Temp, (INT32)SignedDataSize);
  if (Pkcs7 == NULL) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR:  Could not read PKCS7 data.\n",
          __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get the certificate chain.
  //
  SignatureType = OBJ_obj2nid(Pkcs7->type);
  switch (SignatureType) {
  case NID_pkcs7_signed:
    if (Pkcs7->d.sign != NULL) {
      CertChain = Pkcs7->d.sign->cert;
    }
    break;
  case NID_pkcs7_signedAndEnveloped:
    if (Pkcs7->d.signed_and_enveloped != NULL) {
      CertChain = Pkcs7->d.signed_and_enveloped->cert;
    }
    break;
  default:
    break;
  }

  //
  // Ensure we have a certificate stack
  //
  if (CertChain == NULL) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR:  Could not get the certificate stack from signature!\n",
           __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Find out how many certificates were in the PKCS7 signature.
  //
  NumberCertsInSignature = sk_X509_num(CertChain);

  DEBUG((DEBUG_INFO,
         "%a - Number of certificates in the signature: %d\n",
         __FUNCTION__, NumberCertsInSignature));

  if (NumberCertsInSignature == 0) {
    DEBUG((DEBUG_ERROR,
           "%a - ERROR, we did not find any certificates in the signature.\n",
            __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get the leaf signer.
  //
  Status = GetSignerCertificate(Pkcs7, &SignerCert);
  if (Status != EFI_SUCCESS || SignerCert == NULL) {
    DEBUG((DEBUG_ERROR,
      "%a - ERROR:  Could not get the end-entity leaf signer certificate.\n",
      __FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  
  Status = CheckEKUs(SignerCert, RequiredEKUs, RequiredEKUsSize, RequireAllPresent);
  if (Status != EFI_SUCCESS) {
    goto Exit;
  }

Exit:
  
  //
  // Cleanup.  If the signature was not wrapped, then the call to WrapData()
  // will allocate the data and add a header to it, so we must clean it up here.
  //
  if (!IsWrapped && SignedData) {
    free(SignedData);
  }

  if (SignerCert) {
    X509_free(SignerCert);
  }

  if (Pkcs7) {
    PKCS7_free(Pkcs7);
  }

  return Status;
}// VerifyEKUsInPkcs7Signature()

