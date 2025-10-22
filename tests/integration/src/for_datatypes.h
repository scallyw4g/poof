#include <tests/integration/src/common_structures.h>


poof(
  func print_type_data(Type)
  {
    name((Type.name))
    type((Type.type))
    value((Type.value))
    array((Type.array))
    hash((Type.hash))
    indirection((Type.indirection))
    tags((Type.tags))
    location((Type.file):(Type.line))
  }
)

poof(
  for_datatypes(all) @code_fragment
  func (Type)
  {
    print_type_data(Type)

    Type.is_function?
    {
      function args
      {
        Type.map(m)
        {
          m.type (m.indirection)m.name
        }
      }
    }

    Type.is_struct?
    {
      struct members
      {
        Type.map_members(m)
        {
          m.type (m.indirection)m.name
        }
      }
    }

    Type.is_enum?
    {
      enum values
      {
        Type.map_values(m)
        {
          m.type m.name m.value? {= m.value }
        }
      }
    }

    //

  }
)
#include <tests/integration/generated/for_datatypes/for_datatypes_TvI0ir1L.h>


poof(
  for_datatypes(struct enum func) @code_fragment

  func (struct_t)
  {
    struct_t.name struct_t.is_struct? { pass } { fail }
  }

  func (enum_t)
  {
    enum_t.name enum_t.is_enum? { pass } { fail }
  }

  func (func_t)
  {
    func_t.name func_t.is_function? { pass } { fail }
  }
)
#include <tests/integration/generated/for_datatypes/for_datatypes_eF0cqh5M.h>

poof(
  for_datatypes(struct) @code_fragment
  func (struct_t)
  {
    struct_t.name struct_t.is_struct? { pass } { fail }
  }
)
#include <tests/integration/generated/for_datatypes/for_datatypes_SdT7l23L.h>

poof(
  for_datatypes(enum) @code_fragment
  func (enum_t)
  {
    enum_t.name enum_t.is_enum? { pass } { fail }
  }
)
#include <tests/integration/generated/for_datatypes/for_datatypes_JGEIc02V.h>

poof(
  for_datatypes(func) @code_fragment
  func (func_t)
  {
    func_t.name func_t.is_function? { pass } { fail }
  }
)
#include <tests/integration/generated/for_datatypes/for_datatypes_lTJHCJfY.h>

