        struct person_stream_chunk
    {
      person Element;
      person_stream_chunk* Next;
    };

        struct person_stream
    {
      memory_arena *Memory;
      person_stream_chunk* FirstChunk;
      person_stream_chunk* LastChunk;
    };

    link_internal void
    Deallocate(person_stream *Stream)
    {
      NotImplemented;
    }

        struct person_iterator
    {
      person_stream* Stream;
      person_stream_chunk* At;
    };

    link_internal person_iterator
    Iterator(person_stream* Stream)
    {
      person_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    link_internal b32
    IsValid(person_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    link_internal void
    Advance(person_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    link_internal b32
    IsLastElement(person_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        link_internal person *
    Push(person_stream* Stream, person Element)
    {
      if (Stream->Memory == 0)
      {
        Stream->Memory = AllocateArena();
      }

      /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
      person_stream_chunk* NextChunk = (person_stream_chunk*)PushStruct(Stream->Memory, sizeof(person_stream_chunk), 1, 0);
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

      person *Result = &NextChunk->Element;
      return Result;
    }


