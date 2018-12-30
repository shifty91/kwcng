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

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>

#include <cstring>
#include <unistd.h>

#include "config.h"
#include "logger.h"
#include "word_counter.h"

void WordCounter::count_thread()
{
    while (1) {
        // zZz
        auto load = m_queue.pop();

        if (!load)
            break;

        auto res = count(*load);
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_results[res.file()] += res;
            m_global += res;
        }
    }
}

WordCountResult WordCounter::count(const WordCountLoad<>& load) const
{
    WordCountResult result;
    auto prev = load.prev();

    result.file()  = load.file();
    result.chars() = load.size();

    for (std::size_t i = 0; i < load.size(); ++i) {
        if ((config.flags & KwcNGFlags::LINES) && load[i] == L'\n')
            result.lines()++;

        if (!(config.flags & KwcNGFlags::WORDS))
            continue;

        if (std::iswspace(load[i]) && !std::iswspace(prev))
            result.words()++;

        prev = load[i];
    }

    if ((config.flags & KwcNGFlags::WORDS) &&
        load.size() != config.chunk_size &&
        !std::iswspace(prev))
        result.words()++;

    return result;
}

void WordCounter::distribute_work(const Files& files)
{
    for (auto&& file: files) {
        std::unique_ptr<WordCountLoad<>> load;
        std::wifstream ifs;
        std::wistream *is;
        auto prev = L'a';

        if (file == "stdin")
            is = &std::wcin;
        else {
            ifs.open(file);
            if (!ifs) {
                log_err("Failed to open file " << file);
                continue;
            }
            is = &ifs;
        }

        auto handle_move = [&] (std::size_t idx) {
            prev = (*load)[idx];
            m_queue.push(std::move(load));
        };

        while (42) {
            load = std::make_unique<WordCountLoad<>>(config.chunk_size, file);
            load->prev() = prev;
            is->read(load->data(), config.chunk_size);

            if (is->eof()) {
                load->size() = is->gcount();
                break;
            }

            if (is->bad() || is->fail()) {
                log_err("Failed to read from stream");
                log_info("Counting results for file " << file << " will be incorrect");
                goto next_file;
            }

            handle_move(config.chunk_size - 1);
        }

        if (is->gcount() > 0)
            handle_move(is->gcount() - 1);

    next_file:
        continue;
    }
}

void WordCounter::print_result(
    const std::string& file, const WordCountResult& result) const
{
    if (config.flags & KwcNGFlags::PARSEABLE) {
        std::cout << file << ";" << result.lines() << ";"
                  << result.words() << ";" << result.chars() << std::endl;
        return;
    }

    std::cout << "file: " << std::setw(24) << file;
    if (config.flags & KwcNGFlags::LINES)
        std::cout << " lines: " << std::setw(10) << result.lines();
    if (config.flags & KwcNGFlags::WORDS)
        std::cout << " words: " << std::setw(10) << result.words();
    if (config.flags & KwcNGFlags::CHARS)
        std::cout << " chars: " << std::setw(10) << result.chars();
    std::cout << std::endl;
}

void WordCounter::print_results() const
{
    for (auto&& i: m_results)
        print_result(i.first, i.second);
    if (m_results.size() > 1)
        print_result("global", m_global);
}
