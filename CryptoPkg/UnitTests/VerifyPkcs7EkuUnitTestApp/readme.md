# Verify Pkcs7 Enhanced Key Usage Unit Test App

Testing the verification of Enhanced Key Usages in PKCS7 formatted signatures.

## About

This application tests the VerifyEKUsInPkcs7Signature() function in BaseCryptLib.

## VerifyPkcs7EkuUnitTestApp

This application consumes the UnitTestLib and implements various test cases for the verification of Enhanced Key Usages (EKUs) contained in the end-entity (leaf) signing certificate.

## TestEKUCerts folder
This folder contains information on how the test certificate chain and various leaf certificates were created.  It also contains all the files required, and the certificates used in the unit-tests.

### ChainCreationInstructions.txt

This file contains the instructions for creating your own chain, and how to use certreq.exe to create chains.

* **INF files**
  These INF files specify the properties of the certificate to be created.

* **PFX files**
  These Personal Information Exchange (PFX) files contain the certificate, and private key associated with the certificate.  The passwords for these files are specified in ChainCreationInstructions.txt.

* **P7B files**
  These are the detached PKCS7 formatted test signatures used in the unit-tests.

* **CER files**
  These are the certificates that were created.

* **SignFirmwareWithEKUs.cmd**
  This script calls signtool.exe to sign a file with various leaf certificates.

!!! Note
    signtool.exe must be in your PATH.
    Use the Visual Studio command line with the Windows Software Development kit installed.
    To run SignFirmwareWithEKUs.cmd, install the PFX files.
    The passwords are in the ChainCreationInstructions.txt file.

## Copyright

Copyright (c) Microsoft Corporation. All rights reserved.
SPDX-License-Identifier: BSD-2-Clause-Patent