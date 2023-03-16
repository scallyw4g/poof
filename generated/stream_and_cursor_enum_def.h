struct enum_decl_cursor
{
  enum_decl *Start;
  enum_decl *At;
  enum_decl *End;
};

link_internal enum_decl_cursor
EnumDeclCursor(umm ElementCount, memory_arena* Memory)
{
  enum_decl *Start = (enum_decl*)PushStruct(Memory, sizeof(enum_decl), 1, 0);
  enum_decl_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
  };
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



