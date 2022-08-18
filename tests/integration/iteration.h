enum foo
{
  bar = 0,
  baz = 1,
};

meta(
  func (foo Enum)
  {
    (Enum.map_values (Value) {
      (Value.name)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_1.h>
