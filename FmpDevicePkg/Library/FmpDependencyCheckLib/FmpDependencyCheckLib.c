/** @file
  Provides FMP capsule dependency check services when updating the firmware
  image of a FMP device.

  Copyright (c) Microsoft Corporation.<BR>
  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FmpDependencyLib.h>
#include <Library/FmpDependencyCheckLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
// MU_CHANGE Starts
#include <Guid/SystemResourceTable.h>
#include <LastAttemptStatus.h>
// MU_CHANGE Ends

/**
  Check dependency for firmware update.

  @param[in]  ImageTypeId        Image Type Id.
  @param[in]  Version            New version.
  @param[in]  Dependencies       Fmp dependency.
  @param[in]  DependenciesSize   Size, in bytes, of the Fmp dependency.
//MU_CHANGE Starts
  @param[out] LastAttemptStatus  An optional pointer to a UINT32 that holds the
                                 last attempt status to report back to the caller.
                                 A function error code may not always be accompanied
                                 by a last attempt status code.
// MU_CHANGE Ends

  @retval  TRUE    Dependencies are satisfied.
  @retval  FALSE   Dependencies are unsatisfied or dependency check fails.

**/
BOOLEAN
EFIAPI
CheckFmpDependency (
  IN  EFI_GUID                ImageTypeId,
  IN  UINT32                  Version,
  IN  EFI_FIRMWARE_IMAGE_DEP  *Dependencies,    OPTIONAL
//MU_CHANGE Starts
  IN  UINT32                  DependenciesSize,
  OUT UINT32                  *LastAttemptStatus OPTIONAL
//MU_CHANGE Ends
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *Fmp;
  UINTN                             ImageInfoSize;
//MU_CHANGE Starts
  UINT32                            LocalLastAttemptStatus;
//MU_CHANGE Ends
  UINT32                            *DescriptorVer;
  UINT8                             FmpImageInfoCount;
  UINTN                             *DescriptorSize;
  UINT32                            PackageVersion;
  CHAR16                            *PackageVersionName;
  UINTN                             NumberOfFmpInstance;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR     **FmpImageInfoBuf;
  FMP_DEPEX_CHECK_VERSION_DATA      *FmpVersions;
  UINTN                             FmpVersionsCount;
  BOOLEAN                           IsSatisfied;

//MU_CHANGE Starts
  LocalLastAttemptStatus  = LAST_ATTEMPT_STATUS_SUCCESS;
  FmpImageInfoBuf         = NULL;
  DescriptorVer           = NULL;
  DescriptorSize          = NULL;
  NumberOfFmpInstance     = 0;
  FmpVersions             = NULL;
  FmpVersionsCount        = 0;
  IsSatisfied             = TRUE;
  PackageVersionName      = NULL;
//MU_CHANGE Ends

  //
  // Get ImageDescriptors of all FMP instances, and archive them for dependency evaluation.
  //
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiFirmwareManagementProtocolGuid,
                NULL,
                &NumberOfFmpInstance,
                &HandleBuffer
                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CheckFmpDependency: Get Firmware Management Protocol failed. (%r)", Status));
    IsSatisfied = FALSE;
//MU_CHANGE Starts
    LocalLastAttemptStatus = LAST_ATTEMPT_STATUS_DEPENDENCY_ERROR_MEMORY_ALLOCATION_FAILED;
//MU_CHANGE Ends
    goto cleanup;
  }

  FmpImageInfoBuf = AllocateZeroPool (sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR *) * NumberOfFmpInstance);
  if (FmpImageInfoBuf == NULL) {
    IsSatisfied = FALSE;
//MU_CHANGE Starts
    LocalLastAttemptStatus = LAST_ATTEMPT_STATUS_DEPENDENCY_ERROR_MEMORY_ALLOCATION_FAILED;
//MU_CHANGE Ends
    goto cleanup;
  }

  DescriptorVer = AllocateZeroPool (sizeof(UINT32) * NumberOfFmpInstance);
  if (DescriptorVer == NULL ) {
    IsSatisfied = FALSE;
//MU_CHANGE Starts
    LocalLastAttemptStatus = LAST_ATTEMPT_STATUS_DEPENDENCY_ERROR_MEMORY_ALLOCATION_FAILED;
//MU_CHANGE Ends
    goto cleanup;
  }

  DescriptorSize = AllocateZeroPool (sizeof(UINTN) * NumberOfFmpInstance);
  if (DescriptorSize == NULL ) {
    IsSatisfied = FALSE;
//MU_CHANGE Starts
    LocalLastAttemptStatus = LAST_ATTEMPT_STATUS_DEPENDENCY_ERROR_MEMORY_ALLOCATION_FAILED;
//MU_CHANGE Ends
    goto cleanup;
  }

  FmpVersions = AllocateZeroPool (sizeof(FMP_DEPEX_CHECK_VERSION_DATA) * NumberOfFmpInstance);
  if (FmpVersions == NULL) {
    IsSatisfied = FALSE;
//MU_CHANGE Starts
    LocalLastAttemptStatus = LAST_ATTEMPT_STATUS_DEPENDENCY_ERROR_MEMORY_ALLOCATION_FAILED;
//MU_CHANGE Ends
    goto cleanup;
  }

  for (Index = 0; Index < NumberOfFmpInstance; Index ++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareManagementProtocolGuid,
                    (VOID **) &Fmp
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    ImageInfoSize = 0;
    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                    );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }

    FmpImageInfoBuf[Index] = AllocateZeroPool (ImageInfoSize);
    if (FmpImageInfoBuf[Index] == NULL) {
      continue;
    }

    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,               // ImageInfoSize
                    FmpImageInfoBuf[Index],       // ImageInfo
                    &DescriptorVer[Index],        // DescriptorVersion
                    &FmpImageInfoCount,           // DescriptorCount
                    &DescriptorSize[Index],       // DescriptorSize
                    &PackageVersion,              // PackageVersion
                    &PackageVersionName           // PackageVersionName
                    );
    if (EFI_ERROR(Status)) {
      FreePool (FmpImageInfoBuf[Index]);
      FmpImageInfoBuf[Index] = NULL;
      continue;
    }

    if (PackageVersionName != NULL) {
      FreePool (PackageVersionName);
      PackageVersionName = NULL;
    }

    CopyGuid (&FmpVersions[FmpVersionsCount].ImageTypeId, &FmpImageInfoBuf[Index]->ImageTypeId);
    FmpVersions[FmpVersionsCount].Version = FmpImageInfoBuf[Index]->Version;
    FmpVersionsCount ++;
  }

  //
  // Evaluate firmware image's depex, against the version of other Fmp instances.
  //
  if (Dependencies != NULL) {
//MU_CHANGE Starts
    IsSatisfied = EvaluateDependency (Dependencies, DependenciesSize, FmpVersions, FmpVersionsCount, &LocalLastAttemptStatus);
//MU_CHANGE Ends
  }

  if (!IsSatisfied) {
    DEBUG ((DEBUG_ERROR, "CheckFmpDependency: %g\'s dependency is not satisfied!\n", ImageTypeId));
    goto cleanup;
  }

cleanup:
  if (FmpImageInfoBuf != NULL) {
    for (Index = 0; Index < NumberOfFmpInstance; Index ++) {
      if (FmpImageInfoBuf[Index] != NULL) {
        FreePool (FmpImageInfoBuf[Index]);
      }
    }
    FreePool (FmpImageInfoBuf);
  }

  if (DescriptorVer != NULL) {
    FreePool (DescriptorVer);
  }

  if (DescriptorSize != NULL) {
    FreePool (DescriptorSize);
  }

  if (FmpVersions != NULL) {
    FreePool (FmpVersions);
  }

//MU_CHANGE Starts
  if (LastAttemptStatus != NULL && LocalLastAttemptStatus != LAST_ATTEMPT_STATUS_SUCCESS) {
    *LastAttemptStatus = LocalLastAttemptStatus;
  }
//MU_CHANGE Ends

  return IsSatisfied;
}
