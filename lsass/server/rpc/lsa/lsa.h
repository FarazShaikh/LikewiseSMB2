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

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        lsa.h
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        Lsa rpc server functions
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#ifndef _LSASRV_H_
#define _LSASRV_H_


NTSTATUS
LsaSrvClose(
    handle_t b,
    POLICY_HANDLE *hInOut
    );


NTSTATUS
LsaSrvOpenPolicy2(
    handle_t b,
    wchar16_t *system_name,
    ObjectAttribute *attrib,
    UINT32 access_mask,
    POLICY_HANDLE *phPolicy
    );


NTSTATUS
LsaSrvLookupNames(
    handle_t b,
    POLICY_HANDLE hPolicy,
    UINT32 num_names,
    UNICODE_STRING *names,
    RefDomainList **domains,
    TranslatedSidArray *sids,
    UINT16 level,
    UINT32 *count
    );


NTSTATUS
LsaSrvLookupSids(
    handle_t b,
    POLICY_HANDLE hPolicy,
    SID_ARRAY *sids,
    RefDomainList **domains,
    TranslatedNameArray *names,
    UINT16 level,
    UINT32 *count
    );


NTSTATUS
LsaSrvLookupNames2(
    handle_t b,
    POLICY_HANDLE hPolicy,
    UINT32 num_names,
    UNICODE_STRING *names,
    RefDomainList **domains,
    TranslatedSidArray2 *sids,
    UINT16 level,
    UINT32 *count,
    UINT32 unknown1,
    UINT32 unknown2
    );


NTSTATUS
LsaSrvLookupSids2(
    handle_t hBinding,
    POLICY_HANDLE hPolicy,
    SID_ARRAY *sids,
    RefDomainList **domains,
    TranslatedNameArray2 *names,
    UINT16 level,
    UINT32 *count,
    UINT32 unknown1,
    UINT32 unknown2
    );


NTSTATUS
LsaSrvQueryInfoPolicy(
    handle_t b,
    POLICY_HANDLE hPolicy,
    UINT16 level,
    LsaPolicyInformation **info
    );


NTSTATUS
LsaSrvQueryInfoPolicy2(
    handle_t b,
    POLICY_HANDLE hPolicy,
    UINT16 level,
    LsaPolicyInformation **info
    );


NTSTATUS
LsaSrvLookupNames3(
    handle_t b,
    POLICY_HANDLE hPolicy,
    UINT32 num_names,
    UNICODE_STRING *names,
    RefDomainList **domains,
    TranslatedSidArray3 *sids,
    UINT16 level,
    UINT32 *count,
    UINT32 unknown1,
    UINT32 unknown2
    );


#endif /* _LSASRV_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
