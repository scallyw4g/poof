
#include <tests/integration/src/common_structures.h>

poof(
  func foobaz(type) @omit_include
  {
    foobaZ!
  }
)

struct thing
{
  int foo;
  int baz;
};

struct thing_two
{
  int foo2;
  int baz2;
};

poof(foobaz(thing))
// tests/integration/generated/functions/foobaz_.h


poof(
  func func2(type) @omit_include
  {
    @are_equal(type, cs)?
    {
      // type == type
      type.hash == @cs.hash
    }
    {
      // type != type
      type.hash != cs.hash
    }
  }
)

poof(func2(thing))
// tests/integration/generated/functions/func2_thing.h


poof(
  func var_func(arg) @omit_include
  {
    @var new_arg (arg.name)_static_string

    new_arg
  }
)

poof(var_func(thing))
// tests/integration/generated/functions/var_func_thing_786368078.h

poof(
  func poof_comment_func(ignored) @omit_include
  {
    // this comment should exist
    /// this comment should not exist
  }
)

poof(poof_comment_func(thing))
// tests/integration/generated/functions/poof_comment_func_thing.h


poof(
  func transforms_func(arg) @omit_include
  {
    @var new_arg (arg.name)_static_string

    new_arg.to_capital_case
    new_arg.to_capital_case.to_lowercase

    @var new_arg_2 thing_two
  }
)

poof(transforms_func(thing))
// tests/integration/generated/functions/transforms_func_struct_525555696.h
