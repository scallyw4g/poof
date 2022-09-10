enum foobar_dunion_type
{
  type_foobar_dunion_noop,
  type_foo_struct_0,
  type_bar_struct,
};

struct foobar_dunion
{
  enum foobar_dunion_type Type;

  union
  {
    struct foo_struct_0 foo_struct_0;
    struct bar_struct bar_struct;
  };
};


