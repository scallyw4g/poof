// ./include/bonsai_stdlib/src/poof_functions.h:1741:0
link_internal b32
IsValid(datatype_type Value)
{
  b32 Result = False;
  switch (Value)
  {
        case type_datatype_noop:
    case type_declaration:
    case type_enum_member:
    case type_type_def:
    case type_primitive_def:
    case type_macro_def:
    case type_meta_func:
    case type_poof_tag:

    {
      Result = True;
    }
  }
  return Result;
}



link_internal counted_string
ToStringPrefixless(datatype_type Type)
{
  Assert(IsValid(Type));
  counted_string Result = {};

  switch (Type)
  {
        case type_datatype_noop: { Result = CSz("noop"); } break;
    case type_declaration: { Result = CSz("declaration"); } break;
    case type_enum_member: { Result = CSz("member"); } break;
    case type_type_def: { Result = CSz("def"); } break;
    case type_primitive_def: { Result = CSz("def"); } break;
    case type_macro_def: { Result = CSz("def"); } break;
    case type_meta_func: { Result = CSz("func"); } break;
    case type_poof_tag: { Result = CSz("tag"); } break;


    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(datatype_type Type)
{
  Assert(IsValid(Type));

  counted_string Result = {};
  switch (Type)
  {
        case type_datatype_noop: { Result = CSz("type_datatype_noop"); } break;
    case type_declaration: { Result = CSz("type_declaration"); } break;
    case type_enum_member: { Result = CSz("type_enum_member"); } break;
    case type_type_def: { Result = CSz("type_type_def"); } break;
    case type_primitive_def: { Result = CSz("type_primitive_def"); } break;
    case type_macro_def: { Result = CSz("type_macro_def"); } break;
    case type_meta_func: { Result = CSz("type_meta_func"); } break;
    case type_poof_tag: { Result = CSz("type_poof_tag"); } break;


    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

