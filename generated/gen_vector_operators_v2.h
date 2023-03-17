inline b32
operator==( v2 P1, v2 P2 )
{
  b32 Result = (P1.E[0] == P2.E[0] && P1.E[1] == P2.E[1]);
  return Result;
}

inline b32
operator!=( v2 P1, v2 P2 )
{
  b32 Result = !(P1 == P2);
  return Result;
}

inline b32
operator<( v2 P1, v2 P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1]);
  return Result;
}

inline b32
operator<=( v2 P1, v2 P2 )
{
  b32 Result = (P1.E[0] <= P2.E[0] && P1.E[1] <= P2.E[1]);
  return Result;
}

inline b32
operator>( v2 P1, v2 P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1]);
  return Result;
}

inline b32
operator>=( v2 P1, v2 P2 )
{
  b32 Result = (P1.E[0] >= P2.E[0] && P1.E[1] >= P2.E[1]);
  return Result;
}

v2
operator+( v2 P1, v2 P2 )
{
  v2 Result = {
    .E[0] = P1.E[0] + P2.E[0],
    .E[1] = P1.E[1] + P2.E[1],

  };
  return Result;
}

v2
operator-( v2 P1, v2 P2 )
{
  v2 Result = {
    .E[0] = P1.E[0] - P2.E[0],
    .E[1] = P1.E[1] - P2.E[1],

  };
  return Result;
}

void
operator+=( v2 &P1, v2 P2 )
{
  P1.E[0] += P2.E[0];
  P1.E[1] += P2.E[1];

}

void
operator-=( v2 &P1, v2 P2 )
{
  P1.E[0] -= P2.E[0];
  P1.E[1] -= P2.E[1];

}




