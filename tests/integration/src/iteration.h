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
  int *first[0] = 1, *second = 0;

  int bar = 0;
  int baz = (1 << 1) -1;

  float fbaz = 42.0f;
  double dbaz = 42.0;
  long double ldbaz = 42.0;
  long long ldbaz = 42.0;
  long int libaz = 42.0;

  char cbaz = 'f';

  enum foo_enum fenum;
  foo_enum fenum2;
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
