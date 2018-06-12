/** @file
This module contains definitions and prototypes for an interface used
to determine at runtime if source debugging is enabled.

MS_CHANGE_?

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.


Copyright (C) 2016 Microsoft Corporation. All Rights Reserved.

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
  IN UINT32 InitFlag
  );

#endif // __SOURCE_DEBUG_ENABLED_LIB_H__
