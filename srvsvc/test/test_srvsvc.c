/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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

#include "includes.h"


static const char *Win32ErrorToSymbolicName(NET_API_STATUS err)
{
    static char buf[64];

    switch (err) {
    case ERROR_SUCCESS:
        return "ERROR_SUCCESS";
    case ERROR_FILE_NOT_FOUND:
        return "ERROR_FILE_NOT_FOUND";
    case ERROR_NOT_ENOUGH_MEMORY:
        return "ERROR_NOT_ENOUGH_MEMORY";
    case ERROR_BAD_NET_RESP:
        return "ERROR_BAD_NET_RESP";
    case ERROR_INVALID_PARAMETER:
        return "ERROR_INVALID_PARAMETER";
    case ERROR_INVALID_LEVEL:
        return "ERROR_INVALID_LEVEL";
    case ERROR_INTERNAL_ERROR:
        return "ERROR_INTERNAL_ERROR";
    case ERROR_BAD_DESCRIPTOR_FORMAT:
        return "ERROR_BAD_DESCRIPTOR_FORMAT";
    case ERROR_INVALID_SUB_AUTHORITY:
        return "ERROR_INVALID_SUB_AUTHORITY";
    case ERROR_INVALID_ACL:
        return "ERROR_INVALID_ACL";
    case ERROR_INVALID_SID:
        return "ERROR_INVALID_SID";
    case ERROR_INVALID_SECURITY_DESCR:
        return "ERROR_INVALID_SECURITY_DESCR";
    case NERR_DuplicateShare:
        return "NERR_DuplicateShare";
    case NERR_BufTooSmall:
        return "NERR_BufTooSmall";
    case NERR_NetNameNotFound:
        return "NERR_NetNameNotFound";
    case NERR_FileIdNotFound:
        return "NERR_FileIdNotFound";
    }

    snprintf(buf, sizeof(buf)-1,
             "Win32Error[0x%08X/%u]",
             err, err);

    return buf;
}

static
NET_API_STATUS
CleanupShare(
    PCWSTR pwszHostname,
    PCWSTR pwszSharename
    )
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;

    err = SrvSvcCreateContext(pwszHostname, &pContext);
    if (err) goto error;

    err = NetrShareDel(pContext,
                       pwszHostname,
                       pwszSharename,
                       0);
    if (err == NERR_NetNameNotFound)
    {
        err = ERROR_SUCCESS;
    }
    if (err) goto error;

cleanup:

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    return err;

error:

    goto cleanup;
}


int TestNetConnectionEnum(struct test *t, const wchar16_t *hostname,
                      const wchar16_t *user, const wchar16_t *pass,
                      struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 entriesread = 0;
    UINT32 totalentries = 0;
    UINT32 resume_handle = 0;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err)
    {
        goto done;
    }

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					0,/*level*/
					&bufptr,/*bufptr*/
					0,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					NULL/*resume_handle*/
					));
    if (err != NERR_BufTooSmall) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					0,/*level*/
					&bufptr,/*bufptr*/
					0xFFFFFFFF,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					&resume_handle/*resume_handle*/
					));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					0,/*level*/
					&bufptr,/*bufptr*/
					0xFFFFFFFF,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					&resume_handle/*resume_handle*/
					));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					1,/*level*/
					&bufptr,/*bufptr*/
					0,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					NULL/*resume_handle*/
					));
    if (err != NERR_BufTooSmall) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					1,/*level*/
					&bufptr,/*bufptr*/
					0xFFFFFFFF,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					&resume_handle/*resume_handle*/
					));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					1,/*level*/
					&bufptr,/*bufptr*/
					0xFFFFFFFF,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					&resume_handle/*resume_handle*/
					));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetConnectionEnum(pContext,
					hostname,/*servername*/
					ambstowc16s("IPC$"),/*qualifier*/
					2,/*level*/
					&bufptr,/*bufptr*/
					0xFFFFFFFF,/*prefmaxlen*/
					&entriesread,/*entriesread*/
					&totalentries,/*totalentries*/
					&resume_handle/*resume_handle*/
					));
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return TRUE;
}

int TestNetFileEnum(struct test *t, const wchar16_t *hostname,
                    const wchar16_t *user, const wchar16_t *pass,
                    struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 entriesread = 0;
    UINT32 totalentries = 0;
    UINT32 resume_handle = 0;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  2,/*level*/
                                  &bufptr,/*bufptr*/
                                  0,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  NULL/*resume_handle*/
                                  ));
    if (err != NERR_BufTooSmall) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  2,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  2,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  3,/*level*/
                                  &bufptr,/*bufptr*/
                                  0,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  NULL/*resume_handle*/
                                  ));
    if (err != NERR_BufTooSmall) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  3,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  3,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  0,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  123,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  NULL//&resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}

int TestNetFileGetInfo(struct test *t, const wchar16_t *hostname,
                       const wchar16_t *user, const wchar16_t *pass,
                       struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    NET_API_STATUS unknownlevel_err = ERROR_INVALID_LEVEL;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 entriesread = 0;
    UINT32 totalentries = 0;
    UINT32 resume_handle = 0;
    FILE_INFO_2 *fi2_enum = NULL;
    UINT32 i;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    bufptr = NULL;
    CALL_NETAPI(err = NetFileGetInfo(pContext,
                                     hostname,/*servername*/
                                     0,/*fileid*/
                                     2,/*level*/
                                     &bufptr/*bufptr*/
                                     ));
    if (err != ERROR_FILE_NOT_FOUND) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    CALL_NETAPI(err = NetFileGetInfo(pContext,
                                     hostname,/*servername*/
                                     0,/*fileid*/
                                     3,/*level*/
                                     &bufptr/*bufptr*/
                                     ));
    if (err != ERROR_FILE_NOT_FOUND) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    CALL_NETAPI(err = NetFileGetInfo(pContext,
                                     hostname,/*servername*/
                                     0,/*fileid*/
                                     123,/*level*/
                                     &bufptr/*bufptr*/
                                     ));
    if (err != ERROR_INVALID_LEVEL) {
        /* w2k returnd FILE_NOT_FOUND for all unknown levels */
        if (err != ERROR_FILE_NOT_FOUND) netapi_fail(err);
    }
    unknownlevel_err = err;
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    CALL_NETAPI(err = NetFileGetInfo(pContext,
                                     hostname,/*servername*/
                                     0,/*fileid*/
                                     1,/*level*/
                                     &bufptr/*bufptr*/
                                     ));
    if (err != unknownlevel_err) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    CALL_NETAPI(err = NetFileGetInfo(pContext,
                                     hostname,/*servername*/
                                     0,/*fileid*/
                                     0,/*level*/
                                     &bufptr/*bufptr*/
                                     ));
    if (err != unknownlevel_err) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    CALL_NETAPI(err = NetFileEnum(pContext,
                                  hostname,/*servername*/
                                  NULL,/*basepath*/
                                  NULL,/*username*/
                                  2,/*level*/
                                  &bufptr,/*bufptr*/
                                  0xFFFFFFFF,/*prefmaxlen*/
                                  &entriesread,/*entriesread*/
                                  &totalentries,/*totalentries*/
                                  &resume_handle/*resume_handle*/
                                  ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    fi2_enum = (PFILE_INFO_2)bufptr;
    for (i=0; i< entriesread; i++) {

        bufptr = NULL;
        CALL_NETAPI(err = NetFileGetInfo(pContext,
                                         hostname,/*servername*/
                                         fi2_enum[i].fi2_id,/*fileid*/
                                         2,/*level*/
                                         &bufptr/*bufptr*/
                                         ));
        if (err != ERROR_SUCCESS) netapi_fail(err);
        printf("bufptr[%p]\n", bufptr);

        bufptr = NULL;
        CALL_NETAPI(err = NetFileGetInfo(pContext,
                                         hostname,/*servername*/
                                         fi2_enum[i].fi2_id,/*fileid*/
                                         3,/*level*/
                                         &bufptr/*bufptr*/
                                         ));
        if (err != ERROR_SUCCESS) netapi_fail(err);
        printf("bufptr[%p]\n", bufptr);
    }

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    return FALSE;
}

int TestNetFileClose(struct test *t, const wchar16_t *hostname,
                     const wchar16_t *user, const wchar16_t *pass,
                     struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    CALL_NETAPI(err = NetFileClose(pContext,
                                   hostname,/*servername*/
                                   0/*fileid*/
                                   ));
    if (err != NERR_FileIdNotFound) netapi_fail(err);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}

int TestNetSessionEnum(struct test *t, const wchar16_t *hostname,
                       const wchar16_t *user, const wchar16_t *pass,
                       struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 entriesread = 0;
    UINT32 totalentries = 0;
    UINT32 resume_handle = 0;
    const UINT32 levels[5] = { 0, 1, 2, 10, 502 };
    UINT32 i;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    for (i=0; i < 5; i++) {

        bufptr = NULL;
        entriesread = 0;
        totalentries = 0;
        resume_handle = 0;
        CALL_NETAPI(err = NetSessionEnum(pContext,
                                         hostname,/*servername*/
                                         NULL, /*unc client name */
                                         NULL, /*username */
                                         levels[i],/*level*/
                                         &bufptr,/*bufptr*/
                                         0,/*prefmaxlen*/
                                         &entriesread,/*entriesread*/
                                         &totalentries,/*totalentries*/
                                         NULL/*resume_handle*/
                                         ));
        if (err != NERR_BufTooSmall) netapi_fail(err);
        printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
               bufptr, entriesread, totalentries, resume_handle);

        bufptr = NULL;
        entriesread = 0;
        totalentries = 0;
        resume_handle = 0;
        CALL_NETAPI(err = NetSessionEnum(pContext,
                                         hostname,/*servername*/
                                         NULL, /*unc client name */
                                         NULL, /*username */
                                         levels[i],/*level*/
                                         &bufptr,/*bufptr*/
                                         0xFFFFFFFF,/*prefmaxlen*/
                                         &entriesread,/*entriesread*/
                                         &totalentries,/*totalentries*/
                                         &resume_handle/*resume_handle*/
                                         ));
        if (err != ERROR_SUCCESS) netapi_fail(err);
        printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
               bufptr, entriesread, totalentries, resume_handle);

        bufptr = NULL;
        entriesread = 0;
        totalentries = 0;
        CALL_NETAPI(err = NetSessionEnum(pContext,
                                         hostname,/*servername*/
                                         NULL, /*unc client name */
                                         NULL, /*username */
                                         levels[i],/*level*/
                                         &bufptr,/*bufptr*/
                                         0xFFFFFFFF,/*prefmaxlen*/
                                         &entriesread,/*entriesread*/
                                         &totalentries,/*totalentries*/
                                         &resume_handle/*resume_handle*/
                                         ));
        if (err != ERROR_SUCCESS) netapi_fail(err);
        printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
               bufptr, entriesread, totalentries, resume_handle);

    }

    /* test invalid level 123 */
    bufptr = NULL;
    entriesread = 0;
    totalentries = 0;
    resume_handle = 0;
    CALL_NETAPI(err = NetSessionEnum(pContext,
                                     hostname,/*servername*/
                                     NULL, /*unc client name */
                                     NULL, /*username */
                                     123,/*level*/
                                     &bufptr,/*bufptr*/
                                     0xFFFFFFFF,/*prefmaxlen*/
                                     &entriesread,/*entriesread*/
                                     &totalentries,/*totalentries*/
                                     &resume_handle/*resume_handle*/
                                     ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);
    printf("bufptr[%p] entriesread[%u] totalentries[%u] resume_handle[%u]\n",
           bufptr, entriesread, totalentries, resume_handle);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}


static
BOOL
CallNetShareAdd(
    PCWSTR pwszHostname,
    DWORD  dwLevel,
    PWSTR  pwszShareName,
    DWORD  dwType,
    PWSTR  pwszComment,
    PWSTR  pwszPath
    )
{
    BOOL ret = TRUE;
    NET_API_STATUS err = ERROR_SUCCESS;
    PBYTE pBuffer = NULL;
    SHARE_INFO_0 Info0 = {0};
    SHARE_INFO_2 Info2 = {0};
    DWORD dwParmErr = 0;

    switch (dwLevel)
    {
    case 0:
        Info0.shi0_netname = pwszShareName;

        pBuffer = &Info0;
        break;

    case 2:
        Info2.shi2_netname = pwszShareName;
        Info2.shi2_type    = dwType;
        Info2.shi2_remark  = pwszComment;
        Info2.shi2_path    = pwszPath;

        pBuffer = &Info2;
        break;
    }

    err = NetShareAdd(pwszHostname,
                      dwLevel,
                      pBuffer,
                      &dwParmErr);
    ret = (err == ERROR_SUCCESS);

    return ret;
}


static
BOOL
CallNetShareEnum(
    PCWSTR pwszServername,
    DWORD  dwLevel
    )
{
    BOOL ret = TRUE;
    NET_API_STATUS err = ERROR_SUCCESS;
    PBYTE pBuffer = NULL;
    DWORD dwMaxLen = (DWORD)(-1);
    DWORD dwNumEntries = 0;
    DWORD dwLastTotal = 0;
    DWORD dwCalculatedTotal = 0;
    DWORD dwTotal = 0;
    DWORD dwResume = 0;
    DWORD dwLastResume = 0;

    while (dwMaxLen > 40)
    {
        dwCalculatedTotal = 0;
        dwLastTotal       = (DWORD)-1;

        do
        {
            err = NetShareEnum(pwszServername,
                               dwLevel,
                               &pBuffer,
                               dwMaxLen,
                               &dwNumEntries,
                               &dwTotal,
                               &dwResume);
            if (err != ERROR_SUCCESS &&
                err != ERROR_MORE_DATA &&
                err != ERROR_NOT_ENOUGH_MEMORY)
            {
                ret = FALSE;
                goto done;
            }

            if (pBuffer)
            {
                SrvSvcFreeMemory(pBuffer);
                pBuffer = NULL;
            }

            dwLastTotal        = dwTotal;
            dwCalculatedTotal += dwNumEntries;

            /* This is a workaround for a bug in windows server.
               It doesn't set resume handle to the next entry index
               even if very small buffer is allowed and ERROR_MORE_DATA
               is returned */
            if (err == ERROR_MORE_DATA &&
                dwResume == 0)
            {
                dwLastResume += dwNumEntries;
                dwResume      = dwLastResume;
            }
        }
        while (err == ERROR_MORE_DATA);

        if (dwMaxLen > 65536)
        {
            dwMaxLen /= 256;
        }
        else if (dwMaxLen <= 65536 && dwMaxLen > 512)
        {
            dwMaxLen /= 4;
        }
        else if (dwMaxLen <= 512)
        {
            dwMaxLen /= 2;
        }
        else if (dwMaxLen < 32)
        {
            dwMaxLen = 0;
        }

        dwNumEntries = 0;
        dwTotal      = 0;
        dwResume     = 0;
        dwLastResume = 0;
    }

done:
    if (pBuffer)
    {
        SrvSvcFreeMemory(pBuffer);
    }

    return ret;
}


static
BOOLEAN
CallNetShareGetInfo(
    PCWSTR pwszHostname,
    DWORD  dwLevel,
    PWSTR  pwszShareName
    )
{
    BOOLEAN ret = TRUE;
    NET_API_STATUS err = ERROR_SUCCESS;
    PBYTE pBuffer = NULL;

    err = NetShareGetInfo(pwszHostname,
                          pwszShareName,
                          dwLevel,
                          &pBuffer);
    if (err == ERROR_SUCCESS)
    {
        switch (dwLevel)
        {
        case 0:
            RESULT_WSTR(((PSHARE_INFO_0)pBuffer)->shi0_netname);
            break;

        case 1:
            RESULT_WSTR(((PSHARE_INFO_1)pBuffer)->shi1_netname);
            RESULT_UINT(((PSHARE_INFO_1)pBuffer)->shi1_type);
            RESULT_WSTR(((PSHARE_INFO_1)pBuffer)->shi1_remark);
            break;

        case 2:
            RESULT_WSTR(((PSHARE_INFO_2)pBuffer)->shi2_netname);
            RESULT_UINT(((PSHARE_INFO_2)pBuffer)->shi2_type);
            RESULT_WSTR(((PSHARE_INFO_2)pBuffer)->shi2_remark);
            RESULT_WSTR(((PSHARE_INFO_2)pBuffer)->shi2_path);
            break;

        case 501:
            RESULT_WSTR(((PSHARE_INFO_501)pBuffer)->shi501_netname);
            RESULT_UINT(((PSHARE_INFO_501)pBuffer)->shi501_type);
            RESULT_WSTR(((PSHARE_INFO_501)pBuffer)->shi501_remark);
            RESULT_WSTR(((PSHARE_INFO_501)pBuffer)->shi501_netname);
            break;

        case 502:
            RESULT_WSTR(((PSHARE_INFO_502)pBuffer)->shi502_netname);
            RESULT_UINT(((PSHARE_INFO_502)pBuffer)->shi502_type);
            RESULT_WSTR(((PSHARE_INFO_502)pBuffer)->shi502_remark);
            RESULT_WSTR(((PSHARE_INFO_502)pBuffer)->shi502_path);
            break;
        }
    }
    else
    {
        ret = FALSE;
    }

    if (pBuffer)
    {
        SrvSvcFreeMemory(pBuffer);
    }

    return ret;
}


static
BOOLEAN
CallNetShareSetInfo(
    PCWSTR pwszHostname,
    DWORD  dwLevel,
    PWSTR  pwszShareName,
    DWORD  dwType,
    PWSTR  pwszComment,
    PWSTR  pwszPath,
    DWORD  dwFlags
    )
{
    BOOLEAN ret = TRUE;
    NET_API_STATUS err = ERROR_SUCCESS;
    PBYTE pBuffer = NULL;
    SHARE_INFO_0 Info0 = {0};
    SHARE_INFO_1 Info1 = {0};
    SHARE_INFO_2 Info2 = {0};
    SHARE_INFO_501 Info501 = {0};
    SHARE_INFO_502 Info502 = {0};
    DWORD dwParmErr = 0;

    switch (dwLevel)
    {
    case 0:
        Info0.shi0_netname = pwszShareName;

        pBuffer = (PVOID)&Info0;
        break;

    case 1:
        Info1.shi1_netname = pwszShareName;
        Info1.shi1_type    = dwType;
        Info1.shi1_remark  = pwszComment;

        pBuffer = (PVOID)&Info1;
        break;

    case 2:
        Info2.shi2_netname = pwszShareName;
        Info2.shi2_type    = dwType;
        Info2.shi2_remark  = pwszComment;
        Info2.shi2_path    = pwszPath;

        pBuffer = (PVOID)&Info2;
        break;

    case 501:
        Info501.shi501_netname = pwszShareName;
        Info501.shi501_type    = dwType;
        Info501.shi501_remark  = pwszComment;
        Info501.shi501_flags   = dwFlags;

        pBuffer = (PVOID)&Info501;
        break;

    case 502:
        pBuffer = (PVOID)&Info502;
        break;
    }

    err = NetShareSetInfo(pwszHostname,
                          pwszShareName,
                          dwLevel,
                          pBuffer,
                          &dwParmErr);
    ret = (err == ERROR_SUCCESS);

    return ret;
}


int
TestNetShareAdd(
    struct test *t,
    const wchar16_t *hostname,
    const wchar16_t *user,
    const wchar16_t *pass,
    struct parameter *options,
    int optcount
    )
{
    const DWORD dwDefaultLevel = (DWORD)(-1);
    const DWORD dwDefaultType = 0;
    const PSTR pszDefaultName = "TEST";
    const PSTR pszDefaultPath = "/tmp";
    const PSTR pszDefaultComment = "Test Comment";

    NET_API_STATUS err = ERROR_SUCCESS;
    NTSTATUS status = STATUS_SUCCESS;
    BOOL ret = TRUE;
    enum param_err perr = perr_success;
    DWORD dwSelectedLevels[] = { 0 };
    DWORD dwAvailableLevels[] = { 2 };
    DWORD dwLevel = 0;
    PDWORD pdwLevels = NULL;
    DWORD dwNumLevels = 0;
    PWSTR pwszSharename = NULL;
    DWORD dwType = 0;
    PWSTR pwszPath = NULL;
    PWSTR pwszComment = NULL;
    DWORD i = 0;

    TESTINFO(t, hostname, user, pass);

    perr = fetch_value(options, optcount, "level", pt_uint32,
                       &dwLevel, &dwDefaultLevel);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "sharename", pt_w16string,
                       &pwszSharename, &pszDefaultName);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "path", pt_w16string,
                       &pwszPath, &pszDefaultPath);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "comment", pt_w16string,
                       &pwszComment, &pszDefaultComment);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "type", pt_uint32,
                       &dwType, &dwDefaultType);
    if (!perr_is_ok(perr)) perr_fail(perr);

    SET_SESSION_CREDS(hCreds);

    if (dwLevel == (DWORD)(-1))
    {
        pdwLevels   = dwAvailableLevels;
        dwNumLevels = sizeof(dwAvailableLevels)/sizeof(dwAvailableLevels[0]);
    }
    else
    {
        dwSelectedLevels[0] = dwLevel;
        pdwLevels   = dwSelectedLevels;
        dwNumLevels = sizeof(dwSelectedLevels)/sizeof(dwSelectedLevels[0]);
    }

    for (i = 0; i < dwNumLevels; i++)
    {
        dwLevel = pdwLevels[i];

        err = CleanupShare(hostname, pwszSharename);
        if (err != 0) netapi_fail(err);

        ret &= CallNetShareAdd(hostname,
                               dwLevel,
                               pwszSharename,
                               dwType,
                               pwszComment,
                               pwszPath);
    }

    err = CleanupShare(hostname, pwszSharename);
    if (err != 0) netapi_fail(err);

done:
    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();

    return (err == ERROR_SUCCESS &&
            status == STATUS_SUCCESS &&
            ret);
}

int
TestNetShareEnum(
    struct test *t,
    const wchar16_t *hostname,
    const wchar16_t *user,
    const wchar16_t *pass,
    struct parameter *options,
    int optcount
    )
{
    const DWORD dwDefaultLevel = (DWORD)(-1);

    BOOL ret = TRUE;
    enum param_err perr = perr_success;
    DWORD i = 0;
    DWORD dwSelectedLevels[] = { 0 };
    DWORD dwAvailableLevels[] = { 0, 1, 2, 501, 502 };
    DWORD dwLevel = 0;
    PDWORD pdwLevels = NULL;
    DWORD dwNumLevels = 0;

    TESTINFO(t, hostname, user, pass);

    perr = fetch_value(options, optcount, "level", pt_uint32,
                       &dwLevel, &dwDefaultLevel);
    if (!perr_is_ok(perr)) perr_fail(perr);

    SET_SESSION_CREDS(hCreds);

    if (dwLevel == (DWORD)(-1))
    {
        pdwLevels   = dwAvailableLevels;
        dwNumLevels = sizeof(dwAvailableLevels)/sizeof(dwAvailableLevels[0]);
    }
    else
    {
        dwSelectedLevels[0] = dwLevel;
        pdwLevels   = dwSelectedLevels;
        dwNumLevels = sizeof(dwSelectedLevels)/sizeof(dwSelectedLevels[0]);
    }

    for (i = 0; i < dwNumLevels; i++)
    {
        dwLevel = pdwLevels[i];

        ret &= CallNetShareEnum(hostname,
                                dwLevel);
    }

done:
    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();

    return ret;
}


int
TestNetShareGetInfo(
    struct test *t,
    const wchar16_t *hostname,
    const wchar16_t *user,
    const wchar16_t *pass,
    struct parameter *options,
    int optcount
    )
{
    const DWORD dwDefaultLevel = (DWORD)(-1);
    const PSTR pszDefaultName = "TEST";

    BOOLEAN ret = TRUE;
    enum param_err perr = perr_success;
    DWORD i = 0;
    DWORD dwSelectedLevels[] = { 0 };
    DWORD dwAvailableLevels[] = { 0, 1, 2, 501, 502 };
    DWORD dwLevel = 0;
    PWSTR pwszShareName = NULL;
    PDWORD pdwLevels = NULL;
    DWORD dwNumLevels = 0;

    TESTINFO(t, hostname, user, pass);

    perr = fetch_value(options, optcount, "level", pt_uint32,
                       &dwLevel, &dwDefaultLevel);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "sharename", pt_w16string,
                       &pwszShareName, &pszDefaultName);
    if (!perr_is_ok(perr)) perr_fail(perr);

    SET_SESSION_CREDS(hCreds);

    if (dwLevel == (DWORD)(-1))
    {
        pdwLevels   = dwAvailableLevels;
        dwNumLevels = sizeof(dwAvailableLevels)/sizeof(dwAvailableLevels[0]);
    }
    else
    {
        dwSelectedLevels[0] = dwLevel;
        pdwLevels   = dwSelectedLevels;
        dwNumLevels = sizeof(dwSelectedLevels)/sizeof(dwSelectedLevels[0]);
    }

    for (i = 0; i < dwNumLevels; i++)
    {
        dwLevel = pdwLevels[i];

        ret &= CallNetShareGetInfo(hostname,
                                   dwLevel,
                                   pwszShareName);
    }

done:
    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();

    return ret;
}


int TestNetShareSetInfo(struct test *t, const wchar16_t *hostname,
                        const wchar16_t *user, const wchar16_t *pass,
                        struct parameter *options, int optcount)
{
    PCSTR pszDefaultShareName = "TEST";
    PCSTR pszDefaultPath = "c:/tmp";
    PCSTR pszDefaultComment = "Testing comment";
    const DWORD dwDefaultType = 0;
    const DWORD dwDefaultFlags = 0;
    const DWORD dwDefaultLevel = (DWORD)(-1);

    BOOLEAN ret = TRUE;
    NET_API_STATUS err = ERROR_SUCCESS;
    enum param_err perr = perr_success;
    DWORD dwSelectedLevels[] = { 0 };
    DWORD dwAvailableLevels[] = { 0, 1, 2, 501, 502 };
    DWORD dwLevel = 0;
    PDWORD pdwLevels = NULL;
    DWORD dwNumLevels = 0;
    PWSTR pwszShareName = NULL;
    DWORD dwType = 0;
    PWSTR pwszPath = NULL;
    PWSTR pwszComment = NULL;
    DWORD dwFlags = 0;
    DWORD i = 0;

    TESTINFO(t, hostname, user, pass);

    perr = fetch_value(options, optcount, "level", pt_uint32,
                       &dwLevel, &dwDefaultLevel);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "sharename", pt_w16string,
                       &pwszShareName, &pszDefaultShareName);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "path", pt_w16string,
                       &pwszPath, &pszDefaultPath);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "comment", pt_w16string,
                       &pwszComment, &pszDefaultComment);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "type", pt_uint32,
                       &dwType, &dwDefaultType);
    if (!perr_is_ok(perr)) perr_fail(perr);

    perr = fetch_value(options, optcount, "flags", pt_uint32,
                       &dwFlags, &dwDefaultFlags);
    if (!perr_is_ok(perr)) perr_fail(perr);

    SET_SESSION_CREDS(hCreds);

    if (dwLevel == (DWORD)(-1))
    {
        pdwLevels   = dwAvailableLevels;
        dwNumLevels = sizeof(dwAvailableLevels)/sizeof(dwAvailableLevels[0]);
    }
    else
    {
        dwSelectedLevels[0] = dwLevel;
        pdwLevels   = dwSelectedLevels;
        dwNumLevels = sizeof(dwSelectedLevels)/sizeof(dwSelectedLevels[0]);
    }

    for (i = 0; i < dwNumLevels; i++)
    {
        dwLevel = pdwLevels[i];

        ret &= CallNetShareSetInfo(hostname,
                                   dwLevel,
                                   pwszShareName,
                                   dwType,
                                   pwszComment,
                                   pwszPath,
                                   dwFlags);
    }

done:
    RELEASE_SESSION_CREDS;

    LW_SAFE_FREE_MEMORY(pwszShareName);
    LW_SAFE_FREE_MEMORY(pwszComment);
    LW_SAFE_FREE_MEMORY(pwszPath);

    SrvSvcDestroyMemory();
    return ret;
}


int TestNetShareDel(struct test *t, const wchar16_t *hostname,
                    const wchar16_t *user, const wchar16_t *pass,
                    struct parameter *options, int optcount)
{
    const char *def_sharename = "TEST";

    NET_API_STATUS err = ERROR_SUCCESS;
    enum param_err perr = perr_success;
    PSRVSVC_CONTEXT pContext = NULL;
    wchar16_t *sharename = NULL;

    TESTINFO(t, hostname, user, pass);

    perr = fetch_value(options, optcount, "sharename", pt_w16string,
                       &sharename, &def_sharename);
    if (!perr_is_ok(perr)) perr_fail(perr);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);
    INPUT_ARG_WSTR(sharename);

    CALL_NETAPI(err = NetrShareDel(pContext,
                                  hostname,/*servername*/
                                  sharename,/*netname*/
                                  0/*reserved*/
                                  ));
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;

done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}


int TestNetServerGetInfo(struct test *t, const wchar16_t *hostname,
                         const wchar16_t *user, const wchar16_t *pass,
                         struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 i;
    UINT32 levels[] = { 100, 101, 102, 502, 503 };

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    for (i=0; i < 5; i++) {
       bufptr = NULL;
       err = NetServerGetInfo(hostname,/*servername*/
                              levels[i],/*level*/
                              &bufptr/*bufptr*/
                              );
       if (err != ERROR_SUCCESS) netapi_fail(err);
       printf("bufptr[%p]\n", bufptr);
    }

    bufptr = NULL;
    err = NetServerGetInfo(hostname,/*servername*/
                           403,/*level*/
                           &bufptr/*bufptr*/
                           );
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    bufptr = NULL;
    err = NetServerGetInfo(hostname,/*servername*/
                           789,/*level*/
                           &bufptr/*bufptr*/
                           );
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;
done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}

int TestNetServerSetInfo(struct test *t, const wchar16_t *hostname,
                         const wchar16_t *user, const wchar16_t *pass,
                         struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;
    UINT32 parm_err = 0;
    SERVER_INFO_102 *orig102;
    SERVER_INFO_503 *orig503;
    SERVER_INFO_100 info100;
    SERVER_INFO_101 info101;
    SERVER_INFO_102 info102;
    SERVER_INFO_402 info402;
    SERVER_INFO_403 info403;
    SERVER_INFO_502 info502;
    SERVER_INFO_503 info503;
    SERVER_INFO_599 info599;
    SERVER_INFO_1005 info1005;
    SERVER_INFO_1010 info1010;
    SERVER_INFO_1016 info1016;
    SERVER_INFO_1017 info1017;
    SERVER_INFO_1018 info1018;
    SERVER_INFO_1107 info1107;
    SERVER_INFO_1501 info1501;
    SERVER_INFO_1502 info1502;
    SERVER_INFO_1503 info1503;
    SERVER_INFO_1506 info1506;
    SERVER_INFO_1509 info1509;
    SERVER_INFO_1510 info1510;
    SERVER_INFO_1511 info1511;
    SERVER_INFO_1512 info1512;
    SERVER_INFO_1513 info1513;
    SERVER_INFO_1514 info1514;
    SERVER_INFO_1515 info1515;
    SERVER_INFO_1516 info1516;
    SERVER_INFO_1518 info1518;
    SERVER_INFO_1520 info1520;
    SERVER_INFO_1521 info1521;
    SERVER_INFO_1522 info1522;
    SERVER_INFO_1523 info1523;
    SERVER_INFO_1524 info1524;
    SERVER_INFO_1525 info1525;
    SERVER_INFO_1528 info1528;
    SERVER_INFO_1529 info1529;
    SERVER_INFO_1530 info1530;
    SERVER_INFO_1533 info1533;
    SERVER_INFO_1534 info1534;
    SERVER_INFO_1535 info1535;
    SERVER_INFO_1536 info1536;
    SERVER_INFO_1537 info1537;
    SERVER_INFO_1538 info1538;
    SERVER_INFO_1539 info1539;
    SERVER_INFO_1540 info1540;
    SERVER_INFO_1541 info1541;
    SERVER_INFO_1542 info1542;
    SERVER_INFO_1543 info1543;
    SERVER_INFO_1544 info1544;
    SERVER_INFO_1545 info1545;
    SERVER_INFO_1546 info1546;
    SERVER_INFO_1547 info1547;
    SERVER_INFO_1548 info1548;
    SERVER_INFO_1549 info1549;
    SERVER_INFO_1550 info1550;
    SERVER_INFO_1552 info1552;
    SERVER_INFO_1553 info1553;
    SERVER_INFO_1554 info1554;
    SERVER_INFO_1555 info1555;
    SERVER_INFO_1556 info1556;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    bufptr = NULL;
    CALL_NETAPI(err = NetServerGetInfo(pContext,
                                       hostname,/*servername*/
                                       102,/*level*/
                                       &bufptr/*bufptr*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);
    orig102 = (PSERVER_INFO_102)bufptr;

    bufptr = NULL;
    CALL_NETAPI(err = NetServerGetInfo(pContext,
                                       hostname,/*servername*/
                                       503,/*level*/
                                       &bufptr/*bufptr*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);
    orig503 = (PSERVER_INFO_503)bufptr;

    bufptr = NULL;
    parm_err = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       0,/*level*/
                                       bufptr,/*bufptr*/
                                       NULL/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info100;
    parm_err = 0;
    info100.sv100_platform_id = orig102->sv102_platform_id;
    info100.sv100_name        = orig102->sv102_name;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       100,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);

    bufptr = (UINT8*)&info101;
    parm_err = 0;
    info101.sv101_platform_id   = orig102->sv102_platform_id;
    info101.sv101_name          = orig102->sv102_name;
    info101.sv101_version_major = orig102->sv102_version_major;
    info101.sv101_version_minor = orig102->sv102_version_minor;
    info101.sv101_type          = orig102->sv102_type;
    info101.sv101_comment       = orig102->sv102_comment;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       101,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info102;
    parm_err = 0;
    info102.sv102_platform_id   = orig102->sv102_platform_id;
    info102.sv102_name          = orig102->sv102_name;
    info102.sv102_version_major = orig102->sv102_version_major;
    info102.sv102_version_minor = orig102->sv102_version_minor;
    info102.sv102_type          = orig102->sv102_type;
    info102.sv102_comment       = orig102->sv102_comment;
    info102.sv102_users         = orig102->sv102_users;
    info102.sv102_disc          = orig102->sv102_disc;
    info102.sv102_hidden        = orig102->sv102_hidden;
    info102.sv102_announce      = orig102->sv102_announce;
    info102.sv102_anndelta      = orig102->sv102_anndelta;
    info102.sv102_licenses      = orig102->sv102_licenses;
    info102.sv102_userpath      = orig102->sv102_userpath;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       102,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info402;
    parm_err = 0;
    memset(&info402, 0, sizeof(info402));
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       402,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);

    bufptr = (UINT8*)&info403;
    parm_err = 0;
    memset(&info403, 0, sizeof(info403));
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       403,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_LEVEL) netapi_fail(err);

    bufptr = (UINT8*)&info502;
    parm_err = 0;
    info502.sv502_sessopens              = orig503->sv503_sessopens;
    info502.sv502_sessvcs                = orig503->sv503_sessvcs;
    info502.sv502_opensearch             = orig503->sv503_opensearch;
    info502.sv502_sizreqbuf              = orig503->sv503_sizreqbuf;
    info502.sv502_initworkitems          = orig503->sv503_initworkitems;
    info502.sv502_maxworkitems           = orig503->sv503_maxworkitems;
    info502.sv502_rawworkitems           = orig503->sv503_rawworkitems;
    info502.sv502_irpstacksize           = orig503->sv503_irpstacksize;
    info502.sv502_maxrawbuflen           = orig503->sv503_maxrawbuflen;
    info502.sv502_sessusers              = orig503->sv503_sessusers;
    info502.sv502_sessconns              = orig503->sv503_sessconns;
    info502.sv502_maxpagedmemoryusage    = orig503->sv503_maxpagedmemoryusage;
    info502.sv502_maxnonpagedmemoryusage = orig503->sv503_maxnonpagedmemoryusage;
    info502.sv502_enablesoftcompat       = orig503->sv503_enablesoftcompat;
    info502.sv502_enableforcedlogoff     = orig503->sv503_enableforcedlogoff;
    info502.sv502_timesource             = orig503->sv503_timesource;
    info502.sv502_acceptdownlevelapis    = orig503->sv503_acceptdownlevelapis;
    info502.sv502_lmannounce             = orig503->sv503_lmannounce;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       502,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info503;
    parm_err = 0;
    info503.sv503_sessopens               = orig503->sv503_sessopens;
    info503.sv503_sessvcs                 = orig503->sv503_sessvcs;
    info503.sv503_opensearch              = orig503->sv503_opensearch;
    info503.sv503_sizreqbuf               = orig503->sv503_sizreqbuf;
    info503.sv503_initworkitems           = orig503->sv503_initworkitems;
    info503.sv503_maxworkitems            = orig503->sv503_maxworkitems;
    info503.sv503_rawworkitems            = orig503->sv503_rawworkitems;
    info503.sv503_irpstacksize            = orig503->sv503_irpstacksize;
    info503.sv503_maxrawbuflen            = orig503->sv503_maxrawbuflen;
    info503.sv503_sessusers               = orig503->sv503_sessusers;
    info503.sv503_sessconns               = orig503->sv503_sessconns;
    info503.sv503_maxpagedmemoryusage     = orig503->sv503_maxpagedmemoryusage;
    info503.sv503_maxnonpagedmemoryusage  = orig503->sv503_maxnonpagedmemoryusage;
    info503.sv503_enablesoftcompat        = orig503->sv503_enablesoftcompat;
    info503.sv503_enableforcedlogoff      = orig503->sv503_enableforcedlogoff;
    info503.sv503_timesource              = orig503->sv503_timesource;
    info503.sv503_acceptdownlevelapis     = orig503->sv503_acceptdownlevelapis;
    info503.sv503_lmannounce              = orig503->sv503_lmannounce;
    info503.sv503_domain                  = orig503->sv503_domain;
    info503.sv503_maxcopyreadlen          = orig503->sv503_maxcopyreadlen;
    info503.sv503_maxcopywritelen         = orig503->sv503_maxcopywritelen;
    info503.sv503_minkeepsearch           = orig503->sv503_minkeepsearch;
    info503.sv503_maxkeepsearch           = orig503->sv503_maxkeepsearch;
    info503.sv503_minkeepcomplsearch      = orig503->sv503_minkeepcomplsearch;
    info503.sv503_maxkeepcomplsearch      = orig503->sv503_maxkeepcomplsearch;
    info503.sv503_threadcountadd          = orig503->sv503_threadcountadd;
    info503.sv503_numblockthreads         = orig503->sv503_numblockthreads;
    info503.sv503_scavtimeout             = orig503->sv503_scavtimeout;
    info503.sv503_minrcvqueue             = orig503->sv503_minrcvqueue;
    info503.sv503_minfreeworkitems        = orig503->sv503_minfreeworkitems;
    info503.sv503_xactmemsize             = orig503->sv503_xactmemsize;
    info503.sv503_threadpriority          = orig503->sv503_threadpriority;
    info503.sv503_maxmpxct                = orig503->sv503_maxmpxct;
    info503.sv503_oplockbreakwait         = orig503->sv503_oplockbreakwait;
    info503.sv503_oplockbreakresponsewait = orig503->sv503_oplockbreakresponsewait;
    info503.sv503_enableoplocks           = orig503->sv503_enableoplocks;
    info503.sv503_enableoplockforceclose  = orig503->sv503_enableoplockforceclose;
    info503.sv503_enablefcbopens          = orig503->sv503_enablefcbopens;
    info503.sv503_enableraw               = orig503->sv503_enableraw;
    info503.sv503_enablesharednetdrives   = orig503->sv503_enablesharednetdrives;
    info503.sv503_minfreeconnections      = orig503->sv503_minfreeconnections;
    info503.sv503_maxfreeconnections      = orig503->sv503_maxfreeconnections;

    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       503,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info599;
    parm_err = 0;
    info599.sv599_sessopens               = orig503->sv503_sessopens;
    info599.sv599_sessvcs                 = orig503->sv503_sessvcs;
    info599.sv599_opensearch              = orig503->sv503_opensearch;
    info599.sv599_sizreqbuf               = orig503->sv503_sizreqbuf;
    info599.sv599_initworkitems           = orig503->sv503_initworkitems;
    info599.sv599_maxworkitems            = orig503->sv503_maxworkitems;
    info599.sv599_rawworkitems            = orig503->sv503_rawworkitems;
    info599.sv599_irpstacksize            = orig503->sv503_irpstacksize;
    info599.sv599_maxrawbuflen            = orig503->sv503_maxrawbuflen;
    info599.sv599_sessusers               = orig503->sv503_sessusers;
    info599.sv599_sessconns               = orig503->sv503_sessconns;
    info599.sv599_maxpagedmemoryusage     = orig503->sv503_maxpagedmemoryusage;
    info599.sv599_maxnonpagedmemoryusage  = orig503->sv503_maxnonpagedmemoryusage;
    info599.sv599_enablesoftcompat        = orig503->sv503_enablesoftcompat;
    info599.sv599_enableforcedlogoff      = orig503->sv503_enableforcedlogoff;
    info599.sv599_timesource              = orig503->sv503_timesource;
    info599.sv599_acceptdownlevelapis     = orig503->sv503_acceptdownlevelapis;
    info599.sv599_lmannounce              = orig503->sv503_lmannounce;
    info599.sv599_domain                  = orig503->sv503_domain;
    info599.sv599_maxcopyreadlen          = orig503->sv503_maxcopyreadlen;
    info599.sv599_maxcopywritelen         = orig503->sv503_maxcopywritelen;
    info599.sv599_minkeepsearch           = orig503->sv503_minkeepsearch;
    info599.sv599_maxkeepsearch           = orig503->sv503_maxkeepsearch;
    info599.sv599_minkeepcomplsearch      = orig503->sv503_minkeepcomplsearch;
    info599.sv599_maxkeepcomplsearch      = orig503->sv503_maxkeepcomplsearch;
    info599.sv599_threadcountadd          = orig503->sv503_threadcountadd;
    info599.sv599_numblockthreads         = orig503->sv503_numblockthreads;
    info599.sv599_scavtimeout             = orig503->sv503_scavtimeout;
    info599.sv599_minrcvqueue             = orig503->sv503_minrcvqueue;
    info599.sv599_minfreeworkitems        = orig503->sv503_minfreeworkitems;
    info599.sv599_xactmemsize             = orig503->sv503_xactmemsize;
    info599.sv599_threadpriority          = orig503->sv503_threadpriority;
    info599.sv599_maxmpxct                = orig503->sv503_maxmpxct;
    info599.sv599_oplockbreakwait         = orig503->sv503_oplockbreakwait;
    info599.sv599_oplockbreakresponsewait = orig503->sv503_oplockbreakresponsewait;
    info599.sv599_enableoplocks           = orig503->sv503_enableoplocks;
    info599.sv599_enableoplockforceclose  = orig503->sv503_enableoplockforceclose;
    info599.sv599_enablefcbopens          = orig503->sv503_enablefcbopens;
    info599.sv599_enableraw               = orig503->sv503_enableraw;
    info599.sv599_enablesharednetdrives   = orig503->sv503_enablesharednetdrives;
    info599.sv599_minfreeconnections      = orig503->sv503_minfreeconnections;
    info599.sv599_maxfreeconnections      = orig503->sv503_maxfreeconnections;
    info599.sv599_initsesstable           = 0;
    info599.sv599_initconntable           = 0;
    info599.sv599_initfiletable           = 0;
    info599.sv599_initsearchtable         = 0;
    info599.sv599_alertschedule           = 0;
    info599.sv599_errorthreshold          = 0;
    info599.sv599_networkerrorthreshold   = 0;
    info599.sv599_diskspacethreshold      = 0;
    info599.sv599_reserved                = 0;
    info599.sv599_maxlinkdelay            = 0;
    info599.sv599_minlinkthroughput       = 0;
    info599.sv599_linkinfovalidtime       = 0;
    info599.sv599_scavqosinfoupdatetime   = 0;
    info599.sv599_maxworkitemidletime     = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       599,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1005;
    parm_err = 0;
    info1005.sv1005_comment = orig102->sv102_comment;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1005,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1010;
    parm_err = 0;
    info1010.sv1010_disc = orig102->sv102_disc;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1010,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1016;
    parm_err = 0;
    info1016.sv1016_hidden = orig102->sv102_hidden;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1016,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1017;
    parm_err = 0;
    info1017.sv1017_announce = orig102->sv102_announce;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1017,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1018;
    parm_err = 0;
    info1018.sv1018_anndelta = orig102->sv102_anndelta;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1018,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1107;
    parm_err = 0;
    info1107.sv1107_users = orig102->sv102_users;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1107,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1501;
    parm_err = 0;
    info1501.sv1501_sessopens = orig503->sv503_sessopens;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1501,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1502;
    parm_err = 0;
    info1502.sv1502_sessvcs = orig503->sv503_sessvcs;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1502,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1503;
    parm_err = 0;
    info1503.sv1503_opensearch = orig503->sv503_opensearch;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1503,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1506;
    parm_err = 0;
    info1506.sv1506_maxworkitems = orig503->sv503_maxworkitems;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1506,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1509;
    parm_err = 0;
    info1509.sv1509_maxrawbuflen = orig503->sv503_maxrawbuflen;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1509,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1510;
    parm_err = 0;
    info1510.sv1510_sessusers = orig503->sv503_sessusers;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1510,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1511;
    parm_err = 0;
    info1511.sv1511_sessconns = orig503->sv503_sessconns;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1511,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1512;
    parm_err = 0;
    info1512.sv1512_maxnonpagedmemoryusage = orig503->sv503_maxnonpagedmemoryusage;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1512,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1513;
    parm_err = 0;
    info1513.sv1513_maxpagedmemoryusage = orig503->sv503_maxpagedmemoryusage;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1513,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1514;
    parm_err = 0;
    info1514.sv1514_enablesoftcompat = orig503->sv503_enablesoftcompat;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1514,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1515;
    parm_err = 0;
    info1515.sv1515_enableforcedlogoff = orig503->sv503_enableforcedlogoff;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1515,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1516;
    parm_err = 0;
    info1516.sv1516_timesource = orig503->sv503_timesource;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1516,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1518;
    parm_err = 0;
    info1518.sv1518_lmannounce = orig503->sv503_lmannounce;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1518,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1520;
    parm_err = 0;
    info1520.sv1520_maxcopyreadlen = orig503->sv503_maxcopyreadlen;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1520,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1521;
    parm_err = 0;
    info1521.sv1521_maxcopywritelen = orig503->sv503_maxcopywritelen;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1521,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1522;
    parm_err = 0;
    info1522.sv1522_minkeepsearch = orig503->sv503_minkeepsearch;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1522,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1523;
    parm_err = 0;
    info1523.sv1523_maxkeepsearch = orig503->sv503_maxkeepsearch;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1523,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1524;
    parm_err = 0;
    info1524.sv1524_minkeepcomplsearch = orig503->sv503_minkeepcomplsearch;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1524,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1525;
    parm_err = 0;
    info1525.sv1525_maxkeepcomplsearch = orig503->sv503_maxkeepcomplsearch;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1525,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1528;
    parm_err = 0;
    info1528.sv1528_scavtimeout = orig503->sv503_scavtimeout;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1528,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1529;
    parm_err = 0;
    info1529.sv1529_minrcvqueue = orig503->sv503_minrcvqueue;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1529,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1530;
    parm_err = 0;
    info1530.sv1530_minfreeworkitems = orig503->sv503_minfreeworkitems;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1530,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1533;
    parm_err = 0;
    info1533.sv1533_maxmpxct = orig503->sv503_maxmpxct;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1533,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1534;
    parm_err = 0;
    info1534.sv1534_oplockbreakwait = orig503->sv503_oplockbreakwait;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1534,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1535;
    parm_err = 0;
    info1535.sv1535_oplockbreakresponsewait = orig503->sv503_oplockbreakresponsewait;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1535,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1536;
    parm_err = 0;
    info1536.sv1536_enableoplocks = orig503->sv503_enableoplocks;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1536,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1537;
    parm_err = 0;
    info1537.sv1537_enableoplockforceclose = orig503->sv503_enableoplockforceclose;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1537,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1538;
    parm_err = 0;
    info1538.sv1538_enablefcbopens = orig503->sv503_enablefcbopens;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1538,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1539;
    parm_err = 0;
    info1539.sv1539_enableraw = orig503->sv503_enableraw;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1539,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1540;
    parm_err = 0;
    info1540.sv1540_enablesharednetdrives = orig503->sv503_enablesharednetdrives;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1540,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1541;
    parm_err = 0;
    info1541.sv1541_minfreeconnections = orig503->sv503_minfreeconnections;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1541,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1542;
    parm_err = 0;
    info1542.sv1542_maxfreeconnections = orig503->sv503_maxfreeconnections;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1542,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1543;
    parm_err = 0;
    info1543.sv1543_initsesstable = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1543,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1544;
    parm_err = 0;
    info1544.sv1544_initconntable = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1544,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1545;
    parm_err = 0;
    info1545.sv1545_initfiletable = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1545,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1546;
    parm_err = 0;
    info1546.sv1546_initsearchtable = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1546,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1547;
    parm_err = 0;
    info1547.sv1547_alertsched = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1547,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1548;
    parm_err = 0;
    info1548.sv1548_errorthreshold = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1548,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1549;
    parm_err = 0;
    info1549.sv1549_networkerrorthreshold = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1549,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1550;
    parm_err = 0;
    info1550.sv1550_diskspacethreshold = 0xFFFFFFFF;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1550,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1552;
    parm_err = 0;
    info1552.sv1552_maxlinkdelay = 0xFFFFFFFF;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1552,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1553;
    parm_err = 0;
    info1553.sv1553_minlinkthroughput = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1553,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_SUCCESS) netapi_fail(err);

    bufptr = (UINT8*)&info1554;
    parm_err = 0;
    info1554.sv1554_linkinfovalidtime = 0xFFFFFFFF;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1554,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1555;
    parm_err = 0;
    info1555.sv1555_scavqosinfoupdatetime = 0xFFFFFFFF;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1555,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    bufptr = (UINT8*)&info1556;
    parm_err = 0;
    info1556.sv1556_maxworkitemidletime = 0;
    CALL_NETAPI(err = NetServerSetInfo(pContext,
                                       hostname,/*servername*/
                                       1556,/*level*/
                                       bufptr,/*bufptr*/
                                       &parm_err/*parm_err*/
                                       ));
    if (err != ERROR_INVALID_PARAMETER) netapi_fail(err);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;
done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}

int TestNetRemoteTOD(struct test *t, const wchar16_t *hostname,
                     const wchar16_t *user, const wchar16_t *pass,
                     struct parameter *options, int optcount)
{
    NET_API_STATUS err = ERROR_SUCCESS;
    PSRVSVC_CONTEXT pContext = NULL;
    UINT8 *bufptr = NULL;

    TESTINFO(t, hostname, user, pass);

    SET_SESSION_CREDS(hCreds);

    err = SrvSvcCreateContext(hostname, &pContext);
    if (err) goto done;

    INPUT_ARG_PTR(pContext);
    INPUT_ARG_WSTR(hostname);

    bufptr = NULL;
    CALL_NETAPI(err = NetRemoteTOD(pContext,
                                   hostname,/*servername*/
                                   &bufptr/*bufptr*/
                                   ));
    if (err != ERROR_SUCCESS) netapi_fail(err);
    printf("bufptr[%p]\n", bufptr);

    OUTPUT_ARG_PTR(pContext);

    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }

    RELEASE_SESSION_CREDS;

    SrvSvcDestroyMemory();
    return TRUE;
done:
    SrvSvcDestroyMemory();
    if (pContext)
    {
        SrvSvcCloseContext(pContext);
    }
    return FALSE;
}

void SetupSrvSvcTests(struct test *t)
{
    SrvSvcInitMemory();

    AddTest(t, "SRVSVC-NET-CONNECTION-ENUM", TestNetConnectionEnum);
    AddTest(t, "SRVSVC-NET-FILE-ENUM", TestNetFileEnum);
    AddTest(t, "SRVSVC-NET-FILE-GET-INFO", TestNetFileGetInfo);
    AddTest(t, "SRVSVC-NET-FILE-CLOSE", TestNetFileClose);
    AddTest(t, "SRVSVC-NET-SESSION-ENUM", TestNetSessionEnum);
    AddTest(t, "SRVSVC-NET-SHARE-ADD", TestNetShareAdd);
    AddTest(t, "SRVSVC-NET-SHARE-ENUM", TestNetShareEnum);
    AddTest(t, "SRVSVC-NET-SHARE-GET-INFO", TestNetShareGetInfo);
    AddTest(t, "SRVSVC-NET-SHARE-SET-INFO", TestNetShareSetInfo);
    AddTest(t, "SRVSVC-NET-SHARE-DEL", TestNetShareDel);
    AddTest(t, "SRVSVC-NET-SERVER-GET-INFO", TestNetServerGetInfo);
    AddTest(t, "SRVSVC-NET-SERVER-SET-INFO", TestNetServerSetInfo);
    AddTest(t, "SRVSVC-NET-REMOTE-TOD", TestNetRemoteTOD);
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
