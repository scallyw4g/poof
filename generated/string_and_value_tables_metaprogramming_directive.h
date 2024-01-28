link_internal counted_string
ToString(metaprogramming_directive Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case meta_directive_noop: { Result = CSz("noop"); } break;
    case metaprogramming_directive__enum_only: { Result = CSz("only"); } break;
    case metaprogramming_directive__d_union: { Result = CSz("union"); } break;
    case metaprogramming_directive__named_list: { Result = CSz("list"); } break;
    case metaprogramming_directive__for_datatypes: { Result = CSz("datatypes"); } break;
    case metaprogramming_directive__func: { Result = CSz("func"); } break;
    case metaprogramming_directive__polymorphic_func: { Result = CSz("func"); } break;

    
  }
  return Result;
}

link_internal metaprogramming_directive
MetaprogrammingDirective(counted_string S)
{
  metaprogramming_directive Result = {};

  if (StringsMatch(S, CSz("meta_directive_noop"))) { return meta_directive_noop; }
  if (StringsMatch(S, CSz("enum_only"))) {           return metaprogramming_directive__enum_only; }
  if (StringsMatch(S, CSz("d_union"))) {             return metaprogramming_directive__d_union; }
  if (StringsMatch(S, CSz("named_list"))) {          return metaprogramming_directive__named_list; }
  if (StringsMatch(S, CSz("for_datatypes"))) {       return metaprogramming_directive__for_datatypes; }
  if (StringsMatch(S, CSz("func"))) {                return metaprogramming_directive__func; }
  if (StringsMatch(S, CSz("polymorphic_func"))) {    return metaprogramming_directive__polymorphic_func; }

  return Result;
}


