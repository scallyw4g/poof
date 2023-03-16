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
        M.map_array () {
          foo
        }
      M.type M.name M.value
      /* M.array */
    })
  }
)
#include <tests/integration/generated/accessors/anonymous_array_struct_R1CjBJgY.h>



