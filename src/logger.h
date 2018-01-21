// Copyright 2018 Kurt Kanzenbach <kurt@kmk-computers.de>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <libgen.h>
#include <cerrno>

#define KWCNG_BASENAME(str)                     \
    (basename(const_cast<char *>(str)))

#define log_err(msg)                                                    \
    do {                                                                \
        std::cerr << "[ERROR " << KWCNG_BASENAME(__FILE__) << ":"       \
                  << __LINE__ << "]: " << msg;                          \
        if (errno)                                                      \
            std::cerr << ": " << strerror(errno);                       \
        std::cerr << std::endl;                                         \
    } while (0)

#define log_warn(msg)                                                   \
    do {                                                                \
        std::cerr << "[WARNING " << KWCNG_BASENAME(__FILE__) << ":"     \
                  << __LINE__ << "]: " << msg << std::endl;             \
    } while (0)

#define log_info(msg)                                                   \
    do {                                                                \
        std::cout << "[INFO " << KWCNG_BASENAME(__FILE__) << ":"        \
                  << __LINE__ << "]: " << msg << std::endl;             \
    } while (0)

#define EXCEPTION_TYPE(type, msg)                       \
    do {                                                \
        std::stringstream ss;                           \
        ss << msg;                                      \
        log_err(ss.str());                              \
        throw std::type(ss.str());                      \
    } while (0)

#define EXCEPTION(msg)                          \
    EXCEPTION_TYPE(logic_error, msg)

#endif /* _LOGGER_H_ */
