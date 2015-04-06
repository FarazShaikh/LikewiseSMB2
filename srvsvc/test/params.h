/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

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

#ifndef _PARAMS_H_
#define _PARAMS_H_

struct parameter {
    const char *key;
    const char *val;
};


enum param_type {
    pt_string = 1,
    pt_w16string,
    pt_char,
    pt_int32,
    pt_uint32,
    pt_sid
};


enum param_err {
    perr_not_found = 1,
    perr_invalid_out_param = 2,
    perr_unknown_type = 3,
    perr_nullptr_passed = 4,

    perr_success = 0,
    perr_unknown = -1
};


struct param_errstr_map {
    enum param_err perr;
    const char* desc;
};

static const
struct param_errstr_map param_errstr_maps[] = {
    { perr_not_found, "parameter not found" },
    { perr_invalid_out_param, "invalid output parameter" },
    { perr_unknown_type, "unknown parameter type" },
    { perr_nullptr_passed, "null pointer passed" },
    { perr_success, "success" },
    { perr_unknown, "unknown error" }
};

const char *param_errstr(enum param_err perr);


#define perr_is_ok(perr_code)  ((perr_code) == perr_success)
#define perr_fail(perr_code) { \
	printf("Parameter error: %s\n", param_errstr(perr_code)); \
	return FALSE; \
    }


struct parameter* get_optional_params(char *opt, int *count);
const char* find_value(struct parameter *params, int count, const char *key);
enum param_err fetch_value(struct parameter *params, int count,
			   const char *key, enum param_type type, void *val,
			   const void *def);


#endif /* _PARAMS_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
