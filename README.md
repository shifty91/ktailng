# KTAILNG #

## About ##

`ktailng` is a tail replacement written in C++.

## Usage ##

    usage: ktailng [options] <file>
      --follow, -f:  follow changes
      --help, -h:    print this help text
      --number, -n:  show last <lines> lines
      --version, -v: print version information
    ktailng version 1.0 (C) Kurt Kanzenbach <kurt@kmk-computers.de>

## Build ##

    $ git submodule init
    $ git submodule update
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j`nproc`
    $ sudo make install

## Dependencies ##

- Modern Compiler with CPP 17 Support (e.g. gcc >= 7 or clang >= 5)

## License ##

BSD 2-clause

## Author

(C) 2019 Kurt Kanzenbach <kurt@kmk-computers.de>
