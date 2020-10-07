/** @file
This module contains definitions and prototypes for an interface used
to determine at runtime if source debugging is enabled.

MS_CHANGE_?

Copyright (c) Microsoft Corporation. All rights reserved.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SOURCE_DEBUG_ENABLED_LIB_H__
#define __SOURCE_DEBUG_ENABLED_LIB_H__

/**
  Indicates if source debugging is runtime enabled.

  This routine indicates if source debugging is runtime enabled. When the
  SourceLevelDebugPkg is included in the BIOS, this routine is called
  during initialization to indicate if the debugging features should be
  enabled.

  This permits the source level debug package to be included and then
  selectively enabled and disabled.

  @InitFlag      Supplies a value that indicates what kind of initialization
                 is being performed.

  @retval TRUE             Source debugging is runtime enabled.
  @retval FALSE            Source debugging is not runtime enabled.

**/
BOOLEAN
EFIAPI
IsSourceDebugEnabled (
  IN UINT32  InitFlag
  );

#endif // __SOURCE_DEBUG_ENABLED_LIB_H__
