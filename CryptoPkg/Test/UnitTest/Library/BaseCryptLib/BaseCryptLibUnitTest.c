/** @file
  This is a sample to demostrate the usage of the Unit Test Library that
  supports the PEI, DXE, SMM, UEFI SHell, and host execution environments.

  Copyright (c) Microsoft Corporation.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "TestBaseCryptLib.h"

SUITE_DESC  mSuiteDesc[] = {
    //
    // Title--------------------------Package-------------------Sup--Tdn----TestNum------------TestDesc
    //
    {"EKU verify tests",            "CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs7EkuTestNum,       mPkcs7EkuTest},
    {"HASH verify tests",           "CryptoPkg.BaseCryptLib", NULL, NULL, &mHashTestNum,           mHashTest},
    {"HMAC verify tests",           "CryptoPkg.BaseCryptLib", NULL, NULL, &mHmacTestNum,           mHmacTest},
    {"BlockCipher verify tests",    "CryptoPkg.BaseCryptLib", NULL, NULL, &mBlockCipherTestNum,    mBlockCipherTest},
    {"RSA verify tests",            "CryptoPkg.BaseCryptLib", NULL, NULL, &mRsaTestNum,            mRsaTest},
    {"RSACert verify tests",        "CryptoPkg.BaseCryptLib", NULL, NULL, &mRsaCertTestNum,        mRsaCertTest},
    {"PKCS7 verify tests",          "CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs7TestNum,          mPkcs7Test},
    {"PKCS5 verify tests",          "CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs5TestNum,          mPkcs5Test},
    {"Authenticode verify tests",   "CryptoPkg.BaseCryptLib", NULL, NULL, &mAuthenticodeTestNum,   mAuthenticodeTest},
    {"ImageTimestamp verify tests", "CryptoPkg.BaseCryptLib", NULL, NULL, &mImageTimestampTestNum, mImageTimestampTest},
    {"DH verify tests",             "CryptoPkg.BaseCryptLib", NULL, NULL, &mDhTestNum,             mDhTest},
    {"PRNG verify tests",           "CryptoPkg.BaseCryptLib", NULL, NULL, &mPrngTestNum,           mPrngTest},
    {"OAEP encrypt verify tests",   "CryptoPkg.BaseCryptLib", NULL, NULL, &mOaepTestNum,           mOaepTest},
};

/**
  Initialize the unit test framework, suite, and unit tests for the
  sample unit tests and run the unit tests.

  @retval  EFI_SUCCESS           All test cases were dispatched.
  @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                 initialize the unit tests.
**/
EFI_STATUS
EFIAPI
UefiTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UINTN                       SuiteIndex;
  UINTN                       TestIndex;

  Framework = NULL;

  DEBUG(( DEBUG_INFO, "%a v%a\n", UNIT_TEST_NAME, UNIT_TEST_VERSION ));

  //
  // Start setting up the test framework for running the tests.
  //
  Status = InitUnitTestFramework (&Framework, UNIT_TEST_NAME, gEfiCallerBaseName, UNIT_TEST_VERSION);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    UNIT_TEST_SUITE_HANDLE Suite = NULL;
    Status = CreateUnitTestSuite (&Suite, Framework, mSuiteDesc[SuiteIndex].Title, mSuiteDesc[SuiteIndex].Package, mSuiteDesc[SuiteIndex].Sup, mSuiteDesc[SuiteIndex].Tdn);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    for (TestIndex = 0; TestIndex < *mSuiteDesc[SuiteIndex].TestNum; TestIndex++) {
      AddTestCase (Suite, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Description, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->ClassName, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Func, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->PreReq, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->CleanUp, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Context);
    }
  }

  //
  // Execute the tests.
  //
  Status = RunAllTestSuites (Framework);

EXIT:
  if (Framework) {
    FreeUnitTestFramework (Framework);
  }

  return Status;
}

/**
  Standard PEIM entry point for target based unit test execution from PEI.
**/
EFI_STATUS
EFIAPI
PeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return UefiTestMain ();
}

/**
  Standard UEFI entry point for target based unit test execution from DXE, SMM,
  UEFI Shell.
**/
EFI_STATUS
EFIAPI
DxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return UefiTestMain ();
}

/**
  Standard POSIX C entry point for host based unit test execution.
**/
int
main (
  int argc,
  char *argv[]
  )
{
  return UefiTestMain ();
}
