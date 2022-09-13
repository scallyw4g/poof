        struct compound_decl_cursor
    {
      compound_decl *Start;
      compound_decl *At;
      compound_decl *End;
    };

    link_internal compound_decl_cursor
    CompoundDeclCursor(umm ElementCount, memory_arena* Memory)
    {
      compound_decl *Start = (compound_decl*)PushStruct(Memory, sizeof(compound_decl), 1, 0);
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
      memory_arena *Memory;
      compound_decl_stream_chunk* FirstChunk;
      compound_decl_stream_chunk* LastChunk;
    };

    link_internal void
    Deallocate(compound_decl_stream *Stream)
    {
      NotImplemented;
    }

        struct compound_decl_iterator
    {
      compound_decl_stream* Stream;
      compound_decl_stream_chunk* At;
    };

    link_internal compound_decl_iterator
    Iterator(compound_decl_stream* Stream)
    {
      compound_decl_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    link_internal b32
    IsValid(compound_decl_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    link_internal void
    Advance(compound_decl_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    link_internal b32
    IsLastElement(compound_decl_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }


        link_internal compound_decl *
    Push(compound_decl_stream* Stream, compound_decl Element)
    {
      if (Stream->Memory == 0)
      {
        Stream->Memory = AllocateArena();
      }

      /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
      compound_decl_stream_chunk* NextChunk = (compound_decl_stream_chunk*)PushStruct(Stream->Memory, sizeof(compound_decl_stream_chunk), 1, 0);
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



