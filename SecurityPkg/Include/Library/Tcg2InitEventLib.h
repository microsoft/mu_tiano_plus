/** @file
  Initializes TCG event data structures

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef TCG_2_INIT_EVENT_LIB_H_
#define TCG_2_INIT_EVENT_LIB_H_

#include <Base.h>
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
  );

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
  );

#endif // TCG_2_INIT_EVENT_LIB_H_
