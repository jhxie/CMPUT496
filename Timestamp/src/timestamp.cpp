/**
 * @file timestamp.cpp
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
 * Implementation file for the timestamp class.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "biowrapper.h"
#include "cmnutil.h"
#include "timestamp.h"

#include <climits>   /* SIZE_MAX */
#include <cstdio>    /* fileno() */
#include <cstring>   /* memset() */
#include <stdexcept> /* overflow_error runtime_error */

TimeStamp::TimeStamp(size_t pad_size, FILE *input, FILE *output, FILE *log)
        :
        pad_size_{pad_size},
        tot_size_{sizeof(Stamp_) + pad_size_},
        input_{input},
        output_{output},
        log_{log},
        stamp_{NULL},
        bio_base64_{NULL}
{
        using std::overflow_error;
        using std::runtime_error;
        /*
         * Checks whether wrap-around behavior would occur when passed
         * to malloc().
         */
        if (pad_size_ > SIZE_MAX - sizeof(Stamp_)) {
                throw overflow_error("TimeStamp(): pad_size exceeds maximum");
        }

        io_control_(LogSwitch_::ON);

        /*
         * Here total size is used to avoid dealing with the case where
         * 'pad_size' is given as 0: malloc() may return either NULL or an
         * 'unique' pointer value that can be freed later.
         */
        stamp_ = reinterpret_cast<Stamp_ *>(std::malloc(tot_size_));

        if (NULL == stamp_) {
                throw runtime_error("TimeStamp(): malloc() call failed");
        }
        /*
         * Note the trailing padding field is intentially left un-initialized
         * to prevent potential compression algorithms used by ssh from
         * indirectly shortening the actual length of the message.
         */
        std::memset(stamp_, 0, sizeof(Stamp_));
}

TimeStamp::~TimeStamp()
{
        io_control_(LogSwitch_::OFF);
        std::free(stamp_);
}

/*
 * Reveives timestamps 'count' times from 'input_' and record result to 'log_'.
 */
TimeStamp &TimeStamp::operator << (const size_t count)
{
        using std::memset;
        using std::runtime_error;

        auto            casted_tot_size_ = narrow_cast<int, size_t>(tot_size_);
        char            tmp_strftime[1 << 13] = { };

        size_t           i          = 0U;
        FILE            *input_file = (NULL == input_) ? stdin : input_;
        FILE            *log_file   = (NULL == log_) ? stdout : log_;
        struct timespec  current    = { };
        struct tm        tmp_tm     = { };
        BIOWrapper       bio_input(input_file, BIO_NOCLOSE);

        /* Build the chain of the form bio_base64_--bio_input. */
        bio_base64_->push(bio_input);
        memset(&current, 0, sizeof current);
        memset(&tmp_tm, 0, sizeof tmp_tm);
        memset(tmp_strftime, 0, sizeof tmp_strftime);

        for (i = 0; i < count; ++i) {
                if (casted_tot_size_ !=
                    bio_base64_->read(stamp_, casted_tot_size_)) {
                        break;
                }
                /*
                 * clock_gettime() needs to be called after the read from
                 * stdin due to the possibility of be blocked.
                 */
                if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
                        break;
                }

                current = timespec_diff_(&current, &(stamp_->timespec));

                if (NULL == localtime_r(&current.tv_sec, &tmp_tm)) {
                        break;
                }
                if (0 == strftime(tmp_strftime,
                                  sizeof tmp_strftime,
                                  "%s",
                                  &tmp_tm)) {
                        break;
                }
                fprintf(log_file, "%s,%ld\n", tmp_strftime, current.tv_nsec);
        }

        fflush(log_file);
        /* Removes the 'bio_input' from the chain. */
        bio_input.pop();

        if (count != i) {
                throw runtime_error("TimeStamp::operator <<() : "
                                    "failed to receive required amount");
        }
        return *this;
}

/* Send timestamps 'count' times to 'output_'. */
TimeStamp &TimeStamp::operator >> (const size_t count)
{
        using std::runtime_error;

        auto       casted_tot_size_ = narrow_cast<int, size_t>(tot_size_);
        size_t     i                = 0U;
        FILE      *output_file      = (NULL == output_) ? stdout : output_;
        BIOWrapper bio_output(output_file, BIO_NOCLOSE);

        bio_base64_->push(bio_output);

        for (i = 0; i < count; ++i) {
                if (-1 == clock_gettime(CLOCK_REALTIME, &(stamp_->timespec))) {
                        break;
                }
                if (casted_tot_size_ !=
                    bio_base64_->write(stamp_, casted_tot_size_)) {
                        break;
                }
        }

        bio_base64_->flush();
        /* Removes the 'bio_output' from the chain. */
        bio_output.pop();

        if (count != i) {
                throw runtime_error("TimeStamp::operator >>() : "
                                    "failed to send required amount");
        }
        return *this;
}

/* Can only be called in constructor or destructor. */
void TimeStamp::io_control_(LogSwitch_ flip)
{
        /*
         * This nested fileno() switch check is required if the constructor
         * is called in a fashion like:
         * TimeStamp timestamp(0, stdin, stdout, stdout)
         * without the check the program would hang forever.
         */
        switch (flip) {
        case LogSwitch_::OFF:
                for (auto file : {input_, output_, log_}) {
                        /* Prevent calling fileno() on NULL. */
                        if (NULL == file) {
                                continue;
                        }
                        switch (fileno(file)) {
                        case STDIN_FILENO:
                                break;
                        case STDOUT_FILENO:
                                break;
                        case STDERR_FILENO:
                                break;
                        default:
                                std::fclose(file);
                        }
                }
                delete bio_base64_;
                break;
        case LogSwitch_::ON:
                bio_base64_ = new BIOWrapper(BIOWrapper::f_base64());
        }
}

/*
 * Modified from the example from:
 * http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 */
timespec TimeStamp::timespec_diff_(const timespec *end, const timespec *start)
{
        timespec result;

        if (0 > (end->tv_nsec - start->tv_nsec)) {
                result.tv_sec = end->tv_sec - start->tv_sec - 1;
                result.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
        } else {
                result.tv_sec = end->tv_sec - start->tv_sec;
                result.tv_nsec = end->tv_nsec - start->tv_nsec;
        }
        return result;
}
