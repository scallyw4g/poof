inline b32
operator==( v2 P1, v2 P2 )
{
  u32 Index = 0;
  b32 Result = (
  P1.E[Index] == P2.E[Index++] &&
  P1.E[Index] == P2.E[Index++] &&



  1 );
  return Result;
}

