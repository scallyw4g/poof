// ./poof/poof.h:107:0

link_internal meta_transform_op
MetaTransformOp(counted_string S)
{
  meta_transform_op Result = {};

  if (StringsMatch(S, CSz("meta_transform_op_noop"))) { return meta_transform_op_noop; }
  if (StringsMatch(S, CSz("to_capital_case"))) { return to_capital_case; }
  if (StringsMatch(S, CSz("to_snake_case"))) { return to_snake_case; }
  if (StringsMatch(S, CSz("to_lowercase"))) { return to_lowercase; }
  if (StringsMatch(S, CSz("to_uppercase"))) { return to_uppercase; }
  if (StringsMatch(S, CSz("strip_single_prefix"))) { return strip_single_prefix; }
  if (StringsMatch(S, CSz("strip_all_prefix"))) { return strip_all_prefix; }

  return Result;
}

