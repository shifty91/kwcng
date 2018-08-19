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

#ifndef _WORDCOUNT_LOAD_H_
#define _WORDCOUNT_LOAD_H_

#include <vector>
#include <string>
#include <cstring>
#include <cstddef>

template<typename T=wchar_t>
class WordCountLoad
{
public:
    WordCountLoad() :
        m_data{nullptr},
        m_size{0},
        m_prev{L'a'}
    {}

    WordCountLoad(std::size_t size, const std::string& file) :
        m_data{new T[size]},
        m_size{size},
        m_file{file},
        m_prev{L'a'}
    {}

    WordCountLoad(const WordCountLoad& other)
    {
        if (other.m_data) {
            m_data = new T[other.m_size];
            std::memcpy(m_data, other.m_data, sizeof(T) * other.m_size);
        } else
            m_data = nullptr;
        m_size = other.m_size;
        m_file = other.m_file;
        m_prev = other.m_prev;
    }

    WordCountLoad(WordCountLoad&& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        m_file = std::move(other.m_file);
        m_prev = other.m_prev;
    }

    virtual ~WordCountLoad()
    {
        delete[] m_data;
    }

    auto& operator=(const WordCountLoad& other)
    {
        if (other.m_data) {
            m_data = new T[other.m_size];
            std::memcpy(m_data, other.m_data, sizeof(T) * other.m_size);
        } else
            m_data = nullptr;
        m_size = other.m_size;
        m_file = other.m_file;
        m_prev = other.m_prev;

        return *this;
    }

    auto& operator=(WordCountLoad&& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        m_file = std::move(other.m_file);
        m_prev = other.m_prev;

        return *this;
    }

    T operator[](std::size_t idx) const noexcept
    {
        return m_data[idx];
    }

    T *data() noexcept
    {
        return m_data;
    }

    const T *data() const noexcept
    {
        return m_data;
    }

    const std::size_t& size() const noexcept
    {
        return m_size;
    }

    std::size_t& size() noexcept
    {
        return m_size;
    }

    const std::string& file() const noexcept
    {
        return m_file;
    }

    std::string& file() noexcept
    {
        return m_file;
    }

    const T& prev() const noexcept
    {
        return m_prev;
    }

    T& prev() noexcept
    {
        return m_prev;
    }

private:
    T *m_data;
    std::size_t m_size;
    std::string m_file;
    T m_prev;
};

#endif /* _WORDCOUNT_LOAD_H_ */
