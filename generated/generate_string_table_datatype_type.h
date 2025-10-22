// ./poof/poof.h:461:0


link_internal counted_string
ToStringPrefixless(datatype_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
        case type_datatype_noop: { Result = CSz("noop"); } break;
    case type_declaration: { Result = CSz("declaration"); } break;
    case type_enum_member: { Result = CSz("member"); } break;
    case type_type_def: { Result = CSz("def"); } break;
    case type_primitive_def: { Result = CSz("def"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(datatype_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
        case type_datatype_noop: { Result = CSz("type_datatype_noop"); } break;
    case type_declaration: { Result = CSz("type_declaration"); } break;
    case type_enum_member: { Result = CSz("type_enum_member"); } break;
    case type_type_def: { Result = CSz("type_type_def"); } break;
    case type_primitive_def: { Result = CSz("type_primitive_def"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

