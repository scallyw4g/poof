    struct parser_cursor
    {
      parser *Start;
      parser *At;
      parser *End;
    };

    link_internal parser_cursor
    ParserCursor(umm ElementCount, memory_arena* Memory)
    {
      parser *Start = (parser*)PushStruct(Memory, sizeof(parser), 1, 0);
      parser_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

