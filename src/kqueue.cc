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

#include <ktailng_config.h>

#ifdef HAVE_KQUEUE

#include <stdexcept>

#include <fcntl.h>
#include <errno.h>

#include <kqueue.h>

Kqueue::Kqueue(const std::string& filename) :
    Method(filename)
{
    fd_ = open(filename_.c_str(), O_RDONLY);
    if (fd_ < 0)
        throw std::logic_error("open() failed");

    kq_ = kqueue();
    if (kq_ < 0) {
        close(fd_);
        throw std::logic_error("kqueue() failed");
    }

    EV_SET(&change_, fd_, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_ONESHOT,
           NOTE_EXTEND | NOTE_WRITE,
           0, 0);
}

void Kqueue::wait()
{
    struct kevent event;

    while (42) {
        auto nev = kevent(kq_, &change_, 1, &event, 1, NULL);
        if (nev < 0) {
            if (errno == EINTR)
                break;
            throw std::logic_error("kevent() failed");
        }

        if (event.fflags & NOTE_EXTEND || event.fflags & NOTE_WRITE)
            break;
    }
}

#endif
