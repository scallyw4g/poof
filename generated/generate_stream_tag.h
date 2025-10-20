// ./poof/poof.h:828:0

struct tag_stream_chunk
{
  tag Element;
  tag_stream_chunk* Next;
};

struct tag_stream
{
  memory_arena *Memory;
  tag_stream_chunk* FirstChunk;
  tag_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal tag_stream
TagStream(memory_arena *Memory)
{
  tag_stream Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal void
Deallocate(tag_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct tag_iterator
{
  tag_stream* Stream;
  tag_stream_chunk* At;
};

link_internal tag_iterator
Iterator(tag_stream* Stream)
{
  tag_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(tag_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(tag_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(tag_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal tag *
Push(tag_stream* Stream, tag Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  tag_stream_chunk* NextChunk = (tag_stream_chunk*)PushStruct(Stream->Memory, sizeof(tag_stream_chunk), 1, 0);
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

  tag *Result = &NextChunk->Element;
  return Result;
}


