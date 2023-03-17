#include <tests/integration/src/common_structures.h>

poof(
  func (foo_enum Type)
  {
    Type.map_values (Value)
    {
      (Value.name) (Value.value)
    }
  }
)
#include <tests/integration/generated/iteration/anonymous_function_foo_1.h>

poof(
  func (foo_struct_1 Type)
  {
    Type.map_members (Value)
    {
      (Value.type) (Value.name) (Value.value)
    }
  }
)
#include <tests/integration/generated/iteration/anonymous_foo_struct_1_M6dQmZmw.h>

poof(d_union foobar_dunion { foo_struct_1 bar_struct } )
#include <tests/integration/generated/iteration/d_union_foobar_union.h>

poof(
  func (foobar_dunion Foobar)
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
#include <tests/integration/generated/iteration/anonymous_foobar_dunion_E2SFNKQr.h>

