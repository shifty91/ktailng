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

#ifdef HAVE_INOTIFY

#include <stdexcept>

#include <unistd.h>
#include <sys/inotify.h>

#include <inotify.h>

Inotify::Inotify(const std::string& filename) :
    Method(filename)
{
    fd_ = inotify_init();
    if (fd_ < 0)
        throw std::logic_error("Failed to setup inotify");

    wd_ = inotify_add_watch(fd_, filename_.c_str(), IN_MODIFY);
    if (wd_ < 0)
        throw std::logic_error("Failed add inotify notifier");
}

void Inotify::wait()
{
    while (42) {
        struct inotify_event *event;
        unsigned char buf[1 * sizeof(struct inotify_event)];

        auto rc = read(fd_, buf, sizeof(buf));
        if (rc == -1 || rc == 0) {
            // bail if signalled
            if (errno == EINTR)
                break;
            throw std::logic_error("Inotify failed");
        }

        event = (struct inotify_event *)buf;
        if (event->mask & IN_MODIFY)
            break;
    }
}

#endif
