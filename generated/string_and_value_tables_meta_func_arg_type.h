link_internal counted_string
ToString(meta_func_arg_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case type_meta_func_arg_noop: { Result = CSz("noop"); } break;
    case type_datatype: { Result = CSz("datatype"); } break;
    case type_poof_index: { Result = CSz("index"); } break;
    case type_poof_symbol: { Result = CSz("symbol"); } break;

    
  }
  return Result;
}

link_internal meta_func_arg_type
MetaFuncArgType(counted_string S)
{
  meta_func_arg_type Result = {};

  if (StringsMatch(S, CSz("type_meta_func_arg_noop"))) { return type_meta_func_arg_noop; }
  if (StringsMatch(S, CSz("type_datatype"))) { return type_datatype; }
  if (StringsMatch(S, CSz("type_poof_index"))) { return type_poof_index; }
  if (StringsMatch(S, CSz("type_poof_symbol"))) { return type_poof_symbol; }

  return Result;
}


