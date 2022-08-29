
poof( func hashtable(Type) { (hashtable_struct(Type)) (hashtable_impl(Type)) })

poof(
  func hashtable_struct(Type)
  {
    struct (Type.name)_linked_list_node
    {
      (Type.name) Element;
      (Type.name)_linked_list_node *Next;
    };

    struct (Type.name)_hashtable
    {
      umm Size;
      (Type.name)_linked_list_node **Elements;
    };
  }
);

poof(
  func hashtable_impl(Type)
  {
    bonsai_function (Type.name)_linked_list_node *
    Allocate_(Type.name)_linked_list_node(memory_arena *Memory)
    {
      (Type.name)_linked_list_node *Result = Allocate( (Type.name)_linked_list_node, Memory, 1);
      return Result;
    }

    bonsai_function (Type.name)_hashtable
    Allocate_(Type.name)_hashtable(umm ElementCount, memory_arena *Memory)
    {
      (Type.name)_hashtable Result = {};
      Result.Elements = Allocate( (Type.name)_linked_list_node*, Memory, ElementCount);
      Result.Size = ElementCount;
      return Result;
    }

    bonsai_function (Type.name)_linked_list_node *
    GetHashBucket(umm HashValue, (Type.name)_hashtable *Table)
    {
      Assert(Table->Size);
      (Type.name)_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
      return Result;
    }

    bonsai_function (Type.name) *
    GetFirstAtBucket(umm HashValue, (Type.name)_hashtable *Table)
    {
      (Type.name)_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
      (Type.name) *Result = &Bucket->Element;
      return Result;
    }

    bonsai_function (Type.name)*
    Insert((Type.name)_linked_list_node *Node, (Type.name)_hashtable *Table)
    {
      Assert(Table->Size);
      umm HashValue = Hash(&Node->Element) % Table->Size;
      (Type.name)_linked_list_node **Bucket = Table->Elements + HashValue;
      while (*Bucket) Bucket = &(*Bucket)->Next;
      *Bucket = Node;
      return &Bucket[0]->Element;
    }

    bonsai_function (Type.name)*
    Insert((Type.name) Element, (Type.name)_hashtable *Table, memory_arena *Memory)
    {
      (Type.name)_linked_list_node *Bucket = Allocate_(Type.name)_linked_list_node(Memory);
      Bucket->Element = Element;
      Insert(Bucket, Table);
      return &Bucket->Element;
    }
  }
)

poof(
  func dunion_debug_print_prototype(DUnion)
  {
    bonsai_function void DebugPrint( (DUnion.type) *Struct, u32 Depth = 0);
    bonsai_function void DebugPrint( (DUnion.type) Struct, u32 Depth = 0);
  }
)

poof(
  func dunion_debug_print(DUnion)
  {
    bonsai_function void
    DebugPrint( (DUnion.type) *Struct, u32 Depth)
    {
      if (Struct)
      {
        DebugPrint(*Struct, Depth);
      }
    }

    bonsai_function void
    DebugPrint( (DUnion.type) Struct, u32 Depth)
    {
      DebugPrint("(DUnion.type) {\n", Depth);

      /* DebugPrint("Type = ", Depth+4); */
      /* DebugPrint(Struct.Type); */
      /* DebugPrint(";\n"); */

      switch(Struct.Type)
      {
        (DUnion.map_members (M) {
          (M.is_union?
          {
            (M.map_members (UnionMember)
            {
              case type_(UnionMember.type):
              {
                DebugPrint(Struct.(UnionMember.name), Depth+4);
              } break;
            })
          })
        })

        default : { DebugPrint("default while printing (DUnion.type) (DUnion.name) ", Depth+4); DebugLine("Type(%d)", Struct.Type); } break;
      }
      DebugPrint("}\n", Depth);
    }
  }
)

poof(
  func buffer(Type)
  {
    struct (Type.name)_buffer
    {
      (Type.name)* Start;
      umm Count;
    };

    bonsai_function (Type.name)_buffer
    (Type.name.to_capital_case)Buffer(umm ElementCount, memory_arena* Memory)
    {
      (Type.name)_buffer Result = {};

      if (ElementCount)
      {
        Result.Start = Allocate( (Type.name), Memory, ElementCount );
        Result.Count = ElementCount;
      }
      else
      {
        Warn("Attempted to allocate (Type.name)_buffer of 0 length.");
      }

      return Result;
    }
  }
)

poof(
  func generate_cursor(Type)
  {
    struct (Type.name)_cursor
    {
      (Type.name)* Start;
      (Type.name)* At;
      (Type.name)* End;
    };

    bonsai_function (Type.name)_cursor
    (Type.name.to_capital_case)Cursor(umm ElementCount, memory_arena* Memory)
    {
      (Type.name)* Start = ((Type.name)*)PushStruct(Memory, sizeof( (Type.name) ), 1, 0);
      (Type.name)_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }
  }
)

poof(
  func generate_string_table(EnumType)
  {
    bonsai_function counted_string
    ToString( (EnumType.name) Type)
    {
      counted_string Result = {};
      switch (Type)
      {
        (
          EnumType.map_values (EnumValue)
          {
            case (EnumValue.name): { Result = CSz("(EnumValue.name)"); } break;
          }
        )
      }
      return Result;
    }
  }
)

poof(
  func generate_value_table(EnumType)
  {
    bonsai_function (EnumType.name)
    (EnumType.name.to_capital_case)(counted_string S)
    {
      (EnumType.name) Result = {};

      (
        EnumType.map_values(EnumValue)
        {
          if (StringsMatch(S, CSz("(EnumValue.name)"))) { return (EnumValue.name); }
        }
      )

      return Result;
    }
  }
)

poof(
  func generate_stream_chunk_struct(Type)
  {
    struct (Type.name)_stream_chunk
    {
      (Type.name) Element;
      (Type.name)_stream_chunk* Next;
    };
  }
)

poof(
  func generate_stream_push(Type)
  {
    bonsai_function (Type.name) *
    Push((Type.name)_stream* Stream, (Type.name) Element, memory_arena* Memory)
    {
      (Type.name)_stream_chunk* NextChunk = ((Type.name)_stream_chunk*)PushStruct(Memory, sizeof( (Type.name)_stream_chunk ), 1, 0);
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

      (Type.name) *Result = &NextChunk->Element;
      return Result;
    }

    bonsai_function void
    ConcatStreams( (Type.name)_stream *S1, (Type.name)_stream *S2)
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
  }
)

poof(
  func generate_stream_struct(Type)
  {
    struct (Type.name)_stream
    {
      (Type.name)_stream_chunk* FirstChunk;
      (Type.name)_stream_chunk* LastChunk;
    };

  }
)

poof(
  func generate_stream_iterator(Type)
  {
    struct (Type.name)_iterator
    {
      (Type.name)_stream* Stream;
      (Type.name)_stream_chunk* At;
    };

    bonsai_function (Type.name)_iterator
    Iterator((Type.name)_stream* Stream)
    {
      (Type.name)_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid((Type.name)_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance((Type.name)_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

    bonsai_function b32
    IsLastElement((Type.name)_iterator* Iter)
    {
      b32 Result = Iter->At->Next == 0;
      return Result;
    }

  }
)

poof(
  func generate_stream_getters(InputTypeDef)
  {
    InputTypeDef.map_members (Member) {
      bonsai_function InputTypeDef.type
      GetBy(Member.name)( (Member.Type) Needle, (InputTypeDef.type)_stream *Haystack)
      {
        // TODO : Implement matching!
      }
    }
  }
)

poof(
  func generate_stream(Type)
  {
    (generate_stream_chunk_struct(Type))
    (generate_stream_struct(Type))
    (generate_stream_iterator(Type))
    (generate_stream_push(Type))
  }
)

poof(
  func string_and_value_tables(Def)
  {
    ( generate_string_table(Def) )
    ( generate_value_table(Def) )
  }
)

poof(
  func stream_and_cursor(Def)
  {
    ( generate_cursor(Def) )
    ( generate_stream(Def) )
  }
)

poof(
  polymorphic_func void DebugPrint( @TypeDef RuntimeValue, u32 Depth)
  {
    DebugPrint("(TypeDef.name): ", Depth);
    TypeDef.is_enum?
    {
      TypeDef.map_values (ValueDef)
      {
        DebugPrint("(ValueDef.name) (ValueDef.value)", Depth+1);
        DebugPrint(ToString(RuntimeValue), Depth+1);
      }
    }

    TypeDef.is_struct?
    {
      TypeDef.map_members (MemberDef)
      {
        DebugPrint("(MemberDef.type) (MemberDef.name): ", Depth);
        DebugPrint(RuntimeValue.(MemberDef.name), Depth+1);
        DebugPrint("\n");
      }
    }
  }
)

