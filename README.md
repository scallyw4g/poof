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

# Getting started

See [https://scallyw4g.github.io/poof](https://scallyw4g.github.io/poof)

# Project Status

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

