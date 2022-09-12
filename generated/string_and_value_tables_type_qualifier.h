        bonsai_function counted_string
    ToString( type_qualifier Type)
    {
      counted_string Result = {};
      switch (Type)
      {
                  case TypeQual_None: { Result = CSz("TypeQual_None"); } break;
          case TypeQual_Void: { Result = CSz("TypeQual_Void"); } break;
          case TypeQual_Auto: { Result = CSz("TypeQual_Auto"); } break;
          case TypeQual_Class: { Result = CSz("TypeQual_Class"); } break;
          case TypeQual_Struct: { Result = CSz("TypeQual_Struct"); } break;
          case TypeQual_Union: { Result = CSz("TypeQual_Union"); } break;
          case TypeQual_Enum: { Result = CSz("TypeQual_Enum"); } break;
          case TypeQual_Bool: { Result = CSz("TypeQual_Bool"); } break;
          case TypeQual_Inline: { Result = CSz("TypeQual_Inline"); } break;
          case TypeQual_ThreadLocal: { Result = CSz("TypeQual_ThreadLocal"); } break;
          case TypeQual_Static: { Result = CSz("TypeQual_Static"); } break;
          case TypeQual_Extern: { Result = CSz("TypeQual_Extern"); } break;
          case TypeQual_Const: { Result = CSz("TypeQual_Const"); } break;
          case TypeQual_Volatile: { Result = CSz("TypeQual_Volatile"); } break;
          case TypeQual_Signed: { Result = CSz("TypeQual_Signed"); } break;
          case TypeQual_Unsigned: { Result = CSz("TypeQual_Unsigned"); } break;
          case TypeQual_Char: { Result = CSz("TypeQual_Char"); } break;
          case TypeQual_Long: { Result = CSz("TypeQual_Long"); } break;
          case TypeQual_Long_Long: { Result = CSz("TypeQual_Long_Long"); } break;
          case TypeQual_Int: { Result = CSz("TypeQual_Int"); } break;
          case TypeQual_Double: { Result = CSz("TypeQual_Double"); } break;
          case TypeQual_Short: { Result = CSz("TypeQual_Short"); } break;
          case TypeQual_Float: { Result = CSz("TypeQual_Float"); } break;
          case TypeQual_Constexpr: { Result = CSz("TypeQual_Constexpr"); } break;
          case TypeQual_Explicit: { Result = CSz("TypeQual_Explicit"); } break;
          case TypeQual_Operator: { Result = CSz("TypeQual_Operator"); } break;
          case TypeQual_Virtual: { Result = CSz("TypeQual_Virtual"); } break;
          case TypeQual_Noreturn: { Result = CSz("TypeQual_Noreturn"); } break;

      }
      return Result;
    }

        bonsai_function type_qualifier
    TypeQualifier(counted_string S)
    {
      type_qualifier Result = {};

              if (StringsMatch(S, CSz("TypeQual_None"))) { return TypeQual_None; }
        if (StringsMatch(S, CSz("TypeQual_Void"))) { return TypeQual_Void; }
        if (StringsMatch(S, CSz("TypeQual_Auto"))) { return TypeQual_Auto; }
        if (StringsMatch(S, CSz("TypeQual_Class"))) { return TypeQual_Class; }
        if (StringsMatch(S, CSz("TypeQual_Struct"))) { return TypeQual_Struct; }
        if (StringsMatch(S, CSz("TypeQual_Union"))) { return TypeQual_Union; }
        if (StringsMatch(S, CSz("TypeQual_Enum"))) { return TypeQual_Enum; }
        if (StringsMatch(S, CSz("TypeQual_Bool"))) { return TypeQual_Bool; }
        if (StringsMatch(S, CSz("TypeQual_Inline"))) { return TypeQual_Inline; }
        if (StringsMatch(S, CSz("TypeQual_ThreadLocal"))) { return TypeQual_ThreadLocal; }
        if (StringsMatch(S, CSz("TypeQual_Static"))) { return TypeQual_Static; }
        if (StringsMatch(S, CSz("TypeQual_Extern"))) { return TypeQual_Extern; }
        if (StringsMatch(S, CSz("TypeQual_Const"))) { return TypeQual_Const; }
        if (StringsMatch(S, CSz("TypeQual_Volatile"))) { return TypeQual_Volatile; }
        if (StringsMatch(S, CSz("TypeQual_Signed"))) { return TypeQual_Signed; }
        if (StringsMatch(S, CSz("TypeQual_Unsigned"))) { return TypeQual_Unsigned; }
        if (StringsMatch(S, CSz("TypeQual_Char"))) { return TypeQual_Char; }
        if (StringsMatch(S, CSz("TypeQual_Long"))) { return TypeQual_Long; }
        if (StringsMatch(S, CSz("TypeQual_Long_Long"))) { return TypeQual_Long_Long; }
        if (StringsMatch(S, CSz("TypeQual_Int"))) { return TypeQual_Int; }
        if (StringsMatch(S, CSz("TypeQual_Double"))) { return TypeQual_Double; }
        if (StringsMatch(S, CSz("TypeQual_Short"))) { return TypeQual_Short; }
        if (StringsMatch(S, CSz("TypeQual_Float"))) { return TypeQual_Float; }
        if (StringsMatch(S, CSz("TypeQual_Constexpr"))) { return TypeQual_Constexpr; }
        if (StringsMatch(S, CSz("TypeQual_Explicit"))) { return TypeQual_Explicit; }
        if (StringsMatch(S, CSz("TypeQual_Operator"))) { return TypeQual_Operator; }
        if (StringsMatch(S, CSz("TypeQual_Virtual"))) { return TypeQual_Virtual; }
        if (StringsMatch(S, CSz("TypeQual_Noreturn"))) { return TypeQual_Noreturn; }


      return Result;
    }


