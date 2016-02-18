#include <cstdio>
#include <cstdlib>

#include <cctype>
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

#include "timestamp.h"

static const char *const TSSEND_FLAGS = ":n:";

int main(int argc, char *argv[])
{
        using std::printf;
        int option;
        const char *argument = nullptr;

        while (-1 != (option = getopt(argc, argv, TSSEND_FLAGS))) {
                printf("option =%3d (%c); optind = %d",
                       option, printable(option), optind);
                if ('?' == option || ':' == option) {
                        printf("; optopt =%3d (%c)", optopt, printable(optopt));
                }
                printf("\n");

                switch (option) {
                case 'n':
                        argument = optarg;
                        break;
                case ':':
                        break;
                case '?':
                        break;
                default:
                        break;
                }
        }
        /*
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        printf("lines %d\n", w.ws_row);
        printf("columns %d\n", w.ws_col);
        */
        return EXIT_SUCCESS;  // make sure your main returns int
}
