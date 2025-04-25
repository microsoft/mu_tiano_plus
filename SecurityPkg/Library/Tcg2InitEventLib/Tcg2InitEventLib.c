/** @file
  Initializes TCG event data structures

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiLib/UefiLibInternal.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/Tcg2InitEventLib.h>
#include <IndustryStandard/UefiTcgPlatform.h>

/**
  This function initializes the TCG_PCR_EVENT2_HDR for EV_NO_ACTION events. The behavior is defined
  by TCG PC Client PC Client PFP SPec. Secion 9.3.4 EV_NO_ACTION Event Types.

  Note: The EFI Specification ID event is not supported.

  It is considered an error to call this function with a NULL pointer, or an ActivePcrBanks value of 0.

  @param[in, out]   Event           Event Header of an EV_NO_ACTION type event
  @param[in]        EventSize       Size of the Event argument
  @param[in]        ActivePcrBanks  Active PCR Banks bitmap

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.

**/
EFI_STATUS
EFIAPI
InitNoActionEvent (
  IN OUT TCG_PCR_EVENT2_HDR           *Event,
  IN UINT32                           EventSize,
  IN EFI_TCG2_EVENT_ALGORITHM_BITMAP  ActivePcrBanks
  )
{
  UINT32         DigestListCount;
  TPMI_ALG_HASH  HashAlgId;
  UINT8          *DigestBuffer;

  if (Event == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ActivePcrBanks == 0) {
    return EFI_INVALID_PARAMETER;
  }

  DigestBuffer    = (UINT8 *)Event->Digests.digests;
  DigestListCount = 0;

  Event->PCRIndex  = 0;
  Event->EventType = EV_NO_ACTION;

  //
  // Set Hash count & hashAlg accordingly, while Digest.digests[n].digest to all 0
  //
  ZeroMem (&Event->Digests, sizeof (Event->Digests));

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA1) != 0) {
    HashAlgId = TPM_ALG_SHA1;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (HashAlgId));
    DigestBuffer += sizeof (HashAlgId) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA256) != 0) {
    HashAlgId = TPM_ALG_SHA256;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (HashAlgId));
    DigestBuffer += sizeof (HashAlgId) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA384) != 0) {
    HashAlgId = TPM_ALG_SHA384;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (HashAlgId));
    DigestBuffer += sizeof (HashAlgId) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA512) != 0) {
    HashAlgId = TPM_ALG_SHA512;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (HashAlgId));
    DigestBuffer += sizeof (HashAlgId) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SM3_256) != 0) {
    HashAlgId = TPM_ALG_SM3_256;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (HashAlgId));
    DigestBuffer += sizeof (HashAlgId) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  //
  // Set Digests Count
  //
  WriteUnaligned32 ((UINT32 *)&Event->Digests.count, DigestListCount);

  //
  // Set Event Size
  //
  WriteUnaligned32 ((UINT32 *)DigestBuffer, EventSize);

  return EFI_SUCCESS;
}

/**
  This function initializes the TCG_PCR_EVENT2_HDR for HCRTM events.

  It is considered an error to call this function with a NULL pointer, or an ActivePcrBanks value of 0.

  @param[in, out]   Event           Event Header of an HCRTM type event
  @param[in]        EventSize       Size of the Event argument
  @param[in]        ActivePcrBanks  Active PCR Banks bitmap

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.

**/
EFI_STATUS
EFIAPI
InitHcrtmEvent (
  IN OUT TCG_PCR_EVENT2_HDR           *Event,
  IN UINT32                           EventSize,
  IN EFI_TCG2_EVENT_ALGORITHM_BITMAP  ActivePcrBanks
  )
{
  EFI_STATUS     Status;
  TCG_EVENTTYPE  EventType;

  EventType        = Event->EventType;
  Status           = InitNoActionEvent (Event, EventSize, ActivePcrBanks);
  Event->EventType = EventType;

  return Status;
}
