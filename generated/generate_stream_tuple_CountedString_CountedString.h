        struct tuple_CountedString_CountedString_stream_chunk
    {
      tuple_CountedString_CountedString Element;
      tuple_CountedString_CountedString_stream_chunk* Next;
    };

        struct tuple_CountedString_CountedString_stream
    {
      memory_arena *Memory;
      tuple_CountedString_CountedString_stream_chunk* FirstChunk;
      tuple_CountedString_CountedString_stream_chunk* LastChunk;
    };

    link_internal void
    Deallocate(tuple_CountedString_CountedString_stream *Stream)
    {
      Stream->LastChunk = 0;
      Stream->FirstChunk = 0;
      VaporizeArena(Stream->Memory);
    }

        struct tuple_CountedString_CountedString_iterator
    {
      tuple_CountedString_CountedString_stream* Stream;
      tuple_CountedString_CountedString_stream_chunk* At;
    };

    link_internal tuple_CountedString_CountedString_iterator
    Iterator(tuple_CountedString_CountedString_stream* Stream)
    {
      tuple_CountedString_CountedString_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    link_internal b32
    IsValid(tuple_CountedString_CountedString_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    link_internal void
    Advance(tuple_CountedString_CountedString_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    link_internal b32
    IsLastElement(tuple_CountedString_CountedString_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        link_internal tuple_CountedString_CountedString *
    Push(tuple_CountedString_CountedString_stream* Stream, tuple_CountedString_CountedString Element)
    {
      if (Stream->Memory == 0)
      {
        Stream->Memory = AllocateArena();
      }

      /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
      tuple_CountedString_CountedString_stream_chunk* NextChunk = (tuple_CountedString_CountedString_stream_chunk*)PushStruct(Stream->Memory, sizeof(tuple_CountedString_CountedString_stream_chunk), 1, 0);
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

      tuple_CountedString_CountedString *Result = &NextChunk->Element;
      return Result;
    }


