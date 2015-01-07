/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright (c) Likewise Software.  All rights Reserved.
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
 * license@likewise.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *  
 * Module Name:
 *
 *     lsapstore-main.c
 *
 * Abstract:
 *
 *     LSASS Password Store API Implementation
 *
 *     Main API code
 *
 *  Authors: Danilo Almeida (dalmeida@likewise.com)
 */

#include "lsapstore-includes.h"
#include "lsapstore-backend.h"

DWORD
LsaPstoreGetPasswordInfoW(
    IN OPTIONAL PCWSTR DnsDomainName,
    OUT PLSA_MACHINE_PASSWORD_INFO_W* PasswordInfo
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;
    PWSTR defaultDnsDomainName = NULL;
    PCWSTR actualDnsDomainName = DnsDomainName;
    PLSA_MACHINE_PASSWORD_INFO_W passwordInfo = NULL;

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    if (!DnsDomainName)
    {
        // Returns NULL if no default is found.
        dwError = LsaPstoreGetDefaultDomainW(&defaultDnsDomainName);
        GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

        actualDnsDomainName = defaultDnsDomainName;
    }

    if (!actualDnsDomainName)
    {
        dwError = NERR_SetupNotJoined;
        GOTO_CLEANUP_EE(EE);
    }

    dwError = LsaPstorepBackendGetPasswordInfoW(
                    backendState,
                    actualDnsDomainName,
                    &passwordInfo);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

cleanup:
    if (dwError)
    {
        LSA_PSTORE_FREE_PASSWORD_INFO_W(&passwordInfo);
    }

    LW_RTL_FREE(&defaultDnsDomainName);

    *PasswordInfo = passwordInfo;

    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}

DWORD
LsaPstoreSetPasswordInfoW(
    IN PLSA_MACHINE_PASSWORD_INFO_W PasswordInfo
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;
    PWSTR defaultDnsDomainName = NULL;
    BOOLEAN isDefaultDomain = FALSE;

    dwError = LsaPstorepCheckPasswordInfoW(PasswordInfo);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    // Returns NULL if no default is set.
    dwError = LsaPstoreGetDefaultDomainW(&defaultDnsDomainName);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    dwError = LsaPstorepBackendSetPasswordInfoW(backendState, PasswordInfo);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    if (!defaultDnsDomainName)
    {
        // TODO-Perhaps ignore error here?
        dwError = LsaPstoreSetDefaultDomainW(PasswordInfo->Account.DnsDomainName);
        GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

        isDefaultDomain = TRUE;
    }
    else
    {
        isDefaultDomain = LwRtlWC16StringIsEqual(
                                defaultDnsDomainName,
                                PasswordInfo->Account.DnsDomainName,
                                TRUE);
    }

    if (isDefaultDomain)
    {
        dwError = LsaPstorepCallPluginSetPasswordInfo(PasswordInfo);
        GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);
    }

cleanup:
    LSA_PSTORE_FREE(&defaultDnsDomainName);

    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}

DWORD
LsaPstoreDeletePasswordInfoW(
    IN OPTIONAL PCWSTR DnsDomainName
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;
    PWSTR defaultDnsDomainName = NULL;
    BOOLEAN isDefaultDomain = FALSE;
    PLSA_MACHINE_PASSWORD_INFO_W passwordInfo = NULL;
    PCWSTR actualDnsDomainName = NULL;

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    // Returns NULL if no default is found.
    dwError = LsaPstoreGetDefaultDomainW(&defaultDnsDomainName);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    actualDnsDomainName = DnsDomainName ? DnsDomainName : defaultDnsDomainName;

    if (defaultDnsDomainName)
    {
        if (DnsDomainName)
        {
            isDefaultDomain = LwRtlWC16StringIsEqual(
                                    defaultDnsDomainName,
                                    DnsDomainName,
                                    TRUE);
        }
        else
        {
            isDefaultDomain = TRUE;
        }
    }

    if (isDefaultDomain)
    {
        // Get information for plugin, but ignore error since best effort.
        dwError = LsaPstorepBackendGetPasswordInfoW(
                        backendState,
                        defaultDnsDomainName,
                        &passwordInfo);
        dwError = 0;
    }

    if (actualDnsDomainName)
    {
        dwError = LsaPstorepBackendDeletePasswordInfoW(
                        backendState,
                        actualDnsDomainName);
        GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);
    }

    if (isDefaultDomain)
    {
        // Ignore error as LSASS can recover
        dwError = LsaPstoreSetDefaultDomainW(NULL);
        dwError = 0;

        // TODO-Use defaultDnsDomainName if no passwordInfo available.
        dwError = LsaPstorepCallPluginDeletePasswordInfo(
                        passwordInfo ? &passwordInfo->Account : NULL);
        GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);
    }

cleanup:
    LW_RTL_FREE(&defaultDnsDomainName);
    LSA_PSTORE_FREE_PASSWORD_INFO_W(&passwordInfo);

    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}

DWORD
LsaPstoreGetDefaultDomainW(
    OUT PWSTR* DnsDomainName
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;
    PWSTR dnsDomainName = NULL;

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    dwError = LsaPstorepBackendGetDefaultDomainW(
                    backendState,
                    &dnsDomainName);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

cleanup:
    if (dwError)
    {
        LSA_PSTORE_FREE(&dnsDomainName);
    }

    *DnsDomainName = dnsDomainName;

    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}

DWORD
LsaPstoreSetDefaultDomainW(
    IN OPTIONAL PCWSTR DnsDomainName
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;

    if (DnsDomainName && !LsaPstorepWC16StringIsUpcase(DnsDomainName))
    {
        dwError = ERROR_INVALID_PARAMETER;
        GOTO_CLEANUP_EE(EE);
    }

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    dwError = LsaPstorepBackendSetDefaultDomainW(backendState, DnsDomainName);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

cleanup:
    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}

DWORD
LsaPstoreGetJoinedDomainsW(
    OUT PWSTR** DnsDomainNames,
    OUT PDWORD Count
    )
{
    DWORD dwError = 0;
    int EE = 0;
    PLSA_PSTORE_BACKEND_STATE backendState = NULL;

    dwError = LsaPstorepEnsureInitialized(&backendState);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    dwError = LsaPstorepBackendGetJoinedDomainsW(
                    backendState,
                    DnsDomainNames,
                    Count);
    GOTO_CLEANUP_ON_WINERROR_EE(dwError, EE);

    // VALIDATE...

cleanup:
    LSA_PSTORE_LOG_LEAVE_ERROR_EE(dwError, EE);
    return dwError;
}
