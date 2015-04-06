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

NET_API_STATUS
NetrServerGetInfo(
    PSRVSVC_CONTEXT pContext,        /* IN             */
    PCWSTR          pwszServername,  /* IN    OPTIONAL */
    DWORD           dwInfoLevel,     /* IN             */
    PBYTE*          ppBuffer         /*    OUT         */
    );

NET_API_STATUS
NetrServerSetInfo(
    PSRVSVC_CONTEXT pContext,        /* IN              */
    PCWSTR          pwszServername,  /* IN     OPTIONAL */
    DWORD           dwInfoLevel,     /* IN              */
    PBYTE           pBuffer,         /* IN              */
    PDWORD          pdwParmError     /*    OUT OPTIONAL */
    );

NET_API_STATUS
NetShareAdd(
    IN  PCWSTR  pwszServername,
    IN  DWORD   dwLevel,
    IN  PBYTE   pBuffer,
    OUT PDWORD  pdwParmErr
    );

NET_API_STATUS
NetShareDel(
    IN  PCWSTR  pwszServername,
    IN  PCWSTR  pwszSharename,
    IN  DWORD   dwReserved
    );

NET_API_STATUS
NetShareEnum(
    IN  PCWSTR   pwszServername,
    IN  DWORD    dwLevel,
    OUT PBYTE   *ppBuffer,
    IN  DWORD    dwMaxLen,
    OUT PDWORD   pdwNumEntries,
    OUT PDWORD   pdwTotalEntries,
    OUT PDWORD   pdwResume
    );

NET_API_STATUS
NetShareGetInfo(
    IN  PCWSTR  pwszServername,
    IN  PCWSTR  pwszNetname,
    IN  DWORD   dwLevel,
    OUT PBYTE  *ppBuffer
    );

NET_API_STATUS
NetShareSetInfo(
    IN  PCWSTR     pwszServername,
    IN  PCWSTR     pwszNetname,
    IN  DWORD      dwLevel,
    IN  PBYTE      pBuffer,
    OUT PDWORD     pdwParmErr
    );

NET_API_STATUS
NetConnectionEnum(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN     OPTIONAL */
    PCWSTR          pwszQualifier,     /* IN              */
    DWORD           dwInfoLevel,       /* IN              */
    PBYTE*          ppBuffer,          /*    OUT          */
    DWORD           dwPrefmaxlen,      /* IN              */
    PDWORD          pdwEntriesRead,    /*    OUT          */
    PDWORD          pdwTotalEntries,   /*    OUT          */
    PDWORD          pdwResumeHandle    /* IN OUT OPTIONAL */
    );

NET_API_STATUS
NetSessionEnum(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN     OPTIONAL */
    PCWSTR          pwszUncClientname, /* IN     OPTIONAL */
    PCWSTR          pwszUsername,      /* IN     OPTIONAL */
    DWORD           dwInfoLevel,       /* IN              */
    PBYTE*          ppBuffer,          /*    OUT          */
    DWORD           dwPrefmaxLen,      /* IN              */
    PDWORD          pdwEntriesRead,    /*    OUT          */
    PDWORD          pdwTotalEntries,   /*    OUT          */
    PDWORD          pdwResumeHandle    /* IN OUT OPTIONAL */
    );

NET_API_STATUS
NetFileEnum(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN    OPTIONAL  */
    PCWSTR          pwszBasepath,      /* IN    OPTIONAL  */
    PCWSTR          pwszUsername,      /* IN    OPTIONAL  */
    DWORD           dwInfoLevel,       /* IN              */
    PBYTE*          ppBuffer,          /*    OUT          */
    DWORD           dwPrefmaxlen,      /* IN              */
    PDWORD          pwdEntriesRead,    /*    OUT          */
    PDWORD          pdwTotalEntries,   /*    OUT          */
    PDWORD          pdwResumeHandle    /* IN OUT OPTIONAL */
    );

NET_API_STATUS
NetFileGetInfo(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN    OPTIONAL  */
    DWORD           dwFileId,          /* IN              */
    DWORD           dwInfoLevel,       /* IN              */
    PBYTE*          ppBuffer           /*    OUT          */
    );

NET_API_STATUS
NetFileClose(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN    OPTIONAL  */
    DWORD           dwFileId           /* IN              */
    );

NET_API_STATUS
NetRemoteTOD(
    PSRVSVC_CONTEXT pContext,          /* IN              */
    PCWSTR          pwszServername,    /* IN    OPTIONAL  */
    PBYTE*          ppBuffer           /*    OUT          */
    );
