#define upshift_by_31 << 31

enum foo_enum
{
  foo = 1,
  bar = 'a',
  baz = (1 upshift_by_31),
};


struct foo_struct
{
  int *first[0] = 42, *second = -1;

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

struct bar_struct
{
  int fda;
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

#if 1
meta(
  func (foo_struct Type)
  {
    (Type.map_members (Value) {
      (Value.type) (Value.name) (Value.value)
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_2.h>
#endif


meta (d_union foobar_dunion { foo_struct bar_struct } )
#include <tests/integration/generated/d_union_foobar_union.h>

meta(
  func (foobar_dunion Foobar)
  {
    (Foobar.map_members (Member) {
      (Member.type) (Member.name) (Member.value)
      (Member.is_union? {
        weeeeeeeeee
      })
    })
  }
)
#include <tests/integration/generated/anonymous_function_foo_3.h>

