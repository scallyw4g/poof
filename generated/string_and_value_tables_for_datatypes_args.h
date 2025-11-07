// callsite
// ./poof/poof.h:46:0
// (string_and_value_tables) def
// ./include/bonsai_stdlib/src/poof_functions.h:2046:0
link_internal b32
IsValid(for_datatypes_args Value)
{
  b32 Result = False;
  switch (Value)
  {
        case for_datatypes_args_noop:
    case ForDatatypesArg_struct:
    case ForDatatypesArg_union:
    case ForDatatypesArg_enum:
    case ForDatatypesArg_func:
    case ForDatatypesArg_macro:
    case ForDatatypesArg_poof_func:
    case ForDatatypesArg_all:
    case ForDatatypesArg_Count:

    {
      Result = True;
    }
  }
  return Result;
}



link_internal counted_string
ToStringPrefixless(for_datatypes_args Type)
{
  Assert(IsValid(Type));
  counted_string Result = {};

  switch (Type)
  {
        case for_datatypes_args_noop: { Result = CSz("noop"); } break;
    case ForDatatypesArg_struct: { Result = CSz("struct"); } break;
    case ForDatatypesArg_union: { Result = CSz("union"); } break;
    case ForDatatypesArg_enum: { Result = CSz("enum"); } break;
    case ForDatatypesArg_func: { Result = CSz("func"); } break;
    case ForDatatypesArg_macro: { Result = CSz("macro"); } break;
    case ForDatatypesArg_poof_func: { Result = CSz("func"); } break;
    case ForDatatypesArg_all: { Result = CSz("all"); } break;
    case ForDatatypesArg_Count: { Result = CSz("Count"); } break;


        // TODO(Jesse): This is pretty barf and we could do it in a single allocation,
    // but the metaprogram might have to be a bit fancier..
    default:
    {
      u32 CurrentFlags = u32(Type);

      u32 BitsSet = CountBitsSet_Kernighan(CurrentFlags);
      switch(BitsSet)
      {
        case 0: // We likely passed 0 into this function, and the enum didn't have a 0 value
        case 1: // The value we passed in was outside the range of the valid enum values
        {
          Result = FSz("(invalid value (%d))", CurrentFlags);
        } break;

        default:
        {
          u32 FirstValue = UnsetLeastSignificantSetBit(&CurrentFlags);
          Result = ToStringPrefixless(for_datatypes_args(FirstValue));

          while (CurrentFlags)
          {
            u32 Value = UnsetLeastSignificantSetBit(&CurrentFlags);
            cs Next = ToStringPrefixless(for_datatypes_args(Value));
            Result = FSz("%S | %S", Result, Next);
          }
        } break;
      }
    } break;

  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(for_datatypes_args Type)
{
  Assert(IsValid(Type));

  counted_string Result = {};
  switch (Type)
  {
        case for_datatypes_args_noop: { Result = CSz("for_datatypes_args_noop"); } break;
    case ForDatatypesArg_struct: { Result = CSz("ForDatatypesArg_struct"); } break;
    case ForDatatypesArg_union: { Result = CSz("ForDatatypesArg_union"); } break;
    case ForDatatypesArg_enum: { Result = CSz("ForDatatypesArg_enum"); } break;
    case ForDatatypesArg_func: { Result = CSz("ForDatatypesArg_func"); } break;
    case ForDatatypesArg_macro: { Result = CSz("ForDatatypesArg_macro"); } break;
    case ForDatatypesArg_poof_func: { Result = CSz("ForDatatypesArg_poof_func"); } break;
    case ForDatatypesArg_all: { Result = CSz("ForDatatypesArg_all"); } break;
    case ForDatatypesArg_Count: { Result = CSz("ForDatatypesArg_Count"); } break;


        // TODO(Jesse): This is pretty barf and we could do it in a single allocation,
    // but the metaprogram might have to be a bit fancier..
    default:
    {
      u32 CurrentFlags = u32(Type);

      u32 FirstValue = UnsetLeastSignificantSetBit(&CurrentFlags);
      Result = ToString(for_datatypes_args(FirstValue));

      while (CurrentFlags)
      {
        u32 Value = UnsetLeastSignificantSetBit(&CurrentFlags);
        cs Next = ToString(for_datatypes_args(Value));
        Result = FSz("%S | %S", Result, Next);
      }
    } break;

  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal for_datatypes_args
ForDatatypesArgs(counted_string S)
{
  for_datatypes_args Result = {};

    if (StringsMatch(S, CSz("for_datatypes_args_noop"))) { return for_datatypes_args_noop; }
  if (StringsMatch(S, CSz("ForDatatypesArg_struct"))) { return ForDatatypesArg_struct; }
  if (StringsMatch(S, CSz("ForDatatypesArg_union"))) { return ForDatatypesArg_union; }
  if (StringsMatch(S, CSz("ForDatatypesArg_enum"))) { return ForDatatypesArg_enum; }
  if (StringsMatch(S, CSz("ForDatatypesArg_func"))) { return ForDatatypesArg_func; }
  if (StringsMatch(S, CSz("ForDatatypesArg_macro"))) { return ForDatatypesArg_macro; }
  if (StringsMatch(S, CSz("ForDatatypesArg_poof_func"))) { return ForDatatypesArg_poof_func; }
  if (StringsMatch(S, CSz("ForDatatypesArg_all"))) { return ForDatatypesArg_all; }
  if (StringsMatch(S, CSz("ForDatatypesArg_Count"))) { return ForDatatypesArg_Count; }


  return Result;
}


