    bonsai_function void
    DebugPrint( ast_node *Struct, u32 Depth)
    {
      if (Struct)
      {
        DebugPrint(*Struct, Depth);
      }
    }

    bonsai_function void
    DebugPrint( ast_node Struct, u32 Depth)
    {
      DebugPrint("ast_node {\n", Depth);

      /* DebugPrint("Type = ", Depth+4); */
      /* DebugPrint(Struct.Type); */
      /* DebugPrint(";\n"); */

      switch(Struct.Type)
      {
                  
                                    case type_ast_node_statement:
              {
                DebugPrint(Struct.ast_node_statement, Depth+4);
              } break;
              case type_ast_node_access:
              {
                DebugPrint(Struct.ast_node_access, Depth+4);
              } break;
              case type_ast_node_literal:
              {
                DebugPrint(Struct.ast_node_literal, Depth+4);
              } break;
              case type_ast_node_expression:
              {
                DebugPrint(Struct.ast_node_expression, Depth+4);
              } break;
              case type_ast_node_parenthesized:
              {
                DebugPrint(Struct.ast_node_parenthesized, Depth+4);
              } break;
              case type_ast_node_operator:
              {
                DebugPrint(Struct.ast_node_operator, Depth+4);
              } break;
              case type_ast_node_function_call:
              {
                DebugPrint(Struct.ast_node_function_call, Depth+4);
              } break;
              case type_ast_node_return:
              {
                DebugPrint(Struct.ast_node_return, Depth+4);
              } break;
              case type_ast_node_initializer_list:
              {
                DebugPrint(Struct.ast_node_initializer_list, Depth+4);
              } break;
              case type_ast_node_symbol:
              {
                DebugPrint(Struct.ast_node_symbol, Depth+4);
              } break;
              case type_ast_node_variable_def:
              {
                DebugPrint(Struct.ast_node_variable_def, Depth+4);
              } break;
              case type_ast_node_type_specifier:
              {
                DebugPrint(Struct.ast_node_type_specifier, Depth+4);
              } break;




        default : { DebugPrint("default while printing ast_node ast_node ", Depth+4); DebugLine("Type(%d)", Struct.Type); } break;
      }
      DebugPrint("}\n", Depth);
    }

