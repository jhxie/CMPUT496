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
 * Template header for the main sending and receiving functionality of
 * the timestamp program; along with a utility template function for doing
 * conversion between two scalar numeric types.
 */

#ifndef TIMESTAMP_TMP_H
#define TIMESTAMP_TMP_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "timestamp.h"


#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>     /* clock_gettime() read() write() */

#ifdef __cplusplus
}
#endif
/*
 * Note the while loop calling write() is based on the
 * "BSD Sockets: A Quick And Dirty Primer" by Jim Frost.
 */
template<typename IntType>
IntType timestamp(size_t pad_size, IntType rs_count, TimeStampMode mode)
{
        using std::fclose;
        using std::free;
        using std::fprintf;
        using std::malloc;

        IntType        i                     = 0;
        bool           force_break           = false;
        struct tm      tmp_tm                = { };
        char           tmp_strftime[1 << 13] = { };
        TimeStamp     *current               = NULL;
        FILE          *log_file              = timestamp_log_setup(mode);

        if (NULL == log_file) {
                force_break = true;
        }

        memset(current, 0, (sizeof current) + pad_size);
        memset(&tmp_tm, 0, sizeof tmp_tm);
        memset(tmp_strftime, 0, sizeof tmp_strftime);

        for (i = 0; false == force_break && i < rs_count; ++i) {
                if (NULL == timestamp_manipulate(current, mode)) {
                        force_break = true;
                        break;
                }
                if (TimeStampMode::RECEIVE == mode) {
                        if (NULL == localtime_r(&current->timespec.tv_sec,
                                                &tmp_tm)) {
                                force_break = true;
                                break;
                        }
                        if (0 == strftime(tmp_strftime,
                                          sizeof tmp_strftime,
                                          "%s",
                                          &tmp_tm)) {
                                force_break = true;
                                break;
                        }
                        fprintf(log_file,
                                "%s,%ld\n",
                                tmp_strftime,
                                current->timespec.tv_nsec);
                }
        }

        if (NULL != log_file) {
                fclose(log_file);
        }
        free(current);
        return false == force_break ? rs_count + 1 : rs_count;
}

#endif /* TIMESTAMP_TMP_H */
