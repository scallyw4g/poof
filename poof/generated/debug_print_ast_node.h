    bonsai_function void
    DebugPrint( ast_node *UnionStruct, u32 Depth)
    {
      if (UnionStruct)
      {
        DebugPrint(*UnionStruct, Depth+4);
      }
    }

    bonsai_function void
    DebugPrint( ast_node UnionStruct, u32 Depth)
    {
      switch(UnionStruct.Type)
      {
                  
                                    case type_ast_node_statement:
              {
                DebugPrint(UnionStruct.ast_node_statement, Depth+4);
              } break;
              case type_ast_node_access:
              {
                DebugPrint(UnionStruct.ast_node_access, Depth+4);
              } break;
              case type_ast_node_literal:
              {
                DebugPrint(UnionStruct.ast_node_literal, Depth+4);
              } break;
              case type_ast_node_expression:
              {
                DebugPrint(UnionStruct.ast_node_expression, Depth+4);
              } break;
              case type_ast_node_parenthesized:
              {
                DebugPrint(UnionStruct.ast_node_parenthesized, Depth+4);
              } break;
              case type_ast_node_operator:
              {
                DebugPrint(UnionStruct.ast_node_operator, Depth+4);
              } break;
              case type_ast_node_function_call:
              {
                DebugPrint(UnionStruct.ast_node_function_call, Depth+4);
              } break;
              case type_ast_node_return:
              {
                DebugPrint(UnionStruct.ast_node_return, Depth+4);
              } break;
              case type_ast_node_initializer_list:
              {
                DebugPrint(UnionStruct.ast_node_initializer_list, Depth+4);
              } break;
              case type_ast_node_symbol:
              {
                DebugPrint(UnionStruct.ast_node_symbol, Depth+4);
              } break;
              case type_ast_node_variable_def:
              {
                DebugPrint(UnionStruct.ast_node_variable_def, Depth+4);
              } break;
              case type_ast_node_type_specifier:
              {
                DebugPrint(UnionStruct.ast_node_type_specifier, Depth+4);
              } break;




        InvalidDefaultCase;
      }
    }

