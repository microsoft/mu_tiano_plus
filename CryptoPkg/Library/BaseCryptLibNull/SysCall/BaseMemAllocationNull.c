/** @file
  Base Memory Allocation Routines Wrapper for Crypto library over OpenSSL
  during PEI & DXE phases.

Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <CrtLibSupport.h>


//
// -- Memory-Allocation Routines --
//

/* Allocates memory blocks */
void *malloc (size_t size)
{
  ASSERT(FALSE);
  return NULL;

}

/* Reallocate memory blocks */
void *realloc (void *ptr, size_t size)
{
 ASSERT(FALSE);
  return NULL;
}

/* De-allocates or frees a memory block */
void free (void *ptr)
{
  ASSERT(FALSE);
}
