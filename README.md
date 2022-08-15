### Pre-alpha Disclaimer

Features may change or be removed without warning.  Features documented in the
examples folder are sorted in (roughly) descending order from most-stable to
least-stable.  If you to take it for a test-drive and have feedback I'm happy
to hear it :)


# Poof the Magic Dragon, Lived by the C
`poof` is a 'metaprogramming language' that aims to bring some of the niceties
of more modern languages to C.

Poof parses a small subset of C++, so you can use it in your codebase that
makes use of operator overloading and simple templates.

Poof requires a C++ compiler to build, though I'm making an effort to
systematically remove the use of C++ features from the codebase over time.


# Features

Take a look in [examples](https://github.com/jjbandit/poof/tree/master/examples)
for, well, examples of the following.

- Templating
  - Generate new types and code from existing types.

- Datatype member iteration
  - Iterate over the members of a specified type.

- Type constraints
  - Some support for refinement types.
  - This is useful for statically avoiding a specific class of bugs which are
    otherwise not possible to detect at compile time (in pure C).

- Experimental: Static analysis
  - `poof` can be used as a C++ library to build custom static-analyzers.  This
    is currently not documented, but if you're interested in trying this I'd be
    happy to write a tutorial.  Disclaimer: The code that enables this is
    pretty janky right now, and as such using it may be an exercise in
    patience/debugging.


# Getting started

## Required Tools to Build

`poof` depends on two submodules to build, which are themselves entirely
self-contained.  The CRT is the only external dependency, which I may remove
in the future.

### Linux

The officially supported compiler is `clang++`, version 6.0.0.  Yeah, it's
ancient, but it's what shipped on Ubuntu 18.04.  `poof` does compile, and
should work, on newer versions, but I make no promises.

### Windows

Poof requires `bash` to build on Windows.  I believe [Git for
Windows](https://gitforwindows.org/) comes with a shell that should work.
You'll also need `clang++` (preferably 6.0.0), which I'll leave as an exercise
to the reader.

NOTE(Jesse): Once upon a time I targeted MSVC on Windows, but have since given
up on Microsoft products.  Clang has been reliable enough, though it is
slightly more onerous to set up.

### Mac

OSX is currently unsupported because I do not own Apple hardware.  Porting to
Apple should be a minimal amount of effort; I'd be happy to do it if someone
would donate me a Macbook.  PRs are also most welcome. Feel free to open an
issue if you're interested in OSX support.


# Building

To get the source:

`git clone --recursive https://github.com/jjbandit/poof.git poof`

And to build it:

`cd poof && scripts/make.sh`


# Documentation

See the [examples](https://github.com/jjbandit/poof/tree/master/examples)
folder for use cases for `poof`.  For an example of a real project using `poof`
have a look at [bonsai](https://github.com/jjbandit/bonsai).
