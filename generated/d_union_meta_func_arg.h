// ./poof/poof.h:620:0

enum meta_func_arg_type
{
  type_meta_func_arg_noop,
  type_datatype,
  type_poof_index,
  type_poof_symbol,
};

struct meta_func_arg
{
  enum meta_func_arg_type Type;
  counted_string Match;

  union
  {
    struct datatype datatype;
    struct poof_index poof_index;
    struct poof_symbol poof_symbol;
  };
};


