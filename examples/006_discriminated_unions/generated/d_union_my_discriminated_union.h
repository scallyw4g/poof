// main.c:31:0

enum my_discriminated_union_type
{
  type_my_discriminated_union_noop,
  type_foo_struct,
  type_bar_struct,
  type_baz_struct,
  type_flazz,
};

struct my_discriminated_union
{
  enum my_discriminated_union_type Type;

  union
  {
    struct foo_struct foo_struct;
    struct bar_struct bar_struct;
    struct baz_struct baz_struct;
  };
};


