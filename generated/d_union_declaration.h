// ./poof/poof.h:351:0

enum declaration_type
{
  type_declaration_noop,
  type_enum_decl,
  type_function_decl,
  type_compound_decl,
  type_variable_decl,
};

struct declaration
{
  enum declaration_type Type;
  poof_tag_block_array Tags;

  union
  {
    struct enum_decl enum_decl;
    struct function_decl function_decl;
    struct compound_decl compound_decl;
    struct variable_decl variable_decl;
  };
};


