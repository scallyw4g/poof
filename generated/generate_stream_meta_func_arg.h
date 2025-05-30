// ./poof/poof.h:645:0

struct meta_func_arg_stream_chunk
{
  meta_func_arg Element;
  meta_func_arg_stream_chunk* Next;
};

struct meta_func_arg_stream
{
  memory_arena *Memory;
  meta_func_arg_stream_chunk* FirstChunk;
  meta_func_arg_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal meta_func_arg_stream
MetaFuncArgStream(memory_arena *Memory)
{
  meta_func_arg_stream Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal void
Deallocate(meta_func_arg_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct meta_func_arg_iterator
{
  meta_func_arg_stream* Stream;
  meta_func_arg_stream_chunk* At;
};

link_internal meta_func_arg_iterator
Iterator(meta_func_arg_stream* Stream)
{
  meta_func_arg_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(meta_func_arg_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(meta_func_arg_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(meta_func_arg_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal meta_func_arg *
Push(meta_func_arg_stream* Stream, meta_func_arg Element)
{
  Assert(Stream->Memory);

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  meta_func_arg_stream_chunk* NextChunk = (meta_func_arg_stream_chunk*)PushStruct(Stream->Memory, sizeof(meta_func_arg_stream_chunk), 1, 0);
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

  meta_func_arg *Result = &NextChunk->Element;
  return Result;
}


