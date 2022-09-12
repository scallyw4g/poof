        struct person_stream_chunk
    {
      person Element;
      person_stream_chunk* Next;
    };

        struct person_stream
    {
      person_stream_chunk* FirstChunk;
      person_stream_chunk* LastChunk;
    };


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
    Push(person_stream* Stream, person Element, memory_arena* Memory)
    {
      person_stream_chunk* NextChunk = (person_stream_chunk*)PushStruct(Memory, sizeof(person_stream_chunk), 1, 0);
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

    link_internal void
    ConcatStreams( person_stream *S1, person_stream *S2)
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


