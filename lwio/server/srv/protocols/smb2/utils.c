/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

/*
 * Copyright Likewise Software
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
 *        utils.c
 *
 * Abstract:
 *
 *        Likewise IO (LWIO) - SRV
 *
 *        Protocols API - SMBV1
 *
 *        Utilities
 *
 * Authors: Sriram Nambakam (snambakam@likewise.com)
 *
 */

#include "includes.h"

NTSTATUS
SrvBuildTreeRelativePath_SMB_V2(
    PLWIO_SRV_TREE_2 pTree,
    PWSTR            pwszFilename,
    PIO_FILE_NAME    pFilename
    )
{
    NTSTATUS ntStatus     = STATUS_SUCCESS;
    BOOLEAN  bInLock      = FALSE;
    PWSTR    pwszFilePath = NULL;

    if (SrvTree2IsNamedPipe(pTree))
    {
        LWIO_LOCK_RWMUTEX_SHARED(bInLock, &pTree->pShareInfo->mutex);

        ntStatus = SrvBuildFilePath(
                        pTree->pShareInfo->pwszPath,
                        pwszFilename,
                        &pwszFilePath);
        BAIL_ON_NT_STATUS(ntStatus);
    }
    else
    {
        wchar16_t wszBackslash[] = {'\\', 0};

        if (!IsNullOrEmptyString(pwszFilename) &&
            SMBWc16sCmp(pwszFilename, &wszBackslash[0]))
        {
            ntStatus = SrvBuildFilePath(
                            NULL,
                            pwszFilename,
                            &pwszFilePath);
            BAIL_ON_NT_STATUS(ntStatus);
        }

        LWIO_LOCK_RWMUTEX_SHARED(bInLock, &pTree->pShareInfo->mutex);

        pFilename->RootFileHandle = pTree->hFile;
    }

    pFilename->FileName = pwszFilePath;

cleanup:

    LWIO_UNLOCK_RWMUTEX(bInLock, &pTree->pShareInfo->mutex);

    return ntStatus;

error:

    goto cleanup;
}

NTSTATUS
SrvSetStatSession2Info(
    PSRV_EXEC_CONTEXT   pExecContext,
    PLWIO_SRV_SESSION_2 pSession
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (pExecContext->pStatInfo)
    {
        SRV_STAT_SESSION_INFO statSessionInfo =
        {
                .pwszUserPrincipal = pSession->pwszClientPrincipalName,
                .ulUid             = UINT32_MAX,
                .ulGid             = UINT32_MAX,
                .ullSessionId      = pSession->ullUid
        };

        if (pSession->pIoSecurityContext)
        {
            NTSTATUS ntStatus2 = STATUS_SUCCESS;
            TOKEN_UNIX tokenUnix = { 0 };

            ntStatus2 = RtlQueryAccessTokenUnixInformation(
                            IoSecurityGetAccessToken(pSession->pIoSecurityContext),
                            &tokenUnix);
            if (ntStatus2 == STATUS_SUCCESS)
            {
                statSessionInfo.ulUid = tokenUnix.Uid;
                statSessionInfo.ulGid = tokenUnix.Gid;
            }
        }

        ntStatus = SrvStatisticsSetSessionInfo(
                        pExecContext->pStatInfo,
                        &statSessionInfo);
        BAIL_ON_NT_STATUS(ntStatus);
    }

error:

    return ntStatus;

}
