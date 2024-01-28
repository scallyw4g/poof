link_internal counted_string
ToString(function_type Type)
{
  counted_string Result = {};
  switch (Type)
  {
    case function_type_noop: { Result = CSz("noop"); } break;
    case function_type_constructor: { Result = CSz("constructor"); } break;
    case function_type_destructor: { Result = CSz("destructor"); } break;
    case function_type_operator: { Result = CSz("operator"); } break;
    case function_type_normal: { Result = CSz("normal"); } break;

    
  }
  return Result;
}

link_internal function_type
FunctionType(counted_string S)
{
  function_type Result = {};

  if (StringsMatch(S, CSz("function_type_noop"))) { return function_type_noop; }
  if (StringsMatch(S, CSz("function_type_constructor"))) { return function_type_constructor; }
  if (StringsMatch(S, CSz("function_type_destructor"))) { return function_type_destructor; }
  if (StringsMatch(S, CSz("function_type_operator"))) { return function_type_operator; }
  if (StringsMatch(S, CSz("function_type_normal"))) { return function_type_normal; }

  return Result;
}


