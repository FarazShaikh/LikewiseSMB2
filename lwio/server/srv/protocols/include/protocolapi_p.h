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
 *        protocolapi_p.h
 *
 * Abstract:
 *
 *        Likewise IO (LWIO) - SRV
 *
 *        Protocol API (Private to protocol handlers)
 *
 * Authors: Sriram Nambakam (snambakam@likewise.com)
 *
 */

#ifndef __PROTOCOL_API_P_H__
#define __PROTOCOL_API_P_H__

typedef ULONG SRV_PERM_FILE;

#define SRV_PERM_FILE_NONE   0x0
#define SRV_PERM_FILE_READ   0x1
#define SRV_PERM_FILE_WRITE  0x2
#define SRV_PERM_FILE_CREATE 0x4

typedef struct _SRV_EXEC_CONTEXT_SMB_V1* PSRV_EXEC_CONTEXT_SMB_V1;
typedef struct _SRV_EXEC_CONTEXT_SMB_V2* PSRV_EXEC_CONTEXT_SMB_V2;

typedef struct _SRV_PROTOCOL_EXEC_CONTEXT
{
    SMB_PROTOCOL_VERSION protocolVersion;

    union
    {
        PSRV_EXEC_CONTEXT_SMB_V1 pSmb1Context;
        PSRV_EXEC_CONTEXT_SMB_V2 pSmb2Context;
    };

} SRV_PROTOCOL_EXEC_CONTEXT;

#endif /* __PROTOCOL_API_P_H__ */

