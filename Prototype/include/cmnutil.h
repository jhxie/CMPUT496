#ifndef CMNUTIL_H
#define CMNUTIL_H

/**
 * @file cmnutil.h
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
 * Common utility header defines various macros used for feature testing
 * and error checking.
 * This header is based on an existing header the author wrote for another
 * project but modified slightly to let the feature test macro test c++11
 * features instead of c99.
 */

/*
                       +-------------------------------+
                       |Compiler/Libc Test Conditionals|
                       +-------------------------------+
*/
#include <features.h>
/*
 * For gcc, c++11 is fully supported since 5.1, so a test is performed
 * in the following.
 *
 * To avoid erroneous stop of compilation behavior from llvm-clang,
 * an extra macro is tested as well due to the fact that clang also
 * defines __GNUC__ if invoked with "-std=gnu++11" flag.
 *
 * A similar test can be performed for clang, but the technique is
 * not known for the author(jiahui) of this software so it is left out,
 * but as long as you are not using some ancient versions you should be fine.
 *
 * Reference
 * https://gcc.gnu.org/projects/cxx0x.html
 */
#if !defined(__clang__) && defined(__GNUC__)
#if 5 > __GNUC__
#error "<" __FILE__ "> GCC Version Test -- [ FAIL ]"
#error "This software requires gcc to be at least 5.1"
#elif 5 == __GNUC__ && 1 > __GNUC_MINOR__
#error "<" __FILE__ "> GCC Version Test -- [ FAIL ]"
#error "This software requires gcc to be at least 5.1"
#endif
#endif

/* secure_getenv first appeared in glibc 2.17 */
#if defined(__GLIBC__)
#if 2 > __GLIBC__
#error "<" __FILE__ "> GLIBC Version Test -- [ FAIL ]"
#error "This software requires the glibc to be at least 2.17"
#elif 2 == __GLIBC__ && 17 > __GLIBC_MINOR__
#error "<" __FILE__ "> GLIBC Version Test -- [ FAIL ]"
#error "This software requires the glibc to be at least 2.17"
#endif
#endif


/*
                             +-------------------+
                             |Feature Test Macros|
                             +-------------------+
*/
#define _POSIX_C_SOURCE 200809L


/*
                                +--------------+
                                |Utility Macros|
                                +--------------+
*/

#include <cerrno>  /* errno */
#include <cstdio>  /* fprintf() */
#include <cstdlib> /* abort() */
#include <cstdlib> /* strerror() */

/* ANSI Color Escape Sequences */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define CMNUTIL_ERRABRT(expr) \
        do { \
                int status__ = 0; \
                if (0 != (status__ = (expr))) { \
                        std::fprintf(stderr, \
                                     "[%s] on line [%d] within function [%s]" \
                                     "in file [%s]: %s\n", \
                                     #expr, __LINE__, __func__, \
                                     __FILE__, std::strerror(status__)); \
                        std::abort(); \
                } \
        } while (0)


#define CMNUTIL_ERRNOABRT(experr, expr) \
        do { \
                if ((experr) == (expr)) { \
                        std::fprintf(stderr, \
                                     "[%s] on line [%d] within function [%s]" \
                                     "in file [%s]: %s\n", \
                                     #expr, __LINE__, __func__, \
                                     __FILE__, std::strerror(errno)); \
                        std::abort(); \
                } \
        } while (0)
/*
 * The following requires gnu++11 support since compound literals are
 * not supported officially in c++11.
 */
#define CMNUTIL_STREACH(iterator_, ...) \
        for (char **iterator_ = (char *[]){__VA_ARGS__, NULL}; \
             *iterator_; \
             ++iterator_)

#define CMNUTIL_ZFREE(ptr) \
        do { \
                std::free(ptr); \
                ptr = NULL; \
        } while (0)

/*
                            +-----------------------+
                            |Enumeration Definitions|
                            +-----------------------+
*/
enum class UsageError : size_t {
        MISSING_ARGUMENT = 0U,
        UNRECOG_OPTION
};

/*
                            +---------------------+
                            |Function Declarations|
                            +---------------------+
*/
void usage_error(const char *prog_name, UsageError err_type, int option);
#endif /* CMNUTIL_H */
