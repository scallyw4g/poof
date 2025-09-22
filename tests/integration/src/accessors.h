#include <tests/integration/src/common_structures.h>

poof(
  func (bar_struct B)
  {
    B.member(0, (M) {
      M.type M.name
    })
  }
)
#include <tests/integration/generated/accessors/anonymous_bar_struct_XqHSO27n.h>


poof(
  func (array_struct B)
  {
    B.member(0, (M) {
      M.is_array? { isarray } { notarray }
      M.map_array (Index) {
        foo
      }
      M.type M.name
      M.value
      M.array
    })

    B.map (member)
    {
      member.type member.name member.array
    }
  }
)
#include <tests/integration/generated/accessors/anonymous_array_struct_R1CjBJgY.h>


poof(
  func (foofunc F)
  {
    F.is_function? { is_func } { not_func }

    /// return type
    type  ((F.type))

    /// function name
    name  ((F.name))

    /// Unclear what this should be
    /* value ((F.value)) */

    // Args {
    F.map (arg)
    {
      F.arg
    }
    // }
  }
)
#include <tests/integration/generated/accessors/anonymous_foofunc_6akI3os3.h>
