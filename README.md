# Quickstart

```
git clone --recursive https://github.com/scallyw4g/poof.git poof
cd poof && ./make.sh BuildAllBinariesRunAllTests
```

# Documentation

There is a [practical introduction](https://scallywag.software/vim/blog/poof-i)
which demonstrates how to write generic debug printing code with poof.

There is a language primer and reference in [language_introduction.md](language_introduction.md)

# Introduction

`poof` is a metaprogramming environment that aims to augment the C programming
language with modern, and sometimes experimental, metaprogramming techniques.

`poof` was born out of frustration with the lack of ergonomics when
metaprogramming with C++ templates.  The canonical example is printing an enum
value as a string.  What _should_ be an extremly simple operation turns into an
almost comical dance of macros and templates.  If you don't to use macros it's
even more hilarious.  Let's look at an example.

```
enum my_very_important_enum
{
  EnumValue_0,
  EnumValue_1,
  EnumValue_2,
  EnumValue_3,
  EnumValue_4,
};

poof(
  func gen_to_string(enum_t)
  {
    const char *ToString( enum_t.name Value )
    {
      switch (Value)
      {
        enum_t.map(enum_v) { case enum_v.name: return "enum_v.name"; } break;
      }
      return 0;
    }
  }
)

poof( gen_to_string(my_very_important_enum) )
#include "gen_to_string_my_very_important_enum.h"


//
// Inside the generated file 'gen_to_string_my_very_important_enum.h'
//

const char *ToString(my_very_important_enum Type)
{
  switch (Type)
  {
    case EnumValue_0: { return "EnumValue_0"; } break;
    case EnumValue_1: { return "EnumValue_1"; } break;
    case EnumValue_2: { return "EnumValue_2"; } break;
    case EnumValue_3: { return "EnumValue_3"; } break;
    case EnumValue_4: { return "EnumValue_4"; } break;
  }

  return 0;
}
```

# Getting started

See [https://scallyw4g.github.io/poof](https://scallyw4g.github.io/poof)

# Examples

For examples that are less contrived than the ones found in the documentation
section, take a look at the [functions](https://github.com/scallyw4g/bonsai_stdlib/blob/master/src/poof_functions.h)
used to build `poof`, and their [output](https://github.com/scallyw4g/poof/tree/master/generated).

