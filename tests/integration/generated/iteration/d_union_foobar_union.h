// callsite
// tests/integration/src/iteration.h:62:0
// (d_union) def
// tests/integration/src/iteration.h:62:0
enum foobar_dunion_type
{
  type_foobar_dunion_noop,
  type_foo_struct_1,
  type_bar_struct,
};

struct foobar_dunion
{
  enum foobar_dunion_type Type;

  union
  {
    struct foo_struct_1 foo_struct_1;
    struct bar_struct bar_struct;
  };
};

