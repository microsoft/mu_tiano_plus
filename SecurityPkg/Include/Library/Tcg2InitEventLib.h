/** @file
  This describes the interface that should be published by instances of the
  Tcg2InitEventLib. This library can be used to initialize events for logging

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef TCG_2_INIT_EVENT_LIB_H_
#define TCG_2_INIT_EVENT_LIB_H_

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
  );

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
  );

#endif // TCG_2_INIT_EVENT_LIB_H_
