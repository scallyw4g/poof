link_internal counted_string
ToString(datatype_type Type)
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
  return Result;
}

