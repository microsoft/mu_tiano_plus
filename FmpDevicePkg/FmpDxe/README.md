# Introduction
TODO: Give a short introduction of FmpDxe driver.

# Getting Started
TODO: Guide users through getting code up and running on their own system. Ideally including:
1.	Installation process
2.	Software dependencies
3.	Latest releases
4.	API references

# Build and Test
TODO: Describe and show how to build code and run the tests.

# Design Changes
---

- **Date:** 06/15/2020
- **Description/Rationale:** Extending on the more granular LastAttemptStatus support added in FmpDeviceSetImage (),
FmpDeviceCheckImage () also has a LastAttemptStatus parameter added. An image check is always performed by a set
image operation. A more granular status code from the check image path greatly improves overall error isolation when
applying an image.
- **Changes:** This change allows the FmpDeviceLib implementation to return a last attempt status code in the range
LAST_ATTEMPT_STATUS_LIBRARY_ERROR_MIN_ERROR_CODE to LAST_ATTEMPT_STATUS_LIBRARY_ERROR_MAX_ERROR_CODE. Furthermore,
an internal wrapper for CheckTheImage () in FmpDxe was added called CheckTheImageInternal (). This function can return
a last attempt status code for an error in the driver prior to invoking FmpDeviceCheckImage (). These driver error
codes will be in the range of LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL_VENDOR_RANGE_MIN to
LAST_ATTEMPT_STATUS_DRIVER_ERROR_MAX_ERROR_CODE.
- **Impact/Mitigation:**
The change break the build for all FmpDeviceLib instances due to the API change. Each FmpDeviceLib should change to
the new API definition and implement support to return unique values for LastAttemptStatus when appropriate.

---

- **Date:** 10/07/2019
- **Description/Rationale:** Capsule update is the process where each OEM has a lot of interest. Especially when there is capsule update failure, it is helpful to gather more information of the failure. With existed implementation, SetImage routine from FmpDxe driver, which is the most heavy lifting function call during capsule update, will only populate LastAttemptStatus with limited pre-defined error codes which could be consumed/inspected by the OS when it recovers and boots. Thus our proposal is to update the SetImage routine and leverage the LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL_VENDOR_RANGE range newly defined in UEFI Spec 2.8 Section 23.4, so that the error code will provide better granularity when viewing capsule update failure from OS device manager.
- **Changes:** A few error codes (128 total) are reserved from LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL_VENDOR_RANGE range for FmpDxe driver usage, which ranges from thermal and power API failure to capsule payload header check failure. Furthermore, <i>an output pointer of the LastAttemptStatus is added as an input argument for FmpDeviceSetImage function in FmpDeviceLib to allow platform to provide their own platform specific error codes </i> (SPI write failure, SVN checking failure, and more).
- **Impact/Mitigation:**
The italic text above will cause a breaking change for all the FmpDeviceLib instances due to API change. This is to provide a better visibility for OEMs to decode the capsule update failure more efficiently. Each FmpDeviceLib should change to new API definition and populate proper LastAttemptStatus value when applicable.
