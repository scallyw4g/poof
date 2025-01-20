// ./poof/poof.h:808:0

struct todo_stream_chunk
{
  todo Element;
  todo_stream_chunk* Next;
};

struct todo_stream
{
  memory_arena *Memory;
  todo_stream_chunk* FirstChunk;
  todo_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(todo_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct todo_iterator
{
  todo_stream* Stream;
  todo_stream_chunk* At;
};

link_internal todo_iterator
Iterator(todo_stream* Stream)
{
  todo_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(todo_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(todo_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(todo_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal todo *
Push(todo_stream* Stream, todo Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  todo_stream_chunk* NextChunk = (todo_stream_chunk*)PushStruct(Stream->Memory, sizeof(todo_stream_chunk), 1, 0);
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

  todo *Result = &NextChunk->Element;
  return Result;
}


