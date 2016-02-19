#include <cstdio>
#include <cstdlib>
#include <string>

#include "cmnutil.h"
#include "timestamp.h"

void usage(const char *name, int status = EXIT_FAILURE, const char *msg = NULL)
{
        using std::fprintf;

        if (NULL != msg) {
                fprintf(stderr, "%s\n", msg);
        }
        fprintf(stderr,
                "[" ANSI_COLOR_RED "Usage" ANSI_COLOR_RESET "]\n"
                "%s [-h] [-c MESSAGE_COUNT]\n\n"
                "Sends a message containing a timestamp to stdout "
                ANSI_COLOR_RED "MESSAGE_COUNT" ANSI_COLOR_RESET " times and\n"
                "simultaneously receives message from stdin and write the "
                "result to a file\n"
                "indicated by an environment variable named "
                ANSI_COLOR_RED "TSSEND_OUTPUT" ANSI_COLOR_RESET ".\n\n"
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
         * multi-process or multi-thread programs to avoid memory leaks.
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
                        puts(optarg);
                        break;
                case '?':
                        usage(PROGRAM_NAME.c_str(),
                              EXIT_FAILURE,
                              "There is no such option!\n");
                case ':':
                        usage(PROGRAM_NAME.c_str(),
                              EXIT_FAILURE,
                              "Missing Argument");
                case 'h':
                default:
                        usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, NULL);
                }
        }

        if (1 == argc) {
                usage(PROGRAM_NAME.c_str(), EXIT_FAILURE, NULL);
        }
        /*
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        printf("lines %d\n", w.ws_row);
        printf("columns %d\n", w.ws_col);
        */
        return EXIT_SUCCESS;
}
