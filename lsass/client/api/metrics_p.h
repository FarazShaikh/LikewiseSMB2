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
 *        groups_p.h
 *
 * Abstract:
 * 
 *        Likewise Security and Authentication Subsystem (LSASS)
 * 
 *        Private Header (Library)
 * 
 *        Metric Lookup and Management API (Client)
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#ifndef __ARTEFACTS_P_H__
#define __ARTEFACTS_P_H__

typedef struct __LSA_ENUM_PERF_METRICS_INFO
{
    DWORD dwMetricInfoLevel;
    DWORD dwNumMaxMetrics;
    PSTR  pszGUID;
} LSA_ENUM_PERF_METRICS_INFO, *PLSA_ENUM_PERF_METRICS_INFO;

LSASS_API
DWORD
LsaBeginEnumMetrics(
    HANDLE  hLsaConnection,
    DWORD   dwMetricInfoLevel,
    DWORD   dwMaxNumMetrics,
    PHANDLE phResume
    );

LSASS_API
DWORD
LsaEnumMetrics(
    HANDLE  hLsaConnection,
    HANDLE  hResume,
    PDWORD  pdwNumMetricsFound,
    PVOID** pppMetricInfoList
    );

LSASS_API
DWORD
LsaEndEnumMetrics(
    HANDLE hLsaConnection,
    HANDLE hResume
    );

DWORD
LsaValidateMetricInfoLevel(
    DWORD dwMetricInfoLevel
    );

VOID
LsaFreeEnumMetricsInfo(
    PLSA_ENUM_PERF_METRICS_INFO pInfo
    );

#endif /* __PERF_METRICS_P_H__ */

