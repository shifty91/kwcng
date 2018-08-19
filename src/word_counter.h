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

#ifndef _WORD_COUNTER_H_
#define _WORD_COUNTER_H_

#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "concurrent_queue.h"
#include "word_count_result.h"
#include "word_count_load.h"

class WordCounter
{
public:
    using Files = std::vector<std::string>;

    WordCounter() :
        m_global("global")
    {}

    void count_thread();

    void distribute_work(const Files& files);

    void print_results() const;

    void stop()
    {
        m_queue.wake_up();
    }

private:
    WordCountResult count(const WordCountLoad<>& load) const;
    void print_result(const WordCountResult& result) const;

    WordCountResult m_global;
    ConcurrentQueue<std::shared_ptr<WordCountLoad<>>> m_queue;
    std::unordered_map<std::string, WordCountResult> m_results;
    std::mutex m_mutex;
};

#endif /* _WORD_COUNTER_H_ */
