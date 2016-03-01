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
        /*
         * Note the log is only used when receive something:
         * the recorded one-way latency will go to this file.
         * 'input' and 'output' stand for where to read the
         * incoming message, where to write the outgoing message,
         * respectively.
         *
         * This one also acts as a default constructor:
         * in this case 'input' is stdin for the overloaded
         * 'operator <<', 'output' is stdout for the overloaded
         * 'operator >>', 'log' is stdout.
         *
         * Note the class DOES take ownership for the 3 'FILE *'
         * parameters passed-in, so the caller DOES NOT need to
         * worry about freeing them.
         */
        TimeStamp(size_t  pad_size  = 0,
                  FILE   *input     = NULL,
                  FILE   *output    = NULL,
                  FILE   *log       = NULL);
        TimeStamp(const TimeStamp &)               = delete;
        TimeStamp(const TimeStamp &&)              = delete;
        TimeStamp &operator = (const TimeStamp &)  = delete;
        TimeStamp &operator = (const TimeStamp &&) = delete;
        ~TimeStamp();

        /* Reveives from 'input_' 'count' times of timestamp plus padding. */
        TimeStamp &operator << (const size_t count);
        /* Sends to 'output_' 'count' times of timestamp plus padding. */
        TimeStamp &operator >> (const size_t count);

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
        size_t        pad_size_;
        size_t        tot_size_;
        FILE         *input_;
        FILE         *output_;
        FILE         *log_;
        Stamp_       *stamp_;
        BIOWrapper   *bio_base64_;

        void     io_control_(LogSwitch_ flip);
        void     io_control_on_();
        void     io_control_off_();
        timespec timespec_diff_(const timespec *end, const timespec *start);
};

#endif /* TIMESTAMP_H */
