enum foo_bar_baz_type
{
  type_foo_bar_baz_noop,
  type_foo,
  type_bar,
  type_baz,
};

struct foo_bar_baz
{
  enum foo_bar_baz_type Type;

  union
  {
    struct foo foo;
    struct bar bar;
  };
};


