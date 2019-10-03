/** @file
  C Run-Time Libraries (CRT) Wrapper Implementation for OpenSSL-based
  Cryptographic Library.

Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <CrtLibSupport.h>

int errno = 0;

//---------------------------------------------------------
// Standard C Run-time Library Interface Wrapper
//---------------------------------------------------------

//
// -- String Manipulation Routines --
//

/* Scan a string for the last occurrence of a character */
char *strrchr (const char *str, int c)
{
  ASSERT(FALSE);
  return NULL;
}

/* Compare first n bytes of string s1 with string s2, ignoring case */
int strncasecmp (const char *s1, const char *s2, size_t n)
{
  ASSERT(FALSE);
  return 0;
}

/* Read formatted data from a string */
int sscanf (const char *buffer, const char *format, ...)
{
  //
  // Null sscanf() function implementation to satisfy the linker, since
  // no direct functionality logic dependency in present UEFI cases.
  //
  ASSERT(FALSE);
  return 0;
}

/* Maps errnum to an error-message string */
char * strerror (int errnum)
{
  ASSERT(FALSE);
  return NULL;
}

/* Computes the length of the maximum initial segment of the string pointed to by s1
   which consists entirely of characters from the string pointed to by s2. */
size_t strspn (const char *s1 , const char *s2)
{
  ASSERT(FALSE);
  return 0;
}

/* Computes the length of the maximum initial segment of the string pointed to by s1
   which consists entirely of characters not from the string pointed to by s2. */
size_t strcspn (const char *s1, const char *s2)
{
  ASSERT(FALSE);
  return 0;
}

//
// -- Character Classification Routines --
//

/* Determines if a particular character is a decimal-digit character */
int isdigit (int c)
{
  //
  // <digit> ::= [0-9]
  //
  ASSERT(FALSE);
  return 0;
}

/* Determine if an integer represents character that is a hex digit */
int isxdigit (int c)
{
  //
  // <hexdigit> ::= [0-9] | [a-f] | [A-F]
  //
  ASSERT(FALSE);
  return 0;
}

/* Determines if a particular character represents a space character */
int isspace (int c)
{
  //
  // <space> ::= [ ]
  //
  ASSERT(FALSE);
  return 0;
}

/* Determine if a particular character is an alphanumeric character */
int isalnum (int c)
{
  //
  // <alnum> ::= [0-9] | [a-z] | [A-Z]
  //
  ASSERT(FALSE);
  return 0;
}

/* Determines if a particular character is in upper case */
int isupper (int c)
{
  //
  // <uppercase letter> := [A-Z]
  //
  ASSERT(FALSE);
  return 0;
}

//
// -- Data Conversion Routines --
//

/* Convert strings to a long-integer value */
long strtol (const char *nptr, char **endptr, int base)
{
  //
  // Null strtol() function implementation to satisfy the linker, since there is
  // no direct functionality logic dependency in present UEFI cases.
  //
  ASSERT(FALSE);
  return 0;
}

/* Convert strings to an unsigned long-integer value */
unsigned long strtoul (const char *nptr, char **endptr, int base)
{
  //
  // Null strtoul() function implementation to satisfy the linker, since there is
  // no direct functionality logic dependency in present UEFI cases.
  //
  ASSERT(FALSE);
  return 0;
}

/* Convert character to lowercase */
int tolower (int c)
{
  ASSERT(FALSE);
  return 0;
}

//
// -- Searching and Sorting Routines --
//

/* Performs a quick sort */
void qsort (void *base, size_t num, size_t width, int (*compare)(const void *, const void *))
{
  ASSERT(FALSE);
  return;
}

//
// -- Process and Environment Control Routines --
//

/* Get a value from the current environment */
char *getenv (const char *varname)
{
  //
  // Null getenv() function implementation to satisfy the linker, since there is
  // no direct functionality logic dependency in present UEFI cases.
  //
  return NULL;
}

/* Get a value from the current environment */
char *secure_getenv (const char *varname)
{
  //
  // Null secure_getenv() function implementation to satisfy the linker, since
  // there is no direct functionality logic dependency in present UEFI cases.
  //
  // From the secure_getenv() manual: 'just like getenv() except that it
  // returns NULL in cases where "secure execution" is required'.
  //
  return NULL;
}

//
// -- Stream I/O Routines --
//

/* Write data to a stream */
size_t fwrite (const void *buffer, size_t size, size_t count, FILE *stream)
{
  return 0;
}

//
//  -- Dummy OpenSSL Support Routines --
//

int BIO_printf (void *bio, const char *format, ...)
{
  return 0;
}

int BIO_snprintf(char *buf, size_t n, const char *format, ...)
{
  return 0;
}

#ifdef __GNUC__

typedef
VOID
(EFIAPI *NoReturnFuncPtr)(
  VOID
  ) __attribute__((__noreturn__));

STATIC
VOID
EFIAPI
NopFunction (
  VOID
  )
{
}

void abort (void)
{
  NoReturnFuncPtr NoReturnFunc;

  NoReturnFunc = (NoReturnFuncPtr) NopFunction;

  NoReturnFunc ();
}

#else

void abort (void)
{
  // Do nothing
}

#endif

int fclose (FILE *f)
{
  return 0;
}

FILE *fopen (const char *c, const char *m)
{
  return NULL;
}

size_t fread (void *b, size_t c, size_t i, FILE *f)
{
  return 0;
}

uid_t getuid (void)
{
  return 0;
}

uid_t geteuid (void)
{
  return 0;
}

gid_t getgid (void)
{
  return 0;
}

gid_t getegid (void)
{
  return 0;
}

int printf (char const *fmt, ...)
{
  return 0;
}
