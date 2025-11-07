// callsite
// ./poof/poof.h:340:0
// (string_and_value_tables) def
// ./include/bonsai_stdlib/src/poof_functions.h:2046:0
link_internal b32
IsValid(function_type Value)
{
  b32 Result = False;
  switch (Value)
  {
        case function_type_noop:
    case function_type_constructor:
    case function_type_destructor:
    case function_type_operator:
    case function_type_normal:

    {
      Result = True;
    }
  }
  return Result;
}



link_internal counted_string
ToStringPrefixless(function_type Type)
{
  Assert(IsValid(Type));
  counted_string Result = {};

  switch (Type)
  {
        case function_type_noop: { Result = CSz("noop"); } break;
    case function_type_constructor: { Result = CSz("constructor"); } break;
    case function_type_destructor: { Result = CSz("destructor"); } break;
    case function_type_operator: { Result = CSz("operator"); } break;
    case function_type_normal: { Result = CSz("normal"); } break;


    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(function_type Type)
{
  Assert(IsValid(Type));

  counted_string Result = {};
  switch (Type)
  {
        case function_type_noop: { Result = CSz("function_type_noop"); } break;
    case function_type_constructor: { Result = CSz("function_type_constructor"); } break;
    case function_type_destructor: { Result = CSz("function_type_destructor"); } break;
    case function_type_operator: { Result = CSz("function_type_operator"); } break;
    case function_type_normal: { Result = CSz("function_type_normal"); } break;


    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
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


