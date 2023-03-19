inline b32
operator==( v4 P1, v4 P2 )
{
  b32 Result = (P1.E[0] == P2.E[0] && P1.E[1] == P2.E[1] && P1.E[2] == P2.E[2] && P1.E[3] == P2.E[3]);
  return Result;
}

inline b32
operator!=( v4 P1, v4 P2 )
{
  b32 Result = !(P1 == P2);
  return Result;
}

inline b32
operator<( v4 P1, v4 P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1] && P1.E[2] < P2.E[2] && P1.E[3] < P2.E[3]);
  return Result;
}

inline b32
operator<=( v4 P1, v4 P2 )
{
  b32 Result = (P1.E[0] <= P2.E[0] && P1.E[1] <= P2.E[1] && P1.E[2] <= P2.E[2] && P1.E[3] <= P2.E[3]);
  return Result;
}

inline b32
operator>( v4 P1, v4 P2 )
{
  b32 Result = (P1.E[0] < P2.E[0] && P1.E[1] < P2.E[1] && P1.E[2] < P2.E[2] && P1.E[3] < P2.E[3]);
  return Result;
}

inline b32
operator>=( v4 P1, v4 P2 )
{
  b32 Result = (P1.E[0] >= P2.E[0] && P1.E[1] >= P2.E[1] && P1.E[2] >= P2.E[2] && P1.E[3] >= P2.E[3]);
  return Result;
}

/* (gen_vector_operator(Type, {+})) */
/* gen_vector_operator_set(Type, -) */
/* gen_vector_operator_set(Type, *) */
/* gen_vector_operator_set(Type, /) */

v4
operator+( v4 P1, v4 P2 )
{
  v4 Result = {
    .E[0] = P1.E[0] + P2.E[0],
    .E[1] = P1.E[1] + P2.E[1],
    .E[2] = P1.E[2] + P2.E[2],
    .E[3] = P1.E[3] + P2.E[3],

  };
  return Result;
}

v4
operator+( v4 P1, r32 Scalar )
{
  v4 Result = {
    .E[0] = P1.E[0] + Scalar,
    .E[1] = P1.E[1] + Scalar,
    .E[2] = P1.E[2] + Scalar,
    .E[3] = P1.E[3] + Scalar,

  };
  return Result;
}

v4
operator+( r32 Scalar, v4 P1 )
{
  v4 Result = {
    .E[0] = P1.E[0] + Scalar,
    .E[1] = P1.E[1] + Scalar,
    .E[2] = P1.E[2] + Scalar,
    .E[3] = P1.E[3] + Scalar,

  };
  return Result;
}





v4
operator-( v4 P1, v4 P2 )
{
  v4 Result = {
    .E[0] = P1.E[0] - P2.E[0],
    .E[1] = P1.E[1] - P2.E[1],
    .E[2] = P1.E[2] - P2.E[2],
    .E[3] = P1.E[3] - P2.E[3],

  };
  return Result;
}

v4
operator-( v4 P1, r32 Scalar )
{
  v4 Result = {
    .E[0] = P1.E[0] - Scalar,
    .E[1] = P1.E[1] - Scalar,
    .E[2] = P1.E[2] - Scalar,
    .E[3] = P1.E[3] - Scalar,

  };
  return Result;
}

v4
operator-( r32 Scalar, v4 P1 )
{
  v4 Result = {
    .E[0] = P1.E[0] - Scalar,
    .E[1] = P1.E[1] - Scalar,
    .E[2] = P1.E[2] - Scalar,
    .E[3] = P1.E[3] - Scalar,

  };
  return Result;
}





v4
operator*( v4 P1, v4 P2 )
{
  v4 Result = {
    .E[0] = P1.E[0] * P2.E[0],
    .E[1] = P1.E[1] * P2.E[1],
    .E[2] = P1.E[2] * P2.E[2],
    .E[3] = P1.E[3] * P2.E[3],

  };
  return Result;
}

v4
operator*( v4 P1, r32 Scalar )
{
  v4 Result = {
    .E[0] = P1.E[0] * Scalar,
    .E[1] = P1.E[1] * Scalar,
    .E[2] = P1.E[2] * Scalar,
    .E[3] = P1.E[3] * Scalar,

  };
  return Result;
}

v4
operator*( r32 Scalar, v4 P1 )
{
  v4 Result = {
    .E[0] = P1.E[0] * Scalar,
    .E[1] = P1.E[1] * Scalar,
    .E[2] = P1.E[2] * Scalar,
    .E[3] = P1.E[3] * Scalar,

  };
  return Result;
}




v4
operator/( v4 P1, v4 P2 )
{
  v4 Result = {
    .E[0] = P1.E[0] / P2.E[0],
    .E[1] = P1.E[1] / P2.E[1],
    .E[2] = P1.E[2] / P2.E[2],
    .E[3] = P1.E[3] / P2.E[3],

  };
  return Result;
}

void
operator+=( v4 &P1, v4 P2 )
{
  P1.E[0] += P2.E[0];
  P1.E[1] += P2.E[1];
  P1.E[2] += P2.E[2];
  P1.E[3] += P2.E[3];

}

void
operator-=( v4 &P1, v4 P2 )
{
  P1.E[0] -= P2.E[0];
  P1.E[1] -= P2.E[1];
  P1.E[2] -= P2.E[2];
  P1.E[3] -= P2.E[3];

}




