// main.c:108:0

void DebugPrint_baz_struct(struct baz_struct *Struct)
{
  DebugPrint_str("{ ");

      DebugPrint_str("int  BazInt = ");
  DebugPrint_int ( Struct->BazInt );
  DebugPrint_str(";");


  DebugPrint_str(" }");
}
void DebugPrint_bar_struct(struct bar_struct *Struct)
{
  DebugPrint_str("{ ");

      DebugPrint_str("int  BarInt = ");
  DebugPrint_int ( Struct->BarInt );
  DebugPrint_str(";");


  DebugPrint_str(" }");
}
void DebugPrint_nested_struct(struct nested_struct *Struct)
{
  DebugPrint_str("{ ");

      DebugPrint_str("int  NestedInt = ");
  DebugPrint_int ( Struct->NestedInt );
  DebugPrint_str(";");


  DebugPrint_str(" }");
}
void DebugPrint_foo_struct(struct foo_struct *Struct)
{
  DebugPrint_str("{ ");

      DebugPrint_str("nested_struct FooNested = ");
  DebugPrint_nested_struct( Struct->FooNested );
  DebugPrint_str(";");


  DebugPrint_str(" }");
}
void DebugPrint_my_discriminated_union(struct my_discriminated_union *Struct)
{
  DebugPrint_str("{ ");

      DebugPrint_str("my_discriminated_union_type Type = ");
  DebugPrint_my_discriminated_union_type( Struct->Type );
  DebugPrint_str(";");

    // Print d_union members.  See comment at @manually_generate_d_union_switch
  switch (Struct->Type)
  {
        case type_foo_struct:
    {
      DebugPrint_str("\n    foo_struct = ");
      DebugPrint_foo_struct(&Struct->foo_struct );
    } break;
    case type_bar_struct:
    {
      DebugPrint_str("\n    bar_struct = ");
      DebugPrint_bar_struct(&Struct->bar_struct );
    } break;
    case type_baz_struct:
    {
      DebugPrint_str("\n    baz_struct = ");
      DebugPrint_baz_struct(&Struct->baz_struct );
    } break;

    default: { /* Invalid type passed to DebugPrint .. handling the error might be nice */ } break;
  }
  DebugPrint_str(" }");
}

// Print an enum value as a string.
//
// In 'real' code this might be a thunk to ToString(enum) that's already generated.
//
void DebugPrint_my_discriminated_union_type( enum my_discriminated_union_type EnumValue)
{
  switch (EnumValue)
  {
        case type_my_discriminated_union_noop:
    {
      DebugPrint_str("type_my_discriminated_union_noop");
    } break;
    case type_foo_struct:
    {
      DebugPrint_str("type_foo_struct");
    } break;
    case type_bar_struct:
    {
      DebugPrint_str("type_bar_struct");
    } break;
    case type_baz_struct:
    {
      DebugPrint_str("type_baz_struct");
    } break;
    case type_flazz:
    {
      DebugPrint_str("type_flazz");
    } break;
  }
}


