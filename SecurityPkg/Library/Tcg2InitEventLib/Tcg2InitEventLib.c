/** @file
  This instance of the Tcg2InitEventLib is available to initialize events
  for logging purposes.

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
  This function initialize TCG_PCR_EVENT2_HDR for EV_NO_ACTION Event Type other than EFI Specification ID event
  The behavior is defined by TCG PC Client PFP Spec. Section 9.3.4 EV_NO_ACTION Event Types

  It is considered an error to call this function with a NULL pointer, or an ActivePcrBanks value of 0.

  @param[in, out]   Event           Event Header of EV_NO_ACTION Event
  @param[in]        EventSize       Event Size of the EV_NO_ACTION Event
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

  DEBUG ((DEBUG_VERBOSE, "InitNoActionEvent ...\n"));

  if (Event == NULL) {
    DEBUG ((DEBUG_ERROR, "Event parameter is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (ActivePcrBanks == 0) {
    DEBUG ((DEBUG_ERROR, "ActivePcrBanks is 0\n"));
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
    CopyMem (DigestBuffer, &HashAlgId, sizeof (TPMI_ALG_HASH));
    DigestBuffer += sizeof (TPMI_ALG_HASH) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA256) != 0) {
    HashAlgId = TPM_ALG_SHA256;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (TPMI_ALG_HASH));
    DigestBuffer += sizeof (TPMI_ALG_HASH) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA384) != 0) {
    HashAlgId = TPM_ALG_SHA384;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (TPMI_ALG_HASH));
    DigestBuffer += sizeof (TPMI_ALG_HASH) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SHA512) != 0) {
    HashAlgId = TPM_ALG_SHA512;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (TPMI_ALG_HASH));
    DigestBuffer += sizeof (TPMI_ALG_HASH) + GetHashSizeFromAlgo (HashAlgId);
    DigestListCount++;
  }

  if ((ActivePcrBanks & EFI_TCG2_BOOT_HASH_ALG_SM3_256) != 0) {
    HashAlgId = TPM_ALG_SM3_256;
    CopyMem (DigestBuffer, &HashAlgId, sizeof (TPMI_ALG_HASH));
    DigestBuffer += sizeof (TPMI_ALG_HASH) + GetHashSizeFromAlgo (HashAlgId);
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

  DEBUG ((DEBUG_VERBOSE, "InitNoActionEvent - %r\n", EFI_SUCCESS));

  return EFI_SUCCESS;
}

/**
  This function initialize TCG_PCR_EVENT2_HDR for HCRTM Event Type.

  It is considered an error to call this function with a NULL pointer, or an ActivePcrBanks value of 0.

  @param[in, out]   Event           Event Header of HCRTM Event
  @param[in]        EventSize       Event Size of the HCRTM Event
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
