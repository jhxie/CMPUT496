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
 * Public interface header for the timestamp class.
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

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

/* Only forward declaration needed in this header file. */
class BIOWrapper;

class TimeStamp final {
public:
        TimeStamp(TimeStampMode mode,
                  size_t        pad_size   = 0,
                  const char   *env_symbol = NULL);
        TimeStamp()                                = delete;
        TimeStamp(const TimeStamp &)               = delete;
        TimeStamp(const TimeStamp &&)              = delete;
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
        TimeStampMode mode_;
        FILE         *log_;
        /*
         * Records the name of the environment symbol used to specify logfile
         * on the receiver side.
         */
        const char   *log_env_symbol_;
        Stamp_       *stamp_;
        size_t        pad_size_;
        size_t        tot_size_;
        BIOWrapper   *bio_base64_;
        BIOWrapper   *bio_output_;
        BIOWrapper   *bio_input_;

        void     log_control_(LogSwitch_ flip);
        void     log_control_on_();
        void     log_control_off_();
        timespec timespec_diff_(const timespec *end, const timespec *start);
};

#endif /* TIMESTAMP_H */
