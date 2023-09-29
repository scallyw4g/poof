
link_internal meta_func_arg
MetaFuncArg(datatype A)
{
  meta_func_arg Result = {
    .Type = type_datatype,
    .datatype = A
  };
  return Result;
}

link_internal meta_func_arg
MetaFuncArg(poof_index A)
{
  meta_func_arg Result = {
    .Type = type_poof_index,
    .poof_index = A
  };
  return Result;
}

link_internal meta_func_arg
MetaFuncArg(poof_symbol A)
{
  meta_func_arg Result = {
    .Type = type_poof_symbol,
    .poof_symbol = A
  };
  return Result;
}



