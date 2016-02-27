/**
 * @file biowrapper.cpp
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
 * Implementation source file of the BIOWrapper class; lots of member functions
 * can be (implicitly) inlined in the class declaration header, but I choose
 * not to do so since member functions which are not declared inline can
 * still be inlined by the compiler as it sees fit once optimization switch
 * is on.
 */

#include "biowrapper.h"

#include <stdexcept>

BIOWrapper::BIOWrapper(BIO_METHOD *method_type)
        : bio_handle_{BIO_new(method_type)}
{
}

BIOWrapper::BIOWrapper(FILE *file_stream, int close_flag)
        : bio_handle_{BIO_new_fp(file_stream, close_flag)}
{
}

BIOWrapper::~BIOWrapper()
{
        using std::runtime_error;

        if (0 == BIO_free(this->bio_handle_)) {
                throw runtime_error("BIO_free() failed");
        }
}

int BIOWrapper::flush()
{
        return BIO_flush(this->bio_handle_);
}

BIO *BIOWrapper::push(BIO *append) const
{
        return BIO_push(this->bio_handle_, append);
}

int BIOWrapper::read(void *data, int len) const
{
        return BIO_read(this->bio_handle_, data, len);
}

int BIOWrapper::write(const void *data, int len) const
{
        return BIO_write(this->bio_handle_, data, len);
}

BIOWrapper::operator BIO *() const
{
        return bio_handle_;
}

BIO_METHOD *BIOWrapper::f_base64()
{
        return BIO_f_base64();
}
