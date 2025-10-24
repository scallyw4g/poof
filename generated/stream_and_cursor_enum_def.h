// ./include/bonsai_stdlib/src/poof_functions.h:2054:0
struct enum_decl_cursor
{
  enum_decl *Start;
  // TODO(Jesse)(immediate): For the love of fucksakes change these to indices
  enum_decl *At;
  enum_decl *End;
};



link_internal enum_decl_cursor
EnumDeclCursor(umm ElementCount, memory_arena* Memory)
{
  enum_decl *Start = (enum_decl*)PushStruct(Memory, sizeof(enum_decl)*ElementCount, 1, 0);
  enum_decl_cursor Result = {};

  Result.Start = Start;
  Result.End = Start+ElementCount;
  Result.At = Start;

  return Result;
}

link_internal enum_decl*
GetPtr(enum_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  enum_decl *Result = {};
  if (ElementIndex < AtElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal enum_decl*
TryGetPtr(enum_decl_cursor *Cursor, umm ElementIndex)
{
  return GetPtr(Cursor, ElementIndex);
}

link_internal enum_decl*
GetPtrUnsafe(enum_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  enum_decl *Result = {};
  if (ElementIndex < TotalElements(Cursor)) { Result = Cursor->Start+ElementIndex; }
  return Result;
}

link_internal enum_decl
Get(enum_decl_cursor *Cursor, umm ElementIndex)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert(ElementIndex < CurrentCount(Cursor));
  enum_decl Result = Cursor->Start[ElementIndex];
  return Result;
}

link_internal void
Set(enum_decl_cursor *Cursor, umm ElementIndex, enum_decl Element)
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

link_internal enum_decl*
Advance(enum_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  enum_decl * Result = {};
  if ( Cursor->At < Cursor->End ) { Result = Cursor->At++; }
  return Result;
}

link_internal enum_decl *
Push(enum_decl_cursor *Cursor, enum_decl Element)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At < Cursor->End );
  enum_decl *Result = Cursor->At;
  *Cursor->At++ = Element;
  return Result;
}

link_internal enum_decl
Pop(enum_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  Assert( Cursor->At > Cursor->Start );
  enum_decl Result = Cursor->At[-1];
  Cursor->At--;
  return Result;
}

link_internal s32
LastIndex(enum_decl_cursor *Cursor)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  s32 Result = s32(CurrentCount(Cursor))-1;
  return Result;
}

link_internal enum_decl*
LastElement(enum_decl_cursor *Cursor)
{
  enum_decl *Result = {};
  s32 I = LastIndex(Cursor);
  if (I > -1) { Result = Cursor->Start + I; }
  return Result;
}

link_internal b32
Remove(enum_decl_cursor *Cursor, enum_decl Query)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  b32 Result = False;
  CursorIterator(ElementIndex, Cursor)
  {
    enum_decl Element = Get(Cursor, ElementIndex);
    if (AreEqual(Element, Query))
    {
      b32 IsLastIndex = LastIndex(Cursor) == s32(ElementIndex);
      enum_decl Tmp = Pop(Cursor);

      if (IsLastIndex) { Assert(AreEqual(Tmp, Query)); }
      else { Set(Cursor, ElementIndex, Tmp); }
      Result = True;
    }
  }
  return Result;
}


link_internal b32
ResizeCursor(enum_decl_cursor *Cursor, umm Count, memory_arena *Memory)
{
  /* ENSURE_OWNED_BY_THREAD(Cursor); */

  umm CurrentSize = TotalSize(Cursor);

  TruncateToElementCount(Cursor, Count);
  umm NewSize = TotalSize(Cursor);

  Assert(NewSize/sizeof(enum_decl) == Count);

  /* Info("Attempting to reallocate CurrentSize(%u), NewSize(%u)", CurrentSize, NewSize); */
  Ensure(Reallocate((u8*)Cursor->Start, Memory, CurrentSize, NewSize));
  return 0;
}

link_internal void
Unshift( enum_decl_cursor *Cursor )
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


struct enum_decl_stream_chunk
{
  enum_decl Element;
  enum_decl_stream_chunk* Next;
};

struct enum_decl_stream
{
  memory_arena *Memory;
  enum_decl_stream_chunk* FirstChunk;
  enum_decl_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal enum_decl_stream
EnumDeclStream(memory_arena *Memory)
{
  enum_decl_stream Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal void
Deallocate(enum_decl_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct enum_decl_iterator
{
  enum_decl_stream* Stream;
  enum_decl_stream_chunk* At;
};

link_internal enum_decl_iterator
Iterator(enum_decl_stream* Stream)
{
  enum_decl_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(enum_decl_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(enum_decl_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(enum_decl_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal enum_decl *
Push(enum_decl_stream* Stream, enum_decl Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  enum_decl_stream_chunk* NextChunk = (enum_decl_stream_chunk*)PushStruct(Stream->Memory, sizeof(enum_decl_stream_chunk), 1, 0);
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

  enum_decl *Result = &NextChunk->Element;
  return Result;
}



