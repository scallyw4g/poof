    struct datatype_linked_list_node
    {
      datatype Element;
      datatype_linked_list_node *Next;
    };

    struct datatype_hashtable
    {
      umm Size;
      datatype_linked_list_node **Elements;
    };
     bonsai_function datatype_linked_list_node *
    Allocate_datatype_linked_list_node(memory_arena *Memory)
    {
      datatype_linked_list_node *Result = Allocate( datatype_linked_list_node, Memory, 1);
      return Result;
    }

    bonsai_function datatype_hashtable
    Allocate_datatype_hashtable(umm ElementCount, memory_arena *Memory)
    {
      datatype_hashtable Result = {};
      Result.Elements = Allocate( datatype_linked_list_node*, Memory, ElementCount);
      Result.Size = ElementCount;
      return Result;
    }

    bonsai_function datatype_linked_list_node *
    GetHashBucket(umm HashValue, datatype_hashtable *Table)
    {
      Assert(Table->Size);
      datatype_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
      return Result;
    }

    bonsai_function datatype *
    GetFirstAtBucket(umm HashValue, datatype_hashtable *Table)
    {
      datatype_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
      datatype *Result = &Bucket->Element;
      return Result;
    }

    bonsai_function void
    Insert(datatype_linked_list_node *E, datatype_hashtable *Table)
    {
      Assert(Table->Size);
      umm HashValue = Hash(&E->Element) % Table->Size;
      datatype_linked_list_node **Bucket = Table->Elements + HashValue;
      while (*Bucket) Bucket = &(*Bucket)->Next;
      *Bucket = E;
    }

    bonsai_function void
    Insert(datatype E, datatype_hashtable *Table, memory_arena *Memory)
    {
      datatype_linked_list_node *Bucket = Allocate_datatype_linked_list_node(Memory);
      Bucket->Element = E;
      Insert(Bucket, Table);
    }

