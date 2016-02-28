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
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "biowrapper.h"
#include "cmnutil.h"
#include "timestamp.h"

#include <climits>   /* SIZE_MAX */
#include <cstring>   /* memset() */
#include <stdexcept> /* domain_error runtime_error */

TimeStamp::TimeStamp(TimeStampMode mode,
                     size_t        pad_size,
                     const char   *env_symbol)
        : mode_{mode},
          log_{NULL},
          log_env_symbol_{env_symbol},
          pad_size_{pad_size},
          tot_size_{sizeof(Stamp_) + pad_size_},
          bio_base64_{NULL},
          bio_output_{NULL},
          bio_input_{NULL}
{
        /*
         * Checks whether wrap-around behavior would occur when passed
         * to malloc().
         */
        if (pad_size_ > SIZE_MAX - sizeof(Stamp_)) {
                throw std::domain_error("TimeStamp(): pad_size exceeds maximum");
        }

        log_control_(LogSwitch_::ON);

        stamp_ = reinterpret_cast<Stamp_ *>(std::malloc(tot_size_));

        if (NULL == stamp_) {
                throw std::runtime_error("TimeStamp(): malloc() call failed");
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
        log_control_(LogSwitch_::OFF);
        std::free(stamp_);
}

size_t TimeStamp::recv(size_t count)
{
        using std::memset;
        using std::runtime_error;

        if (TimeStampMode::SEND == mode_) {
                throw runtime_error("recv(): not allowed in SEND mode");
        }

        auto            casted_tot_size_ = narrow_cast<int, size_t>(tot_size_);
        size_t          i                     = 0U;
        struct timespec current               = { };
        struct tm       tmp_tm                = { };
        char            tmp_strftime[1 << 13] = { };

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
                current.tv_sec = current.tv_sec - stamp_->timespec.tv_sec;
                current.tv_nsec = current.tv_nsec - stamp_->timespec.tv_nsec;

                if (NULL == localtime_r(&current.tv_sec, &tmp_tm)) {
                        break;
                }
                if (0 == strftime(tmp_strftime,
                                  sizeof tmp_strftime,
                                  "%s",
                                  &tmp_tm)) {
                        break;
                }
                fprintf(log_, "%s,%ld\n", tmp_strftime, current.tv_nsec);
        }
        return i;
}

size_t TimeStamp::send(size_t count)
{
        using std::runtime_error;

        if (TimeStampMode::RECEIVE == mode_) {
                throw runtime_error("send(): not allowed in RECEIVE mode");
        }

        size_t i                = 0U;
        auto   casted_tot_size_ = narrow_cast<int, size_t>(tot_size_);

        for (i = 0; i < count; ++i) {
                if (-1 == clock_gettime(CLOCK_REALTIME, &(stamp_->timespec))) {
                        break;
                }
                if (casted_tot_size_ !=
                    bio_base64_->write(stamp_, casted_tot_size_)) {
                        break;
                }
        }
        return i;
}

/* Can only be called in constructor or destructor. */
void TimeStamp::log_control_(LogSwitch_ flip)
{
        switch (flip) {
        case LogSwitch_::OFF:
                log_control_off_();
                break;
        case LogSwitch_::ON:
                log_control_on_();
        }
}


void  TimeStamp::log_control_on_()
{
        using std::fopen;
        using std::runtime_error;

        const char        *output_file_name = NULL;
        static const char *getenv_err       = "log_control_(): secure_getenv()"
                                              " failed";
        static const char *fopen_err        = "log_control_(): fopen() failed";

        /* Base64 BIO needs to be constructed regardless of the mode. */
        bio_base64_ = new BIOWrapper(BIOWrapper::f_base64());

        /* Set up bio_output_; again, the mode does not matter. */
        if (NULL == log_env_symbol_) {
                /* log_ is an alias for stdout. */
                log_ = stdout;
                /* Transfer the ownership of log_. */
                bio_output_ = new BIOWrapper(log_, BIO_NOCLOSE);
        } else {
                output_file_name = secure_getenv(log_env_symbol_);
                if (NULL == output_file_name) {
                        throw runtime_error(getenv_err);
                }
                log_ = fopen(output_file_name, "w");
                if (NULL == log_) {
                        throw runtime_error(fopen_err);
                }
                /* Transfer the ownership of log_. */
                bio_output_ = new BIOWrapper(log_, BIO_CLOSE);
        }

        if (TimeStampMode::RECEIVE == mode_) {
                bio_input_ = new BIOWrapper(stdin, BIO_NOCLOSE);
                bio_base64_->push(*bio_input_);
        } else if (TimeStampMode::SEND == mode_) {
                bio_base64_->push(*bio_output_);
        }
}

void  TimeStamp::log_control_off_()
{
        /*
         * The ownership of the log_ is already transferred to
         * BIOWrapper class, so nothing needs to be done here
         * since the resource deallocation is the responsibility
         * of bio_output_.
         */
        if (TimeStampMode::RECEIVE == mode_) {
                bio_output_->flush();
                delete bio_input_;
        } else if (TimeStampMode::SEND == mode_) {
                bio_base64_->flush();
        }
        delete bio_base64_;
        delete bio_output_;
}
