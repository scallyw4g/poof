link_internal void
DebugPrint( datatype *Struct, u32 Depth)
{
  if (Struct)
  {
    DebugPrint(*Struct, Depth);
  }
}

link_internal void
DebugPrint( datatype Struct, u32 Depth)
{
  DebugPrint("datatype {\n", Depth);

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




    default : { DebugPrint("default while printing (datatype) (datatype) ", Depth+4); DebugLine("Type(%d)", Struct.Type); } break;
  }
  DebugPrint("}\n", Depth);
}

