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
  enum_decl_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
  };
  return Result;
}

link_internal enum_decl*
GetPtr(enum_decl_cursor *Cursor, umm ElementIndex)
{
  enum_decl *Result = {};
  if (ElementIndex < AtElements(Cursor)) {
    Result = Cursor->Start+ElementIndex;
  }
  return Result;
}

link_internal enum_decl
Get(enum_decl_cursor *Cursor, umm ElementIndex)
{
  Assert(ElementIndex < CurrentCount(Cursor));
  enum_decl Result = Cursor->Start[ElementIndex];
  return Result;
}

link_internal void
Set(enum_decl_cursor *Cursor, umm ElementIndex, enum_decl Element)
{
  umm CurrentElementCount = CurrentCount(Cursor);
  Assert (ElementIndex <= CurrentElementCount);

  Cursor->Start[ElementIndex] = Element;
  if (ElementIndex == CurrentElementCount)
  {
    Cursor->At++;
  }
}

link_internal enum_decl *
Push(enum_decl_cursor *Cursor, enum_decl Element)
{
  Assert( Cursor->At < Cursor->End );
  enum_decl *Result = Cursor->At;
  *Cursor->At++ = Element;
  return Result;
}

link_internal enum_decl
Pop(enum_decl_cursor *Cursor)
{
  Assert( Cursor->At > Cursor->Start );
  enum_decl Result = Cursor->At[-1];
  Cursor->At--;
  return Result;
}

link_internal s32
LastIndex(enum_decl_cursor *Cursor)
{
  s32 Result = s32(CurrentCount(Cursor))-1;
  return Result;
}

link_internal b32
Remove(enum_decl_cursor *Cursor, enum_decl Query)
{
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
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

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



