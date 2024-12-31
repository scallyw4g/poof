
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
    @var new_arg (arg.name)_whatever

    new_arg.to_capital_case
    new_arg.to_capital_case.to_lowercase

    new_arg.to_snake_case
    new_arg.to_uppercase
    new_arg.strip_single_prefix
    new_arg.strip_all_prefix

    @var new_arg_2 thing_two

    arg.hash
    arg.hash.to_lowercase

    // foo
    arg.member(0, (m) {m.name})
    arg.member(0, (m) {m.name.to_capital_case})
    arg.member(0, (m) {m.name.to_lowercase})
    arg.member(0, (m) {m.name.to_uppercase})

    // foo2
    new_arg_2.member(0, (m) {m.name})
    new_arg_2.member(0, (m) {m.name.to_uppercase})
    new_arg_2.member(0, (m) {m.name.to_uppercase.to_lowercase}).to_uppercase
  }
)

poof(transforms_func(thing))
// tests/integration/generated/functions/transforms_func_struct_525555696.h


poof(
  func gen_struct_func(arg)  @omit_include
  {
    struct new_(arg)
    {
    };

    @var var_thing new_thing

    var_thing
  }
)

poof(gen_struct_func(thing))
// tests/integration/generated/functions/gen_struct_func_struct_473003373.h
