// ./poof/poof.h:512:0

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
    {
      Result = True;
    }
  }
  return Result;
}


