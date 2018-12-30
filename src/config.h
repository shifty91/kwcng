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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <thread>
#include <cstdint>

#include <gfm/gfm.h>

enum class KwcNGOpt: std::uint32_t {
    LINES     = BIT(0),
    WORDS     = BIT(1),
    CHARS     = BIT(2),
    PARSEABLE = BIT(3),
};

GFM_DECLARE_FLAG_MAP(KwcNGOpt);

struct KwcNGConfig {
    KwcNGConfig() :
        max_threads{std::thread::hardware_concurrency()},
        chunk_size{DEFAULT_CHUNK_SIZE}
    {}

    static const inline std::size_t DEFAULT_CHUNK_SIZE = 4096;
    KwcNGOptFlags flags;
    std::size_t max_threads;
    std::size_t chunk_size;
};

extern KwcNGConfig config;

#endif /* _CONFIG_H_ */
