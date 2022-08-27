    bonsai_function void
    DebugPrint( declaration *Struct, u32 Depth)
    {
      if (Struct)
      {
        DebugPrint(*Struct, Depth+4);
      }
    }

    bonsai_function void
    DebugPrint( declaration Struct, u32 Depth)
    {
      switch(Struct.Type)
      {
                  
                                    case type_enum_decl:
              {
                DebugPrint(Struct.enum_decl, Depth+4);
              } break;
              case type_function_decl:
              {
                DebugPrint(Struct.function_decl, Depth+4);
              } break;
              case type_compound_decl:
              {
                DebugPrint(Struct.compound_decl, Depth+4);
              } break;
              case type_variable_decl:
              {
                DebugPrint(Struct.variable_decl, Depth+4);
              } break;




        default : { DebugPrint("default while printing (UnionMember.type) (UnionMember.name)"); } break;
      }
    }

