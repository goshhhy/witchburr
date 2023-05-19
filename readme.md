witchburr
=========

a C++20 assembly for ppc64

witchburr is closely based on [oaknut](https://github.com/merryhime/oaknut/), an assembly emitter for Aarch64. it is intended
to be used the same way, and in the same places. some, but not all, of the source files
are modified from those originally in oaknut.

witchburr is also very slightly simpler on account of not having to account for windows or macOS
as target platforms. it is actively tested on Linux. if it does not work on FreeBSD this
is a bug, please report it.

license
=======

this project is provided under the MIT license.
