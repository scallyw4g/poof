        bonsai_function counted_string
    ToString( declaration_type Type)
    {
      counted_string Result = {};
      switch (Type)
      {
                    case type_declaration_noop: { Result = CSz("type_declaration_noop"); } break;
            case type_enum_decl: { Result = CSz("type_enum_decl"); } break;
            case type_function_decl: { Result = CSz("type_function_decl"); } break;
            case type_compound_decl: { Result = CSz("type_compound_decl"); } break;
            case type_variable_decl: { Result = CSz("type_variable_decl"); } break;

      }
      return Result;
    }

        bonsai_function declaration_type
    DeclarationType(counted_string S)
    {
      declaration_type Result = {};

                if (StringsMatch(S, CSz("type_declaration_noop"))) { return type_declaration_noop; }
          if (StringsMatch(S, CSz("type_enum_decl"))) { return type_enum_decl; }
          if (StringsMatch(S, CSz("type_function_decl"))) { return type_function_decl; }
          if (StringsMatch(S, CSz("type_compound_decl"))) { return type_compound_decl; }
          if (StringsMatch(S, CSz("type_variable_decl"))) { return type_variable_decl; }


      return Result;
    }


