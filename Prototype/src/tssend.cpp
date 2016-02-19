/**
 * @file tssend.cpp
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
 * Main source file of the sender side of the timestamp program.
 */

#include <cerrno>    /* errno */
#include <cinttypes> /* strtoumax() */
#include <cstdint>   /* uintmax_t */
#include <cstdio>    /* fprintf() */
#include <cstdlib>   /* EXIT_FAILURE EXIT_SUCCESS */
#include <string>

#include "cmnutil.h"
#include "timestamp.h"
#include "tssend_tmp.h"

static uintmax_t input_validate(const char *const candidate)
{
        char *endptr = NULL;
        errno = 0;
        uintmax_t result = strtoumax(candidate, &endptr, 10);

        /* Check overflow */
        if (UINTMAX_MAX == result && ERANGE == errno) {
                return 0U;
        /*
         * From the manual page of strtoumax(),
         * "
         * if there were no digits at all, strtoul() stores the original value
         * of nptr in endptr (and returns 0)
         * "
         * so an argument with some numbers mixed-in would work in this case.
         */
        } else if (0U == result && endptr == candidate) {
                return 0U;
        }
        return result;
}

static void usage(const char *name, int status, const char *msg = NULL)
{
        using std::fprintf;

        if (NULL != msg) {
                fprintf(stderr,
                        "[" ANSI_COLOR_CYAN "Error" ANSI_COLOR_RESET "]\n"
                        "%s\n\n",
                        msg);
        }
        fprintf(stderr,
                "[" ANSI_COLOR_CYAN "Usage" ANSI_COLOR_RESET "]\n"
                "%s [-h] [-c MESSAGE_COUNT]\n\n"
                "Sends a message containing a timestamp to stdout "
                ANSI_COLOR_MAGENTA "MESSAGE_COUNT" ANSI_COLOR_RESET " times.\n"
#if 0
                "simultaneously receives message from stdin and write the "
                "result to a file\n"
                "indicated by an environment variable named "
                ANSI_COLOR_MAGENTA "TSSEND_OUTPUT" ANSI_COLOR_RESET ".\n\n"
#endif
                ANSI_COLOR_MAGENTA "NOTE" ANSI_COLOR_RESET "\n"
                "It would print gibberish if shell redirection isn't used!\n\n"
                "[" ANSI_COLOR_CYAN "Optional Arguments" ANSI_COLOR_RESET "]\n"
                "-h, --help\tshow this help message and exit\n"
                "-c, --count\tnumber of messages to be sent\n",
                NULL == name ? "" : name);
        exit(status);
}

int main(int argc, char *argv[])
{
        using std::fprintf;
        using std::printf;
        using std::string;

        int                         opt            = 0;
        uintmax_t                   send_count     = 0U;
        /*
         * Prohibit getopt_long() from printing error message of its own by
         * prefixing the optstring formal parameter (TSSEND_FLAGS actual
         * argument in this case) by a colon.
         */
        static const char *const    TSSEND_FLAGS   = ":c:";
        /*
         * From the manual page (section 3) of getopt(),
         * "by  default, getopt() permutes the contents of argv as it scans",
         * so a deep copy is required here.
         * Alternatively use strdup() and followed by a free() later on; but
         * personally I think RAII is a better approach especially for
         * multi-process or multi-thread programs to avoid many potential bugs.
         */
        const string                PROGRAM_NAME   = string(argv[0]);
        /*
         * There is no size indicator given to getopt_long(), so an extra
         * "empty" member is padded, similar to the convention of a c string.
         * To maintain compatibility with c, NULL is used rather than c++11's
         * recommendation, nullptr.
         */
        static const struct option  long_options[] = {
                {"count", required_argument, NULL, 'c'},
                {"help",  no_argument,       NULL, 'h'},
                {NULL,    0,                 NULL,  0}
        };

        while (-1 != (opt = getopt_long(argc,
                                        argv,
                                        TSSEND_FLAGS,
                                        long_options,
                                        NULL))) {
                switch (opt) {
                case 'c':
                        send_count = input_validate(optarg);
                        break;
                case '?':
                        usage(PROGRAM_NAME.c_str(),
                              EXIT_FAILURE,
                              "There is no such option!");
                case ':':
                        usage(PROGRAM_NAME.c_str(),
                              EXIT_FAILURE,
                              "Missing argument!");
                case 'h':
                default:
                        usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, NULL);
                }
        }

        if (1 == argc) {
                usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, NULL);
        }

        /*
         * If the above branch is taken, all the code following would NEVER
         * be executed since usage does not return to its caller.
         */
        if (0U == send_count) {
                usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, "Invalid argument!");
        }
        /* Rely on implicit instantiation to call template function tssend()
         * with type uintmax_t; note the returned actual number of timestamps
         * written is not checked.
         */
        tssend<uintmax_t>(send_count);
        return EXIT_SUCCESS;
}
