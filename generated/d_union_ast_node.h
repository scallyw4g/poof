// ./poof/poof.h:986:0

enum ast_node_type
{
  type_ast_node_noop,
  type_ast_node_statement,
  type_ast_node_access,
  type_ast_node_literal,
  type_ast_node_expression,
  type_ast_node_parenthesized,
  type_ast_node_operator,
  type_ast_node_function_call,
  type_ast_node_return,
  type_ast_node_initializer_list,
  type_ast_node_symbol,
  type_ast_node_variable_def,
  type_ast_node_type_specifier,
};

struct ast_node
{
  enum ast_node_type Type;

  union
  {
    struct ast_node_statement ast_node_statement;
    struct ast_node_access ast_node_access;
    struct ast_node_literal ast_node_literal;
    struct ast_node_expression ast_node_expression;
    struct ast_node_parenthesized ast_node_parenthesized;
    struct ast_node_operator ast_node_operator;
    struct ast_node_function_call ast_node_function_call;
    struct ast_node_return ast_node_return;
    struct ast_node_initializer_list ast_node_initializer_list;
    struct ast_node_symbol ast_node_symbol;
    struct ast_node_variable_def ast_node_variable_def;
    struct ast_node_type_specifier ast_node_type_specifier;
  };
};


