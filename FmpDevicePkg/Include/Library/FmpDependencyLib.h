/** @file
  Fmp Capsule Dependency support functions for Firmware Management Protocol based
  firmware updates.

// MU_CHANGE Starts
  Copyright (c) Microsoft Corporation.<BR>
// MU_CHANGE Ends
  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FMP_DEPENDENCY_LIB__
#define __FMP_DEPENDENCY_LIB__

#include <PiDxe.h>
#include <Protocol/FirmwareManagement.h>

//
// Data struct to store FMP ImageType and version for dependency check.
//
typedef struct {
  EFI_GUID ImageTypeId;
  UINT32   Version;
} FMP_DEPEX_CHECK_VERSION_DATA;

/**
  Validate the dependency expression and output its size.

// MU_CHANGE Starts
  @param[in]   Dependencies       Pointer to the EFI_FIRMWARE_IMAGE_DEP.
  @param[in]   MaxDepexSize       Max size of the dependency.
  @param[out]  DepexSize          Size of dependency.
  @param[out]  LastAttemptStatus  An optional pointer to a UINT32 that holds the
                                  last attempt status to report back to the caller.
                                  A function error code may not always be accompanied
                                  by a last attempt status code.
// MU_CHANGE Ends

// MU_CHANGE Starts
  @retval TRUE    The dependency expression is valid.
  @retval FALSE   The dependency expression is invalid.
// MU_CHANGE Ends

**/
BOOLEAN
EFIAPI
ValidateDependency (
  IN  EFI_FIRMWARE_IMAGE_DEP  *Dependencies,
  IN  UINTN                   MaxDepexSize,
// MU_CHANGE Starts
  OUT UINT32                  *DepexSize,
  OUT UINT32                  *LastAttemptStatus OPTIONAL
// MU_CHANGE Ends
  );

/**
  Get dependency from firmware image.

// MU_CHANGE Starts
  @param[in]  Image               Points to the firmware image.
  @param[in]  ImageSize           Size, in bytes, of the firmware image.
  @param[out] DepexSize           Size, in bytes, of the dependency.
  @param[out] LastAttemptStatus   An optional pointer to a UINT32 that holds the
                                  last attempt status to report back to the caller.
                                  A function error code may not always be accompanied
                                  by a last attempt status code.
// MU_CHANGE Ends
  @retval  The pointer to dependency.
  @retval  Null

**/
EFI_FIRMWARE_IMAGE_DEP*
EFIAPI
GetImageDependency (
  IN  EFI_FIRMWARE_IMAGE_AUTHENTICATION *Image,
  IN  UINTN                             ImageSize,
// MU_CHANGE Starts
  OUT UINT32                            *DepexSize,
  OUT UINT32                            *LastAttemptStatus  OPTIONAL
// MU_CHANGE Ends
  );

/**
  Evaluate the dependencies. The caller must search all the Fmp instances and
  gather their versions into FmpVersions parameter. If there is PUSH_GUID opcode
  in dependency expression with no FmpVersions provided, the dependency will
  evaluate to FALSE.

  @param[in]   Dependencies       Dependency expressions.
  @param[in]   DependenciesSize   Size of Dependency expressions.
  @param[in]   FmpVersions        Array of Fmp ImageTypeId and version. This
                                  parameter is optional and can be set to NULL.
  @param[in]   FmpVersionsCount   Element count of the array. When FmpVersions
                                  is NULL, FmpVersionsCount must be 0.
// MU_CHANGE Starts
  @param[out]  LastAttemptStatus  An optional pointer to a UINT32 that holds the
                                  last attempt status to report back to the caller.
                                  A function error code may not always be accompanied
                                  by a last attempt status code.
// MU_CHANGE Ends

  @retval TRUE    Dependency expressions evaluate to TRUE.
  @retval FALSE   Dependency expressions evaluate to FALSE.

**/
BOOLEAN
EFIAPI
EvaluateDependency (
// MU_CHANGE Starts
  IN  EFI_FIRMWARE_IMAGE_DEP        *Dependencies,
  IN  UINTN                         DependenciesSize,
  IN  FMP_DEPEX_CHECK_VERSION_DATA  *FmpVersions,      OPTIONAL
  IN  UINTN                         FmpVersionsCount,
  OUT UINT32                        *LastAttemptStatus OPTIONAL
// MU_CHANGE Ends
  );

#endif
