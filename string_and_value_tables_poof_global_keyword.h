// ./poof/poof.h:15:0

link_internal counted_string
ToString(poof_global_keyword Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case poof_global_keyword_noop: { Result = CSz("poof_global_keyword_noop"); } break;
    case poof_error: { Result = CSz("poof_error"); } break;
    case are_equal: { Result = CSz("are_equal"); } break;

    
  }
  return Result;
}

link_internal poof_global_keyword
PoofGlobalKeyword(counted_string S)
{
  poof_global_keyword Result = {};

  if (StringsMatch(S, CSz("poof_global_keyword_noop"))) { return poof_global_keyword_noop; }
  if (StringsMatch(S, CSz("poof_error"))) { return poof_error; }
  if (StringsMatch(S, CSz("are_equal"))) { return are_equal; }

  return Result;
}


