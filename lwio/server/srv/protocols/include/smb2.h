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
 *        smb2.h
 *
 * Abstract:
 *
 *        Likewise IO (LWIO) - SRV
 *
 *        SMB V2 Protocol Handler API
 *
 * Authors: Sriram Nambakam (snambakam@likewise.com)
 *
 */

#ifndef __SMB_V2_H__
#define __SMB_V2_H__

NTSTATUS
SrvProtocolInit_SMB_V2(
    PSMB_PROD_CONS_QUEUE pAsyncWorkQueue
    );;

NTSTATUS
SrvProtocolExecute_SMB_V2(
    PSRV_EXEC_CONTEXT pExecContext
    );

NTSTATUS
SrvProtocolCloseFile_SMB_V2(
    PLWIO_SRV_TREE_2 pTree,
    PLWIO_SRV_FILE_2 pFile
    );

VOID
SrvProtocolFreeContext_SMB_V2(
    PSRV_EXEC_CONTEXT_SMB_V2 pProtocolContext
    );

NTSTATUS
SrvBuildNegotiateResponse_SMB_V2(
    IN  PLWIO_SRV_CONNECTION pConnection,
    IN  PSMB_PACKET          pSmbRequest,
    OUT PSMB_PACKET*         ppSmbResponse
    );

VOID
SrvProtocolShutdown_SMB_V2(
    VOID
    );

NTSTATUS
SrvDetectZctWrite_SMB_V2(
    IN PSRV_CONNECTION pConnection,
    IN PSMB_PACKET pPacket,
    OUT PSRV_EXEC_CONTEXT* ppZctExecContext
    );

#endif /* __SMB_V2_H__ */
