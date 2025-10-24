// ./include/bonsai_stdlib/src/poof_functions.h:1853:0
link_internal for_datatypes_args
ForDatatypesArgsPrefixless(counted_string S)
{
  for_datatypes_args Result = {};

    if (StringsMatch(S, CSz("datatypes_args_noop"))) { return for_datatypes_args_noop; }
  if (StringsMatch(S, CSz("struct"))) { return ForDatatypesArg_struct; }
  if (StringsMatch(S, CSz("union"))) { return ForDatatypesArg_union; }
  if (StringsMatch(S, CSz("enum"))) { return ForDatatypesArg_enum; }
  if (StringsMatch(S, CSz("func"))) { return ForDatatypesArg_func; }
  if (StringsMatch(S, CSz("macro"))) { return ForDatatypesArg_macro; }
  if (StringsMatch(S, CSz("poof_func"))) { return ForDatatypesArg_poof_func; }
  if (StringsMatch(S, CSz("all"))) { return ForDatatypesArg_all; }
  if (StringsMatch(S, CSz("Count"))) { return ForDatatypesArg_Count; }


  return Result;
}

