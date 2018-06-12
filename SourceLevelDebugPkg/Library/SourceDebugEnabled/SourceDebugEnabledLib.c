/*++

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
  IN UINT32 InitFlag
  )
{
  return FALSE;
}
