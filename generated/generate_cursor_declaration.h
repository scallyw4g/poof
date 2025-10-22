// ./poof/poof.h:441:0


struct declaration_cursor
{
  declaration *Start;
  // TODO(Jesse)(immediate): For the love of fucksakes change these to indices
  declaration *At;
  declaration *End;
};



link_internal declaration_cursor
DeclarationCursor(umm ElementCount, memory_arena* Memory)
{
  declaration *Start = (declaration*)PushStruct(Memory, sizeof(declaration)*ElementCount, 1, 0);
  declaration_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
  };
  return Result;
}

link_internal declaration*
GetPtr(declaration_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  declaration *Result = {};
  if (ElementIndex < AtElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal declaration*
TryGetPtr(declaration_cursor *Cursor, umm ElementIndex)
{
  return GetPtr(Cursor, ElementIndex);
}

link_internal declaration*
GetPtrUnsafe(declaration_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  declaration *Result = {};
  if (ElementIndex < TotalElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal declaration
Get(declaration_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert(ElementIndex < CurrentCount(Cursor));
  declaration Result = Cursor->Start[ElementIndex];
  return Result;
}

link_internal void
Set(declaration_cursor *Cursor, umm ElementIndex, declaration Element)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  umm CurrentElementCount = CurrentCount(Cursor);
  Assert (ElementIndex <= CurrentElementCount);

  Cursor->Start[ElementIndex] = Element;
  if (ElementIndex == CurrentElementCount)
  {
    Cursor->At++;
  }
}

link_internal declaration*
Advance(declaration_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  declaration * Result = {};
  if ( Cursor->At < Cursor->End ) { Result = Cursor->At++; }
  return Result;
}

link_internal declaration *
Push(declaration_cursor *Cursor, declaration Element)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At < Cursor->End );
  declaration *Result = Cursor->At;
  *Cursor->At++ = Element;
  return Result;
}

link_internal declaration
Pop(declaration_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At > Cursor->Start );
  declaration Result = Cursor->At[-1];
  Cursor->At--;
  return Result;
}

link_internal s32
LastIndex(declaration_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  s32 Result = s32(CurrentCount(Cursor))-1;
  return Result;
}

link_internal declaration*
LastElement(declaration_cursor *Cursor)
{
  declaration *Result = {};
  s32 I = LastIndex(Cursor);
  if (I > -1) { Result = Cursor->Start + I; }
  return Result;
}

link_internal b32
Remove(declaration_cursor *Cursor, declaration Query)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  b32 Result = False;
  CursorIterator(ElementIndex, Cursor)
  {
    declaration Element = Get(Cursor, ElementIndex);
    if (AreEqual(Element, Query))
    {
      b32 IsLastIndex = LastIndex(Cursor) == s32(ElementIndex);
      declaration Tmp = Pop(Cursor);

      if (IsLastIndex) { Assert(AreEqual(Tmp, Query)); }
      else { Set(Cursor, ElementIndex, Tmp); }
      Result = True;
    }
  }
  return Result;
}


link_internal b32
ResizeCursor(declaration_cursor *Cursor, umm Count, memory_arena *Memory)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  umm CurrentSize = TotalSize(Cursor);

  TruncateToElementCount(Cursor, Count);
  umm NewSize = TotalSize(Cursor);

  Assert(NewSize/sizeof(declaration) == Count);

  /* Info("Attempting to reallocate CurrentSize(%u), NewSize(%u)", CurrentSize, NewSize); */
  Ensure(Reallocate((u8*)Cursor->Start, Memory, CurrentSize, NewSize));
  return 0;
}

link_internal void
Unshift( declaration_cursor *Cursor )
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */
  umm Count = AtElements(Cursor);
  if (Count)
  {
    for (umm Index = 1; Index < Count; ++Index)
    {
      Cursor->Start[Index-1] = Cursor->Start[Index];
    }

    // NOTE(Jesse): This is actually correct, even though it doesn't look
    // like it at first glance.  At is OnePastLast, so decrementing and
    // then clearing overwrites the last value that was set.
    Cursor->At--;
    *Cursor->At = {};
  }
}



