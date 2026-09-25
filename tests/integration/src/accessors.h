#include <tests/integration/src/common_structures.h>

struct foo
{
};

struct foo poof(@foo_tag(bar))
{
};


poof( func (foo F) @omit_include { F.tag_value(foo_tag) } )
// tests/integration/generated/accessors/poof_func.anonymous$foo$6UDBSxku.h


poof(
  func (bar_struct B) @omit_include
  {
    B.member(0, (M) {
      M.type M.name
    })
  }
)
// tests/integration/generated/accessors/poof_func.anonymous$bar_struct$9bWjNL42.h


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
// tests/integration/generated/accessors/poof_func.anonymous$array_struct$t9AolvIE.h


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
// tests/integration/generated/accessors/function_test$foofunc$Pb9U2E8c.h

poof(function_test(foofunc0))
// tests/integration/generated/accessors/function_test$foofunc0$ixbkXujS.h

poof(function_test(typedefd_foofunc))
// tests/integration/generated/accessors/function_test$typedefd_foofunc$lxjDEZCy.h


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
// tests/integration/generated/accessors/function_test_1$foo_struct_1$udYxRZY1.h

poof(
  func (foo_struct_1 type) @omit_include
  {
    // {
    type.member(_embedded3, (m) {
        m.type m.name 

        m.map_members(m2)
        {
          m2.type m2.name
        }

        m.has_tag(type_tag)?
        {
          @var enum_t m.tag_value(type_tag)

          enum_t.map_values(enum_v)
          {
            enum_v.type enum_v.name
          }
        }
        {
          error
        }
    })
    // }

    // {
      type.map(m)
      {
        m.is_union?
        {
          m.has_tag(type_tag)?
          {
            @var enum_t m.tag_value(type_tag)

            // has!
            {
              enum_t
              enum_t.map_values(enum_v)
              {
                enum_v.type enum_v.name
              }
            }
          }
        }
      }
    // }
  }
)
// tests/integration/generated/accessors/poof_func.anonymous$foo_struct_1$mEYwJngE.h
