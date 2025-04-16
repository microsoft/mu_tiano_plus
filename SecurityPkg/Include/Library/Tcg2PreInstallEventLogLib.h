/** @file -- Tcg2PreInstallEventLogLib.h
  This describes the interface that should be published by instances of the
  Tcg2PreInstallEventLogLib. This library can be used to publish TPM EventLog
  entries for measurements before the TCG2 protocol is installed and for
  platforms that start at the DXE phase.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef TCG_2_PRE_INSTALL_EVENT_LOG_LIB_H_
#define TCG_2_PRE_INSTALL_EVENT_LOG_LIB_H_

/**
  Create the EventLog entries.
**/
VOID
EFIAPI
CreateTcg2PreInstallEventLogEntries (
  VOID
  );

#endif // TCG_2_DXE_EVENT_LOG_LIB_H_
