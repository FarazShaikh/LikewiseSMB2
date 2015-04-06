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
 *        types.h
 *
 * Abstract:
 *
 *        Likewise Server Service (srvsvc) RPC client and server
 *
 *        Type definitions
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */


#ifndef _SRVSVC_TYPES_H_
#define _SRVSVC_TYPES_H_

#include <lw/types.h>
#include <lw/ntstatus.h>

typedef UINT32 WINERR;
typedef UINT64 NtTime;


#ifndef NET_API_STATUS_DEFINED
#define NET_API_STATUS_DEFINED 1

#ifdef _DCE_IDL_
cpp_quote("#ifndef NET_API_STATUS_DEFINED")
cpp_quote("#define NET_API_STATUS_DEFINED 1")
#endif

typedef UINT32 NET_API_STATUS;

#ifdef _DCE_IDL_
cpp_quote("#endif")
#endif

#endif


/* Don't require DCE/RPC environment when simply building
   a client using rpc library */
#if !defined(_DCE_IDL_)
#if defined(SRVSVC_BUILD)
#include <dce/rpc.h>

typedef unsigned32 RPCSTATUS;

#else
typedef void* handle_t;
typedef unsigned long error_status_t;

#if SIZEOF_LONG_INT == 8
typedef unsigned int      RPCSTATUS;
#else /* SIZEOF_LONG_INT == 4 */
typedef unsigned long int RPCSTATUS;
#endif

#endif /* defined(SRVSVC_BUILD) */
#endif /* !defined(_DCE_IDL_) */

#endif /* _SRVSVC_TYPES_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
