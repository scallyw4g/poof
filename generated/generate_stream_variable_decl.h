// ./poof/poof.h:286:0

struct variable_decl_stream_chunk
{
  variable_decl Element;
  variable_decl_stream_chunk* Next;
};

struct variable_decl_stream
{
  memory_arena *Memory;
  variable_decl_stream_chunk* FirstChunk;
  variable_decl_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(variable_decl_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct variable_decl_iterator
{
  variable_decl_stream* Stream;
  variable_decl_stream_chunk* At;
};

link_internal variable_decl_iterator
Iterator(variable_decl_stream* Stream)
{
  variable_decl_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(variable_decl_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(variable_decl_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(variable_decl_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal variable_decl *
Push(variable_decl_stream* Stream, variable_decl Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  variable_decl_stream_chunk* NextChunk = (variable_decl_stream_chunk*)PushStruct(Stream->Memory, sizeof(variable_decl_stream_chunk), 1, 0);
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

  variable_decl *Result = &NextChunk->Element;
  return Result;
}


