enum meta_func_arg_type
{
  type_meta_func_arg_noop,
  type_datatype,
  type_poof_index,
};

struct meta_func_arg
{
  enum meta_func_arg_type Type;
  counted_string Match;

  union
  {
    struct datatype datatype;
    struct poof_index poof_index;
  };
};


