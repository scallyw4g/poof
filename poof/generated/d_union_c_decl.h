enum struct_member_type
{
  type_struct_member_noop,
  type_variable_decl,
  type_struct_decl,
  type_union_decl,
  type_function_decl,
};

struct struct_member
{
  enum struct_member_type Type;

  union
  {
    struct variable_decl variable_decl;
    struct struct_decl struct_decl;
    struct union_decl union_decl;
    struct function_decl function_decl;
  };
};


