// callsite
// ./include/bonsai_stdlib/src/work_queue_magic.h:145:0

// def (string_and_value_tables)
// ./include/bonsai_stdlib/src/poof_functions.h:2181:0
link_internal b32
IsValid(async_function_call_type Value)
{
  b32 Result = False;
  switch (Value)
  {
    
    {
      Result = True;
    }
  }
  return Result;
}



link_internal counted_string
ToStringPrefixless(async_function_call_type Type)
{
  cs Result = {};
  if (IsValid(Type))
  {
    switch (Type)
    {
      

      
    }
  }
  else
  {
    Result = CSz("(CORRUPT ENUM VALUE)");
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal counted_string
ToString(async_function_call_type Type)
{
  Assert(IsValid(Type));

  counted_string Result = {};
  switch (Type)
  {
    

    
  }
  /* if (Result.Start == 0) { Info("Could not convert value(%d) to (enum_t.name)", Type); } */
  return Result;
}

link_internal async_function_call_type
AsyncFunctionCallType(counted_string S)
{
  async_function_call_type Result = {};

  

  return Result;
}


