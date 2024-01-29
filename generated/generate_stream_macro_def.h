// ./poof/poof.h:735:0

struct macro_def_stream_chunk
{
  macro_def Element;
  macro_def_stream_chunk* Next;
};

struct macro_def_stream
{
  memory_arena *Memory;
  macro_def_stream_chunk* FirstChunk;
  macro_def_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(macro_def_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct macro_def_iterator
{
  macro_def_stream* Stream;
  macro_def_stream_chunk* At;
};

link_internal macro_def_iterator
Iterator(macro_def_stream* Stream)
{
  macro_def_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(macro_def_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(macro_def_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(macro_def_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal macro_def *
Push(macro_def_stream* Stream, macro_def Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  macro_def_stream_chunk* NextChunk = (macro_def_stream_chunk*)PushStruct(Stream->Memory, sizeof(macro_def_stream_chunk), 1, 0);
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

  macro_def *Result = &NextChunk->Element;
  return Result;
}


