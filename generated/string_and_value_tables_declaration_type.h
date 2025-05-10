// ./poof/poof.h:381:0

link_internal counted_string
ToStringPrefixless(declaration_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
        case type_declaration_noop: { Result = CSz("noop"); } break;
    case type_enum_decl: { Result = CSz("decl"); } break;
    case type_function_decl: { Result = CSz("decl"); } break;
    case type_compound_decl: { Result = CSz("decl"); } break;
    case type_variable_decl: { Result = CSz("decl"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(declaration_type Type)
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
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal declaration_type
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


