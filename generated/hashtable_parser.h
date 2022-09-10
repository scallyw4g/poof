    struct parser_linked_list_node
    {
      parser Element;
      parser_linked_list_node *Next;
    };

    struct parser_hashtable
    {
      umm Size;
      parser_linked_list_node **Elements;
    };
     bonsai_function parser_linked_list_node *
    Allocate_parser_linked_list_node(memory_arena *Memory)
    {
      parser_linked_list_node *Result = Allocate( parser_linked_list_node, Memory, 1);
      return Result;
    }

    bonsai_function parser_hashtable
    Allocate_parser_hashtable(umm ElementCount, memory_arena *Memory)
    {
      parser_hashtable Result = {};
      Result.Elements = Allocate( parser_linked_list_node*, Memory, ElementCount);
      Result.Size = ElementCount;
      return Result;
    }

    bonsai_function parser_linked_list_node *
    GetHashBucket(umm HashValue, parser_hashtable *Table)
    {
      Assert(Table->Size);
      parser_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
      return Result;
    }

    bonsai_function parser *
    GetFirstAtBucket(umm HashValue, parser_hashtable *Table)
    {
      parser_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
      parser *Result = &Bucket->Element;
      return Result;
    }

    bonsai_function parser *
    Insert(parser_linked_list_node *Node, parser_hashtable *Table)
    {
      Assert(Table->Size);
      umm HashValue = Hash(&Node->Element) % Table->Size;
      parser_linked_list_node **Bucket = Table->Elements + HashValue;
      while (*Bucket) Bucket = &(*Bucket)->Next;
      *Bucket = Node;
      return &Bucket[0]->Element;
    }

    bonsai_function parser*
    Insert(parser Element, parser_hashtable *Table, memory_arena *Memory)
    {
      parser_linked_list_node *Bucket = Allocate_parser_linked_list_node(Memory);
      Bucket->Element = Element;
      Insert(Bucket, Table);
      return &Bucket->Element;
    }

