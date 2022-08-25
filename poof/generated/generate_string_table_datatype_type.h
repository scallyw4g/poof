    bonsai_function counted_string
    ToString( datatype_type Type)
    {
      counted_string Result = {};
      switch (Type)
      {
                    case type_datatype_noop: { Result = CSz("type_datatype_noop"); } break;
            case type_d_compound_decl: { Result = CSz("type_d_compound_decl"); } break;
            case type_struct_member: { Result = CSz("type_struct_member"); } break;
            case type_enum_def: { Result = CSz("type_enum_def"); } break;
            case type_enum_member: { Result = CSz("type_enum_member"); } break;
            case type_type_def: { Result = CSz("type_type_def"); } break;
            case type_primitive_def: { Result = CSz("type_primitive_def"); } break;
            case type_stl_container_def: { Result = CSz("type_stl_container_def"); } break;

      }
      return Result;
    }

