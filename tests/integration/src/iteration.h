#include <tests/integration/src/common_structures.h>



poof(
  func (foo_enum Type) @omit_include
  {
    Type.map_values(Value)
    {
      (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foo_enum$8JAfOWNX.h

poof(
  func (foo_enum Type) @omit_include
  {
    Type.map(Value)
    {
      (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foo_enum$VhueyP4G.h


poof(
  func (foo_struct_1 Type) @omit_include
  {
    Type.map_members(Value)
    {
      (Value.type) (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foo_struct_1$g2VrVMoJ.h

poof(
  func (foo_struct_1 Type) @omit_include
  {
    Type.map(Value)
    {
      (Value.type) (Value.name) (Value.value)
    }
  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foo_struct_1$JPXNUriq.h


poof(
  func (foo_enum Type) @omit_include
  {
    Type.map_values(Value).sep( ||| )
    {
      Value.has_tag(foo_tag)? { has_foo_tag((Value.name)) }
    }
  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foo_enum$FPahQTP3.h

poof(d_union foobar_dunion { foo_struct_1 bar_struct } )
#include <tests/integration/generated/iteration/poof_builtin.d_union$$TniUcDPl.h>

poof(
  func (foobar_dunion Foobar) @omit_include
  {
    -- all members start --

    Foobar.map_members(Member)
    { (Member.type) (Member.name) (Member.value) }

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
            contains(nested_struct) { (UM.type) (UM.name) }
          }
        }
      }
    }

    -- filtered members end --


  }
)
// tests/integration/generated/iteration/poof_func.anonymous$foobar_dunion$2j5WkWho.h


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
// tests/integration/generated/iteration/symbol_map_test$foo_struct_1.79487798$DPc3oAOM.h


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
// tests/integration/generated/iteration/index_map_test$foo_struct_1.3$3VDHbv6s.h


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
// tests/integration/generated/iteration/func_arg_map_test$test_func$l3V2RXl0.h



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
// tests/integration/generated/iteration/var_func$foo_struct_1.counted_string$yxbYK9Iw.h
