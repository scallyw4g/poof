// ./poof/poof.h:986:0

struct declaration_iterator
{
  declaration_stream* Stream;
  declaration_stream_chunk* At;
};

link_internal declaration_iterator
Iterator(declaration_stream* Stream)
{
  declaration_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(declaration_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(declaration_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(declaration_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


