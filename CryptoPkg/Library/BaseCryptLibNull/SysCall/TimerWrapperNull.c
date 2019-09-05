/** @file
  C Run-Time Libraries (CRT) Time Management Routines Wrapper Implementation
  for OpenSSL-based Cryptographic Library (used in DXE & RUNTIME).

Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <CrtLibSupport.h>


/* Get the system time as seconds elapsed since midnight, January 1, 1970. */
//INTN time(
//  INTN *timer
//  )
time_t time (time_t *timer)
{
  ASSERT(FALSE);
  return 0;
}

//
// Convert a time value from type time_t to struct tm.
//
struct tm * gmtime (const time_t *timer)
{
  ASSERT(FALSE);
  return NULL;
}
