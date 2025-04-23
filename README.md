# Quickstart

```
git clone --recursive https://github.com/scallyw4g/poof.git poof
cd poof && ./make.sh BuildAllBinariesRunAllTests
```

# Documentation

There is a language primer and reference in [language_introduction.md](language_introduction.md)

There's a [website](https://scallyw4g.github.io/poof/) with some more in-depth
examples and an interactive playground if you want quickly give poof a try.

# About

`poof` is a metaprogramming environment that aims to augment the C programming
language with modern, and sometimes experimental, metaprogramming techniques.

`poof` was born out of frustration with the lack of ergonomics when
metaprogramming with C++ templates.  The canonical example is printing an enum
value as a string.  What _should_ be an extremly simple operation turns into an
almost comical dance of macros and templates.  If you don't to use macros it's
even more hilarious.  In `poof`, printing any enum value as it's
human-readable string is one concise line, and going back the other way
from-string-to-int is also a 1-liner.

# Getting started

See [https://scallyw4g.github.io/poof](https://scallyw4g.github.io/poof)

# Project Status

At the time of this writing, `poof` relies on itself to generate >15k LoC and
makes use of every documented feature.

[Bonsai](https://github.com/scallyw4g/bonsai) relies on `poof` to generate >40k
LoC.

## Roadmap to 0.2.0

[✓] = finished

[o] = started

[\_] = unstarted

---

### Documentation
[✓] Write example code

[✓] Formally specify feature set for 0.2.0

[o] Write language documentation

[o] Complete implementation of feature set for 0.2.0

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

For examples that are less contrived than the ones found in the documentation
section, take a look at the [functions](https://github.com/scallyw4g/bonsai_stdlib/blob/master/headers/poof_functions.h)
used to build `poof`, and their [output](https://github.com/scallyw4g/poof/tree/master/poof/generated).

