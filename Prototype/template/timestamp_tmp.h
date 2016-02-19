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
 * the timestamp program.
 */

#ifndef TIMESTAMP_TMP_H
#define TIMESTAMP_TMP_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>     /* clock_gettime() read() write() */

#ifdef __cplusplus
}
#endif

#include "timestamp.h"
/*
 * Note the while loop calling write() is based on the
 * "BSD Sockets: A Quick And Dirty Primer" by Jim Frost.
 */
template<typename IntType>
auto timestamp(IntType rs_count, TimeStampMode mode) -> decltype(rs_count + 1)
{
        using std::fclose;
        using std::fopen;
        using std::fprintf;

        IntType         i                     = 0;
        bool            force_break           = false;
        FILE           *output_file           = NULL;
        const char     *output_file_name      = NULL;
        struct timespec current               = { };
        struct tm       tmp_tm                = { };
        char            tmp_strftime[1 << 13] = { };

        memset(&current, 0, sizeof current);
        memset(&tmp_tm, 0, sizeof tmp_tm);
        memset(tmp_strftime, 0, sizeof tmp_strftime);

        switch (mode) {
        case TimeStampMode::RECEIVE:
                output_file_name = secure_getenv(ENV_TIMESTAMP_OUTPUT);
                if (NULL == output_file_name) {
                        force_break = true;
                        break;
                }
                output_file = fopen(output_file_name, "w");
                if (NULL == output_file) {
                        force_break = true;
                        break;
                }
                break;
        case TimeStampMode::SEND:
                        break;
        }

        for (i = 0; false == force_break && i < rs_count; ++i) {
                if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
                        force_break = true;
                        break;
                }
                if (NULL == timestamp_manipulate(&current, mode)) {
                        force_break = true;
                        break;
                }
                if (TimeStampMode::RECEIVE == mode) {
                        if (NULL == localtime_r(&current.tv_sec, &tmp_tm)) {
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
                        fprintf(output_file,
                                "%lld,%ld\n",
                                current.tv_sec,
                                current.tv_nsec);
                }
        }

        if (NULL != output_file) {
                fclose(output_file);
        }
        return force_break == false ? rs_count + 1 : rs_count;
}

#endif /* TIMESTAMP_TMP_H */
