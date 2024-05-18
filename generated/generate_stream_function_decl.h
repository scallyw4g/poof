// ./poof/poof.h:313:0

struct function_decl_stream_chunk
{
  function_decl Element;
  function_decl_stream_chunk* Next;
};

struct function_decl_stream
{
  memory_arena *Memory;
  function_decl_stream_chunk* FirstChunk;
  function_decl_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(function_decl_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct function_decl_iterator
{
  function_decl_stream* Stream;
  function_decl_stream_chunk* At;
};

link_internal function_decl_iterator
Iterator(function_decl_stream* Stream)
{
  function_decl_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(function_decl_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(function_decl_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(function_decl_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal function_decl *
Push(function_decl_stream* Stream, function_decl Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  function_decl_stream_chunk* NextChunk = (function_decl_stream_chunk*)PushStruct(Stream->Memory, sizeof(function_decl_stream_chunk), 1, 0);
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

  function_decl *Result = &NextChunk->Element;
  return Result;
}


