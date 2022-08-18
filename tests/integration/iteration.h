enum foo_enum
{
  bar = 0,
  baz = 1,
};

meta(
  func (foo_enum Type)
  {
    (Type.map_values (Value) {
      (Value.name)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_1.h>


struct foo_struct
{
  int bar;
  int baz;
};

meta(
  func (foo_struct Type)
  {
    (Type.map_members (Value) {
      (Value.type) (Value.name)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_2.h>
