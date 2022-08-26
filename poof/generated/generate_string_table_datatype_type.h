    bonsai_function counted_string
    ToString( datatype_type Type)
    {
      counted_string Result = {};
      switch (Type)
      {
                    case type_datatype_noop: { Result = CSz("type_datatype_noop"); } break;
            case type_declaration: { Result = CSz("type_declaration"); } break;
            case type_enum_member: { Result = CSz("type_enum_member"); } break;
            case type_type_def: { Result = CSz("type_type_def"); } break;

      }
      return Result;
    }

