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
 *        srvsvc_netservergetinfo.c
 *
 * Abstract:
 *
 *        Likewise Server Service (srvsvc) RPC client and server
 *
 *        SrvSvcNetServerGetInfo server API
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


NET_API_STATUS
SrvSvcNetrServerGetInfo(
    /* [in] */ handle_t b,
    /* [in] */ wchar16_t *server_name,
    /* [in] */ UINT32 level,
    /* [out] */ srvsvc_NetSrvInfo *info
    )
{
    const DWORD dwPolicyAccessMask = LSA_ACCESS_LOOKUP_NAMES_SIDS |
                                     LSA_ACCESS_VIEW_POLICY_INFO;

    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    unsigned32 rpcStatus = RPC_S_OK;
    PWSTR pwszProtSeq = NULL;
    PSTR pszLsaLpcSocketPath = NULL;
    PWSTR pwszLsaLpcSocketPath = NULL;
    LSA_BINDING hLsaBinding = NULL;
    CHAR szHostname[64] = {0};
    PWSTR pwszLocalHost = NULL;
    POLICY_HANDLE hLocalPolicy = NULL;
    LsaPolicyInformation *pPolInfo = NULL;
    PCSTR pszComment = "Likewise CIFS";
    PWSTR pwszHostname = NULL;
    PWSTR pwszComment = NULL;
    SERVER_INFO_101 *pInfo101 = NULL;
    SERVER_INFO_102 *pInfo102 = NULL;
    BOOLEAN bStandalone = FALSE;

    dwError = LwMbsToWc16s("ncalrpc", &pwszProtSeq);
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = SrvSvcConfigGetLsaLpcSocketPath(&pszLsaLpcSocketPath);
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = LwMbsToWc16s(
                  pszLsaLpcSocketPath,
                  &pwszLsaLpcSocketPath);
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = LwNtStatusToWin32Error(
                  LsaInitBindingFull(
                      &hLsaBinding,
                      pwszProtSeq,
                      NULL,
                      pwszLsaLpcSocketPath,
                      NULL,
                      NULL,
                      NULL));
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = gethostname(szHostname, sizeof(szHostname));
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = LwMbsToWc16s(szHostname, &pwszLocalHost);
    BAIL_ON_SRVSVC_ERROR(dwError);

    dwError = LwNtStatusToWin32Error(
                  LsaOpenPolicy2(
                      hLsaBinding,
                      pwszLocalHost,
                      NULL,
                      dwPolicyAccessMask,
                      &hLocalPolicy));
    BAIL_ON_SRVSVC_ERROR(dwError);

    // Query for the DNS name information.  Fallback to the
    // account domain in case we are not joined to any domain

    ntStatus = LsaQueryInfoPolicy(
                   hLsaBinding,
                   hLocalPolicy,
                   LSA_POLICY_INFO_DNS,
                   &pPolInfo);
    if (ntStatus == STATUS_INVALID_INFO_CLASS)
    {
        bStandalone = TRUE;

        ntStatus = LsaQueryInfoPolicy(
                       hLsaBinding,
                       hLocalPolicy,
                       LSA_POLICY_INFO_ACCOUNT_DOMAIN,
                       &pPolInfo);
    }
    dwError = LwNtStatusToWin32Error(ntStatus);
    BAIL_ON_SRVSVC_ERROR(dwError);

    switch (level)
    {
    case 101:
        dwError = SrvSvcSrvAllocateMemory(
                      sizeof(*pInfo101),
                      OUT_PPVOID(&pInfo101));
        BAIL_ON_SRVSVC_ERROR(dwError);

        if (!bStandalone)
        {
            dwError = SrvSvcSrvAllocateWC16StringFromUnicodeString(
                          &pInfo101->sv101_name,
                          &pPolInfo->dns.name);
        }
        else
        {
            dwError = SrvSvcSrvAllocateWC16StringFromUnicodeString(
                          &pInfo101->sv101_name,
                          &pPolInfo->account_domain.name);
        }
        BAIL_ON_SRVSVC_ERROR(dwError);

        dwError = SrvSvcSrvAllocateWC16StringFromCString(
                      &pInfo101->sv101_comment,
                      pszComment);
        BAIL_ON_SRVSVC_ERROR(dwError);

        pInfo101->sv101_platform_id    = 500;
        pInfo101->sv101_version_major  = 5;
        pInfo101->sv101_version_minor  = 1;
        pInfo101->sv101_type           = 0x0001003;

        info->info101 = pInfo101;

        break;

    case 102:
        dwError = SrvSvcSrvAllocateMemory(
                      sizeof(*pInfo102),
                      OUT_PPVOID(&pInfo102));
        BAIL_ON_SRVSVC_ERROR(dwError);

        if (!bStandalone)
        {
            dwError = SrvSvcSrvAllocateWC16StringFromUnicodeString(
                          &pInfo102->sv102_name,
                          &pPolInfo->dns.name);
        }
        else
        {
            dwError = SrvSvcSrvAllocateWC16StringFromUnicodeString(
                          &pInfo102->sv102_name,
                          &pPolInfo->account_domain.name);
        }

        BAIL_ON_SRVSVC_ERROR(dwError);

        dwError = SrvSvcSrvAllocateWC16StringFromCString(
                      &pInfo102->sv102_comment,
                      pszComment);
        BAIL_ON_SRVSVC_ERROR(dwError);

        pInfo102->sv102_platform_id    = 500;
        pInfo102->sv102_version_major  = 5;
        pInfo102->sv102_version_minor  = 1;
        pInfo102->sv102_type           = 0x0001003;
        pInfo102->sv102_users          = 0;
        pInfo102->sv102_disc           = 0;
        pInfo102->sv102_hidden         = 0;
        pInfo102->sv102_announce       = 0;
        pInfo102->sv102_anndelta       = 0;
        pInfo102->sv102_licenses       = 5;
        pInfo102->sv102_userpath       = NULL;

        info->info102 = pInfo102;

        break;

    default:
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_SRVSVC_ERROR(dwError);
    }

cleanup:
    if (hLsaBinding && hLocalPolicy)
    {
        LsaClose(hLsaBinding, hLocalPolicy);
    }

    if (pPolInfo)
    {
        LsaRpcFreeMemory(pPolInfo);
    }

    LsaFreeBinding(&hLsaBinding);

    LW_SAFE_FREE_MEMORY(pszLsaLpcSocketPath);
    LW_SAFE_FREE_MEMORY(pwszLsaLpcSocketPath);
    LW_SAFE_FREE_MEMORY(pwszProtSeq);
    LW_SAFE_FREE_MEMORY(pwszLocalHost);

    return dwError;

error:

    switch (level)
    {
    case 101:
        SrvSvcSrvFreeServerInfo101(pInfo101);
        break;

    case 102:
        SrvSvcSrvFreeServerInfo102(pInfo102);
        break;
    }

    memset(info, 0, sizeof(*info));

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
