link_internal meta_arg_operator
MetaArgOperator(counted_string S)
{
  meta_arg_operator Result = {};

  if (StringsMatch(S, CSz("meta_arg_operator_noop"))) { return meta_arg_operator_noop; }
  if (StringsMatch(S, CSz("name"))) { return name; }
  if (StringsMatch(S, CSz("type"))) { return type; }
  if (StringsMatch(S, CSz("value"))) { return value; }
  if (StringsMatch(S, CSz("array"))) { return array; }
  if (StringsMatch(S, CSz("hash"))) { return hash; }
  if (StringsMatch(S, CSz("map_array"))) { return map_array; }
  if (StringsMatch(S, CSz("map_values"))) { return map_values; }
  if (StringsMatch(S, CSz("map_members"))) { return map_members; }
  if (StringsMatch(S, CSz("map"))) { return map; }
  if (StringsMatch(S, CSz("sep"))) { return sep; }
  if (StringsMatch(S, CSz("member"))) { return member; }
  if (StringsMatch(S, CSz("is_enum"))) { return is_enum; }
  if (StringsMatch(S, CSz("is_struct"))) { return is_struct; }
  if (StringsMatch(S, CSz("is_union"))) { return is_union; }
  if (StringsMatch(S, CSz("is_pointer"))) { return is_pointer; }
  if (StringsMatch(S, CSz("is_defined"))) { return is_defined; }
  if (StringsMatch(S, CSz("is_compound"))) { return is_compound; }
  if (StringsMatch(S, CSz("is_primitive"))) { return is_primitive; }
  if (StringsMatch(S, CSz("is_function"))) { return is_function; }
  if (StringsMatch(S, CSz("is_array"))) { return is_array; }
  if (StringsMatch(S, CSz("is_type"))) { return is_type; }
  if (StringsMatch(S, CSz("is_named"))) { return is_named; }
  if (StringsMatch(S, CSz("contains_type"))) { return contains_type; }
  if (StringsMatch(S, CSz("has_tag"))) { return has_tag; }
  if (StringsMatch(S, CSz("tag_value"))) { return tag_value; }

  return Result;
}

