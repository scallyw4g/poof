    struct datatype_cursor
    {
      datatype* Start;
      datatype* At;
      datatype* End;
    };

    bonsai_function datatype_cursor
    DatatypeCursor(umm ElementCount, memory_arena* Memory)
    {
      datatype* Start = (datatype*)PushStruct(Memory, sizeof( datatype ), 1, 0);
      datatype_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

