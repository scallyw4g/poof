// tests/integration/src/discriminated_union.h:30:0

struct foo_bar_baz_cust
{
  enum custom_enum_tag Type;
  foo common;
  long long int  barf;

  union
  {
    struct foo_bar_baz_cust_type_foo foo_bar_baz_cust_type_foo;
    struct foo_bar_baz_cust_type_bar foo_bar_baz_cust_type_bar;
    struct foo foo;
    struct bar bar;
  };
};


