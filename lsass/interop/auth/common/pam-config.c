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
 *        pam-config.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Pluggable Authentication Module
 *
 *        Configuration API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "includes.h"

#define LSA_IGNORE_LIST_UPDATE_INTERVAL (5*60)
#define LSA_USER_IGNORE_LIST_PATH CONFIGDIR "/user-ignore"
#define LSA_GROUP_IGNORE_LIST_PATH CONFIGDIR "/group-ignore"

static
DWORD
LsaPamGetConfigFromServer(
    OUT PLSA_PAM_CONFIG *ppConfig
    );

DWORD
LsaPamGetConfig(
    OUT PLSA_PAM_CONFIG* ppConfig
    )
{
    DWORD dwError = 0;
    PLSA_PAM_CONFIG pConfig = NULL;

    dwError = LsaPamGetConfigFromServer(&pConfig);
    if (dwError)
    {
        dwError = LsaUtilAllocatePamConfig(&pConfig);
        BAIL_ON_LSA_ERROR(dwError);
    }

cleanup:
    *ppConfig = pConfig;

    return dwError;

error:
    if ( pConfig )
    {
        LsaPamFreeConfig(pConfig);
        pConfig = NULL;
    }

    goto cleanup;
}

VOID
LsaPamFreeConfig(
    IN PLSA_PAM_CONFIG pConfig
    )
{
    LsaUtilFreePamConfig(pConfig);
}

static
DWORD
LsaPamGetConfigFromServer(
    OUT PLSA_PAM_CONFIG *ppConfig
    )
{
    DWORD dwError = 0;
    PLSA_PAM_CONFIG pConfig = NULL;
    HANDLE hLsaConnection = NULL;

    dwError = LsaOpenServer(&hLsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaGetPamConfig(hLsaConnection, &pConfig);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:
    if (hLsaConnection != NULL)
    {
        LsaCloseServer(hLsaConnection);
        hLsaConnection = NULL;
    }

    *ppConfig = pConfig;

    return dwError;

error:
    if (pConfig)
    {
        LsaPamFreeConfig(pConfig);
        pConfig = NULL;
    }

    goto cleanup;
}

DWORD
LsaReadIgnoreList(
    PCSTR pListPath,
    PSTR* ppIgnoreList
    )
{
    PSTR pIgnoreList = NULL;
    DWORD dwError = 0;
    struct stat fileStat = {0};
    int iListFd = -1;
    size_t sOffset = 0;
    ssize_t ssRead = 0;

    if (stat(pListPath, &fileStat) < 0)
    {
        dwError = LwMapErrnoToLwError(errno);
        if (dwError == LwMapErrnoToLwError(ENOENT) ||
            dwError == LwMapErrnoToLwError(ENOTDIR))
        {
            dwError = LwAllocateString(
                            "",
                            &pIgnoreList);
            BAIL_ON_LSA_ERROR(dwError);
        }
        BAIL_ON_LSA_ERROR(dwError);
    }
    else
    {
        if ((iListFd = open(pListPath, O_RDONLY, 0)) < 0)
        {
            dwError = LwMapErrnoToLwError(errno);
            BAIL_ON_LSA_ERROR(dwError);
        }
        dwError = LwAllocateMemory(
                        fileStat.st_size + 1,
                        (PVOID*)&pIgnoreList);
        BAIL_ON_LSA_ERROR(dwError);

        sOffset = 0;
        while (sOffset < fileStat.st_size)
        {
            ssRead = read(
                        iListFd,
                        pIgnoreList,
                        fileStat.st_size - sOffset);
            if (ssRead < 0)
            {
                dwError = LwMapErrnoToLwError(errno);
                if (dwError == LwMapErrnoToLwError(EINTR))
                {
                    dwError = 0;
                    ssRead = 0;
                }
                BAIL_ON_LSA_ERROR(dwError);
            }
            sOffset += ssRead;
        }
        pIgnoreList[sOffset] = 0;
    }

    *ppIgnoreList = pIgnoreList;

cleanup:
    if (dwError)
    {
        *ppIgnoreList = NULL;
        LW_SAFE_FREE_MEMORY(pIgnoreList);
    }
    if (iListFd != -1)
    {
        close(iListFd);
    }
    return dwError;

error:
    goto cleanup;
}

DWORD
LsaParseIgnoreList(
    PSTR pIgnoreList,
    DWORD (*pReadExternalList)(PCSTR, PLW_HASH_TABLE),
    PLW_HASH_TABLE* ppIgnoreHash
    )
{
    DWORD dwError = 0;
    PSTR pSavePtr = NULL;
    PSTR pToken = strtok_r(pIgnoreList, "\r\n", &pSavePtr);
    PSTR pTokenCopy = NULL;
    PLW_HASH_TABLE pIgnoreHash = NULL;

    dwError = LwHashCreate(
                    10,
                    LwHashStringCompare,
                    LwHashStringHash,
                    LwHashFreeStringKey,
                    NULL,
                    &pIgnoreHash);
    BAIL_ON_LSA_ERROR(dwError);

    while (pToken)
    {
        if (pToken[0] == '+')
        {
            if (pToken[1] == 0)
            {
                dwError = pReadExternalList(
                                NULL,
                                pIgnoreHash);
            }
            else
            {
                dwError = pReadExternalList(
                                pToken + 1,
                                pIgnoreHash);
            }
            BAIL_ON_LSA_ERROR(dwError);
        }
        else
        {
            dwError = LwAllocateString(
                            pToken,
                            &pTokenCopy);
            BAIL_ON_LSA_ERROR(dwError);
            dwError = LwHashSetValue(
                            pIgnoreHash,
                            pTokenCopy,
                            pTokenCopy);
            BAIL_ON_LSA_ERROR(dwError);
        }

        pToken = strtok_r(NULL, "\r\n", &pSavePtr);
    }


cleanup:
    if (dwError)
    {
        LwHashSafeFree(&pIgnoreHash);
    }
    *ppIgnoreHash = pIgnoreHash;
    return dwError;

error:
    goto cleanup;
}

DWORD
LsaReadSystemGroupList(
    IN PCSTR pFileName,
    IN OUT PLW_HASH_TABLE pGroupIgnoreHash
    )
{
#if !HAVE_DECL_FGETGRENT_R
    return 0;
#else
    DWORD dwError = 0;
    PSTR pBuffer = NULL;
    size_t bufferLen = 100;
    struct group groupBuffer = { 0 };
    struct group* pGroup = NULL;
    FILE* pLocalFile = NULL;
    PSTR pNameCopy = NULL;

    pLocalFile = fopen(pFileName? pFileName : "/etc/group", "r");
    // Ignore if the file cannot be opened
    if (pLocalFile)
    {
        while(1)
        {
#if FGETPWENT_R_TAKES_5_ARGS
            dwError = fgetgrent_r(
                            pLocalFile,
                            &groupBuffer,
                            pBuffer,
                            bufferLen,
                            &pGroup);
#elif FGETPWENT_R_RETURNS_INT
            dwError = fgetgrent_r(
                            pLocalFile,
                            &groupBuffer,
                            pBuffer,
                            bufferLen);
            if (!dwError)
            {
                pGroup = &groupBuffer;
            }
#else
            // Solaris man page recommends setting errno to 0 before
            // calling the function.
            errno = 0;
            pGroup = fgetgrent_r(
                            pLocalFile,
                            &groupBuffer,
                            pBuffer,
                            bufferLen);
            if (!pGroup)
            {
                dwError = errno;
            }
#endif
            if (!dwError)
            {
                dwError = LwAllocateString(
                                pGroup->gr_name,
                                &pNameCopy);
                BAIL_ON_LSA_ERROR(dwError);

                dwError = LwHashSetValue(
                                pGroupIgnoreHash,
                                pNameCopy,
                                pNameCopy);
                BAIL_ON_LSA_ERROR(dwError);

                pNameCopy = NULL;
            }
            else if (dwError == ERANGE)
            {
                LW_SAFE_FREE_MEMORY(pBuffer);
                bufferLen *= 2;
                dwError = LwAllocateMemory(
                                bufferLen,
                                (PVOID*)&pBuffer);
                BAIL_ON_LSA_ERROR(dwError);
            }
            else if (dwError == ENOENT)
            {
                dwError = 0;
                break;
            }
            else
            {
                dwError = LwMapErrnoToLwError(dwError);
                BAIL_ON_LSA_ERROR(dwError);
            }
        }
    }

cleanup:
    if (pLocalFile)
    {
        fclose(pLocalFile);
    }
    LW_SAFE_FREE_STRING(pBuffer);
    LW_SAFE_FREE_STRING(pNameCopy);
    return dwError;

error:
    goto cleanup;
#endif /* HAVE_FGETGRENT_R */
}

DWORD
LsaReadSystemUserList(
    IN PCSTR pFileName,
    IN OUT PLW_HASH_TABLE pUserIgnoreHash
    )
{
#if !HAVE_DECL_FGETPWENT_R
    return 0;
#else
    DWORD dwError = 0;
    PSTR pBuffer = NULL;
    size_t bufferLen = 100;
    struct passwd accountBuffer = { 0 };
    struct passwd* pAccount = NULL;
    FILE* pLocalFile = NULL;
    PSTR pNameCopy = NULL;

    dwError = LwAllocateMemory(
                    bufferLen,
                    (PVOID*)&pBuffer);
    BAIL_ON_LSA_ERROR(dwError);

    pLocalFile = fopen(pFileName? pFileName : "/etc/passwd", "r");
    // Ignore if the file cannot be opened
    if (pLocalFile)
    {
        while(1)
        {
#if FGETPWENT_R_TAKES_5_ARGS
            dwError = fgetpwent_r(
                            pLocalFile,
                            &accountBuffer,
                            pBuffer,
                            bufferLen,
                            &pAccount);
#elif FGETPWENT_R_RETURNS_INT
            dwError = fgetpwent_r(
                            pLocalFile,
                            &accountBuffer,
                            pBuffer,
                            bufferLen);
            if (!dwError)
            {
                pAccount = &accountBuffer;
            }
#else
            // Solaris man page recommends setting errno to 0 before
            // calling the function.
            errno = 0;
            pAccount = fgetpwent_r(
                            pLocalFile,
                            &accountBuffer,
                            pBuffer,
                            bufferLen);
            if (!pAccount)
            {
                dwError = errno;
            }
#endif
            if (!dwError)
            {
                dwError = LwAllocateString(
                                pAccount->pw_name,
                                &pNameCopy);
                BAIL_ON_LSA_ERROR(dwError);

                dwError = LwHashSetValue(
                                pUserIgnoreHash,
                                pNameCopy,
                                pNameCopy);
                BAIL_ON_LSA_ERROR(dwError);

                pNameCopy = NULL;
            }
            else if (dwError == ERANGE)
            {
                LW_SAFE_FREE_MEMORY(pBuffer);
                bufferLen *= 2;
                dwError = LwAllocateMemory(
                                bufferLen,
                                (PVOID*)&pBuffer);
                BAIL_ON_LSA_ERROR(dwError);
            }
            else if (dwError == ENOENT)
            {
                dwError = 0;
                break;
            }
            else
            {
                dwError = LwMapErrnoToLwError(dwError);
                BAIL_ON_LSA_ERROR(dwError);
            }
        }
        fclose(pLocalFile);
        pLocalFile = NULL;
    }

cleanup:
    if (pLocalFile)
    {
        fclose(pLocalFile);
    }
    LW_SAFE_FREE_STRING(pBuffer);
    LW_SAFE_FREE_STRING(pNameCopy);
    return dwError;

error:
    goto cleanup;
#endif /* HAVE_FGETPWENT_R */
}

DWORD
LsaReadIgnoreHashes()
{
    DWORD dwError = 0;
    time_t tCurrentTime = 0;
    PSTR pIgnoreList = NULL;
    PLW_HASH_TABLE pUserIgnoreHash = NULL;
    PLW_HASH_TABLE pGroupIgnoreHash = NULL;

    if (time(&tCurrentTime) == (time_t)-1)
    {
        dwError = LwMapErrnoToLwError(errno);
        BAIL_ON_LSA_ERROR(dwError);
    }
    
    if (tCurrentTime < gtIgnoreHashLastUpdated +
            LSA_IGNORE_LIST_UPDATE_INTERVAL)
    {
        goto cleanup;
    }

    dwError = LsaReadIgnoreList(
                    LSA_USER_IGNORE_LIST_PATH,
                    &pIgnoreList);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaParseIgnoreList(
                    pIgnoreList,
                    &LsaReadSystemUserList,
                    &pUserIgnoreHash);
    BAIL_ON_LSA_ERROR(dwError);

    LW_SAFE_FREE_MEMORY(pIgnoreList);

    dwError = LsaReadIgnoreList(
                    LSA_GROUP_IGNORE_LIST_PATH,
                    &pIgnoreList);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaParseIgnoreList(
                    pIgnoreList,
                    &LsaReadSystemGroupList,
                    &pGroupIgnoreHash);
    BAIL_ON_LSA_ERROR(dwError);

    LwHashSafeFree(&gpUserIgnoreHash);
    gpUserIgnoreHash = pUserIgnoreHash;
    LwHashSafeFree(&gpGroupIgnoreHash);
    gpGroupIgnoreHash = pGroupIgnoreHash;

    gtIgnoreHashLastUpdated = tCurrentTime;

cleanup:
    LW_SAFE_FREE_STRING(pIgnoreList);
    return dwError;

error:
    LwHashSafeFree(&pUserIgnoreHash);
    LwHashSafeFree(&pGroupIgnoreHash);
    goto cleanup;
}

BOOLEAN
LsaShouldIgnoreGroup(
    PCSTR pszName
    )
{
    // Ignore errors
    LsaReadIgnoreHashes();

    if (gpGroupIgnoreHash)
    {
        return LwHashExists(gpGroupIgnoreHash, pszName);
    }
    return FALSE;
}

BOOLEAN
LsaShouldIgnoreUser(
    PCSTR pszName
    )
{
    // Ignore errors
    LsaReadIgnoreHashes();

    if (gpUserIgnoreHash)
    {
        return (LwHashExists(gpUserIgnoreHash, pszName));
    }
    return FALSE;
}

VOID
LsaFreeIgnoreHashes(VOID)
{
    LwHashSafeFree(&gpUserIgnoreHash);
    LwHashSafeFree(&gpGroupIgnoreHash);
}
