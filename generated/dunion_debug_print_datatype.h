// callsite
// ./poof/poof.h:717:0
// (dunion_debug_print) def
// ./include/bonsai_stdlib/src/poof_functions.h:1126:0
/* dunion_debug_print_prototype(tagged_union_t) */


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

link_internal void DebugPrint( macro_def *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("macro_def ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                        DebugPrint("macro_type Type =", Depth+2);
    DebugPrint(&RuntimeStruct->Type, 1);
    DebugPrint(";\n");




                DebugPrint("c_token NameT {\n", Depth+2);
    DebugPrint(&RuntimeStruct->NameT, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("c_token_cursor Body {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Body, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("counted_string_buffer NamedArguments {\n", Depth+2);
    DebugPrint(&RuntimeStruct->NamedArguments, Depth+4);
    DebugPrint("}\n", Depth+2);



                    DebugPrint("b32 Variadic =", Depth+2);
    DebugPrint(&RuntimeStruct->Variadic, 1);
    DebugPrint(";\n");




                    DebugPrint("b32 Undefed =", Depth+2);
    DebugPrint(&RuntimeStruct->Undefed, 1);
    DebugPrint(";\n");




                    DebugPrint("b32 IsExpanding =", Depth+2);
    DebugPrint(&RuntimeStruct->IsExpanding, 1);
    DebugPrint(";\n");





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

link_internal void DebugPrint( macro_def RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}

link_internal void DebugPrint( meta_func *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("meta_func ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                        DebugPrint("cs Name =", Depth+2);
    DebugPrint(&RuntimeStruct->Name, 1);
    DebugPrint(";\n");




                DebugPrint("c_token SourceToken {\n", Depth+2);
    DebugPrint(&RuntimeStruct->SourceToken, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("meta_func_arg_buffer Args {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Args, Depth+4);
    DebugPrint("}\n", Depth+2);



                DebugPrint("parser Body {\n", Depth+2);
    DebugPrint(&RuntimeStruct->Body, Depth+4);
    DebugPrint("}\n", Depth+2);



                    DebugPrint("meta_func_directive Directives =", Depth+2);
    DebugPrint(&RuntimeStruct->Directives, 1);
    DebugPrint(";\n");




                    DebugPrint("cs HeaderFormatString =", Depth+2);
    DebugPrint(&RuntimeStruct->HeaderFormatString, 1);
    DebugPrint(";\n");





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

link_internal void DebugPrint( meta_func RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}

link_internal void DebugPrint( poof_tag *RuntimeStruct, u32 Depth = 0)
{
  /* if (Depth == 0) */
  {
    DebugPrint("poof_tag ", Depth);
  }

  if (RuntimeStruct)
  {
    DebugPrint("{\n", Depth);
                        DebugPrint("cs Name =", Depth+2);
    DebugPrint(&RuntimeStruct->Name, 1);
    DebugPrint(";\n");




                    DebugPrint("cs Value =", Depth+2);
    DebugPrint(&RuntimeStruct->Value, 1);
    DebugPrint(";\n");





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

link_internal void DebugPrint( poof_tag RuntimePtr, u32 Depth = 0)
{
  DebugPrint(&RuntimePtr, Depth);
}





link_internal void
DebugPrint( datatype *Struct, u32 Depth)
{
  DebugPrint("datatype {\n", Depth);

  if (Struct)
  {
    unbox(Struct)
    {
            
                  {
        unboxed_value( declaration, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( primitive_def, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( enum_member, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( type_def, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( macro_def, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( meta_func, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
      } break;
      {
        unboxed_value( poof_tag, Struct, Unboxed  )
        DebugPrint(Unboxed, Depth+4);
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

