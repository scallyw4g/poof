        struct compound_decl_cursor
    {
      compound_decl* Start;
      compound_decl* At;
      compound_decl* End;
    };

    bonsai_function compound_decl_cursor
    CompoundDeclCursor(umm ElementCount, memory_arena* Memory)
    {
      compound_decl* Start = (compound_decl*)PushStruct(Memory, sizeof( compound_decl ), 1, 0);
      compound_decl_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

            struct compound_decl_stream_chunk
    {
      compound_decl Element;
      compound_decl_stream_chunk* Next;
    };

        struct compound_decl_stream
    {
      compound_decl_stream_chunk* FirstChunk;
      compound_decl_stream_chunk* LastChunk;
    };


        struct compound_decl_iterator
    {
      compound_decl_stream* Stream;
      compound_decl_stream_chunk* At;
    };

    bonsai_function compound_decl_iterator
    Iterator(compound_decl_stream* Stream)
    {
      compound_decl_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid(compound_decl_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance(compound_decl_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    bonsai_function b32
    IsLastElement(compound_decl_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        bonsai_function compound_decl *
    Push(compound_decl_stream* Stream, compound_decl Element, memory_arena* Memory)
    {
      compound_decl_stream_chunk* NextChunk = (compound_decl_stream_chunk*)PushStruct(Memory, sizeof( compound_decl_stream_chunk ), 1, 0);
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

      compound_decl *Result = &NextChunk->Element;
      return Result;
    }

    bonsai_function void
    ConcatStreams( compound_decl_stream *S1, compound_decl_stream *S2)
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



