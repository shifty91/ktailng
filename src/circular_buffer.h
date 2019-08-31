// Copyright 2019 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <cstdint>
#include <vector>
#include <thread>
#include <condition_variable>
#include <memory>
#include <string>

template<typename T>
class CircularBuffer
{
public:
    CircularBuffer(std::size_t size) :
        size_{size}, write_idx_{0}, read_idx_{0}, used_{0}
    {
        // allocate heap memory
        data_.resize(size);
    }

    virtual ~CircularBuffer()
    {}

    auto size() const
    {
        return size_;
    }

    auto used() const
    {
        std::lock_guard lock(mutex_);
        return used_;
    }

    void push(T&& elem)
    {
        std::lock_guard lock(mutex_);

        data_[write_idx_++] = std::forward<T>(elem);

        // handle wrap around
        if (write_idx_ == size_)
            write_idx_ = 0;

        // update used and read index
        if (used_ != size_)
            used_++;
        else
            read_idx_ = write_idx_;

        // wake up one reader
        cond_.notify_one();
    }

    T pop()
    {
        std::unique_lock lock(mutex_);

        // data available?
        cond_.wait(lock, [this] { return used_; });

        auto elem = std::move(data_[read_idx_++]);

        // handle wrap around
        if (read_idx_ == size_)
            read_idx_ = 0;

        // update used
        used_--;

        return elem;
    }

    T try_pop()
    {
        std::lock_guard lock(mutex_);

        // return "null" when no data available
        if (!used_)
            return T();

        auto elem = std::move(data_[read_idx_++]);

        // handle wrap around
        if (read_idx_ == size_)
            read_idx_ = 0;

        // update used
        used_--;

        return elem;
    }

private:
    std::vector<T> data_;
    std::size_t size_;
    std::size_t write_idx_;
    std::size_t read_idx_;
    std::size_t used_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

using KtailNGBuffer = CircularBuffer<std::unique_ptr<std::string>>;

#endif /* _CIRCULAR_BUFFER_H_ */
