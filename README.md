### Pre-alpha Disclaimer

`poof` is quickly approaching 0.1.0-alpha where features will stabilize
somewhat, however it's not quite there yet.  Expect some churn in the next few
weeks.

# Quickstart

```
git clone --recursive https://github.com/scallyw4g/poof.git poof
cd poof && ./make.sh BuildAllBinariesRunAllTests
```

# Documentation

There's a [website](https://scallyw4g.github.io/poof/) with examples and an
interactive playground if you want quickly give poof a try.

Proper documentation is currently a WIP, but there is example code on the
website as well as in this repository.

# About

`poof` is a 'metaprogramming language' that aims to bring some of the niceties
of more modern languages to C.


# Features

- Template Metaprogramming
  - Generate new code and types from existing types.

- Datatype Iteration
  - Iterate over the members of a specified type.

- Type constraints
  - Some support for refinement types.
  - This is useful for statically avoiding a specific class of bugs which are
    otherwise not possible to detect at compile time (in pure C).
  - This has proven to be extremely useful in a small number of cases.  I
    expect to add more type constraints as `poof` matures.

- Experimental: Static analysis
  - `poof` can be used as a C++ library to build custom static analyzers.  This
    is not well documented, but if you're interested in trying this I'd be
    happy to write a tutorial.
  - The code that enables this is somewhat janky right now, but static-analysis
    is a medium-term goal for `poof`, so it will get better Soon™


# Getting started

## Required Tools to Build

`poof` requires a C++ compiler to build, though I'm making an effort to remove
the use of C++ features in favor of using `poof` itself.

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

Currently unsupported.  If you're interested in compiling poof on OSX please
open an issue :)

# Building

### Clone:

`git clone --recursive https://github.com/scallyw4g/poof.git poof`

### Build:

`./make.sh BuildAllBinariesRunAllTests` Builds everything and runs all tests

---

`./make.sh BuildAllBinaries` Builds `poof` and test binaries

`./make.sh BuildPoof` Builds poof binary; required for running integration tests

`./make.sh BuildParserTests` Builds parser test suite binary

---

`./make.sh BuildPoofEmcc` Build poof targeting WASM.  Requires `emcc` to be installed

`./make.sh BootstrapWeb` Once you've installed emcc, this bootstraps the rest of the WASM compilation tools

---

`./make.sh RunPoof` Runs poof against its own codebase.  Useful for development

---

`./make.sh RunAllTests` Run all test suites

`./make.sh RunParserTests` Runs parser test suite

`./make.sh RunIntegrationTests` Runs integration test suite

---

`./make.sh BootstrapExtendedIntegrationTests` Downloads source for extended integration tests

`./make.sh RunExtendedIntegrationTests` Runs extended integration test suite

---


# Current Status

`poof` is currently undergoing a hardening phase before a 0.1.0-alpha release.

At the time of this writing, the tool relies on itself to generate ~3.5k LoC
(14k if you count debug printing code) and makes use of every documented
feature.  [Bonsai](https://github.com/scallyw4g/bonsai) relies on `poof` to
generate ~15k LoC (>30k including DebugPrint).

## Roadmap to 0.1.0-alpha

[✓] = finished

[o] = started

[\_] = unstarted

---

### Documentation
[✓] Write example code

[o] Formally specify feature set for 0.1.0-alpha

[\_] Write language documentation

### Behavior & Testing
[✓] Parse include graph of `poof` (excluding CRT headers)

[✓] Parse include graph of `poof` (including CRT headers)

[✓] Write test suite that exhaustively validates behavior of the C parser

[✓] Write test suite that validates the behavior of `poof`

### Hardening
[✓] parse all C headers on my linux system

[o] parse all C headers on my windows system

[o] parse some open-source C and C++ projects
  - [✓] C++ https://github.com/scallyw4g/poof
  - [✓] C++ https://github.com/scallyw4g/bonsai
  - [✓] C++ https://github.com/HandmadeHero
  - [o] C++ https://github.com/oil/oil
  - [✓] C++ https://github.com/nakst/gf
  - [✓] C https://github.com/ndilieto/uacme
  - [✓] C https://github.com/sqlite/sqlite
  - [✓] C https://github.com/heroseh/hcc
  - [o] C https://github.com/redis/redis

[✓] collate the above projects into an extended integration test suite to ensure we don't break the parser

### UX
[✓] Fix a laundry list of issues I have logged that result in unnecessary friction

[✓] Audit error messages; find nonsensical, misleading or missing errors

[✓] Control whitespace in generated code



# Examples

See the [examples](https://github.com/scallyw4g/poof/tree/master/examples)
folder for, well, examples of how to use `poof`.  Take note that these examples
are meant to demonstrate language features, and make heavy use of printf.  This
is not a typical use-case, but it makes visualizing the output of `poof` easier.

For less contrived examples of using `poof` have a look at the
[functions](https://github.com/scallyw4g/poof/blob/master/poof/functions.h) used
to build `poof`, and their
[output](https://github.com/scallyw4g/poof/tree/master/poof/generated).

