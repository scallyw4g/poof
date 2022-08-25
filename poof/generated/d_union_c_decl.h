enum struct_member_type
{
  type_struct_member_noop,
  type_variable_decl,
  type_compound_decl,
  type_function_decl,
};

struct struct_member
{
  enum struct_member_type Type;

  union
  {
    struct variable_decl variable_decl;
    struct compound_decl compound_decl;
    struct function_decl function_decl;
  };
};


