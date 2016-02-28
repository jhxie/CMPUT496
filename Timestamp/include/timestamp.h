/**
 * @file timestamp.h
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
 * Public header used to export an external linkage function used by
 * the template function timestamp().
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

/**
 * @def ENV_TIMESTAMP_OUTPUT
 * @brief Records the name of the environment symbol used to specify logfile
 *        on the receiver side.
 */
#define ENV_TIMESTAMP_OUTPUT "TIMESTAMP_OUTPUT"

#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#ifdef __cplusplus
}
#endif

enum class TimeStampMode : int {
        RECEIVE,
        SEND
};

class TimeStamp final {
public:
        TimeStamp()                                = delete;
        TimeStamp(const TimeStamp &)               = delete;
        TimeStamp(const TimeStamp &&)              = delete;
        /* Prohibits implicit invocation of conversion constructor. */
        explicit TimeStamp(size_t pad_size);
        TimeStamp &operator = (const TimeStamp &)  = delete;
        TimeStamp &operator = (const TimeStamp &&) = delete;
        ~TimeStamp();

        size_t recv(size_t count);
        size_t send(size_t count);

private:
        /* data */
        struct Stamp_ {
                struct timespec timespec;
                char            padding[];
        };
        enum class LogSwitch_ : int {
                OFF = 0,
                ON
        };
        FILE     *log_;
        Stamp_   *stamp_;
        size_t   pad_size_;
        size_t   tot_size_;

        FILE *log_control_(LogSwitch_ flip);
};

struct timespec *timestamp_manipulate(TimeStamp *ts, TimeStampMode mode);
struct timespec *timestamp_send(TimeStamp *ts, size_t pad_size);
FILE *timestamp_log_setup(TimeStampMode mode)__attribute__((warn_unused_result));
#endif /* TIMESTAMP_H */
