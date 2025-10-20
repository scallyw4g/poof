// ./poof/poof.h:347:0

struct enum_member_stream_chunk
{
  enum_member Element;
  enum_member_stream_chunk* Next;
};

struct enum_member_stream
{
  memory_arena *Memory;
  enum_member_stream_chunk* FirstChunk;
  enum_member_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal enum_member_stream
EnumMemberStream(memory_arena *Memory)
{
  enum_member_stream Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal void
Deallocate(enum_member_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct enum_member_iterator
{
  enum_member_stream* Stream;
  enum_member_stream_chunk* At;
};

link_internal enum_member_iterator
Iterator(enum_member_stream* Stream)
{
  enum_member_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(enum_member_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(enum_member_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(enum_member_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal enum_member *
Push(enum_member_stream* Stream, enum_member Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  enum_member_stream_chunk* NextChunk = (enum_member_stream_chunk*)PushStruct(Stream->Memory, sizeof(enum_member_stream_chunk), 1, 0);
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

  enum_member *Result = &NextChunk->Element;
  return Result;
}


