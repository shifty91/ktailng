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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <stdexcept>

#include <circular_buffer.h>
#include <reader.h>
#include <writer.h>
#include <barrier.h>
#include <ktailng_config.h>

#include <kopt/kopt.h>

[[noreturn]] static inline
void print_usage_and_die(const Kopt::OptionParser& parser, int die)
{
    std::cerr << parser.get_usage("<file>");
    std::cerr << "ktailng version " << VERSION << " (C) Kurt Kanzenbach <kurt@kmk-computers.de>"
              << std::endl;
    std::exit(die ? EXIT_FAILURE : EXIT_SUCCESS);
}

[[noreturn]] static inline
void print_version_and_die()
{
    std::cerr << "ktailng version " << VERSION << " -- A tail replacement" << std::endl;
    std::cerr << "Copyright (C) 2019 Kurt Kanzenbach <kurt@kmk-computers.de>" << std::endl;
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    Kopt::OptionParser parser{argc, argv};
    std::size_t num;

    // arguments
    parser.add_flag_option("help", "print this help text", 'h');
    parser.add_flag_option("follow", "follow changes", 'f');
    parser.add_argument_option("number", "show last <lines> lines", 'n');
    parser.add_flag_option("version", "print version information", 'v');

    try {
        parser.parse();

        if (*parser["version"])
            print_version_and_die();
        if (*parser["help"])
            print_usage_and_die(parser, 0);

        if (parser.unparsed_options().size() != 1)
            throw std::logic_error("No or too many files given.");

        if (*parser["number"])
            num = parser["number"]->to<std::size_t>();
        else
            num = 1000;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        print_usage_and_die(parser, 1);
    }

    auto&& file = parser.unparsed_options()[0];

    // let's go
    try {
        KtailNGBuffer buf(num);
        KtailNGBarrier barrier;

        Writer writer(buf, barrier, file, *parser["follow"]);
        Reader reader(buf, barrier, *parser["follow"]);

        std::thread writer_thread(std::bind(&Writer::write, &writer));
        std::thread reader_thread(std::bind(&Reader::read, &reader));

        writer_thread.join();
        reader_thread.join();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
