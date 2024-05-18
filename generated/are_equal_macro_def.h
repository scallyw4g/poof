// ./poof/poof.h:753:0

link_internal b32
AreEqual(macro_def *Thing1, macro_def *Thing2)
{
  b32 Result = MemoryIsEqual((u8*)Thing1, (u8*)Thing2, sizeof( macro_def ) );

  return Result;
}

link_internal b32
AreEqual(macro_def Thing1, macro_def Thing2)
{
  b32 Result = MemoryIsEqual((u8*)&Thing1, (u8*)&Thing2, sizeof( macro_def ) );

  return Result;
}

