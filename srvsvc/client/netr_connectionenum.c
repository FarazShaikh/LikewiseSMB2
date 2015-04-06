/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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

#include "includes.h"

static
NET_API_STATUS
SrvSvcCopyNetConnCtr(
    UINT32             level,
    srvsvc_NetConnCtr* ctr,
    UINT32*            entriesread,
    UINT8**            bufptr
    );

NET_API_STATUS
NetrConnectionEnum(
    PSRVSVC_CONTEXT pContext,
    const wchar16_t *servername,
    const wchar16_t *qualifier,
    UINT32 level,
    UINT8 **bufptr,
    UINT32 prefmaxlen,
    UINT32 *entriesread,
    UINT32 *totalentries,
    UINT32 *resume_handle
    )
{
    NET_API_STATUS status = ERROR_SUCCESS;
    dcethread_exc* pDceException  = NULL;
    srvsvc_NetConnCtr ctr;
    srvsvc_NetConnCtr0 ctr0;
    srvsvc_NetConnCtr1 ctr1;
    UINT32 l = level;

    BAIL_ON_INVALID_PTR(pContext, status);
    BAIL_ON_INVALID_PTR(bufptr, status);
    BAIL_ON_INVALID_PTR(entriesread, status);
    BAIL_ON_INVALID_PTR(totalentries, status);

    memset(&ctr, 0, sizeof(ctr));
    memset(&ctr0, 0, sizeof(ctr0));
    memset(&ctr1, 0, sizeof(ctr1));
    *entriesread = 0;
    *bufptr = NULL;

    switch (level)
    {
        case 0:
            ctr.ctr0 = &ctr0;
            break;
        case 1:
            ctr.ctr1 = &ctr1;
            break;
        default:
            status = ERROR_INVALID_LEVEL;
            BAIL_ON_WIN_ERROR(status);
            break;
    }

    TRY
    {
        status = _NetrConnectionEnum(
                    pContext->hBinding,
                    (wchar16_t *)servername,
                    (wchar16_t *)qualifier,
                    &l,
                    &ctr,
                    prefmaxlen,
                    totalentries,
                    resume_handle);
    }
    CATCH_ALL(pDceException)
    {
        NTSTATUS ntStatus = LwRpcStatusToNtStatus(pDceException->match.value);
        status = LwNtStatusToWin32Error(ntStatus);
    }
    ENDTRY;
    BAIL_ON_WIN_ERROR(status);

    if (l != level)
    {
        status = ERROR_BAD_NET_RESP;
        BAIL_ON_WIN_ERROR(status);
    }

    status = SrvSvcCopyNetConnCtr(l, &ctr, entriesread, bufptr);
    BAIL_ON_WIN_ERROR(status);

cleanup:

    switch (level)
    {
        case 0:
            if (ctr.ctr0 == &ctr0) {
                ctr.ctr0 = NULL;
            }
            break;
        case 1:
            if (ctr.ctr1 == &ctr1) {
                ctr.ctr1 = NULL;
            }
            break;
    }

    SrvSvcClearNetConnCtr(l, &ctr);

    return status;

error:

    goto cleanup;
}

static
NET_API_STATUS
SrvSvcCopyNetConnCtr(
    UINT32             level,
    srvsvc_NetConnCtr* ctr,
    UINT32*            entriesread,
    UINT8**            bufptr
    )
{
    NET_API_STATUS status = ERROR_SUCCESS;
    int i;
    int count = 0;
    void *ptr = NULL;

    BAIL_ON_INVALID_PTR(entriesread, status);
    BAIL_ON_INVALID_PTR(bufptr, status);
    BAIL_ON_INVALID_PTR(ctr, status);

    *entriesread = 0;
    *bufptr = NULL;

    switch (level) {
    case 0:
        if (ctr->ctr0) {
            PCONNECTION_INFO_0 a0;

            count = ctr->ctr0->count;

            status = SrvSvcAllocateMemory(&ptr,
                                          sizeof(CONNECTION_INFO_0) * count,
                                          NULL);
            BAIL_ON_WIN_ERROR(status);

            a0 = (PCONNECTION_INFO_0)ptr;

            for (i=0; i < count; i++) {
                 a0[i] = ctr->ctr0->array[i];
            }
        }
        break;
    case 1:
        if (ctr->ctr1) {
            PCONNECTION_INFO_1 pConnectionInfo;

            count = ctr->ctr1->count;

            status = SrvSvcAllocateMemory(&ptr,
                                          sizeof(CONNECTION_INFO_1) * count,
                                          NULL);
            BAIL_ON_WIN_ERROR(status);

            pConnectionInfo = (PCONNECTION_INFO_1)ptr;

            for (i=0; i < count; i++)
            {
                 pConnectionInfo[i] = ctr->ctr1->array[i];

                 pConnectionInfo[i].coni1_username = NULL;
                 pConnectionInfo[i].coni1_netname  = NULL;

                 if (ctr->ctr1->array[i].coni1_username)
                 {
                     status = SrvSvcAddDepStringW(
                                 pConnectionInfo,
                                 ctr->ctr1->array[i].coni1_username,
                                 &pConnectionInfo[i].coni1_username);
                     BAIL_ON_WIN_ERROR(status);
                 }
                 if (ctr->ctr1->array[i].coni1_netname)
                 {
                     status = SrvSvcAddDepStringW(
                                 pConnectionInfo,
                                 ctr->ctr1->array[i].coni1_netname,
                                 &pConnectionInfo[i].coni1_netname);
                     BAIL_ON_WIN_ERROR(status);
                 }
            }
        }
        break;
    }

    *entriesread = count;
    *bufptr = (UINT8 *)ptr;

cleanup:
    return status;

error:
    if (ptr) {
        SrvSvcFreeMemory(ptr);
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
