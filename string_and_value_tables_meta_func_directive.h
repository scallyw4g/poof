// ./poof/poof.h:74:0


link_internal counted_string
ToStringPrefixless(meta_func_directive Type)
{
  counted_string Result = {};
  switch (Type)
  {
        case meta_func_directive_noop: { Result = CSz("noop"); } break;
    case omit_include: { Result = CSz("include"); } break;
    case code_fragment: { Result = CSz("fragment"); } break;
    case origin_comment_format: { Result = CSz("format"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(meta_func_directive Type)
{
  counted_string Result = {};
  switch (Type)
  {
        case meta_func_directive_noop: { Result = CSz("meta_func_directive_noop"); } break;
    case omit_include: { Result = CSz("omit_include"); } break;
    case code_fragment: { Result = CSz("code_fragment"); } break;
    case origin_comment_format: { Result = CSz("origin_comment_format"); } break;

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (EnumType.name)", Type); } */
  return Result;
}

link_internal meta_func_directive
MetaFuncDirective(counted_string S)
{
  meta_func_directive Result = {};

    if (StringsMatch(S, CSz("meta_func_directive_noop"))) { return meta_func_directive_noop; }
  if (StringsMatch(S, CSz("omit_include"))) { return omit_include; }
  if (StringsMatch(S, CSz("code_fragment"))) { return code_fragment; }
  if (StringsMatch(S, CSz("origin_comment_format"))) { return origin_comment_format; }

  return Result;
}


