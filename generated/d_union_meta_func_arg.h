enum meta_func_arg_type
{
  type_meta_func_arg_noop,
  type_datatype,
};

struct meta_func_arg
{
  enum meta_func_arg_type Type;
  counted_string Match;

  union
  {
    struct datatype datatype;
  };
};


