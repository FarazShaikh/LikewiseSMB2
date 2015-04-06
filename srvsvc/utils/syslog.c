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
 *        syslog.c
 *
 * Abstract:
 *
 *        Likewise IO (SRVSVC)
 *
 *        Logging API
 *
 *        Implemenation of logging to syslog
 *
 * Authors: Sriram Nambakam (snambakam@likewisesoftware.com)
 *
 */

#include "includes.h"

DWORD
SrvSvcOpenSyslog(
    PCSTR       pszIdentifier,
    SRVSVC_LOG_LEVEL maxAllowedLogLevel,
    DWORD       dwOptions,
    DWORD       dwFacility,
    PHANDLE     phLog
    )
{
    DWORD dwError = 0;
    PSRVSVC_SYS_LOG pSyslog = NULL;

    dwError = LwAllocateMemory(
                sizeof(SRVSVC_SYS_LOG),
                (PVOID*)&pSyslog);
    if (dwError)
    {
        goto error;
    }

    dwError = SrvSvcAllocateString(
                  (IsNullOrEmptyString(pszIdentifier) ? "lwio" : pszIdentifier),
                  &pSyslog->pszIdentifier);
    if (dwError)
    {
        goto error;
    }

    pSyslog->dwOptions = dwOptions;
    pSyslog->dwFacility = dwFacility;

    openlog(
        pSyslog->pszIdentifier,
        pSyslog->dwOptions,
        pSyslog->dwFacility);

    pSyslog->bOpened = TRUE;

    SrvSvcSetSyslogMask(maxAllowedLogLevel);

    dwError = SrvSvcSetupLogging(
                    (HANDLE)pSyslog,
                    maxAllowedLogLevel,
                    &SrvSvcLogToSyslog);
    if (dwError)
    {
        goto error;
    }

    *phLog = (HANDLE)pSyslog;

cleanup:

    return dwError;

error:

    *phLog = (HANDLE)NULL;

    if (pSyslog)
    {
        SrvSvcFreeSysLogInfo(pSyslog);
    }

    goto cleanup;
}

VOID
SrvSvcSetSyslogMask(
    SRVSVC_LOG_LEVEL logLevel
    )
{
    DWORD dwSysLogLevel;

    switch (logLevel)
    {
        case SRVSVC_LOG_LEVEL_ALWAYS:
        {
            dwSysLogLevel = LOG_UPTO(LOG_INFO);
            break;
        }
        case SRVSVC_LOG_LEVEL_ERROR:
        {
            dwSysLogLevel = LOG_UPTO(LOG_ERR);
            break;
        }

        case SRVSVC_LOG_LEVEL_WARNING:
        {
            dwSysLogLevel = LOG_UPTO(LOG_WARNING);
            break;
        }

        case SRVSVC_LOG_LEVEL_INFO:
        {
            dwSysLogLevel = LOG_UPTO(LOG_INFO);
            break;
        }

        default:
        {
            dwSysLogLevel = LOG_UPTO(LOG_INFO);
            break;
        }
    }

    setlogmask(dwSysLogLevel);
}

VOID
SrvSvcLogToSyslog(
    HANDLE      hLog,
    SRVSVC_LOG_LEVEL logLevel,
    PCSTR       pszFormat,
    va_list     msgList
    )
{
    switch (logLevel)
    {
        case SRVSVC_LOG_LEVEL_ALWAYS:
        {
            lsmb_vsyslog(LOG_INFO, pszFormat, msgList);
            break;
        }
        case SRVSVC_LOG_LEVEL_ERROR:
        {
            lsmb_vsyslog(LOG_ERR, pszFormat, msgList);
            break;
        }

        case SRVSVC_LOG_LEVEL_WARNING:
        {
            lsmb_vsyslog(LOG_WARNING, pszFormat, msgList);
            break;
        }

        case SRVSVC_LOG_LEVEL_INFO:
        {
            lsmb_vsyslog(LOG_INFO, pszFormat, msgList);
            break;
        }

        default:
        {
            lsmb_vsyslog(LOG_INFO, pszFormat, msgList);
            break;
        }
    }
}

DWORD
SrvSvcCloseSyslog(
    HANDLE hLog
    )
{
    PSRVSVC_SYS_LOG pSysLog = (PSRVSVC_SYS_LOG)hLog;

    SrvSvcResetLogging();

    if (pSysLog)
    {
        SrvSvcFreeSysLogInfo(pSysLog);
    }
    return 0;
}

VOID
SrvSvcFreeSysLogInfo(
    PSRVSVC_SYS_LOG pSysLog
    )
{
    if (pSysLog->bOpened)
    {
        /* close connection to syslog */
        closelog();
    }

    SRVSVC_SAFE_FREE_STRING(pSysLog->pszIdentifier);

    LwFreeMemory(pSysLog);
}

