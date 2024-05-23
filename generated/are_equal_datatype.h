// ./poof/poof.h:456:0

link_internal b32
AreEqual(datatype *Thing1, datatype *Thing2)
{
  b32 Result = MemoryIsEqual((u8*)Thing1, (u8*)Thing2, sizeof( datatype ) );

  return Result;
}

link_internal b32
AreEqual(datatype Thing1, datatype Thing2)
{
  b32 Result = MemoryIsEqual((u8*)&Thing1, (u8*)&Thing2, sizeof( datatype ) );

  return Result;
}

