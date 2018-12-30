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
#include <stdexcept>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <cstdlib>
#include <clocale>

#include <kopt/kopt.h>

#include "kwcng_config.h"
#include "config.h"
#include "concurrent_queue.h"
#include "word_counter.h"
#include "logger.h"

using Threads = std::vector<std::thread>;

[[noreturn]] static inline
void print_usage_and_die(const Kopt::OptionParser& parser, int die)
{
    std::cerr << parser.get_usage("[files]");
    std::cerr << "By default all options are enabled. If no file is specified, stdin is used"
              << std::endl;
    std::cerr << "kwcng version " << VERSION << " (C) Kurt Kanzenbach <kurt@kmk-computers.de>"
              << std::endl;
    std::exit(die ? EXIT_FAILURE : EXIT_SUCCESS);
}

[[noreturn]] static inline
void print_version_and_die()
{
    std::cerr << "kwcng version " << VERSION << " -- wordcount utility" << std::endl;
    std::cerr << "Copyright (C) 2018 Kurt Kanzenbach <kurt@kmk-computers.de>" << std::endl;
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    Kopt::OptionParser parser{argc, argv};
    WordCounter::Files files;
    WordCounter counter;
    Threads threads;

    // setup arguments
    parser.add_flag_option("lines", "count lines", 'l');
    parser.add_flag_option("words", "count words", 'w');
    parser.add_flag_option("chars", "count characters", 'c');
    parser.add_flag_option("parseable", "parseable output for use in scripts", 'p');
    parser.add_argument_option("max_threads", "maximum number of threads to be used", 'm');
    parser.add_argument_option("chunk_size", "thread workload size", 't');
    parser.add_flag_option("help", "print this help text", 'h');
    parser.add_flag_option("version", "print version information", 'v');

    // parse and configure
    try {
        parser.parse();

        if (*parser["version"])
            print_version_and_die();
        if (*parser["help"])
            print_usage_and_die(parser, 0);

        if (*parser["lines"])
            config.flags |= KwcNGOpt::LINES;
        if (*parser["words"])
            config.flags |= KwcNGOpt::WORDS;
        if (*parser["chars"])
            config.flags |= KwcNGOpt::CHARS;
        if (*parser["parseable"])
            config.flags |= KwcNGOpt::PARSEABLE;
        if (*parser["max_threads"])
            config.max_threads = parser["max_threads"]->to<std::size_t>();
        if (*parser["chunk_size"])
            config.chunk_size = parser["chunk_size"]->to<std::size_t>();
    } catch (const std::exception& ex) {
        std::cerr << "Error while parsing command line arguments: " << ex.what()
                  << std::endl;
        print_usage_and_die(parser, 1);
    }
    if (!config.max_threads || !config.chunk_size)
        print_usage_and_die(parser, 1);
    if (!(config.flags & (KwcNGOpt::LINES | KwcNGOpt::WORDS | KwcNGOpt::CHARS)))
        config.flags |= KwcNGOpt::LINES | KwcNGOpt::WORDS | KwcNGOpt::CHARS;

    files = parser.unparsed_options();
    if (files.empty())
        files.emplace_back("stdin");

    if (!std::setlocale(LC_ALL, "")) {
        log_err("setlocale() failed");
        return EXIT_FAILURE;
    }

    threads.reserve(config.max_threads);
    for (auto i = 0u; i < config.max_threads; ++i)
        threads.emplace_back(std::bind(&WordCounter::count_thread, &counter));

    counter.distribute_work(files);

    counter.stop();

    for (auto i = 0u; i < config.max_threads; ++i)
        threads[i].join();

    counter.print_results();

    return 0;
}
