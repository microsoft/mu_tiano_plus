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

Copyright (c) 2018, Microsoft Corporation

All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
