/**
 * @file tsutil.h
 * @author Jiahui Xie
 *
 * @section LICENSE
 *
 * Copyright © 2016 Jiahui Xie
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 *
 * Private header containing headers, and functions with internal linkages
 * used by timestamp.c.
 */

#if !defined(TSUTIL_H) && defined(TSONLY)
#define TSUTIL_H

#include <cerrno>    /* errno */
#include <cinttypes> /* strtoumax() */
#include <cstddef>   /* NULL */
#include <cstdint>   /* uintmax_t */
#include <cstdio>    /* fprintf() */
#include <cstdlib>   /* EXIT_FAILURE EXIT_SUCCESS */
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <getopt.h>  /* getopt_long() */
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include "cmnutil.h"
#include "timestamp.h"
#include "timestamp_tmp.h"

uintmax_t argument_parse(int *operating_mode, int argc, char *argv[]);
static uintmax_t count_validate(const char *const candidate);
static void usage(const char *name, int status, const char *msg = NULL);

#endif /* TSUTIL_H */
