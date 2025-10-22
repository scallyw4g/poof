#include <tests/integration/src/common_structures.h>

struct foo
{
};

struct foo poof(@foo_tag(bar))
{
};


poof( func (foo F) @omit_include { F.tag_value(foo_tag) } )
// tests/integration/generated/accessors/anonymous_foo_5DPVctl0.h


poof(
  func (bar_struct B) @omit_include
  {
    B.member(0, (M) {
      M.type M.name
    })
  }
)
// tests/integration/generated/accessors/anonymous_bar_struct_XqHSO27n.h


poof(
  func (array_struct B) @omit_include
  {
    B.member(0, (M) {
      M.is_array? { isarray } { notarray }
      M.map_array (Index) {
        Index foo
      }
      M.type
      M.name
      M.value
      M.array
    })

    B.map (member)
    {
      member.type member.name member.array
    }
  }
)
// tests/integration/generated/accessors/anonymous_array_struct_R1CjBJgY.h


poof(
  func function_test(F) @omit_include
  {
    F.is_function? { is_func } { not_func }

    /// true if return type is not void

    F.value? { has return type } { return void }

    /// return type
    value ((F.value))

    /// function name
    name  ((F.name))

    /// Unclear what this should be
    /// type  ((F.type))

    // Args {
    F.map (arg)
    {
      arg
    }
    // }
  }
)

poof(function_test(foofunc))
// tests/integration/generated/accessors/anonymous_foofunc_6akI3os3.h

poof(function_test(foofunc0))
// tests/integration/generated/accessors/function_test_foofunc0.h

poof(function_test(typedefd_foofunc))
// tests/integration/generated/accessors/function_test_typedefd_foofunc.h


poof(
  func function_test_1(type) @omit_include
  {
    type.map(M)
    {
      M.is_function?
      {
        /// true if return type is not void
        M.value? { has_return_type } { no_return_type }

        /// return type
        value ((M.value))

        /// function name
        name  ((M.name))

        /// Unclear what this should be
        type  ((M.type))

        // Args {
        M.map_args (arg)
        {
          arg
        }
        // }

      }
    }
  }
)

poof(function_test_1(foo_struct_1))
// tests/integration/generated/accessors/function_test_1_foo_struct_1.h
