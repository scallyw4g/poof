        struct enum_decl_cursor
    {
      enum_decl* Start;
      enum_decl* At;
      enum_decl* End;
    };

    bonsai_function enum_decl_cursor
    EnumDeclCursor(umm ElementCount, memory_arena* Memory)
    {
      enum_decl* Start = (enum_decl*)PushStruct(Memory, sizeof( enum_decl ), 1, 0);
      enum_decl_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

            struct enum_decl_stream_chunk
    {
      enum_decl Element;
      enum_decl_stream_chunk* Next;
    };

        struct enum_decl_stream
    {
      enum_decl_stream_chunk* FirstChunk;
      enum_decl_stream_chunk* LastChunk;
    };


        struct enum_decl_iterator
    {
      enum_decl_stream* Stream;
      enum_decl_stream_chunk* At;
    };

    bonsai_function enum_decl_iterator
    Iterator(enum_decl_stream* Stream)
    {
      enum_decl_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid(enum_decl_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance(enum_decl_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    bonsai_function b32
    IsLastElement(enum_decl_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        bonsai_function enum_decl *
    Push(enum_decl_stream* Stream, enum_decl Element, memory_arena* Memory)
    {
      enum_decl_stream_chunk* NextChunk = (enum_decl_stream_chunk*)PushStruct(Memory, sizeof( enum_decl_stream_chunk ), 1, 0);
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

      enum_decl *Result = &NextChunk->Element;
      return Result;
    }

    bonsai_function void
    ConcatStreams( enum_decl_stream *S1, enum_decl_stream *S2)
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



