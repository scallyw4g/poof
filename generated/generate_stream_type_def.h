// ./poof/poof.h:458:0

struct type_def_stream_chunk
{
  type_def Element;
  type_def_stream_chunk* Next;
};

struct type_def_stream
{
  memory_arena *Memory;
  type_def_stream_chunk* FirstChunk;
  type_def_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal type_def_stream
TypeDefStream(memory_arena *Memory)
{
  type_def_stream Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal void
Deallocate(type_def_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct type_def_iterator
{
  type_def_stream* Stream;
  type_def_stream_chunk* At;
};

link_internal type_def_iterator
Iterator(type_def_stream* Stream)
{
  type_def_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(type_def_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(type_def_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(type_def_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal type_def *
Push(type_def_stream* Stream, type_def Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  type_def_stream_chunk* NextChunk = (type_def_stream_chunk*)PushStruct(Stream->Memory, sizeof(type_def_stream_chunk), 1, 0);
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

  type_def *Result = &NextChunk->Element;
  return Result;
}


