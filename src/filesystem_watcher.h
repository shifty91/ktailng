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

#ifndef _FILESYSTEM_WATCHER_H_
#define _FILESYSTEM_WATCHER_H_

#include <memory>
#include <string>
#include <stdexcept>

#include <method.h>
#include <kqueue.h>
#include <inotify.h>
#include <ktailng_config.h>

class FilesystemWatcher
{
public:
    FilesystemWatcher(const std::string& filename) :
        filename_{filename}
    {
#ifdef HAVE_INOTIFY
        // Linux style
        method_ = std::make_unique<Inotify>(filename_);
#elif HAVE_KQUEUE
        // BSD style
        method_ = std::make_unique<Kqueue>(filename_);
#endif
    }

    virtual ~FilesystemWatcher()
    {}

    virtual void wait()
    {
        if (!method_)
            throw std::logic_error("No filesystem watch mechanism found");
        method_->wait();
    }

private:
    std::string filename_;
    std::unique_ptr<Method> method_;
};

#endif /* _FILESYSTEM_WATCHER_H_ */
