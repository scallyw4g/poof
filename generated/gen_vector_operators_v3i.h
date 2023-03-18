inline b32
operator==( v3i P1, v3i P2 )
{
  b32 Result = (P1.E[0] == P2.E[0] && P1.E[1] == P2.E[1] && P1.E[2] == P2.E[2]);
  return Result;
}

inline b32
operator!=( v3i P1, v3i P2 )
{
  b32 Result = !(P1 == P2);
  return Result;
}

inline b32
operator<( v3i P1, v3i P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1] && P1.E[2] < P2.E[2]);
  return Result;
}

inline b32
operator<=( v3i P1, v3i P2 )
{
  b32 Result = (P1.E[0] <= P2.E[0] && P1.E[1] <= P2.E[1] && P1.E[2] <= P2.E[2]);
  return Result;
}

inline b32
operator>( v3i P1, v3i P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1] && P1.E[2] < P2.E[2]);
  return Result;
}

inline b32
operator>=( v3i P1, v3i P2 )
{
  b32 Result = (P1.E[0] >= P2.E[0] && P1.E[1] >= P2.E[1] && P1.E[2] >= P2.E[2]);
  return Result;
}

/* gen_vector_operator_set(Type, +) */
/* gen_vector_operator_set(Type, -) */
/* gen_vector_operator_set(Type, *) */
/* gen_vector_operator_set(Type, /) */

v3i
operator+( v3i P1, v3i P2 )
{
  v3i Result = {
    .E[0] = P1.E[0] + P2.E[0],
    .E[1] = P1.E[1] + P2.E[1],
    .E[2] = P1.E[2] + P2.E[2],

  };
  return Result;
}

v3i
operator+( v3i P1, s32 Scalar )
{
  v3i Result = {
    .E[0] = P1.E[0] + Scalar,
    .E[1] = P1.E[1] + Scalar,
    .E[2] = P1.E[2] + Scalar,

  };
  return Result;
}

v3i
operator+( s32 Scalar, v3i P1 )
{
  v3i Result = {
    .E[0] = P1.E[0] + Scalar,
    .E[1] = P1.E[1] + Scalar,
    .E[2] = P1.E[2] + Scalar,

  };
  return Result;
}





v3i
operator-( v3i P1, v3i P2 )
{
  v3i Result = {
    .E[0] = P1.E[0] - P2.E[0],
    .E[1] = P1.E[1] - P2.E[1],
    .E[2] = P1.E[2] - P2.E[2],

  };
  return Result;
}

v3i
operator-( v3i P1, s32 Scalar )
{
  v3i Result = {
    .E[0] = P1.E[0] - Scalar,
    .E[1] = P1.E[1] - Scalar,
    .E[2] = P1.E[2] - Scalar,

  };
  return Result;
}

v3i
operator-( s32 Scalar, v3i P1 )
{
  v3i Result = {
    .E[0] = P1.E[0] - Scalar,
    .E[1] = P1.E[1] - Scalar,
    .E[2] = P1.E[2] - Scalar,

  };
  return Result;
}





v3i
operator*( v3i P1, v3i P2 )
{
  v3i Result = {
    .E[0] = P1.E[0] * P2.E[0],
    .E[1] = P1.E[1] * P2.E[1],
    .E[2] = P1.E[2] * P2.E[2],

  };
  return Result;
}

v3i
operator*( v3i P1, s32 Scalar )
{
  v3i Result = {
    .E[0] = P1.E[0] * Scalar,
    .E[1] = P1.E[1] * Scalar,
    .E[2] = P1.E[2] * Scalar,

  };
  return Result;
}

v3i
operator*( s32 Scalar, v3i P1 )
{
  v3i Result = {
    .E[0] = P1.E[0] * Scalar,
    .E[1] = P1.E[1] * Scalar,
    .E[2] = P1.E[2] * Scalar,

  };
  return Result;
}




v3i
operator/( v3i P1, v3i P2 )
{
  v3i Result = {
    .E[0] = P1.E[0] / P2.E[0],
    .E[1] = P1.E[1] / P2.E[1],
    .E[2] = P1.E[2] / P2.E[2],

  };
  return Result;
}

void
operator+=( v3i &P1, v3i P2 )
{
  P1.E[0] += P2.E[0];
  P1.E[1] += P2.E[1];
  P1.E[2] += P2.E[2];

}

void
operator-=( v3i &P1, v3i P2 )
{
  P1.E[0] -= P2.E[0];
  P1.E[1] -= P2.E[1];
  P1.E[2] -= P2.E[2];

}




