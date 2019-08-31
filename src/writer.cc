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

#include <stdexcept>

#include <writer.h>

void Writer::read()
{
    std::ifstream ifs(filename_);
    auto old_pos = pos_, pos = pos_;

    if (!ifs)
        throw std::logic_error("Failed to open file");

    ifs.seekg(pos_);

    // read file
    while (42) {
        auto line = std::make_unique<std::string>();

        std::getline(ifs, *line);

        old_pos = pos;
        pos = ifs.tellg();

        if (ifs.eof())
           break;

        if (ifs.bad() || ifs.fail())
            throw std::logic_error("I/O error while reading file");

        buffer_.push(std::move(line));
    }

    pos_ = old_pos;
}

void Writer::write()
{
    read();

    barrier_.arrive();

    if (!follow_)
        return;

    while (42) {
        watcher_.wait();
        read();
    }
}
