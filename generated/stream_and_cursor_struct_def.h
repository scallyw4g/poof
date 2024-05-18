// ./poof/poof.h:170:0

struct compound_decl_cursor
{
  compound_decl *Start;
  // TODO(Jesse)(immediate): For the love of fucksakes change these to indices
  compound_decl *At;
  compound_decl *End;
  /* OWNED_BY_THREAD_MEMBER(); */
};



link_internal compound_decl_cursor
CompoundDeclCursor(umm ElementCount, memory_arena* Memory)
{
  compound_decl *Start = (compound_decl*)PushStruct(Memory, sizeof(compound_decl)*ElementCount, 1, 0);
  compound_decl_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
    /* OWNED_BY_THREAD_MEMBER_INIT() */
  };
  return Result;
}

link_internal compound_decl*
GetPtr(compound_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  compound_decl *Result = {};
  if (ElementIndex < AtElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal compound_decl*
GetPtrUnsafe(compound_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  compound_decl *Result = {};
  if (ElementIndex < TotalElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal compound_decl
Get(compound_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert(ElementIndex < CurrentCount(Cursor));
  compound_decl Result = Cursor->Start[ElementIndex];
  return Result;
}

link_internal void
Set(compound_decl_cursor *Cursor, umm ElementIndex, compound_decl Element)
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

link_internal compound_decl*
Advance(compound_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  compound_decl * Result = {};
  if ( Cursor->At < Cursor->End ) { Result = Cursor->At++; }
  return Result;
}

link_internal compound_decl *
Push(compound_decl_cursor *Cursor, compound_decl Element)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At < Cursor->End );
  compound_decl *Result = Cursor->At;
  *Cursor->At++ = Element;
  return Result;
}

link_internal compound_decl
Pop(compound_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At > Cursor->Start );
  compound_decl Result = Cursor->At[-1];
  Cursor->At--;
  return Result;
}

link_internal s32
LastIndex(compound_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  s32 Result = s32(CurrentCount(Cursor))-1;
  return Result;
}

link_internal compound_decl*
LastElement(compound_decl_cursor *Cursor)
{
  compound_decl *Result = {};
  s32 I = LastIndex(Cursor);
  if (I > -1) { Result = Cursor->Start + I; }
  return Result;
}

link_internal b32
Remove(compound_decl_cursor *Cursor, compound_decl Query)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  b32 Result = False;
  CursorIterator(ElementIndex, Cursor)
  {
    compound_decl Element = Get(Cursor, ElementIndex);
    if (AreEqual(Element, Query))
    {
      b32 IsLastIndex = LastIndex(Cursor) == s32(ElementIndex);
      compound_decl Tmp = Pop(Cursor);

      if (IsLastIndex) { Assert(AreEqual(Tmp, Query)); }
      else { Set(Cursor, ElementIndex, Tmp); }
      Result = True;
    }
  }
  return Result;
}


link_internal b32
ResizeCursor(compound_decl_cursor *Cursor, umm Count, memory_arena *Memory)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  umm CurrentSize = TotalSize(Cursor);

  TruncateToElementCount(Cursor, Count);
  umm NewSize = TotalSize(Cursor);

  Assert(NewSize/sizeof(compound_decl) == Count);

  /* Info("Attempting to reallocate CurrentSize(%u), NewSize(%u)", CurrentSize, NewSize); */
  Ensure(Reallocate((u8*)Cursor->Start, Memory, CurrentSize, NewSize));
  return 0;
}

link_internal void
Unshift( compound_decl_cursor *Cursor )
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


struct compound_decl_stream_chunk
{
  compound_decl Element;
  compound_decl_stream_chunk* Next;
};

struct compound_decl_stream
{
  memory_arena *Memory;
  compound_decl_stream_chunk* FirstChunk;
  compound_decl_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(compound_decl_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct compound_decl_iterator
{
  compound_decl_stream* Stream;
  compound_decl_stream_chunk* At;
};

link_internal compound_decl_iterator
Iterator(compound_decl_stream* Stream)
{
  compound_decl_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(compound_decl_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(compound_decl_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(compound_decl_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal compound_decl *
Push(compound_decl_stream* Stream, compound_decl Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  compound_decl_stream_chunk* NextChunk = (compound_decl_stream_chunk*)PushStruct(Stream->Memory, sizeof(compound_decl_stream_chunk), 1, 0);
  NextChunk->Element = Element;

  if (!Stream->FirstChunk)
  {
    Assert(!Stream->LastChunk);
    Stream->FirstChunk = NextChunk;
    Stream->LastChunk = NextChunk;
  }
  else
  {
    Stream->LastChunk->Next = NextChunk;
    Stream->LastChunk = NextChunk;
  }

  Assert(NextChunk->Next == 0);
  Assert(Stream->LastChunk->Next == 0);

  Stream->ChunkCount += 1;

  compound_decl *Result = &NextChunk->Element;
  return Result;
}



