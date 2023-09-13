
link_internal meta_func_arg
MetaFuncArg(datatype A)
{
  meta_func_arg Result = {
    .Type = type_datatype,
    .datatype = A
  };
  return Result;
}

// TODO(Jesse)(poof): This is bugged!  It can't find the members for the ConstructorArgT.
// Pasting the implementation here does not help.
/* gen_constructor(ConstructorArgT) */
link_internal meta_func_arg
MetaFuncArg(poof_index A)
{
  meta_func_arg Result = {
    .Type = type_poof_index,
    .poof_index = A
  };
  return Result;
}

// TODO(Jesse)(poof): This is bugged!  It can't find the members for the ConstructorArgT.
// Pasting the implementation here does not help.
/* gen_constructor(ConstructorArgT) */
link_internal meta_func_arg
MetaFuncArg(poof_symbol A)
{
  meta_func_arg Result = {
    .Type = type_poof_symbol,
    .poof_symbol = A
  };
  return Result;
}

// TODO(Jesse)(poof): This is bugged!  It can't find the members for the ConstructorArgT.
// Pasting the implementation here does not help.
/* gen_constructor(ConstructorArgT) */


