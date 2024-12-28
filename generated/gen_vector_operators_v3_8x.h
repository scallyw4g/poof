// ./include/bonsai_stdlib/src/avx2_v3.h:30:0

link_internal void
DeepCopy(v3_8x *Src, v3_8x *Dest)
{
  *Dest = *Src;
}

/* inline b32 */
/* operator==( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = ( P1.Raw[0] == P2.Raw[0] && P1.Raw[1] == P2.Raw[1] && P1.Raw[2] == P2.Raw[2] ); */
/*   return Result; */
/* } */


// NOTE(Jesse): This is for contaiers that call AreEqual()
/* link_internal b32 */
/* AreEqual(v3_8x V1, v3_8x V2) */
/* { */
/*   b32 Result = V1 == V2; */
/*   return Result; */
/* } */

// NOTE(Jesse): Can't gen != because the condition welding it together
// is not &&, it's ||
//
/* gen_vector_comparator(Type, {!=}) */
/* inline b32 */
/* operator!=( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = !(P1 == P2); */
/*   return Result; */
/* } */

/* inline b32 */
/* operator<( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = ( P1.Raw[0] < P2.Raw[0] && P1.Raw[1] < P2.Raw[1] && P1.Raw[2] < P2.Raw[2] ); */
/*   return Result; */
/* } */


/* inline b32 */
/* operator<=( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = ( P1.Raw[0] <= P2.Raw[0] && P1.Raw[1] <= P2.Raw[1] && P1.Raw[2] <= P2.Raw[2] ); */
/*   return Result; */
/* } */


/* inline b32 */
/* operator>( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = ( P1.Raw[0] > P2.Raw[0] && P1.Raw[1] > P2.Raw[1] && P1.Raw[2] > P2.Raw[2] ); */
/*   return Result; */
/* } */


/* inline b32 */
/* operator>=( v3_8x P1, v3_8x P2 ) */
/* { */
/*   b32 Result = ( P1.Raw[0] >= P2.Raw[0] && P1.Raw[1] >= P2.Raw[1] && P1.Raw[2] >= P2.Raw[2] ); */
/*   return Result; */
/* } */


inline v3_8x
operator+( v3_8x P1, v3_8x P2 )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] + P2.Raw[0]),
      Cast(f32_8x, P1.Raw[1] + P2.Raw[1]),
      Cast(f32_8x, P1.Raw[2] + P2.Raw[2]),
    }};
  return Result;
}

inline v3_8x
operator+( v3_8x P1, f32_8x Scalar )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] + Scalar),
      Cast(f32_8x, P1.Raw[1] + Scalar),
      Cast(f32_8x, P1.Raw[2] + Scalar),
    }};
  return Result;
}

inline v3_8x
operator+( f32_8x Scalar, v3_8x P1 )
{
  v3_8x Result = {{
      Cast(f32_8x, Scalar + P1.Raw[0]),
      Cast(f32_8x, Scalar + P1.Raw[1]),
      Cast(f32_8x, Scalar + P1.Raw[2]),
    }};
  return Result;
}



inline v3_8x
operator-( v3_8x P1, v3_8x P2 )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] - P2.Raw[0]),
      Cast(f32_8x, P1.Raw[1] - P2.Raw[1]),
      Cast(f32_8x, P1.Raw[2] - P2.Raw[2]),
    }};
  return Result;
}

inline v3_8x
operator-( v3_8x P1, f32_8x Scalar )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] - Scalar),
      Cast(f32_8x, P1.Raw[1] - Scalar),
      Cast(f32_8x, P1.Raw[2] - Scalar),
    }};
  return Result;
}

inline v3_8x
operator-( f32_8x Scalar, v3_8x P1 )
{
  v3_8x Result = {{
      Cast(f32_8x, Scalar - P1.Raw[0]),
      Cast(f32_8x, Scalar - P1.Raw[1]),
      Cast(f32_8x, Scalar - P1.Raw[2]),
    }};
  return Result;
}



inline v3_8x
operator*( v3_8x P1, v3_8x P2 )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] * P2.Raw[0]),
      Cast(f32_8x, P1.Raw[1] * P2.Raw[1]),
      Cast(f32_8x, P1.Raw[2] * P2.Raw[2]),
    }};
  return Result;
}

inline v3_8x
operator*( v3_8x P1, f32_8x Scalar )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] * Scalar),
      Cast(f32_8x, P1.Raw[1] * Scalar),
      Cast(f32_8x, P1.Raw[2] * Scalar),
    }};
  return Result;
}

inline v3_8x
operator*( f32_8x Scalar, v3_8x P1 )
{
  v3_8x Result = {{
      Cast(f32_8x, Scalar * P1.Raw[0]),
      Cast(f32_8x, Scalar * P1.Raw[1]),
      Cast(f32_8x, Scalar * P1.Raw[2]),
    }};
  return Result;
}



inline v3_8x
operator/( v3_8x P1, v3_8x P2 )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] / P2.Raw[0]),
      Cast(f32_8x, P1.Raw[1] / P2.Raw[1]),
      Cast(f32_8x, P1.Raw[2] / P2.Raw[2]),
    }};
  return Result;
}

inline v3_8x
operator/( v3_8x P1, f32_8x Scalar )
{
  v3_8x Result = {{
      Cast(f32_8x, P1.Raw[0] / Scalar),
      Cast(f32_8x, P1.Raw[1] / Scalar),
      Cast(f32_8x, P1.Raw[2] / Scalar),
    }};
  return Result;
}

inline v3_8x
operator/( f32_8x Scalar, v3_8x P1 )
{
  v3_8x Result = {{
      Cast(f32_8x, Scalar / P1.Raw[0]),
      Cast(f32_8x, Scalar / P1.Raw[1]),
      Cast(f32_8x, Scalar / P1.Raw[2]),
    }};
  return Result;
}



inline v3_8x &
operator+=( v3_8x &P1, v3_8x P2 )
{
  P1.Raw[0] += P2.Raw[0];
  P1.Raw[1] += P2.Raw[1];
  P1.Raw[2] += P2.Raw[2];
  return P1;
}

inline v3_8x &
operator+=( v3_8x &P1, f32_8x Scalar )
{
  P1.Raw[0] += Scalar;
  P1.Raw[1] += Scalar;
  P1.Raw[2] += Scalar;
  return P1;
}


inline v3_8x &
operator-=( v3_8x &P1, v3_8x P2 )
{
  P1.Raw[0] -= P2.Raw[0];
  P1.Raw[1] -= P2.Raw[1];
  P1.Raw[2] -= P2.Raw[2];
  return P1;
}

inline v3_8x &
operator-=( v3_8x &P1, f32_8x Scalar )
{
  P1.Raw[0] -= Scalar;
  P1.Raw[1] -= Scalar;
  P1.Raw[2] -= Scalar;
  return P1;
}


inline v3_8x &
operator*=( v3_8x &P1, v3_8x P2 )
{
  P1.Raw[0] *= P2.Raw[0];
  P1.Raw[1] *= P2.Raw[1];
  P1.Raw[2] *= P2.Raw[2];
  return P1;
}

inline v3_8x &
operator*=( v3_8x &P1, f32_8x Scalar )
{
  P1.Raw[0] *= Scalar;
  P1.Raw[1] *= Scalar;
  P1.Raw[2] *= Scalar;
  return P1;
}


inline v3_8x &
operator/=( v3_8x &P1, v3_8x P2 )
{
  P1.Raw[0] /= P2.Raw[0];
  P1.Raw[1] /= P2.Raw[1];
  P1.Raw[2] /= P2.Raw[2];
  return P1;
}

inline v3_8x &
operator/=( v3_8x &P1, f32_8x Scalar )
{
  P1.Raw[0] /= Scalar;
  P1.Raw[1] /= Scalar;
  P1.Raw[2] /= Scalar;
  return P1;
}




