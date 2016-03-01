/**
 * @file biowrapper.h
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
 * Header declaration of the BIOWrapper class; it wraps openssl BIO pointer
 * as well as a few functions associated with it inside the class.
 * An application of the RAII idiom.
 */

#ifndef BIOWRAPPER_H
#define BIOWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/bio.h>
#include <openssl/evp.h>

#ifdef __cplusplus
}
#endif

class BIOWrapper final {
public:
        /*
         * Prohibits compiler-generated default/copy/move constructors to avoid
         * double free.
         */
        BIOWrapper()                                     = delete;
        BIOWrapper(const BIOWrapper &)                   = delete;
        BIOWrapper(const BIOWrapper &&)                  = delete;
        BIOWrapper(BIO_METHOD *method_type);
        BIOWrapper(FILE *file_stream, int close_flag);
        ~BIOWrapper();

        int flush();
        BIO *pop() const;
        BIO *push(BIO *append) const;
        /*
         * OpenSSL version 1.0.2e still uses int rather than size_t
         * for some unknown reason.
         */
        int read(void *data, int len) const;
        int write(const void *data, int len) const;

        /* Allows implicit conversion triggered by compilers and runtimes. */
        operator BIO *() const;
        /*
         * Prohibits compiler-generated copy/move assignment member function
         * to avoid double free.
         */
        BIOWrapper &operator =(const BIOWrapper &other)  = delete;
        BIOWrapper &operator =(const BIOWrapper &&other) = delete;

        static BIO_METHOD *f_base64();
private:
        /* data */
        BIO *bio_handle_;
};

#endif /* BIOWRAPPER_H */
