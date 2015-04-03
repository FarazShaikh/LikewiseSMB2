/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

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
 *        async_handler.h
 *
 * Abstract:
 *
 *        Likewise Posix File System Driver (PVFS)
 *
 *        Async IRP dispatch routines
 *
 * Authors: Gerald Carter <gcarter@likewise.com>
 */

#ifndef _PVFS_ASYNC_HANDLER_H_
#define _PVFS_ASYNC_HANDLER_H_


/* Work Context API */

NTSTATUS
PvfsCreateWorkContext(
    OUT PPVFS_WORK_CONTEXT *ppWorkContext,
    IN  LONG lFlags,
    IN  PVOID pContext,
    IN  PPVFS_WORK_CONTEXT_CALLBACK pfnCompletion,
    IN  PPVFS_WORK_CONTEXT_FREE_CTX pfnFreeContext
    );

VOID
PvfsFreeWorkContext(
    IN OUT PPVFS_WORK_CONTEXT *ppWorkContext
    );

NTSTATUS
PvfsScheduleIoWorkItem(
    IN PPVFS_WORK_CONTEXT pWorkContext
    );


/* Cancellation routine */

VOID
PvfsQueueCancelIrp(
    PIRP pIrp,
    PVOID pCancelContext
    );

NTSTATUS
PvfsQueueCancelIrpIfRequested(
    PPVFS_IRP_CONTEXT pIrpContext
    );


/* Pvfs IRP Async API */

NTSTATUS
PvfsAsyncCreate(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncClose(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncLockControl(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncRead(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncWrite(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncFlushBuffers(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncQueryInformationFile(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncSetInformationFile(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncQueryDirInformation(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncQueryVolumeInformation(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncQuerySecurityFile(
    PPVFS_IRP_CONTEXT  pIrpContext
    );

NTSTATUS
PvfsAsyncSetSecurityFile(
    PPVFS_IRP_CONTEXT  pIrpContext
    );


/* Pvfs IRP Wrapper API */

VOID
PvfsIrpMarkPending(
    IN PPVFS_IRP_CONTEXT pIrpContext,
    IN PIO_IRP_CALLBACK CancelCallback,
    IN OPTIONAL PVOID CancelCallbackContext
    );

VOID
PvfsAsyncIrpComplete(
    PPVFS_IRP_CONTEXT pIrpContext
    );


#endif /* _PVFS_ASYNC_HANDLER_H_ */

/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
