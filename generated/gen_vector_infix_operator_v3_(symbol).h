inline v3
operator^( v3 P1, v3 P2 )
{
  v3 Result = {
    .E[0] = P1.E[0] ^ P2.E[0],
    .E[1] = P1.E[1] ^ P2.E[1],
    .E[2] = P1.E[2] ^ P2.E[2],

  };
  return Result;
}



