    bonsai_function void
    DebugPrint( datatype *Struct, u32 Depth)
    {
      if (Struct)
      {
        DebugPrint(*Struct, Depth+4);
      }
    }

    bonsai_function void
    DebugPrint( datatype Struct, u32 Depth)
    {
      switch(Struct.Type)
      {
                  
                                    case type_declaration:
              {
                DebugPrint(Struct.declaration, Depth+4);
              } break;
              case type_primitive_def:
              {
                DebugPrint(Struct.primitive_def, Depth+4);
              } break;
              case type_enum_member:
              {
                DebugPrint(Struct.enum_member, Depth+4);
              } break;
              case type_type_def:
              {
                DebugPrint(Struct.type_def, Depth+4);
              } break;




        InvalidDefaultCase;
      }
    }

