# KWCNG #

## About ##

`kwcng` is a wordcount utility written in C++ using STL threads.

## Usage ##

    usage: kwcng [options] [files]
      options:
      --lines, -l:       count lines
      --words, -w:       count words
      --chars, -c:       count character
      --parseable, -p:   parseable output for use in scripts
      --max_threads, -m: maximum number of threads to be used
      --chunk_size, -t:  thread workload size
      --help, -h:        print this help text
      --version, -v:     print version information
    By default all options are enabled. If no file is specified, stdin is used
    kwcng version 1.0 (C) Kurt Kanzenbach <kurt@kmk-computers.de>

## Build ##

### Linux ###

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j8
    $ sudo make install

### FreeBSD ###

    $ mkdir build
    $ cd build
    $ CC=gcc7 CXX=g++7 cmake -DCUSTOM_RPATH="/usr/local/lib/gcc7" ..
    $ make -j8
    $ sudo make install

## Dependencies ##

- Modern Compiler with CPP 17 Support (e.g. gcc >= 7 or clang >= 5)

## License ##

BSD 2-clause

## Author

(C) 2018 Kurt Kanzenbach <kurt@kmk-computers.de>
