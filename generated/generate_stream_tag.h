        struct tag_stream_chunk
    {
      tag Element;
      tag_stream_chunk* Next;
    };

        struct tag_stream
    {
      tag_stream_chunk* FirstChunk;
      tag_stream_chunk* LastChunk;
    };


        struct tag_iterator
    {
      tag_stream* Stream;
      tag_stream_chunk* At;
    };

    link_internal tag_iterator
    Iterator(tag_stream* Stream)
    {
      tag_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    link_internal b32
    IsValid(tag_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    link_internal void
    Advance(tag_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    link_internal b32
    IsLastElement(tag_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        link_internal tag *
    Push(tag_stream* Stream, tag Element, memory_arena* Memory)
    {
      tag_stream_chunk* NextChunk = (tag_stream_chunk*)PushStruct(Memory, sizeof(tag_stream_chunk), 1, 0);
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

      tag *Result = &NextChunk->Element;
      return Result;
    }

    link_internal void
    ConcatStreams( tag_stream *S1, tag_stream *S2)
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


