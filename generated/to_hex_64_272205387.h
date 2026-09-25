// callsite
// ./include/bonsai_stdlib/src/counted_string.cpp:873:0

// def (to_hex_64)
// ./include/bonsai_stdlib/src/counted_string.cpp:844:0
link_internal cs
ToHex( s64 Value )
{
  cs Result = FSz("%lx", Value);
  return Result;
}
link_internal cs
ToHex( u64 Value )
{
  cs Result = FSz("%lx", Value);
  return Result;
}
link_internal cs
ToHex( r64 Value )
{
  cs Result = FSz("%lx", Value);
  return Result;
}


