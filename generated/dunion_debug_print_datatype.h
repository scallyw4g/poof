// ./poof/poof.h:512:0


/* dunion_debug_print_prototype(DUnion) */

link_internal void DebugPrint( declaration *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("declaration ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                        DebugPrint("declaration_type Type =", Depth+2);
    DebugPrint(&RuntimeStruct->Type, 1);
    DebugPrint(";\n");




                DebugPrint("poof_tag_block_array Tags {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Tags, Depth+4);
    DebugPrint("}\n", Depth+2);



            DebugPrint("union (anonymous)\n", Depth+2);
    /* if (Depth == 0) */
    {
      DebugPrint("}\n", Depth);
    }
  }
  else
  {
    DebugPrint(" = (null)\n", Depth);
  }

}

link_internal void DebugPrint( declaration RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}

link_internal void DebugPrint( primitive_def *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("primitive_def ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                    DebugPrint("type_spec TypeSpec {\n", Depth+2);
    DebugPrint(&RuntimeStruct->TypeSpec, Depth+4);
    DebugPrint("}\n", Depth+2);




    /* if (Depth == 0) */
    {
      DebugPrint("}\n", Depth);
    }
  }
  else
  {
    DebugPrint(" = (null)\n", Depth);
  }

}

link_internal void DebugPrint( primitive_def RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}

link_internal void DebugPrint( enum_member *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("enum_member ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                    DebugPrint("c_token NameT {\n", Depth+2);
    DebugPrint(&RuntimeStruct->NameT, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("ast_node_expression Expr {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Expr, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("poof_tag_block_array Tags {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Tags, Depth+4);
    DebugPrint("}\n", Depth+2);
    /* if (Depth == 0) */
    {
      DebugPrint("}\n", Depth);
    }
  }
  else
  {
    DebugPrint(" = (null)\n", Depth);
  }

}

link_internal void DebugPrint( enum_member RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}

link_internal void DebugPrint( type_def *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("type_def ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                        DebugPrint("counted_string Alias =", Depth+2);
    DebugPrint(&RuntimeStruct->Alias, 1);
    DebugPrint(";\n");




                DebugPrint("type_spec Type {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Type, Depth+4);
    DebugPrint("}\n", Depth+2);
    /* if (Depth == 0) */
    {
      DebugPrint("}\n", Depth);
    }
  }
  else
  {
    DebugPrint(" = (null)\n", Depth);
  }

}

link_internal void DebugPrint( type_def RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}



link_internal void
DebugPrint( datatype *Struct, u32 Depth)
{
  DebugPrint("datatype {\n", Depth);

  if (Struct)
  {
    switch(Struct->Type)
    {
                        case type_declaration:
      {
        DebugPrint(&Struct->declaration, Depth+4);
      } break;
      case type_primitive_def:
      {
        DebugPrint(&Struct->primitive_def, Depth+4);
      } break;
      case type_enum_member:
      {
        DebugPrint(&Struct->enum_member, Depth+4);
      } break;
      case type_type_def:
      {
        DebugPrint(&Struct->type_def, Depth+4);
      } break;



      default : { DebugPrint("default while printing (struct) (datatype) ", Depth+4); DebugLine("Type(%d)", Struct->Type); } break;
    }
  }
  else
  {
    DebugPrint("(null)", Depth);
  }
  DebugPrint("}\n", Depth);
}

link_internal void
DebugPrint( datatype Struct, u32 Depth)
{
  DebugPrint(&Struct, Depth);
}

