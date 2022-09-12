        struct tuple_CountedString_CountedString_stream_chunk
    {
      tuple_CountedString_CountedString Element;
      tuple_CountedString_CountedString_stream_chunk* Next;
    };

        struct tuple_CountedString_CountedString_stream
    {
      tuple_CountedString_CountedString_stream_chunk* FirstChunk;
      tuple_CountedString_CountedString_stream_chunk* LastChunk;
    };


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
    Push(tuple_CountedString_CountedString_stream* Stream, tuple_CountedString_CountedString Element, memory_arena* Memory)
    {
      tuple_CountedString_CountedString_stream_chunk* NextChunk = (tuple_CountedString_CountedString_stream_chunk*)PushStruct(Memory, sizeof(tuple_CountedString_CountedString_stream_chunk), 1, 0);
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

    link_internal void
    ConcatStreams( tuple_CountedString_CountedString_stream *S1, tuple_CountedString_CountedString_stream *S2)
    {
      if (S1->LastChunk)
      {
        Assert(S1->FirstChunk);

        if (S2->FirstChunk)
        {
          Assert(S2->LastChunk);
          S1->LastChunk->Next = S2->FirstChunk;
          S1->LastChunk = S2->LastChunk;
        }
        else
        {
          Assert(!S2->LastChunk);
        }
      }
      else
      {
        Assert(!S1->FirstChunk);
        Assert(!S1->LastChunk);

        if(S2->FirstChunk)
        {
          Assert(S2->LastChunk);
        }
        else
        {
          Assert(!S2->LastChunk);
        }

        *S1 = *S2;
      }
    }


