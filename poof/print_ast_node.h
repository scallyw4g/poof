link_internal void PrintAstNode(ast_node* Node, string_builder *Builder);
link_internal void PrintAstNode(ast_node_expression *Node, string_builder *Builder);
link_internal void PrintAstNode(ast_node_statement* Node, string_builder *Builder);

link_internal void
PrintAstNode(ast_node_expression *Node, string_builder *Builder)
{
  if (Node)
  {
    PrintAstNode(Node->Value, Builder);
    PrintAstNode(Node->Next, Builder);
  }
}

link_internal void
PrintAstNode(ast_node_statement *Node, string_builder *Builder)
{
  if (Node)
  {
    PrintAstNode(Node->LHS, Builder);
    PrintAstNode(Node->RHS, Builder);
    PrintAstNode(Node->Next, Builder);
  }
}

link_internal void
PrintAstNode(ast_node *Node, string_builder *Builder)
{
  if (Node)
  {
    switch(Node->Type)
    {
      case type_ast_node_parenthesized:
      {
        auto Child = SafeAccess(ast_node_parenthesized, Node);
        Append(Builder, CSz("("));
        PrintAstNode(Child->Expr, Builder);
        Append(Builder, CSz(")"));

        if (Child->IsCast)
        {
          NotImplemented;
          Assert(Child->CastValue);
          PrintAstNode(Child->CastValue, Builder);
        }
        else
        {
          Assert(!Child->CastValue);
        }
      } break;

      case type_ast_node_operator:
      {
        auto Child = SafeAccess(ast_node_operator, Node);
        Append(Builder, CSz(" "));
        Append(Builder, Child->Token.Value); // Operator token
        Append(Builder, CSz(" "));
        PrintAstNode(Child->Operand, Builder);
      } break;

      case type_ast_node_symbol:
      {
        auto Child = SafeAccess(ast_node_symbol, Node);
        Append(Builder, Child->Token.Value);
      } break;

      case type_ast_node_literal:
      {
        auto Child = SafeAccess(ast_node_literal, Node);
        Append(Builder, Child->Token.Value);
        /* Append(Builder, CSz(" ")); */
      } break;

      case type_ast_node_expression:
      {
        auto Child = SafeAccess(ast_node_expression, Node);
        PrintAstNode(Child, Builder);
      } break;

      case type_ast_node_variable_def:
      {
        auto Child = SafeAccess(ast_node_variable_def, Node);
        /* DebugPrint(Child->Type); */
        /* DebugPrint(Child->Decl); */

        PrintAstNode(Child->Value, Builder);
      } break;

      case type_ast_node_statement:
      {
        auto Child = SafeAccess(ast_node_statement, Node);
        PrintAstNode(Child, Builder);
      } break;













      case type_ast_node_function_call:
      {
        NotImplemented;
        auto Child = SafeAccess(ast_node_function_call, Node);
      } break;

      case type_ast_node_access:
      {
        NotImplemented;
        auto Child = SafeAccess(ast_node_access, Node);
        PrintAstNode(Child->Symbol, Builder);
      } break;

      case type_ast_node_return:
      {
        NotImplemented;
        auto Child = SafeAccess(ast_node_return, Node);
        PrintAstNode(Child->Value, Builder);
      } break;

      case type_ast_node_initializer_list:
      case type_ast_node_type_specifier:
      {
        NotImplemented;
      } break;

      case type_ast_node_noop:
      {
        InvalidCodePath();
      } break;
    }
  }
}

link_internal counted_string
PrintAstNode(ast_node_expression *Node, memory_arena *Memory)
{
  string_builder Builder = {};
  PrintAstNode(Node, &Builder);
  counted_string Value = Finalize(&Builder, Memory);
  return Value;
}

link_internal counted_string
PrintAstNode(ast_node *Node, memory_arena *Memory)
{
  string_builder Builder = {};
  PrintAstNode(Node, &Builder);
  counted_string Value = Finalize(&Builder, Memory);
  return Value;
}

