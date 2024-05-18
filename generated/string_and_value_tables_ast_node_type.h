// ./poof/poof.h:921:0

link_internal counted_string
ToStringPrefixless(ast_node_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case type_ast_node_noop: { Result = CSz("noop"); } break;
    case type_ast_node_statement: { Result = CSz("statement"); } break;
    case type_ast_node_access: { Result = CSz("access"); } break;
    case type_ast_node_literal: { Result = CSz("literal"); } break;
    case type_ast_node_expression: { Result = CSz("expression"); } break;
    case type_ast_node_parenthesized: { Result = CSz("parenthesized"); } break;
    case type_ast_node_operator: { Result = CSz("operator"); } break;
    case type_ast_node_function_call: { Result = CSz("call"); } break;
    case type_ast_node_return: { Result = CSz("return"); } break;
    case type_ast_node_initializer_list: { Result = CSz("list"); } break;
    case type_ast_node_symbol: { Result = CSz("symbol"); } break;
    case type_ast_node_variable_def: { Result = CSz("def"); } break;
    case type_ast_node_type_specifier: { Result = CSz("specifier"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(ast_node_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case type_ast_node_noop: { Result = CSz("type_ast_node_noop"); } break;
    case type_ast_node_statement: { Result = CSz("type_ast_node_statement"); } break;
    case type_ast_node_access: { Result = CSz("type_ast_node_access"); } break;
    case type_ast_node_literal: { Result = CSz("type_ast_node_literal"); } break;
    case type_ast_node_expression: { Result = CSz("type_ast_node_expression"); } break;
    case type_ast_node_parenthesized: { Result = CSz("type_ast_node_parenthesized"); } break;
    case type_ast_node_operator: { Result = CSz("type_ast_node_operator"); } break;
    case type_ast_node_function_call: { Result = CSz("type_ast_node_function_call"); } break;
    case type_ast_node_return: { Result = CSz("type_ast_node_return"); } break;
    case type_ast_node_initializer_list: { Result = CSz("type_ast_node_initializer_list"); } break;
    case type_ast_node_symbol: { Result = CSz("type_ast_node_symbol"); } break;
    case type_ast_node_variable_def: { Result = CSz("type_ast_node_variable_def"); } break;
    case type_ast_node_type_specifier: { Result = CSz("type_ast_node_type_specifier"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal ast_node_type
AstNodeType(counted_string S)
{
  ast_node_type Result = {};

  if (StringsMatch(S, CSz("type_ast_node_noop"))) { return type_ast_node_noop; }
  if (StringsMatch(S, CSz("type_ast_node_statement"))) { return type_ast_node_statement; }
  if (StringsMatch(S, CSz("type_ast_node_access"))) { return type_ast_node_access; }
  if (StringsMatch(S, CSz("type_ast_node_literal"))) { return type_ast_node_literal; }
  if (StringsMatch(S, CSz("type_ast_node_expression"))) { return type_ast_node_expression; }
  if (StringsMatch(S, CSz("type_ast_node_parenthesized"))) { return type_ast_node_parenthesized; }
  if (StringsMatch(S, CSz("type_ast_node_operator"))) { return type_ast_node_operator; }
  if (StringsMatch(S, CSz("type_ast_node_function_call"))) { return type_ast_node_function_call; }
  if (StringsMatch(S, CSz("type_ast_node_return"))) { return type_ast_node_return; }
  if (StringsMatch(S, CSz("type_ast_node_initializer_list"))) { return type_ast_node_initializer_list; }
  if (StringsMatch(S, CSz("type_ast_node_symbol"))) { return type_ast_node_symbol; }
  if (StringsMatch(S, CSz("type_ast_node_variable_def"))) { return type_ast_node_variable_def; }
  if (StringsMatch(S, CSz("type_ast_node_type_specifier"))) { return type_ast_node_type_specifier; }

  return Result;
}


