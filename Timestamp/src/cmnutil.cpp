/**
 * @file cmnutil.cpp
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
 * Various utility function definitions.
 */

#include "cmnutil.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#ifdef __cplusplus
}
#endif

ssize_t bseq_read(int fd, void *seq, size_t count)
{
        char           *buffer      = reinterpret_cast<char *>(seq);
        ssize_t         bcount      = 0;
        ssize_t         breach      = 0;

        /*
         * Assume the underlying type of ssize_t and size_t are not
         * long long, unsigned long long, respectively;
         * the architecture must also support long long integer.
         */
        while (narrow_cast<long long, ssize_t>(bcount) <
               narrow_cast<long long, size_t>(count)) {
                breach = read(fd, seq, count - bcount);

                switch (breach) {
                case -1:
                        if (EINTR == errno) {
                                breach = 0;
                        } else {
                                return -1;
                        }
                        break;
                default:
                        bcount += breach;
                        buffer += breach;
                }
        }
        return bcount;
}

/* All code duplication generally should be avoided; but under this scenario
 * it is left this way (suggestions?).
 */
ssize_t bseq_write(int fd, const void *seq, size_t count)
{
        const char     *buffer      = reinterpret_cast<const char *>(seq);
        ssize_t         bcount      = 0;
        ssize_t         breach      = 0;

        while (narrow_cast<long long, ssize_t>(bcount) <
               narrow_cast<long long, size_t>(count)) {
                breach = write(fd, seq, count - bcount);

                switch (breach) {
                case -1:
                        if (EINTR == errno) {
                                breach = 0;
                        } else {
                                return -1;
                        }
                        break;
                default:
                        bcount += breach;
                        buffer += breach;
                }
        }
        return bcount;
}
