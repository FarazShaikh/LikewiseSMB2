/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        main.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (SMBSS)
 *
 *        Service Entry API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *          Kyle Stemen (kstemen@likewisesoftware.com)
 */
#include "includes.h"
#include "ioinit.h"
#include "ioipc.h"

static
NTSTATUS
LwIoSvcmParseArgs(
    ULONG ArgCount,
    PWSTR* ppArgs,
    PSMBSERVERINFO pSMBServerInfo
    );

static
NTSTATUS
LwioSrvSetDefaults(
    PLWIO_CONFIG pConfig
    );

static
DWORD
SMBSrvInitialize(
    VOID
    );

static
DWORD
SMBInitCacheFolders(
    VOID
    );

static
DWORD
SMBSrvExecute(
    VOID
    );

#ifdef ENABLE_STATIC_DRIVERS

extern NTSTATUS IO_DRIVER_ENTRY(rdr)(IO_DRIVER_HANDLE, ULONG);
extern NTSTATUS IO_DRIVER_ENTRY(srv)(IO_DRIVER_HANDLE, ULONG);
extern NTSTATUS IO_DRIVER_ENTRY(npfs)(IO_DRIVER_HANDLE, ULONG);
extern NTSTATUS IO_DRIVER_ENTRY(pvfs)(IO_DRIVER_HANDLE, ULONG);

static IO_STATIC_DRIVER gStaticDrivers[] =
{
#ifdef ENABLE_RDR
    IO_STATIC_DRIVER_ENTRY(rdr),
#endif
#ifdef ENABLE_SRV
    IO_STATIC_DRIVER_ENTRY(srv),
#endif
#ifdef ENABLE_PVFS
    IO_STATIC_DRIVER_ENTRY(pvfs),
#endif
#ifdef ENABLE_NPFS
    IO_STATIC_DRIVER_ENTRY(npfs),
#endif
#ifdef ENABLE_DFS
    IO_STATIC_DRIVER_ENTRY(dfs),
#endif

    IO_STATIC_DRIVER_END
};

#endif

static LWMsgContext* pContext = NULL;
static LWMsgProtocol* pProtocol = NULL;
static LWMsgServer* pServer = NULL;

NTSTATUS
LwIoSvcmInit(
    PCWSTR pServiceName,
    PLW_SVCM_INSTANCE pInstance
    )
{
    return STATUS_SUCCESS;
}

VOID
LwIoSvcmDestroy(
    PLW_SVCM_INSTANCE pInstance
    )
{
    return;
}

NTSTATUS
LwIoSvcmStart(
    PLW_SVCM_INSTANCE pInstance,
    ULONG ArgCount,
    PWSTR* ppArgs,
    ULONG FdCount,
    int* pFds
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    ntStatus = LwioSrvInitializeConfig(&gLwioServerConfig);
    BAIL_ON_NT_STATUS(ntStatus);

    pthread_mutex_init(&gServerInfo.lock, NULL);
    gServerInfo.pLock = &gServerInfo.lock;

    ntStatus = LwIoSvcmParseArgs(ArgCount, ppArgs, &gServerInfo);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = LwioInitLogging_r(
        "lwio",
        gServerInfo.logTarget,
        gServerInfo.maxAllowedLogLevel,
        gServerInfo.szLogFilePath);
    BAIL_ON_NT_STATUS(ntStatus);

    LWIO_LOG_VERBOSE("Logging started");

    ntStatus = LwioSrvRefreshConfig(&gLwioServerConfig);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = LwioSrvSetDefaults(&gLwioServerConfig);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = SMBSrvInitialize();
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = SMBSrvExecute();
    BAIL_ON_NT_STATUS(ntStatus);

cleanup:

    return ntStatus;

error:

    goto cleanup;
}

NTSTATUS
LwIoSvcmStop(
    PLW_SVCM_INSTANCE pInstance
    )
{
    LWIO_LOG_VERBOSE("LWIO main cleaning up");

    IoCleanup();

    if (pServer)
    {
        LWMsgStatus status2 = lwmsg_server_stop(pServer);

        if (status2)
        {
            LWIO_LOG_ERROR("Error stopping server. [Error code:%d]", status2);
        }

        lwmsg_server_delete(pServer);
    }

    if (pContext)
    {
        lwmsg_context_delete(pContext);
    }

    LWIO_LOG_INFO("LWIO Service exiting...");

    LwioShutdownLogging_r();

    if (gServerInfo.pLock)
    {
        pthread_mutex_destroy(&gServerInfo.lock);
        gServerInfo.pLock = NULL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
LwIoSvcmRefresh(
    PLW_SVCM_INSTANCE pInstance
    )
{
    return LwioSrvRefreshConfig(&gLwioServerConfig);
}

static LW_SVCM_MODULE gService =
{
    .Size = sizeof(gService),
    .Init = LwIoSvcmInit,
    .Destroy = LwIoSvcmDestroy,
    .Start = LwIoSvcmStart,
    .Stop = LwIoSvcmStop,
    .Refresh = LwIoSvcmRefresh
};

PLW_SVCM_MODULE
(LW_RTL_SVCM_ENTRY_POINT_NAME)(
    VOID
    )
{
    return &gService;
}

static
NTSTATUS
LwioSrvSetDefaults(
    PLWIO_CONFIG pConfig
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    struct rlimit rlim = {0};
    int err = 0;
    LWIO_CONFIG defaultConfig;
    rlim_t maxOpenFileDescriptors = 0;

    gpServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_ERROR;

    *(gpServerInfo->szLogFilePath) = '\0';

    strncpy(gpServerInfo->szCachePath, CACHEDIR, PATH_MAX);
    gpServerInfo->szCachePath[PATH_MAX] = '\0';

    strncpy(gpServerInfo->szPrefixPath, PREFIXDIR, PATH_MAX);
    gpServerInfo->szPrefixPath[PATH_MAX] = '\0';

    setlocale(LC_ALL, "");

    // Enforce configuration settings

    ntStatus = LwioSrvInitializeConfig(&defaultConfig);
    BAIL_ON_NT_STATUS(ntStatus);    

    if (pConfig->MaxOpenFileDescriptors > 0)
    {
            maxOpenFileDescriptors = pConfig->MaxOpenFileDescriptors;
            LWIO_LOG_VERBOSE("Setting maxOpenFileDescriptors to %d\n",
                maxOpenFileDescriptors);
    }
    else
    {
            maxOpenFileDescriptors = RLIM_INFINITY;
            LWIO_LOG_VERBOSE("Setting maxOpenFileDescriptors to unlimited\n");
    }

    rlim.rlim_cur = maxOpenFileDescriptors;
    rlim.rlim_max = maxOpenFileDescriptors;

    if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
    {
        err = errno;

        ntStatus = LwErrnoToNtStatus(err);

        LWIO_LOG_ERROR(
            "Failed to set maximum file descriptors to %d - %s (0x%x).  "
            "Using default (%d)\n",
            pConfig->MaxOpenFileDescriptors,
            LwNtStatusToDescription(ntStatus),
            ntStatus,
            defaultConfig.MaxOpenFileDescriptors);

        rlim.rlim_cur = defaultConfig.MaxOpenFileDescriptors;
        rlim.rlim_max = defaultConfig.MaxOpenFileDescriptors;

        if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
        {
            err = errno;
            ntStatus = LwErrnoToNtStatus(err);

            LWIO_LOG_ERROR(
                "Failed to set default file descriptor limit (%d - %s) (0x%x).\n",
                defaultConfig.MaxOpenFileDescriptors,
                LwNtStatusToDescription(ntStatus),
                ntStatus);
        }
    }

    ntStatus = STATUS_SUCCESS;

cleanup:

    LwioSrvFreeConfigContents(&defaultConfig);

    return ntStatus;

error:

    goto cleanup;    
}

static
NTSTATUS
LwIoSvcmParseArgs(
    ULONG ArgCount,
    PWSTR* ppArgs,
    PSMBSERVERINFO pSMBServerInfo
    )
{
    typedef enum {
        PARSE_MODE_OPEN = 0,
        PARSE_MODE_LOGFILE,
        PARSE_MODE_LOGLEVEL
    } ParseMode;
    NTSTATUS status = STATUS_SUCCESS;
    ParseMode parseMode = PARSE_MODE_OPEN;
    int iArg = 1;
    PWSTR pArg = NULL;
    BOOLEAN bLogTargetSet = FALSE;
    PSTR pConverted = NULL;
    static const WCHAR paramLogFile[] = {'-','-','l','o','g','f','i','l','e','\0'};
    static const WCHAR paramLogLevel[] = {'-','-','l','o','g','l','e','v','e','l','\0'};
    static const WCHAR paramSyslog[] = {'-','-','s','y','s','l','o','g','\0'};
    static const WCHAR paramError[] = {'e','r','r','o','r','\0'};
    static const WCHAR paramWarning[] = {'w','a','r','n','i','n','g','\0'};
    static const WCHAR paramInfo[] = {'i','n','f','o','\0'};
    static const WCHAR paramVerbose[] = {'v','e','r','b','o','s','e','\0'};
    static const WCHAR paramDebug[] = {'d','e','b','u','g','\0'};
    static const WCHAR paramTrace[] = {'t','r','a','c','e','\0'};

    do
    {
        pArg = ppArgs[iArg++];
        if (pArg == NULL || *pArg == '\0')
        {
            break;
        }

        switch(parseMode)
        {
        case PARSE_MODE_OPEN:
            if (LwRtlWC16StringIsEqual(pArg, paramLogFile, TRUE))
            {
                parseMode = PARSE_MODE_LOGFILE;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramLogLevel, TRUE))
            {
                parseMode = PARSE_MODE_LOGLEVEL;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramSyslog, TRUE))
            {
                pSMBServerInfo->logTarget = LWIO_LOG_TARGET_SYSLOG;
                bLogTargetSet = TRUE;
            }
            else
            {
                status = STATUS_INVALID_PARAMETER;
                BAIL_ON_NT_STATUS(status);
            }
            break;

        case PARSE_MODE_LOGFILE:

        {
            status = LwRtlCStringAllocateFromWC16String(&pConverted, pArg);
            BAIL_ON_NT_STATUS(status);

            strncpy(pSMBServerInfo->szLogFilePath, pConverted, PATH_MAX);
            pSMBServerInfo->szLogFilePath[PATH_MAX] = '\0';

            SMBStripWhitespace(pSMBServerInfo->szLogFilePath, TRUE, TRUE);

            if (!strcmp(pSMBServerInfo->szLogFilePath, "."))
            {
                pSMBServerInfo->logTarget = LWIO_LOG_TARGET_CONSOLE;
            }
            else
            {
                pSMBServerInfo->logTarget = LWIO_LOG_TARGET_FILE;
            }

            bLogTargetSet = TRUE;

            parseMode = PARSE_MODE_OPEN;

            break;
        }

        case PARSE_MODE_LOGLEVEL:
            if (LwRtlWC16StringIsEqual(pArg, paramError, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_ERROR;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramWarning, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_WARNING;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramInfo, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_INFO;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramVerbose, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_VERBOSE;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramDebug, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_DEBUG;
            }
            else if (LwRtlWC16StringIsEqual(pArg, paramTrace, TRUE))
            {
                pSMBServerInfo->maxAllowedLogLevel = LWIO_LOG_LEVEL_TRACE;
            }
            else
            {
                status = STATUS_INVALID_PARAMETER;
                BAIL_ON_NT_STATUS(status);
            }

            parseMode = PARSE_MODE_OPEN;
            break;
        }

    } while (iArg < ArgCount);

    if (!bLogTargetSet)
    {
        pSMBServerInfo->logTarget = LWIO_LOG_TARGET_CONSOLE;
    }

error:

    RTL_FREE(&pConverted);

    return status;
}

static
DWORD
SMBSrvInitialize(
    VOID
    )
{
    DWORD dwError = 0;

    dwError = SMBInitCacheFolders();
    BAIL_ON_LWIO_ERROR(dwError);

#ifdef ENABLE_STATIC_DRIVERS
    dwError = IoInitialize(gStaticDrivers);
#else
    dwError = IoInitialize(NULL);
#endif
    BAIL_ON_LWIO_ERROR(dwError);

error:

    return dwError;
}

static
DWORD
SMBInitCacheFolders(
    VOID
    )
{
    DWORD dwError = 0;
    BOOLEAN bExists = FALSE;

    dwError = SMBCheckDirectoryExists(
                        CACHEDIR,
                        &bExists);
    BAIL_ON_LWIO_ERROR(dwError);

    if (!bExists) {
        mode_t cacheDirMode = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;

        dwError = SMBCreateDirectory(CACHEDIR, cacheDirMode);
        BAIL_ON_LWIO_ERROR(dwError);
    }

error:

    return dwError;
}

LWMsgBool
LwIoDaemonLogIpc (
    LWMsgLogLevel level,
    const char* pszMessage,
    const char* pszFunction,
    const char* pszFilename,
    unsigned int line,
    void* pData
    )
{
    LWIO_LOG_LEVEL ioLevel = LWIO_LOG_LEVEL_DEBUG;
    LWMsgBool result = LWMSG_FALSE;

    switch (level)
    {
    case LWMSG_LOGLEVEL_ALWAYS:
        ioLevel = LWIO_LOG_LEVEL_ALWAYS;
        break;
    case LWMSG_LOGLEVEL_ERROR:
        ioLevel = LWIO_LOG_LEVEL_ERROR;
        break;
    case LWMSG_LOGLEVEL_WARNING:
        ioLevel = LWIO_LOG_LEVEL_WARNING;
        break;
    case LWMSG_LOGLEVEL_INFO:
        ioLevel = LWIO_LOG_LEVEL_INFO;
        break;
    case LWMSG_LOGLEVEL_VERBOSE:
        ioLevel = LWIO_LOG_LEVEL_VERBOSE;
        break;
    case LWMSG_LOGLEVEL_DEBUG:
        ioLevel = LWIO_LOG_LEVEL_DEBUG;
        break;
    case LWMSG_LOGLEVEL_TRACE:
        ioLevel = LWIO_LOG_LEVEL_TRACE;
        break;
    }
    
    if (pszMessage)
    {
        if (gLwioMaxLogLevel >= ioLevel)
        {
            LWIO_LOCK_LOGGER;
            if (gLwioMaxLogLevel >= ioLevel)
            {
                LwioLogMessage(gpfnLwioLogger, ghLwioLog, ioLevel, "[IPC] %s", pszMessage);
                result = LWMSG_TRUE;
            }
            LWIO_UNLOCK_LOGGER;
        }
        else
        {
            result = LWMSG_FALSE;
        }
    }
    else
    {
        result = (gLwioMaxLogLevel >= ioLevel);
    }

    return result;
}

static
DWORD
SMBSrvExecute(
    VOID
    )
{
    DWORD dwError = 0;
    LWMsgTime timeout = { 30, 0 }; /* 30 seconds */
    PCSTR pszSmNotify = NULL;
    int notifyFd = -1;
    char notifyCode = 0;
    int ret = 0;

    dwError = MAP_LWMSG_STATUS(lwmsg_context_new(NULL, &pContext));
    BAIL_ON_LWIO_ERROR(dwError);

    lwmsg_context_set_log_function(pContext, LwIoDaemonLogIpc, NULL);

    dwError = MAP_LWMSG_STATUS(lwmsg_protocol_new(pContext, &pProtocol));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = LwIoDaemonIpcAddProtocolSpec(pProtocol);
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = IoIpcAddProtocolSpec(pProtocol);
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_new(pContext, pProtocol, &pServer));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = LwIoDaemonIpcAddDispatch(pServer);
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = IoIpcAddDispatch(pServer);
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_set_endpoint(
                    pServer,
                    LWMSG_SERVER_MODE_LOCAL,
                    LWIO_SERVER_FILENAME,
                    (S_IRWXU | S_IRWXG | S_IRWXO)));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_set_max_clients(
                    pServer,
                    512));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_set_max_dispatch(
                    pServer,
                    10));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_set_timeout(
                    pServer,
                    LWMSG_TIMEOUT_IDLE,
                    &timeout));
    BAIL_ON_LWIO_ERROR(dwError);

    dwError = MAP_LWMSG_STATUS(lwmsg_server_start(pServer));
    BAIL_ON_LWIO_ERROR(dwError);

    if ((pszSmNotify = getenv("LIKEWISE_SM_NOTIFY")) != NULL)
    {
        notifyFd = atoi(pszSmNotify);
        
        do
        {
            ret = write(notifyFd, &notifyCode, sizeof(notifyCode));
        } while(ret != sizeof(notifyCode) && errno == EINTR);

        if (ret < 0)
        {
            LWIO_LOG_ERROR("Could not notify service manager: %s (%i)", strerror(errno), errno);
            dwError = LwMapErrnoToLwError(errno);
            BAIL_ON_LWIO_ERROR(dwError);
        }

        close(notifyFd);
    }

cleanup:

    return dwError;

error:

    LWIO_LOG_ERROR("SMB Server stopping due to error [code: %d]", dwError);

    if (pServer)
    {
        LWMsgStatus status2 = lwmsg_server_stop(pServer);

        if (status2)
        {
            LWIO_LOG_ERROR("Error stopping server. [Error code:%d]", status2);
        }

        lwmsg_server_delete(pServer);
    }

    if (pContext)
    {
        lwmsg_context_delete(pContext);
    }

    goto cleanup;
}

/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
