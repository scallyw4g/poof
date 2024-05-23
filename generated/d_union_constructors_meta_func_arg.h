// ./poof/poof.h:636:0


link_internal meta_func_arg
MetaFuncArg( datatype A, counted_string Match   )
{
  meta_func_arg Result = {
    .Type = type_datatype,
    .datatype = A,

 .Match = Match   ,

  };
  return Result;
}

link_internal meta_func_arg
MetaFuncArg( poof_index A, counted_string Match   )
{
  meta_func_arg Result = {
    .Type = type_poof_index,
    .poof_index = A,

 .Match = Match   ,

  };
  return Result;
}

link_internal meta_func_arg
MetaFuncArg( poof_symbol A, counted_string Match   )
{
  meta_func_arg Result = {
    .Type = type_poof_symbol,
    .poof_symbol = A,

 .Match = Match   ,

  };
  return Result;
}

