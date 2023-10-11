# Pre-alpha note

A small subset of the functionality described here is currently not
implemented.  I have use cases in mind for nearly everything in this document,
barring a single feature.

# Introduction

The `poof` metaprogramming environment consists of two phases.  First, a
bespoke C parser builds internal datastructures representing the types in your
program.  Those datastructures are then exposed via a small scripting language
embedded directly into your source files.

# Language Overview

The `poof` language is a DSL embedded in your source files.  Since C compilers
don't know or understand poof, we need a way of tricking them into ignoring our
DSL.  We can do that by slightly abusing the preprocessor, and defining a macro
that erases everything inside of it.  Don't worry, this is the only time we're
ever going to even think about the preprocessor.

```
#define poof(...)
```

# Functions

Writing `poof` code is similar to other C-like languages.  Typically, you define
functions, then call those functions.  A poof function looks something like this:

```
poof( func my_poof_func() { ... do stuff here ... } )
```

Calling that function looks like this:

```
poof( my_poof_func() )
#include <generated/my_poof_func.h>
```

The output of this function call gets written to the include file immediately
following the `poof` statement.  Poof generates and inserts the includes for
you, so you can just write the `poof` code and not worry about the output
files.  That said, it blindly uses the include on the next line, so if you want
to change the name or location of a specific generated file, you can manually
edit the include and it will respect that edit.

Users can also define anonymous functions, which execute immediately.

```
poof( func() { ... do stuff here ... } )
#include <generated/anonymous_function_k563f9da.h>
```

This is useful for quickly doing one-off functionality.


## Function Arguments

Arguments in `poof` are strongly typed.  There are a few types, listed here
from most common to least:

```
type_datatype
type_poof_symbol
type_poof_index
```


### type_datatype

Arguments of `type_datatype` are by far the most common.  These are type
arguments similar in spirit to template arguments in other languages that
support template metaprogramming.  Let's look at a few examples:


```
poof( func my_poof_func(type_datatype Arg1) {} )

poof( my_poof_func(int) )
#include <generated/my_poof_func_int.h>

poof( my_poof_func(float) )
#include <generated/my_poof_func_float.h>

struct mystruct { int foo; double bar; };

poof( my_poof_func(mystruct) )
#include <generated/my_poof_func_mystruct.h>
```

Arguments in `poof` are so commonly type_datatype that you can skip typing it.
If an argument does not have an explicit type, it's implicilty `type_datatype`.

```
// Arg1 is implicitly `type_datatype` in this function definition
poof( func my_poof_func(Arg1) {} )
```

### type_poof_symbol

Symbols in poof are similar to strings in other languages, with one key
difference; they can be used to look up members on types.  Symbols are still
relatively early in development and may change somewhat, but the principal is
sound and they are likely to stay largely intact.

An example of a symbol argument being defined:

```
poof( func my_poof_func(type_poof_symbol Arg1) {} )
```

Calling a function with a symbol argument has slightly special syntax:

```
// Symbols are surrounded with curly braces such that you can put anything
// between them, including commas and additional sets of braces:

poof( my_poof_func({symbol_argument}) )

poof( my_poof_func({this is, legal}) )

poof( my_poof_func({this is {also} legal}) )

// All of these arguments are considered a single symbol.  A future revision of
// poof may add a distinct case for singular symbols, as opposed to multiple or
// complex symbols.
```


### type_poof_index

The index type in poof is similar to an integer type in other languages,
however indexes in poof have an optional maximum value.  This feature is
questionable, and is likely to be modified in future revisions of the language.

An example of an index argument being defined:

```
poof( func my_poof_func(type_poof_index Arg1) {} )
```

### anonymous function arguments

Anonymous functions are unnamed, and take type arguments that are defined.

```
struct my_struct { int foo; float bar; };

// Here we declare an anonymous function, and it executes immediately.
//
poof( func (my_struct Arg1) {} )
#include <generated/anonymous_function_my_struct_9fk15od.h>


// Passing undefined types, while legal for named functions, is not legal for
// anonymous functions.
//
poof( func (totally_bogus_undefind_type_name Arg1) {} ) // <-- Poof Type Error

```

## Function Bodies

Now we get to the fun part, what functions actually do.  Let's start with a
trivial example that has an obvious 1:1 mapping using the C preprocessor, and
C++ templates.


```
//
// Generate a struct with a single member of the type we pass

poof( func make_new_struct(Arg1) {
  struct new_struct {
    Arg1.name value;
  };
})

poof( make_new_struct(int) )
#include <generated/make_new_struct_int.h>

new_struct instance;
instance.value = 1;


//
// Using the C preprocessor to do the same might look like

#define make_new_struct(Arg1) \
  struct new_struct {         \
    Arg1 value;            \
  };

make_new_struct(int)

new_struct instance;
instance.value = 1;

//
// Or using C++ templates

template <typename Arg1>
struct new_struct {
  Arg1 value;
};

new_struct<int> instance;
instance.value = 1;
```


## Type operators

The core of the `poof` language revolves around the idea of applying operators
to type arguments.  In the previous example, we made use of the `.name`
operator to replace the type argument with the name of the type.

There are many type operators in `poof`, which we'll go through in order here.


```
// literal values
name
type
value
array

// iterative
map_members
map_values
map_array
map

// utility
sep

// accessors
member

// predicates
is_enum
is_struct
is_union
is_defined
is_compound
is_primitive
is_function
is_array
is_type
is_named
contains_type
```

### Literal Operators

:fire: name

Replaces the type argument with the name of the type.

```
// For example, the name of the following struct is my_struct

struct my_struct
{
  int foo;
};
```

:fire: type

Replaces the type argument with the type of the type.  The type of a struct is
`struct`, the type of an enum is `enum`, the type of an integer literal is `int` .. etc.

:fire: value

Replaces the type argument with the default value of the type.  A common
use case for this is when iterating over bitfield enums and the value of
the named enum member needs to be known.

NOTE: This only emits a value if the member has a default value.  It seems
plausibly useful that, for enums specifically, it always emits their integer value.

Given this example enum:

```
enum example_enum
{
  enum_value_0,
  enum_value_1,
  enum_value_2 = 42,
  enum_value_3 = (1 << 31),
};
```

The .value operator for enum_value_0 outputs:

The .value operator for enum_value_1 outputs:

The .value operator for enum_value_2 outputs: 42

The .value operator for enum_value_3 outputs: (1 << 31)

Given this example struct:

```
struct example_struct
{
  int foo;
  int bar = 0;
  float the_answer = 42.f;
};
```

The .value operator for foo outputs:

The .value operator for bar outputs: 0

The .value operator for the_answer outputs: 42.f


:fire: array

Replaces the type argument with the array size of the type.  This is currently
not implemented, and will remain so until I discover a use-case. I expect I
will discover one eventually, so I'm putting it on the Alpha roadmap.

### Iterative Operators

Iterators are the bread-and-butter of most poof functions.

:fire: map_members

Iterator that visits each member of a struct or union, in the order they are defined.

A quick example of mapping over a struct with an anonymous function:

```
struct my_struct { int foo; float bar; };

poof(
  func (my_struct S)
  {
    S.map_members (Member)
    {
      Member.type Member.name
    }
  }
)

// This function will output:

int foo
float bar

```

:fire: map_values

Iterator that visits each member of an enum, in the order they are defined.

A quick example of mapping over an enum with an anonymous function:

```
enum my_enum {
  myenum_value0,
  myenum_value1,
  myenum_value2 = (1 << 31),
};

poof(
  func (my_enum E)
  {
    E.map_values (Value)
    {
      Value.name Value.value
    }
  }
)

// This function will output:

myenum_value0
myenum_value1
myenum_value2 (1 << 31)

```

:fire: map_array

Iterator that visits each index of an array, in the order they are defined.

This iterator is difficult to produce a contrived example for, but will be
explored later in more advanced examples.

:fire: map

Iterator that is agnostic of the input type.  Any of the input types above will
work.  If the input type cannot be iterated, `poof` will emit an error.

:fire: sep

Utility operator used in conjunction with iterators.  Sep accepts a symbol as
an argument and interleaves that symbol between executions of the iterator.

```
struct my_struct { int foo; float bar; };

poof(
  func (my_struct S)
  {
    S.map_members.sep({&&}) (Member) { Member.name }
  }
)

// This function will output:

foo && bar

```

### Accessor Operators

:fire: member

The member operator accesses a member by index, or by name.

NOTE: Access by name is currently not implemented.

```
struct my_struct { int foo; float bar; };

poof(
  func (my_struct S)
  {
    S.member (0, (ZerothMember) {
      ZerothMember.name
    })
    S.member (1, (FirstMember) {
      FirstMember.name
    })

    S.member (foo, (fooMember) {
      fooMember.name
    })
    S.member (bar, (barMember) {
      barMember.name
    })
  }
)

// This function will output:

foo
bar

foo
bar
```


### Predicate Operators

When iterating over a type it is sometimes useful to be able to do branching.
While `poof` does not have arbitrary branching conditionals, it does have some
useful builtin predicates.

Generally, the syntax for executing a predicate is:

```
TypeArgument.predicate? { true branch } { false branch }
```

The false branch is optional, and I've personally found it to be rarely useful.

Note there is no keyword between the true and false branches.  This is intentional.
If there were an else keyword of some sort, it would need to be a distinct
symbol that does not ever appear in C code.  I decided that would be ugly, and
to have the branches be implicitly one-after-the-other.  This is somewhat
subject to change in the future; there are problems with this approach as well.


:fire: is_enum

```
struct my_struct { int foo; float bar; };

poof(
  func (my_struct S)
  {
    S.is_enum? { is an enum! } { is not an enum }
  }
)

// This function will output:

is not an enum

```

:fire: is_struct

```
struct my_struct { int foo; float bar; };

poof(
  func (my_struct S)
  {
    S.is_struct? { is a struct! } { is not a struct }
  }
)

// This function will output:

is a struct!

```

:fire: is_union

Checks if the given type a union.

:fire: is_defined

Checks if the given type is defined.  Extremely useful in rare cases.

:fire: is_compound

Checks if the given type is either a struct or union.

:fire: is_primitive

Checks if the given type is primitive.  Primitive types are the following (including valid combinations, and pointers to these types):

`int, float, long, double, char, unsigned, void`

:fire: is_function

Checks if the given type is a function.

:fire: is_array

Checks if the given type has an array length.

:fire: is_type

Checks if the given type is equal to the type name specified.

:fire: is_named

Checks if the member has a specific name.

:fire: contains_type

Checks if a compound type (struct or union) contains a given type.  This is a
sugar operator; it could be implemented by users with a loop and conditional,
but the sugar'd version is very concise by comparison.


## Extended examples

See the [poof](https://github.com/scallyw4g/poof) website for some more in-depth examples.
