### Poof is in pre-alpha

Features may change or be removed without warning.  Features documented in the
examples folder are sorted in (roughly) descending order from most-stable to
least-stable if you want to take it for a test-drive :)


# Poof the Magic Dragon Lived by the C

Thanks for your interest in `poof`!

Poof is a 'metaprogramming C compiler' that aims to bring some of the niceties
of modern languages to C, but without a lot of heartache that comes along with
them.  C++, I'm looking at you.

Poof currently parses a small subset of C++, so you can use it if your codebase
has operator overloading or (sane) templates.


# Features

- Templating
  - Generate new types and code from existing types.
  - Useful for: implementing generic containers.

- Datatype member iteration
  - Iterate over the members of a specified type.
  - Useful for: printing enum values as strings, generating discriminated unions, and more.

- Type constraints
  - Some support for refinement types.
  - Useful for: statically avoiding some bugs which is otherwise not possible in C.

- Experimental: Static analysis
  - `poof` can be used as a C library to build custom static-analyzers.  This
    is currently not documented, but if you're interested in trying this I'd be
    happy to write a tutorial.


# Getting started

## Required Tools to Build

`poof` depends on two submodules to build, which are themselves entirely
self-contained.  The CRT is the only external dependency, which I may remove
in the future.

### Linux

The officially supported compiler is `clang++`, version 6.0.0.  Yeah, it's
ancient, but it's what shipped on Ubuntu 18.04.  `poof` does compile, and
should work, on newer versions, however I make no promises.

### Windows

Poof requires `bash` to build on Windows.  I believe installing [Git for
Windows](https://gitforwindows.org/) comes with a shell that should work.
You'll also need `clang++`, which I'll leave as an exercise to the reader.

Once upon a time I targeted MSVC on Windows, but have since gotten fed up with
Visual Studio.  Clang has been reliable enough for this project, though it is
slightly more onerous to set up.

### Mac

OSX is currently unsupported because I do not have Apple hardware to use to
port it.  Porting it to Apple should be a minimal amount of effort and I'd be
happy to do it if someone would donate me a Macbook.  PRs are also most
welcome, email me if you're interested in OSX support.


# Building

To get the source:

`git clone --recursive https://github.com/jjbandit/poof.git poof`

And to build it:

`cd poof && scripts/make.sh`


# Babys first poof

See the [examples](https://github.com/jjbandit/poof/tree/master/examples)
folder for, well, examples of how you might use `poof`.  For examples of a real
project using `poof` have a look at [bonsai](https://github.com/jjbandit/bonsai)









