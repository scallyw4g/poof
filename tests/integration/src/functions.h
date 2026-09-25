
#include <tests/integration/src/common_structures.h>

poof(
  func foobaz(type) @omit_include
  {
    foobaZ!
  }
)

/* poof( func poof_noop(ignored) @code_fragment {} ) */

struct thing
{
  int foo;
  int baz;

  // NOTE(Jesse): Test we can call a poof function from inside a struct
  /* poof(poof_noop(foobaz)) */
};

struct thing_two
{
  int foo2;
  int baz2;
};

poof(foobaz(thing))
// tests/integration/generated/functions/foobaz$thing$aA3KDoYv.h


poof(
  func nested_func(type) @omit_include
  {
    foobaz(type)
  }
)

poof(nested_func(thing))
// tests/integration/generated/functions/nested_func$thing$630pRD6X.h

// just testing the overload works
poof(
  func are_equal(type)
  {
    type
  }
)

poof(
  func overload_test_func(type) @omit_include
  {
    // should prroduce the hash of the counted_string (cs) type
    @are_equal(type, cs)?
    {
      // type == type
      type.hash == @cs.hash
    }
    {
      // type != type
      type.hash != cs.hash
    }

    // should produce the value of whatever type is
    are_equal(type)
  }
)

poof(overload_test_func(thing))
// tests/integration/generated/functions/overload_test_func$thing$ZLl5Kj2G.h


poof(
  func var_func(arg) @omit_include
  {
    @var new_arg (arg.name)_static_string

    new_arg
  }
)

poof(var_func(thing))
// tests/integration/generated/functions/var_func$thing.525555696$VEx3POGk.h

poof(
  func poof_comment_func(ignored) @omit_include
  {
    // this comment should exist
    /// this comment should not exist
  }
)

poof(poof_comment_func(thing))
// tests/integration/generated/functions/poof_comment_func$thing$4rhNU8xi.h


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
// tests/integration/generated/functions/transforms_func$thing.746483492.thing_two$aIMG5brV.h


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
// tests/integration/generated/functions/gen_struct_func$thing.473003373$sVeyFUCM.h


typedef void (*func_ptr_type)();

poof(
  func func_taking_typdefd_func_arg(arg)  @omit_include
  {
    arg
  }
)

poof(func_taking_typdefd_func_arg(func_ptr_type*))
// tests/integration/generated/functions/func_taking_typdefd_func_arg_KUoyRicK.h
