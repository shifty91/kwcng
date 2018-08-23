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

#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

/**
 * Basic implementation of a WorkQueue used in producer/consumer scenario.
 *
 * Locking is performed as described in:
 *   http://en.cppreference.com/w/cpp/thread/condition_variable
 *
 * The combination from unique_locks and condition variables results
 * in really short and correct code. Nice!
 */
template<typename T>
class ConcurrentQueue
{
public:
    ConcurrentQueue() :
        m_stop{false}
    {}

    void push(T&& element)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_queue.push(std::forward<T>(element));
        }
        m_cv.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [this] { return !m_queue.empty() || m_stop; });
        if (m_stop && m_queue.empty())
            return T();
        auto element = std::move(m_queue.front());
        m_queue.pop();
        return element;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_queue.empty();
    }

    void wake_up()
    {
        m_stop = true;
        m_cv.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stop;
};

#endif /* _CONCURRENT_QUEUE_H_ */
