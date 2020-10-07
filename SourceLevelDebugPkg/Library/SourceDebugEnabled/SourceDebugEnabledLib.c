/*++

MS_CHANGE_?

Copyright (c) Microsoft Corporation. All rights reserved.
SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:

MsSourceDebugEnabledLib.c

Abstract:

This module implements routines that indicate if source debugging is
runtime enabled.

Environment:

PEI/DXE

--*/

#include <Library/SourceDebugEnabledLib.h>

/**
  Check if source debugging is runetime enabled.

  @InitFlag      Supplies a value that indicates what kind of initialization
                 is being performed.

  @return TRUE   Source debugging is enabled.
  @return FALSE  Source debugging is not enabled.

**/
BOOLEAN
EFIAPI
IsSourceDebugEnabled (
  IN UINT32  InitFlag
  )
{
  return FALSE;
}
