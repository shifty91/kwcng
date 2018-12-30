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
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>

#include "kwcng_config.h"
#include "config.h"
#include "concurrent_queue.h"
#include "word_counter.h"
#include "logger.h"

using Threads = std::vector<std::thread>;

static struct option long_opts[] = {
    { "lines",       no_argument,       NULL, 'l' },
    { "words",       no_argument,       NULL, 'w' },
    { "chars",       no_argument,       NULL, 'c' },
    { "parseable",   no_argument,       NULL, 'p' },
    { "max_threads", required_argument, NULL, 'm' },
    { "chunk_size",  required_argument, NULL, 't' },
    { "help",        no_argument,       NULL, 'h' },
    { "version",     no_argument,       NULL, 'v' },
    { NULL,          0,                 NULL,  0  },
};

[[noreturn]] static inline
void print_usage_and_die(int die)
{
    std::cerr << "usage: kwcng [options] [files]" << std::endl;
    std::cerr << "  options:" << std::endl;
    std::cerr << "  --lines, -l:       count lines" << std::endl;
    std::cerr << "  --words, -w:       count words" << std::endl;
    std::cerr << "  --chars, -c:       count character" << std::endl;
    std::cerr << "  --parseable, -p:   parseable output for use in scripts" << std::endl;
    std::cerr << "  --max_threads, -m: maximum number of threads to be used" << std::endl;
    std::cerr << "  --chunk_size, -t:  thread workload size" << std::endl;
    std::cerr << "  --help, -h:        print this help text" << std::endl;
    std::cerr << "  --version, -v:     print version information"  << std::endl;
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
    // parse args
    WordCounter::Files files;
    WordCounter counter;
    Threads threads;
    int c;

    config.max_threads = std::thread::hardware_concurrency();
    config.chunk_size  = KwcNGConfig::DEFAULT_CHUNK_SIZE;
    while ((c = getopt_long(argc, argv, "lwcpm:t:hv", long_opts, NULL)) != -1) {
        switch (c) {
        case 'l':
            config.flags |= KwcNGOpt::LINES;
            break;
        case 'w':
            config.flags |= KwcNGOpt::WORDS;
            break;
        case 'c':
            config.flags |= KwcNGOpt::CHARS;
            break;
        case 'p':
            config.flags |= KwcNGOpt::PARSEABLE;
            break;
        case 'm':
            config.max_threads = std::atoll(optarg);
            break;
        case 't':
            config.chunk_size = std::atoll(optarg);
            break;
        case 'v':
            print_version_and_die();
        case 'h':
            print_usage_and_die(0);
        default:
            print_usage_and_die(1);
        }
    }
    if (!config.max_threads || !config.chunk_size)
        print_usage_and_die(1);
    if (!(config.flags & (KwcNGOpt::LINES | KwcNGOpt::WORDS | KwcNGOpt::CHARS)))
        config.flags |= KwcNGOpt::LINES | KwcNGOpt::WORDS | KwcNGOpt::CHARS;

    argc -= optind;
    argv += optind;

    for (auto i = 0; i < argc; ++i)
        files.emplace_back(argv[i]);

    if (!argc)
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
