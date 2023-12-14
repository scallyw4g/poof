link_internal counted_string
ToString(meta_func_directive Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case meta_func_directive_noop: { Result = CSz("meta_func_directive_noop"); } break;
    case omit_include: { Result = CSz("omit_include"); } break;
  }
  return Result;
}

link_internal meta_func_directive
MetaFuncDirective(counted_string S)
{
  meta_func_directive Result = {};

  if (StringsMatch(S, CSz("meta_func_directive_noop"))) { return meta_func_directive_noop; }
  if (StringsMatch(S, CSz("omit_include"))) { return omit_include; }

  return Result;
}


