/**
 * @file timestamp_tmp.h
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
 * Template private header for the main sending functionality of the
 * tssend program.
 */

#ifndef TIMESTAMP_TMP_H
#define TIMESTAMP_TMP_H

#include <ctime>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#ifdef __cplusplus
}
#endif /* TIMESTAMP_TMP_H */

/*
 * Note the while loop calling write() is based on the
 * "BSD Sockets: A Quick And Dirty Primer" by Jim Frost.
 */
template<typename IntType>
auto tssend(IntType send_count) -> decltype(send_count + 1)
{
        IntType         i           = 0;
        bool            force_break = false;
        /* buffer is declared as a char * type to avoid undefined behavior
         * of doing pointer arithmetic on a void * type.
         */
        char           *buffer      = NULL;
        ssize_t         bcount      = 0;
        ssize_t         bwrite      = 0;
        struct timespec begin       = { };

        memset(&begin, 0, sizeof begin);

        for (i = 0; false == force_break && i < send_count; ++i) {
                switch(clock_gettime(CLOCK_REALTIME, &begin)) {
                case 0:
                        break;
                case -1:
                        force_break = true;
                        goto tssend_end;
                }
                bcount = 0;
                bwrite = 0;
                buffer = reinterpret_cast<char *>(&begin);
                while (static_cast<long long>(bcount) <
                       static_cast<long long>(sizeof begin)) {
                        bwrite = write(STDOUT_FILENO,
                                       buffer,
                                       (sizeof begin) - bcount);
                        if (-1 == bwrite) {
                                force_break = true;
                                goto tssend_end;
                        } else if (0 < bwrite) {
                                bcount += bwrite;
                                buffer += bwrite;
                        }
                }
        }
tssend_end:
        return force_break == false ? send_count + 1 : send_count;
}

#endif
