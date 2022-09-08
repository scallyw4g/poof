#define poof(...)

struct foo {};
struct bar {};

enum custom_enum_tag
{
  tag_foo,
  tag_bar,
};

poof(
  d_union foo_bar_baz
  {
    foo,
    bar,
    baz enum_only,
  }
)
#include <tests/integration/generated/discriminated_union/d_union_foo_bar_baz.h>


enum custom_enum_tag
{
  tag_foo,
  tag_bar,
};

poof(
  d_union foo_bar_baz_cust custom_enum_tag
  {
    foo,
    bar,
    baz enum_only,
  },
  {
    foo common;
    long long int barf;
  }
)
#include <tests/integration/generated/discriminated_union/d_union_foo_bar_baz_cust.h>
