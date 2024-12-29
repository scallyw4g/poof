#include <tests/integration/src/common_structures.h>

poof(
  func (foo_enum Type) @omit_include
  {
    Type.map_values (Value)
    {
      (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/anonymous_function_foo_1.h

poof(
  func (foo_enum Type) @omit_include
  {
    Type.map (Value)
    {
      (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/anonymous_foo_enum_5DGxlNu5.h


poof(
  func (foo_struct_1 Type) @omit_include
  {
    Type.map_members (Value)
    {
      (Value.type) (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/anonymous_foo_struct_1_M6dQmZmw.h

poof(
  func (foo_struct_1 Type) @omit_include
  {
    Type.map (Value)
    {
      (Value.type) (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/anonymous_foo_struct_1_03rZn5Ny.h


poof(d_union foobar_dunion { foo_struct_1 bar_struct } )
#include <tests/integration/generated/iteration/d_union_foobar_union.h>

poof(
  func (foobar_dunion Foobar) @omit_include
  {
    -- all members start --

    Foobar.map_members (Member)
    {
      (Member.type) (Member.name) (Member.value)
    }

    -- all members end --

    -- filtered members start --

    Foobar.map_members(M)
    {
      M.is_union?
      {
        M.map_members(UM)
        {
          UM.contains_type(nested_struct)?
          {
            (UM.type) (UM.name)
          }
        }
      }
    }

    -- filtered members end --


  }
)
// tests/integration/generated/iteration/anonymous_foobar_dunion_E2SFNKQr.h


poof(
  func symbol_map_test(type, type_poof_symbol symbol) @omit_include
  {
    symbol.map(s, index)
    {
      type.name iteration (index) s
    }
  }
)

poof(symbol_map_test(foo_struct_1, {foo bar baz}))
// tests/integration/generated/iteration/symbol_map_test_foo_struct_1_79487798.h


poof(
  func index_map_test(type, type_poof_index count) @omit_include
  {
    count.map(index)
    {
      type.name iteration index / count
    }
  }
)

poof(index_map_test(foo_struct_1, 3))
// tests/integration/generated/iteration/index_map_test_foo_struct_1_3.h


int test_func(int *bar, float **baz, foo_struct_1 &fuzz)

poof(
  func func_arg_map_test(func_t) @omit_include
  {
    func_t.map(arg)
    {
      arg
    }
  }
)

poof(func_arg_map_test(test_func))
// tests/integration/generated/iteration/func_arg_map_test_test_func.h



poof(
  func var_func(arg) @omit_include
  {
    @var new_string_name counted_string

    new_string_name.map(member)
    {
      member
    }

  }
)

poof(var_func(foo_struct_1))
// tests/integration/generated/iteration/var_func_struct_156032401.h
