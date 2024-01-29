// ./poof/poof.h:461:0

struct d_union_member_stream_chunk
{
  d_union_member Element;
  d_union_member_stream_chunk* Next;
};

struct d_union_member_stream
{
  memory_arena *Memory;
  d_union_member_stream_chunk* FirstChunk;
  d_union_member_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(d_union_member_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct d_union_member_iterator
{
  d_union_member_stream* Stream;
  d_union_member_stream_chunk* At;
};

link_internal d_union_member_iterator
Iterator(d_union_member_stream* Stream)
{
  d_union_member_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(d_union_member_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(d_union_member_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(d_union_member_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal d_union_member *
Push(d_union_member_stream* Stream, d_union_member Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  d_union_member_stream_chunk* NextChunk = (d_union_member_stream_chunk*)PushStruct(Stream->Memory, sizeof(d_union_member_stream_chunk), 1, 0);
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

  d_union_member *Result = &NextChunk->Element;
  return Result;
}


