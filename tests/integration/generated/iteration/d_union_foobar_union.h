enum foobar_dunion_type
{
  type_foobar_dunion_noop,
  type_foo_struct,
  type_bar_struct,
};

struct foobar_dunion
{
  enum foobar_dunion_type Type;

  union
  {
    struct foo_struct foo_struct;
    struct bar_struct bar_struct;
  };
};


