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

#ifndef _WORD_COUNT_RESULT_H_
#define _WORD_COUNT_RESULT_H_

#include <string>
#include <cstddef>

class WordCountResult
{
public:
    WordCountResult() :
        m_words{0}, m_lines{0}, m_chars{0}
    {}

    WordCountResult(const std::string& file) :
        m_file{file}, m_words{0}, m_lines{0}, m_chars{0}
    {}

    const std::string& file() const noexcept
    {
        return m_file;
    }

    std::string& file() noexcept
    {
        return m_file;
    }

    const std::size_t& words() const noexcept
    {
        return m_words;
    }

    std::size_t& words() noexcept
    {
        return m_words;
    }

    const std::size_t& lines() const noexcept
    {
        return m_lines;
    }

    std::size_t& lines() noexcept
    {
        return m_lines;
    }

    const std::size_t& chars() const noexcept
    {
        return m_chars;
    }

    std::size_t& chars() noexcept
    {
        return m_chars;
    }

    auto& operator+=(const WordCountResult& rhs) noexcept
    {
        m_words += rhs.m_words;
        m_lines += rhs.m_lines;
        m_chars += rhs.m_chars;
        return *this;
    }

private:
    std::string m_file;
    std::size_t m_words;
    std::size_t m_lines;
    std::size_t m_chars;
};

#endif /* _WORD_COUNT_RESULT_H_ */
