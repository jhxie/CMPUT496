#include <cctype>
#include <cstdlib>
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

#include "cmnutil.h"

void usage_error(const char *prog_name, UsageError err_type, int option)
{
        using std::fprintf;
        if (nullptr != prog_name && 0 != option) {
                switch (err_type) {
                case UsageError::MISSING_ARGUMENT:
                        fprintf(stderr, "Missing argument");
                        break;
                case UsageError::UNRECOG_OPTION:
                        fprintf(stderr, "Unrecognized option");
                        break;
                }
                fprintf(stderr, "(-%c)\n", printable(option));
        }
        fprintf(stderr, "Usage: %s [-n arg]\n", prog_name);
        std::exit(EXIT_FAILURE);
}
