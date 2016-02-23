/**
 * @file timestamp.cpp
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
 * Main source file of the timestamp program.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* All the depedendent headers are put into a separate private header. */
#define  TSONLY
#include "tsutil.h"
#undef  TSONLY

int main(int argc, char *argv[])
{
        using std::fprintf;
        using std::printf;
        using std::string;

        /*
         * The reason not to use enumeration instead is c++ has stronger
         * type checking than c does: static_cast between enumerator and int
         * is potentially unsafe when used within getopt_long(), have to stick
         * to the basics.
         */
#define RECEIVER    'r'
#define SENDER      's'
#define UNSPECIFIED  0
        int                         operating_mode   = UNSPECIFIED;
        int                         opt              = 0;
        uintmax_t                   send_recv_count  = 0U;
        /*
         * Prohibit getopt_long() from printing error message of its own by
         * prefixing the optstring formal parameter (TSSEND_FLAGS actual
         * argument in this case) by a colon.
         */
        static const char *const    TSSEND_FLAGS     = ":c:rs";
        /*
         * From the manual page (section 3) of getopt(),
         * "by default, getopt() permutes the contents of argv as it scans",
         * so a deep copy is required here.
         * Alternatively use strdup() and followed by a free() later on; but
         * personally I think RAII is a better approach especially for (future)
         * multi-process or multi-thread programs to avoid many potential bugs.
         */
        const string                PROGRAM_NAME     = string(argv[0]);
        /*
         * There is no size indicator given to getopt_long(), so an extra
         * "empty" member is padded, similar to the convention of a c string.
         * To maintain compatibility with c, NULL is used rather than c++11's
         * recommendation, nullptr.
         * Note the designated initializer is a c99 feature, but in c++ it is
         * not standardized because POD is not encouraged to be used in c++,
         * however gcc supports it as an extension regardless.
         * For better readability it is kept this way with some portability
         * sacrificed.
         */
        static const struct option  long_options[] = {
                {
                        .name    = "count",
                        .has_arg = required_argument,
                        .flag    = NULL,
                        .val     = 'c'
                },
                {
                        .name    = "help",
                        .has_arg = no_argument,
                        .flag    = NULL,
                        .val     = 'h'},
                {
                        .name    = "receiver",
                        .has_arg = no_argument,
                        .flag    = NULL,
                        .val     = 'r'
                },
                {
                        .name    = "sender",
                        .has_arg = no_argument,
                        .flag    = NULL,
                        .val     = 's'
                },
                {NULL,       0,                 NULL,   0}
        };

        while (-1 != (opt = getopt_long(argc,
                                        argv,
                                        TSSEND_FLAGS,
                                        long_options,
                                        NULL))) {
                switch (opt) {
                /* 'case 0:' is for 'sender' and 'receiver' flag;
                 * even though nothing needs to be done it is listed here
                 * because explicit is better than implicit.
                 * */
                case 0:
                        break;
                case 'c':
                        send_recv_count = input_validate(optarg);
                        break;
                case 'r':
                case 's':
                        operating_mode = opt;
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

        if (1 == argc || UNSPECIFIED == operating_mode) {
                usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, NULL);
        }

        /*
         * If the above branch is taken, all the code following would NEVER
         * be executed since usage does not return to its caller.
         */
        if (0U == send_recv_count) {
                usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, "Invalid argument!");
        }

        if (NULL == secure_getenv(ENV_TIMESTAMP_OUTPUT)) {
                usage(PROGRAM_NAME.c_str(),
                      EXIT_FAILURE,
                      "Environment variable missing!");
        }
        /* Rely on implicit instantiation to call template function tssend()
         * with type uintmax_t; note the returned actual number of timestamps
         * written is not checked.
         */
        switch (operating_mode) {
        case RECEIVER:
                timestamp<uintmax_t>(send_recv_count, TimeStampMode::RECEIVE);
                break;
        case SENDER:
                timestamp<uintmax_t>(send_recv_count, TimeStampMode::SEND);
        }
        return EXIT_SUCCESS;
#undef RECEIVER
#undef SENDER
#undef UNSPECIFIED
}
struct timespec *timestamp_manipulate(struct timespec *ts, TimeStampMode mode)
{
        /*
         * Buffer is declared as a char * type to avoid undefined behavior
         * of doing pointer arithmetic on a void * type.
         */
        char           *buffer      = NULL;
        ssize_t         bcount      = 0;
        ssize_t         breach      = 0;
        struct timespec receiver_ts = { };

        memset(&receiver_ts, 0, sizeof(struct timespec));

        switch (mode) {
        case TimeStampMode::RECEIVE:
                buffer = reinterpret_cast<char *>(&receiver_ts);
                break;
        case TimeStampMode::SEND:
                buffer = reinterpret_cast<char *>(ts);
        }

        /*
         * Assume the underlying type of ssize_t and size_t are not
         * long long, unsigned long long, respectively;
         * the architecture must also support long long integer.
         */
        while (narrow_cast<long long, ssize_t>(bcount) <
               narrow_cast<long long, size_t>(sizeof(struct timespec))) {
                switch (mode) {
                case TimeStampMode::RECEIVE:
                        breach = read(STDIN_FILENO,
                                      buffer,
                                      (sizeof(struct timespec)) - bcount);
                        break;
                case TimeStampMode::SEND:
                        breach = write(STDOUT_FILENO,
                                       buffer,
                                       (sizeof(struct timespec)) - bcount);
                        break;
                }
                if (-1 == breach) {
                        return NULL;
                } else if (0 < breach) {
                        bcount += breach;
                        buffer += breach;
                }
        }

        /*
         * For receivers, an update to the current time is required since
         * the above read() system call could possibly block for a long time
         * especially across a lossy network.
         */
        if (TimeStampMode::RECEIVE == mode) {
                switch (clock_gettime(CLOCK_REALTIME, ts)) {
                case 0:
			/* Paul */
                        printf("X1:  %lld,%ld\n",
                                ts->tv_sec,
                                ts->tv_nsec);
                        printf("X2:  %lld,%ld\n",
                                receiver_ts.tv_sec,
                                receiver_ts.tv_nsec);
                        printf("X3:  %lld,%ld\n",
                                ts->tv_sec - receiver_ts.tv_sec,
                                ts->tv_nsec - receiver_ts.tv_nsec);

                        /* Both fields are arithmetic types */
                        ts->tv_sec = ts->tv_sec - receiver_ts.tv_nsec;
                        ts->tv_nsec = ts->tv_nsec - receiver_ts.tv_nsec;

			/* Paul */
                        printf("X:  %lld,%ld\n",
                                ts->tv_sec,
                                ts->tv_nsec);

                        break;
                case -1:
                        return NULL;
                }
        }
        return ts;
}

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

static void usage(const char *name, int status, const char *msg)
{
        using std::fprintf;

        if (NULL != msg) {
                fprintf(stderr,
                        "[" ANSI_COLOR_BLUE "Error" ANSI_COLOR_RESET "]\n"
                        "%s\n\n",
                        msg);
        }
        fprintf(stderr,
                "[" ANSI_COLOR_BLUE "Usage" ANSI_COLOR_RESET "]\n"
                "%s [-h] [-r | -s] [-c MESSAGE_COUNT]\n\n"

                "<" ANSI_COLOR_CYAN "Receiver Mode" ANSI_COLOR_RESET ">\n"
                "Receives messages containing timestamps from stdin "
                ANSI_COLOR_MAGENTA "MESSAGE_COUNT" ANSI_COLOR_RESET
                " times.\n\n"

                "<" ANSI_COLOR_CYAN "Sender   Mode" ANSI_COLOR_RESET ">\n"
                "Sends messages containing timestamps to stdout "
                ANSI_COLOR_MAGENTA "MESSAGE_COUNT" ANSI_COLOR_RESET
                " times.\n\n"
#if 0
                "simultaneously receives message from stdin and write the "
                "result to a file\n"
                "indicated by an environment variable named "
                ANSI_COLOR_MAGENTA "TSSEND_OUTPUT" ANSI_COLOR_RESET ".\n\n"
#endif
                "[" ANSI_COLOR_BLUE "Optional Arguments" ANSI_COLOR_RESET "]\n"
                "-h, --help\tshow this help message and exit\n"
                "-r, --receiver\toperates in receiver mode\n"
                "-s, --sender\toperates in sender mode\n"
                "-c, --count\tnumber of messages to be sent\n"
                "\n[" ANSI_COLOR_BLUE "NOTE" ANSI_COLOR_RESET "]\n"
                "1. Environment variable "
                ANSI_COLOR_MAGENTA ENV_TIMESTAMP_OUTPUT ANSI_COLOR_RESET
                " needs to be set for receiver to work properly.\n"
                "2. It will print gibberish if shell redirection isn't used"
                " on the sender side.\n",
                NULL == name ? "" : name);
        exit(status);
}

