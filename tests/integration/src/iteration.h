#define upshift_by_31 << 31

enum foo_enum
{
  foo = 1,
  bar = 'a',
  baz = (1 upshift_by_31),
};

meta(
  func (foo_enum Type)
  {
    (Type.map_values (Value) {
      (Value.name) (Value.value)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_1.h>


struct foo_struct
{
  int bar = 0;
  int baz = (1 << 1) -1;

  volatile int baz = 42;

  const double *baz = 'foo';
};

meta(
  func (foo_struct Type)
  {
    (Type.map_members (Value) {
      (Value.type) (Value.name) (Value.value)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_2.h>
