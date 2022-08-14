### Poof is in pre-alpha

Features may change or be removed without warning.  Features documented in the
examples folder are sorted in (roughly) descending order from most-stable to
least-stable if you want to take it for a test-drive :)

# Poof the Magic Dragon Lived by the C

Thanks for your interest in `poof`!

Poof is a 'metaprogramming C compiler' that aims to bring some of the niceties
of modern languages to C, but without a lot of heartache that comes along with them.

Poof currently parses a small subset of C++, so you can still use it if your
codebase has operator overloading or (sane) templates.


# Getting started

## Required Tools to Build

### Linux

The officially supported compiler is `clang++`, version 6.0.0.  Yeah, it's
ancient, but it's what shipped on Ubuntu 18.04.  It does compile, and should
work, on newer versions, however I make no promises.

### Windows

Poof requires "bash" to build on Windows.
I believe installing [Git for Windows](https://gitforwindows.org/) comes with a shell that should work.
You'll also need `clang++`, which I'll leave as an exercise to the reader.

I used to build the Windows version with MSVC but got fed up with how much of a
PITA Visual Studio is these days and gave up.  Clang seems reliable enough for
this project at present.

### Mac

OSX is currently unsupported because I do not have Apple hardware to port it
with.  Porting it to Apple should be a minimal amount of effort.  I'd be happy
to do it if someone would donate me a Macbook.  PRs are also most welcome.


# Building

To get the source:

`git clone --recursive https://github.com/jjbandit/poof.git poof`

And to build it:

`cd poof && scripts/make.sh`











