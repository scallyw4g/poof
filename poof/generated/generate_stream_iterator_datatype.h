    struct declaration_iterator
    {
      declaration_stream* Stream;
      declaration_stream_chunk* At;
    };

    bonsai_function declaration_iterator
    Iterator(declaration_stream* Stream)
    {
      declaration_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid(declaration_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance(declaration_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    bonsai_function b32
    IsLastElement(declaration_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


