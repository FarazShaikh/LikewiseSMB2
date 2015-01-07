/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        pam-logging.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Pluggable Authentication Module
 *
 *        Logging API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "includes.h"

/* LOG_AUTHPRIV is not defined in solaris */
#ifndef LOG_AUTHPRIV
#define LOG_AUTHPRIV LOG_AUTH
#endif

void
LsaPamInitLog(
    void
    )
{
    if (!gbLogInitialized) {

#if defined(EXPLICIT_OPEN_CLOSE_LOG)

       openlog(MODULE_NAME, LOG_PID, LOG_AUTHPRIV);

#endif

       gbLogInitialized = TRUE;
    }
}

void
LsaPamCloseLog(
    void
    )
{
    if (gbLogInitialized) {

#if defined(EXPLICIT_OPEN_CLOSE_LOG)

       closelog();

#endif

       gbLogInitialized = FALSE;
    }
}

void
LsaPamLogMessage(
    DWORD dwLogLevel,
    PSTR pszFormat, ...
    )
{
    if ((gdwLogLevel != LSA_PAM_LOG_LEVEL_DISABLED) &&
        (gdwLogLevel >= dwLogLevel))
    {
       va_list argp;

       va_start(argp, pszFormat);

       LsaPamInitLog();

       switch (dwLogLevel)
       {
           case LSA_PAM_LOG_LEVEL_ALWAYS:
           {
               lsass_vsyslog(LOG_INFO, pszFormat, argp);
               break;
           }
           case LSA_PAM_LOG_LEVEL_ERROR:
           {
               lsass_vsyslog(LOG_ERR, pszFormat, argp);
               break;
           }

           case LSA_PAM_LOG_LEVEL_WARNING:
           {
               lsass_vsyslog(LOG_WARNING, pszFormat, argp);
               break;
           }

           case LSA_PAM_LOG_LEVEL_INFO:
           {
               lsass_vsyslog(LOG_INFO, pszFormat, argp);
               break;
           }

           default:
           {
               lsass_vsyslog(LOG_INFO, pszFormat, argp);
               break;
           }
       }

       va_end(argp);
    }
}

void
LsaPamSetLogLevel(
    DWORD dwLogLevel
    )
{
    gdwLogLevel = dwLogLevel;
}

