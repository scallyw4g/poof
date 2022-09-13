struct declaration_cursor
{
  declaration *Start;
  declaration *At;
  declaration *End;
};

link_internal declaration_cursor
DeclarationCursor(umm ElementCount, memory_arena* Memory)
{
  declaration *Start = (declaration*)PushStruct(Memory, sizeof(declaration), 1, 0);
  declaration_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
  };
  return Result;
}

