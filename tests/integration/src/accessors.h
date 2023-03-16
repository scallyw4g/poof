#include <tests/integration/src/common_structures.h>

poof(
  func (bar_struct B)
  {
    B.member(0, (M) {
      M.type M.name
    })
  }
)
#include <tests/integration/generated/accessors/anonymous_bar_struct_rHXPMI2N.h>


poof(
  func (array_struct B)
  {
    B.member(0, (M) {
        /* M.is_array? { isarray } { notarray } */
        M.map_array (Index) {
        }
      /* M.type M.name M.value M.array */
    })
  }
)
#include <tests/integration/generated/accessors/anonymous_array_struct_jBUFDQ5P.h>


