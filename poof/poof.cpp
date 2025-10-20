#if !BONSAI_EMCC
  #define PLATFORM_WINDOW_IMPLEMENTATIONS 1
  #define PLATFORM_GL_IMPLEMENTATIONS 1

  #define BONSAI_DEBUG_SYSTEM_LOADER_API 1
  #define BONSAI_DEBUG_SYSTEM_API 1

  #define DEBUG_PRINT 0
#endif

#define STDLIB_SHADER_PATH "include/bonsai_stdlib/shaders/"

#include <bonsai_stdlib/bonsai_stdlib.h>
#include <bonsai_stdlib/bonsai_stdlib.cpp>


#include <bonsai_stdlib/src/debug_print.h>

struct type_spec;
link_internal void DebugPrint(type_spec *TypeSpec, u32 Depth = 0);

#include <poof/poof.h>
#include <poof/print_ast_node.h>




/* #if DEBUG_PRINT */
/* #include <bonsai_stdlib/headers/debug_print.h> */

/* #else */
/* _Pragma("clang diagnostic push") */
/* _Pragma("clang diagnostic ignored \"-Wunused-macros\"") */

/* #define DebugPrint(...)                                   \ */
/*   _Pragma("clang diagnostic push")                        \ */
/*   _Pragma("clang diagnostic ignored \"-Wunused-value\"")  \ */
/*   (__VA_ARGS__)                                           \ */
/*   _Pragma("clang diagnostic pop") */

/* _Pragma("clang diagnostic pop") // unused-macros */
/* #endif */



//
// Error Reporting
//

link_internal void PoofTypeError(parser* Parser, parse_error_code ErrorCode, counted_string ErrorMessage, c_token* ErrorToken);


//
// Preprocessor
//

link_internal b32         TryTransmuteOperatorToken(c_token *T);
link_internal b32         TryTransmuteNumericToken(c_token *T);
link_internal b32         TryTransmuteIdentifierToken(c_token *T);
link_internal macro_def * TryTransmuteIdentifierToMacro(parse_context *Ctx, parser *Parser, c_token *T, macro_def *ExpandingMacro);
link_internal macro_def * IdentifierShouldBeExpanded(parse_context *Ctx, parser *Parser, c_token *T, macro_def *ExpandingMacro);

link_internal c_token_cursor * ResolveInclude(parse_context *Ctx, parser *Parser, c_token *T);
link_internal parser * ExpandMacro(parse_context *Ctx, parser *Parser, macro_def *Macro, memory_arena *PermMemory, memory_arena *TempMemory, b32 ScanArgsForAdditionalMacros = False, b32 WasCalledFromExpandMacroConstantExpression = False);
link_internal s64      ResolveMacroConstantExpression(parse_context *Ctx, parser *Parser, memory_arena *PermMemory, memory_arena *TempMemory, s64 PreviousValue, b32 LogicalNotNextValue);

link_internal macro_def * GetMacroDef(parse_context *Ctx, counted_string DefineValue) ;
link_internal c_token *   EatIfBlock(parser *Parser, erase_token_mode Erased);
link_internal c_token *   EraseAllRemainingIfBlocks(parser *Parser);


//
// C Parser
//

link_internal compound_decl ParseStructBody(parse_context *, c_token *, poof_tag_block_array *);
link_internal declaration   ParseStructMember(parse_context *Ctx, c_token *StructNameT);

link_internal ast_node_expression* ParseExpression(parse_context *Ctx);
link_internal void                 ParseExpression(parse_context *Ctx, ast_node_expression *Result);
link_internal void                 ParseExpression(parse_context *Ctx, ast_node** Result);

link_internal parser MaybeParseFunctionBody(parser *Parser, memory_arena *Memory);
link_internal void MaybeParseStaticBuffers(parse_context *Ctx, parser *Parser, ast_node **Dest);
link_internal declaration FinalizeDeclaration(parse_context *Ctx, parser *Parser, type_spec *TypeSpec);


//
// C type manipulation & resolution
//

link_internal datatype * GetDatatypeByName(datatype_hashtable *, cs);
link_internal datatype * GetDatatypeByName(program_datatypes *, cs);

link_internal counted_string PrintTypeSpec(type_spec *TypeSpec, memory_arena *Memory);
link_internal counted_string ToString(type_spec *TypeSpec, memory_arena *Memory);

link_internal counted_string GetTypeNameFor(parse_context*, declaration*, memory_arena *);
link_internal counted_string GetTypeNameFor(parse_context*,    datatype*, typedef_resolution_behavior, memory_arena *);

link_internal counted_string GetNameForDecl(declaration* Decl);
link_internal counted_string GetNameForDatatype(datatype *Data, memory_arena *Memory);
link_internal counted_string GetTypeTypeForDatatype(datatype *Data, memory_arena *);
link_internal type_indirection_info * GetIndirectionInfoForDatatype(program_datatypes*, datatype*);

link_internal datatype* ResolveToBaseType(program_datatypes *, type_spec );
link_internal datatype* ResolveToBaseType(program_datatypes *, datatype *);
link_internal datatype* ResolveToBaseType(program_datatypes *, type_def *);


link_internal datatype* ResolveToBaseType(parse_context *Ctx, type_spec );
link_internal datatype* ResolveToBaseType(parse_context *Ctx, datatype *);
link_internal datatype* ResolveToBaseType(parse_context *Ctx, type_def *);


//
// Poof
//

// TODO(Jesse): Maybe formalize this; make distinct from calling `.map` on a
// symbol (which is untyped) and guarantee the types are defined ..?
link_internal counted_string_stream ParseDatatypeList(parse_context *Ctx, parser* Parser, program_datatypes* Datatypes, tagged_counted_string_stream_stream* NameLists, memory_arena* Memory);

link_internal b32       ParseAndTypeCheckArgs(parse_context *Ctx, parser *Parser, c_token *FunctionT, meta_func *Func, meta_func_arg_buffer *ArgInstances, meta_func_arg_buffer *ArgsInScope, memory_arena *Memory);
link_internal meta_func ParseMetaFunctionDef(parser* Parser, counted_string FuncName, memory_arena *Memory);
link_internal meta_func ParseMapMetaFunctionInstance(parser *, cs, memory_arena *);

link_internal counted_string Execute(meta_func* Func, parse_context* Ctx, memory_arena* Memory, umm *Depth);
link_internal counted_string Execute(meta_func* Func, meta_func_arg_buffer *Args, parse_context* Ctx, memory_arena* Memory, umm *Depth);
link_internal void           DoTrueFalse( parse_context *Ctx, parser *Scope, meta_func_arg_buffer *ReplacePatterns, b32 DoTrueBranch, string_builder *OutputBuilder, memory_arena *Memory, umm *Depth);

link_internal counted_string CallFunction(parse_context *Ctx, c_token *FunctionT, meta_func *Func, meta_func_arg_buffer *ArgInstances, memory_arena *Memory, umm *Depth);



link_internal b32 TryParsePoofKeywordAndTagList(parser *, poof_tag_block_array *);
link_internal b32 TryParsePoofTag(parser *, poof_tag *);



/****************************                   ******************************/
/****************************  Error Reporting  ******************************/
/****************************                   ******************************/


link_internal void
PoofNotImplementedError(parser* Parser, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ParseErrorCode_NotImplemented, CSz("Poof Not-Implemented Error"), ErrorMessage, ErrorToken);
}

link_internal void
PoofUserlandError(parser* Parser, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ParseErrorCode_PoofUserlandError, CSz("Poof Userland Error"), ErrorMessage, ErrorToken);
}

link_internal void
PoofError(parser* Parser, parse_error_code ErrorCode, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ErrorCode, CSz("Poof Error"), ErrorMessage, ErrorToken);
}

link_internal void
PoofSyntaxError(parser* Parser, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ParseErrorCode_SyntaxError, CSz("Poof Syntax Error"), ErrorMessage, ErrorToken);
}

link_internal void
PoofTypeError(parser* Parser, parse_error_code ErrorCode, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ErrorCode, CSz("Poof Type Error"), ErrorMessage, ErrorToken);
}

link_internal void
PoofTagError(parser* Parser, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ParseErrorCode_InvalidTag, CSz("Poof Tag Error"), ErrorMessage, ErrorToken);
}


link_internal void
InternalCompilerError(parser* Parser, counted_string ErrorMessage, c_token* ErrorToken)
{
  OutputContextMessage(Parser, ParseErrorCode_InternalCompilerError, CSz("XX INTERNAL COMPILER ERROR XX"), ErrorMessage, ErrorToken);
  RuntimeBreak();
}



/****************************                   ******************************/
/****************************  Macro Expansion  ******************************/
/****************************                   ******************************/



link_internal void
ParseMacroArgument(parser* Parser, c_token_buffer *Result)
{
  Result->Start = Parser->Tokens->At;

  u32 Depth = 0;
  b32 Done = False;
  while (c_token *T = PeekTokenRawPointer(Parser) )
  {
    if (Done) break;

    if (Depth == 0 && T->Type == CTokenType_Comma)
    {
      Done = True;
    }
    else if (Depth > 0 && T->Type == CTokenType_CloseParen)
    {
      --Depth;
    }
    else if (T->Type == CTokenType_OpenParen)
    {
      ++Depth;
    }

    if (!Done) PopTokenRaw(Parser);
  }

  Result->Count = (umm)(Parser->Tokens->At - Result->Start);

  OptionalToken(Parser, CTokenType_Comma);

  return;
}

link_internal void
CopyRemainingIntoCursor(c_token_cursor *Src, c_token_cursor *Dest)
{
  TIMED_FUNCTION();

  u32 SrcElements = (u32)Remaining(Src);
  u32 DestElements = (u32)Remaining(Dest);

  /* DebugLine(FormatCountedString(GetTranArena(), CSz("copy %lu -> %lu"), SrcElements, DestElements)); */
  /* DumpSingle(Src, Src->Start); */
  /* DumpSingle(Dest, Dest->Start); */

  if ( SrcElements <= DestElements )
  {
    for ( u32 TokenIndex = 0;
          TokenIndex < SrcElements;
          ++TokenIndex)
    {
      Push(Src->At[TokenIndex], Dest);
    }
  }
  else
  {
    Error("Dest c_token_cursor was too full to hold entire source buffer!" );
  }
}

// TODO(Jesse, metaprogramming): Almost an exact duplicate of CopyBufferIntoCursor
// @duplicate_copy_x_into_cursor
link_internal void
CopyCursorIntoCursor(c_token_cursor *Src, c_token_cursor *Dest, u32 Skip = 0)
{
  TIMED_FUNCTION();

  u32 SrcCount = (u32)Count(Src);
  u32 DestRem = (u32)Remaining(Dest);
  if ( SrcCount <= DestRem )
  {
    for ( u32 TokenIndex = Skip;
          TokenIndex < SrcCount;
          ++TokenIndex)
    {
      Push(Src->Start[TokenIndex], Dest);
    }
  }
  else
  {
    Error("Dest c_token_cursor was too full to hold entire source buffer!" );
  }
}

// @duplicate_copy_x_into_cursor
link_internal void
CopyBufferIntoCursor(c_token_buffer *Src, c_token_cursor *Dest, u32 Skip = 0)
{
  if ( Src->Count <= Remaining(Dest) )
  {
    for ( u32 TokenIndex = Skip;
          TokenIndex < Src->Count;
          ++TokenIndex)
    {
      Push(Src->Start[TokenIndex], Dest);
    }
  }
  else
  {
    Error("Dest c_token_cursor was too full to hold entire source buffer!" );
  }
}

link_internal c_token_buffer *
TryMacroArgSubstitution(c_token *T, counted_string_buffer *NamedArguments, c_token_buffer_buffer *ArgInstanceValues)
{
  c_token_buffer *Result = {};

  u32 ArgIndex = (u32)IndexOf(NamedArguments, T->Value);
  if (ArgIndex < NamedArguments->Count && ArgIndex < ArgInstanceValues->Count)
  {
    Result = ArgInstanceValues->Start + ArgIndex;
  }

  return Result;
}

link_internal void
CopyVaArgsInto(c_token_buffer_stream *VarArgs, c_token_cursor *Dest, b32 DoCommas)
{
  ITERATE_OVER(VarArgs)
  {
    c_token_buffer* Arg = GET_ELEMENT(Iter);
    CopyBufferIntoCursor(Arg, Dest);

    if (DoCommas)
    {
      if (IsLastElement(&Iter) == False)
      {
        Push(CToken(CTokenType_Comma, CSz(",")), Dest);
      }
    }
  }
}

// TODO(Jesse, tags: immediate, memory):  The memory allocation in this function
// is super bad.  What we need is Begin/End TemporaryMemory such that we can
// allocate a bunch of temp shit on the TempMemory arena and roll it all back
// at the end of the function.  The only thing that goes on the PermMemory is
// the Result.
link_internal parser *
ExpandMacro( parse_context *Ctx,
             parser *Parser,
             macro_def *Macro,
             memory_arena *PermMemory,

             // NOTE(Jesse)(expand_macro_temp_mem): Doing Begin/End temp memory here is actually quite
             // slow compared to doing a new arena (yikes!).  This is because
             // we clear the memory on the main thread at the time that we do
             // the end temp memory, whereas Windows puts dirty pages on a
             // queue to be cleared on another thread.  Not totally sure how
             // to fix this and preserve the constraint that the memory we get
             // from Allocate is always cleared..
             memory_arena *TempMemory_,

             b32 ScanArgsForAdditionalMacros,
             b32 WasCalledFromExpandMacroConstantExpression
           )
{
  TIMED_FUNCTION();

  memory_arena *TempMemory = AllocateArena(Megabytes(5), False, False);

  // @expand_macro_temp_mem
  /* temp_memory_handle MemHandle = BeginTemporaryMemory(TempMemory); */

  parser *FirstPass = AllocateParserPtr(
                        CSz("IF YOU SEE THIS IT'S A BUG"),
                        0,
                        (u32)Kilobytes(32),
                        TokenCursorSource_MacroExpansion,
                        0,
                        {0, 0},
                        TempMemory
                      );

  Assert(Macro->Body.At == Macro->Body.Start);
  Assert(Macro->Body.Up.Tokens == 0);

  EatWhitespaceAndComments(Parser);

  // NOTE(Jesse): Because of how the main preprocessor loop works (the part
  // that does #if #else evaluation) this routine must be able to require the
  // macro name token without calling RequireTokenRaw().
  //
  // TODO(Jesse, correctness): Is this a bug, or is the above comment wrong?
  //
  c_token MacroNameT = RequireTokenRaw(Parser, CToken(CT_MacroLiteral, Macro->NameT->Value));

  // NOTE(Jesse): These get filled out in the case where we've got a macro
  // function to expand.  The expansion loop works for both function and
  // keyword style macros if they're left empty
  //
  c_token_buffer_stream VarArgs = CTokenBufferStream(&Global_PermMemory);

  c_token_buffer_buffer ArgInstanceValues = {};

  TIMED_BLOCK("Parse Instance Args");
  switch (Macro->Type)
  {
    case type_macro_keyword:
    {
      // Keyword macros don't need any special treatment
    } break;

    case type_macro_function:
    {
      // Parse instance args

      c_token *Start = PeekTokenPointer(Parser);
      if (Start && Start->Type == CTokenType_OpenParen)
      {
        // TODO(Jesse)(safety, immediate): This is super janky.  We can only snap a pointer to this and access it while it's on the stack..
        c_token_cursor Tokens = CTokenCursor(Start, Start, CSz(DEFAULT_FILE_IDENTIFIER), TokenCursorSource_IntermediateRepresentaton, {0,0} );
        parser InstanceArgs_ = { {}, {}, .Tokens = &Tokens };
        parser *InstanceArgs = &InstanceArgs_;

        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
        InstanceArgs->Tokens->End = Parser->Tokens->At;

        TrimFirstToken(InstanceArgs, CTokenType_OpenParen);
        TrimLastToken(InstanceArgs, CTokenType_CloseParen);

        Assert(Ctx);

        if (ScanArgsForAdditionalMacros)
        {
          // TODO(Jesse): Can call PopTokenRawPointer() instead?
          // @optimize_call_advance_instead_of_being_dumb
          while (c_token *T = PeekTokenRawPointer(InstanceArgs))
          {
            if (T->Type == CTokenType_Identifier)
            {
              TryTransmuteIdentifierToMacro(Ctx, InstanceArgs, T, Macro);
            }
            PopTokenRaw(InstanceArgs);
          }
        }
        FullRewind(InstanceArgs);

        if ( TotalElements(InstanceArgs->Tokens) == 1 &&
              Macro->NamedArguments.Count == 0 )
        {
          OptionalToken(InstanceArgs, CToken(CTokenType_Void, CSz("void")));
        }

        //
        // Parse the instance args into their buffer
        //

        if (Macro->NamedArguments.Count)
        {
          ArgInstanceValues = CTokenBufferBuffer(Macro->NamedArguments.Count, TempMemory);
          for ( u32 ArgIndex = 0;
                ArgIndex < ArgInstanceValues.Count;
                ++ArgIndex)
          {
            ParseMacroArgument(InstanceArgs, ArgInstanceValues.Start+ArgIndex);
          }
        }

        if (Macro->Variadic)
        {
          while (TokensRemain(InstanceArgs))
          {
            if (VarArgs.Memory == 0)
            {
            }
                
            c_token_buffer *Arg = Push(&VarArgs, {});
            ParseMacroArgument(InstanceArgs, Arg);
          }
        }
        if (TokensRemain(InstanceArgs) != 0)
        {
          ParseError(Parser, CSz("Invalid number of arguments to macro"), Start);
        }
      }

    } break;

    InvalidDefaultWhileParsing(Parser, CSz("Invalid Macro Type detected"));
  }
  END_BLOCK();

  /* c_token *ResetToken = PeekTokenRawPointer(Parser); */



  TIMED_BLOCK("Parameter Sub && Pasting");

  parser *MacroBody = DuplicateCTokenCursor2(&Macro->Body, TempMemory);

  //
  // Pre-scan for any macros such that we can mark __VA_ARGS__ to expand with
  // comma separators.  This is to support @va_args_paste_into_another_macro
  //

  while (c_token *T = PeekTokenRawPointer(MacroBody))
  {
    Assert(T->Type != CT_MacroLiteral);

    // NOTE(Jesse): Can't do this because IdentifierShouldBeExpanded needs to
    // be able to peek the next token.  I suppose we could make it just blindly
    // trust we're on a macro, but I'd much rather have assertions in that
    // function.  Maybe this is indicative that that function is weird and we
    // should delete it?
    //
    // RequireTokenRaw(MacroBody, T);


    if ( T->Type == CTokenType_Identifier )
    {
      macro_def *ThisMacro = IdentifierShouldBeExpanded(Ctx, MacroBody, T, Macro);
      if (ThisMacro && ThisMacro->Type == type_macro_function)
      {
        RequireTokenRaw(MacroBody, T);

        u32 Depth = 0;

        for (;;)
        {
          T = PopTokenRawPointer(MacroBody);

          if (T->Type == CT_Preprocessor_VA_ARGS_)
          {
            Assert(T->Flags == 0);
            T->Flags |= va_args_flags_expand_with_commas;
          }

          if (T->Type == CTokenType_OpenParen)
          {
            ++Depth;
          }

          if (T->Type == CTokenType_CloseParen)
          {
            // NOTE(Jesse): Decrementing first is kinda wonky, but we start on
            // the macro token and always hit an open paren
            Assert(Depth > 0);
            if (--Depth == 0)
            {
              break;
            }
          }

          continue;
        }

      }
      else
      {
        RequireTokenRaw(MacroBody, T);
      }
    }
    else
    {
      RequireTokenRaw(MacroBody, T);
    }
  }

  FullRewind(MacroBody);

  //
  // Do parameter substitution and pasting
  //

  // @optimize_call_advance_instead_of_being_dumb
  while (c_token *T = PeekTokenRawPointer(MacroBody))
  {
    if (MacroBody->ErrorCode) break;

    switch (T->Type)
    {
      case CT_Preprocessor_VA_ARGS_:
      {
        RequireToken(MacroBody, T->Type);
        CopyVaArgsInto(&VarArgs, FirstPass->Tokens, T->Flags & va_args_flags_expand_with_commas);
      } break;

      case CTokenType_Identifier:
      {
        RequireToken(MacroBody, T);

        if (c_token_buffer *ArgBuffer = TryMacroArgSubstitution(T, &Macro->NamedArguments, &ArgInstanceValues ))
        {
          c_token_cursor ArgTokens = CTokenCursor(ArgBuffer, CSz(DEFAULT_FILE_IDENTIFIER), TokenCursorSource_IntermediateRepresentaton, {0,0} );
          parser Arguments = MakeParser(&ArgTokens);
          while (c_token* ArgToken = PeekTokenRawPointer(&Arguments))
          {
            if (ArgToken->Type == CT_MacroLiteral ||
               (ArgToken->Type == CTokenType_Identifier && TryTransmuteIdentifierToMacro(Ctx, &Arguments, ArgToken, Macro)) )
            {
              parser *Expanded = ExpandMacro(Ctx, &Arguments, ArgToken->Macro.Def, PermMemory, TempMemory);
              if (Expanded->ErrorCode == ParseErrorCode_None)
              {
                // Copy expanded buffer into FirstPass, marking self-referring
                // keyword instances such that we don't expand them again.
                while (c_token *ExpandedT = PopTokenRawPointer(Expanded))
                {
                  // TODO(Jesse, tags: metaprogramming, correctness): Test this
                  // path actually does what I intended.  At the moment I
                  // observed self-refe'd macros not getting expanded due to
                  // MacroShouldBeExpanded()
                  //
                  // @mark_keyword_macros_self_referential
                  //
                  if (Macro->Type == type_macro_keyword && StringsMatch(ExpandedT->Value, Macro->NameT->Value))
                  {
                    ExpandedT->Type = CT_MacroLiteral_SelfRefExpansion;
                  }
                  Push(*ExpandedT, FirstPass->Tokens);
                }
              }
              else
              {
                ParseInfoMessage( FirstPass,
                                  FormatCountedString(GetTranArena(), CSz("While Expanding %S"), ArgToken->Value),
                                  ArgToken);
              }
            }
            else
            {
              // @token_control_pointers
              Ensure(PopTokenRawPointer(&Arguments) == ArgToken);
              Push(*ArgToken, FirstPass->Tokens);
            }

          }
        }
        else
        {
          Push(*T, FirstPass->Tokens);
        }
      } break;

      case CT_PreprocessorPaste:
      {
        c_token *Prev = PeekTokenPointer(FirstPass, -1);
        c_token *Next = PeekTokenPointer(MacroBody, 1);

        // TODO(Jesse): Once token control functions return pointers we can
        // rewrite this such that Next gets assigned here instead of in the
        // PeekTokenPointer call above
        //
        // @token_control_pointers
        RequireToken(MacroBody, CT_PreprocessorPaste);
        RequireToken(MacroBody, Next->Type);

        if (c_token_buffer *ArgBuffer = TryMacroArgSubstitution(Next, &Macro->NamedArguments, &ArgInstanceValues ))
        {
          c_token_cursor Tokens = CTokenCursor(ArgBuffer, CSz(DEFAULT_FILE_IDENTIFIER), TokenCursorSource_IntermediateRepresentaton, {0,0} );
          parser Temp = MakeParser(&Tokens);
          Next = PeekTokenPointer(&Temp);

          if (Next && Next->Type == CT_MacroLiteral)
          {
            // NOTE(Jesse, tags: begin_temporary_memory, memory_leak): We need BeginTemporaryMemory here
            parser *Expanded = ExpandMacro(Ctx, &Temp, Next->Macro.Def, PermMemory, TempMemory);
            if (Expanded->ErrorCode)
            {
              // TOOD(Jesse, tags: immediate): Write a test that exercises this
              // path.  Right now there's a bug blocking it
              ParseInfoMessage( &Temp,
                                FormatCountedString(GetTranArena(), CSz("0 While Expanding %S"), Next->Value),
                                Next);
            }
            else
            {
              Next = PopTokenPointer(Expanded);
              CopyRemainingIntoCursor(Expanded->Tokens, FirstPass->Tokens);
              /* DumpChain(FirstPass); */
            }
            // NOTE(Jesse, tags: end_temporary_memory begin_temporary_memory):
            // We would roll-back the temp-mem here
          }
          else
          {
            // If we ever hit this assertion (I think) it means that Next was
            // declared as a macro _after_ the currently expanding macro.  I
            // was not sure if this case would ever happen, but it might.
            //
            // TODO(Jesse, correctness, immediate): Prove/Deny this can ever be
            // hit, and the above statement is correct.
            if (Next && Next->Type == CTokenType_Identifier && TryTransmuteIdentifierToMacro(Ctx, &Temp, Next, Macro) )
            {
              Assert(False);
            }

            // @token_control_pointers
            Ensure(PopTokenPointer(&Temp) == Next);
            CopyRemainingIntoCursor(Temp.Tokens, FirstPass->Tokens);
          }

        }


        if (Prev && Next)
        {
          // This is a rediculous special case.
          //
          // @non_portable_va_args_paste for more information on wtf is going
          // on.  Somebody is definitely fired.
          if ( Prev->Type == CTokenType_Comma &&
               Next->Type == CT_Preprocessor_VA_ARGS_ &&
               VarArgs.FirstChunk == 0 )
          {
            Prev->Type = CT_Preprocessor_Nuked;
            Prev->Erased = True;
            Prev->Value.Start = 0;
            Prev->Value.Count = 0;
          }
          else if ( Next->Type == CT_Preprocessor_VA_ARGS_ )
          {
            // Don't paste __VA_ARGS__ onto stuff
            //
            // TODO(Jesse, correctness): What do real compilers do?  No idea.
            //
            CopyVaArgsInto(&VarArgs, FirstPass->Tokens, True);
          }
          else
          {
            Prev->Type = CT_PreprocessorPaste_InvalidToken;
            Prev->Value = Concat(Prev->Value, Next->Value, &Global_PermMemory);
            Prev->UnsignedValue = 0;

            if (TryTransmuteKeywordToken(Prev, 0))
            {
            }
            else if (TryTransmuteNumericToken(Prev))
            {
            }
            else if (TryTransmuteOperatorToken(Prev))
            {
            }
            else if (TryTransmuteIdentifierToken(Prev))
            {
            }
            else
            {
              umm CurrentSize = TotalSize(FirstPass->Tokens);
              TruncateToCurrentElements(FirstPass->Tokens);
              /* umm NewSize = TotalSize(&FirstPass->Tokens); */
              /* Reallocate((u8*)FirstPass->Tokens->Start, GetTranArena(), CurrentSize, NewSize); */

              ParseError( FirstPass,
                          ParseErrorCode_InvalidTokenGenerated,
                          FormatCountedString(GetTranArena(), CSz("Invalid token generated during paste (%S)"), Prev->Value),
                          Prev);
            }

          }
        }
        else if (Next) // No Prev pointer .. macro was called like : CONCAT(,only_passed_one_thing)
        {
          Push(*Next, FirstPass->Tokens);
        }
      } break;

      default:
      {
        PopTokenRaw(MacroBody);
        Push(*T, FirstPass->Tokens);
      } break;
    }

  }
  END_BLOCK();

  parser *Result = AllocateParserPtr(
                      FormatCountedString(PermMemory, CSz("macro_expansion(%S)"), Macro->NameT->Value),
                      0,
                      (u32)Kilobytes(32),
                      TokenCursorSource_MacroExpansion,
                      0,
                      {0,0},
                      /* PermMemory */
                      TempMemory
                    );


  { // NOTE(Jesse): Unnecessary to reallocate here .. blow whole allocation
    // away at the end of the function
    umm CurrentSize = TotalSize(FirstPass->Tokens);
    TruncateToCurrentElements(FirstPass->Tokens);
  }

  Rewind(FirstPass->Tokens);

  Macro->IsExpanding = True;

#if 0
    umm CurrentSize = TotalSize(Result->Tokens);
    TruncateToCurrentElements(Result->Tokens);
#else
  if (FirstPass->ErrorCode == ParseErrorCode_None)
  {
    TIMED_BLOCK("Expand Body");

    u32 IdentifiersToSkip = 0;
    while (c_token *T = PeekTokenRawPointer(FirstPass))
    {
      T->LineNumber = MacroNameT.LineNumber;
      T->Filename = MacroNameT.Filename;

      switch(T->Type)
      {
        case CTokenType_Identifier:
        {

          if (WasCalledFromExpandMacroConstantExpression)
          {
            if (StringsMatch(CSz("defined"), T->Value))
            {
              IdentifiersToSkip = 2;
            }
          }

          // TODO(Jesse): What is specified to happen if a named argument
          // has the same value as a defined macro?  Is this correct?

          if ( (IdentifiersToSkip == 0) && TryTransmuteIdentifierToMacro(Ctx, FirstPass, T, Macro))
          {
            parser *Expanded = ExpandMacro(Ctx, FirstPass, T->Macro.Def, PermMemory, TempMemory);
            if (Expanded->ErrorCode)
            {
              ParseInfoMessage( FirstPass,
                                FormatCountedString(GetTranArena(), CSz("While Expanding %S"), T->Value),
                                T);
            }
            else
            {
              CopyRemainingIntoCursor(Expanded->Tokens, Result->Tokens);
            }
          }
          else
          {
            if (IdentifiersToSkip != 0) {--IdentifiersToSkip;}
            /* if (!WasCalledFromExpandMacroConstantExpression) */
            {
              // @token_control_pointers
              Ensure(PopTokenRawPointer(FirstPass) == T);
              Push(*T, Result->Tokens);
            }
          }

        } break;

        case CT_MacroLiteral:
        {
          parser *Expanded = ExpandMacro(Ctx, FirstPass, T->Macro.Def, PermMemory, TempMemory);
          if (Expanded->ErrorCode)
          {
            ParseInfoMessage( FirstPass,
                              FormatCountedString(GetTranArena(), CSz("While Expanding %S"), T->Value),
                              T);
          }
          else
          {
            CopyRemainingIntoCursor(Expanded->Tokens, Result->Tokens);
          }
        } break;


        default:
        {
          // @token_control_pointers
          Ensure(PopTokenRawPointer(FirstPass) == T);
          Push(*T, Result->Tokens);
        } break;
      }

    }

    END_BLOCK();
  }
#endif

#if 1
  {
    TIMED_BLOCK("Finalize");
    parser *Temp = AllocateParserPtr( Result->Tokens->Filename,
                                      0,
                                      (u32)AtElements(Result->Tokens),
                                      Result->Tokens->Source,
                                      0,
                                      {},
                                      PermMemory);

    Assert(AtElements(Result->Tokens) == Count(Temp->Tokens));

    TruncateToCurrentElements(Result->Tokens);
    Rewind(Result->Tokens);

    Assert(Count(Result->Tokens) == Count(Temp->Tokens));
    Assert(AtElements(Result->Tokens) == 0);
    Assert(AtElements(Temp->Tokens) == 0);

    CopyCursorIntoCursor(Result->Tokens, Temp->Tokens);
    Result = Temp;
#else
    TruncateToCurrentElements(Result->Tokens);
#endif

    Rewind(Result->Tokens);

    /* LogDirect("\n\n -- %S\n", Macro->Name); */
    /* DumpChain(FirstPass); */
    /* DumpChain(Result); */
    /* LogDirect("\n"); */

    Result->ErrorCode = FirstPass->ErrorCode;
    Parser->ErrorCode = FirstPass->ErrorCode;

    Macro->IsExpanding = False;

    // @expand_macro_temp_mem
    /* TIMED_NAMED_BLOCK(END_TEMP_MEM); */
    /*   EndTemporaryMemory(&MemHandle); */
    /* END_BLOCK(); */

    VaporizeArena(TempMemory);
    END_BLOCK();
  }

  return Result;
}







link_internal counted_string*
GetByValue(counted_string_hashtable *Table, counted_string Value)
{
  counted_string *Result = {};

  counted_string_linked_list_node *Bucket = GetHashBucket(umm(Hash(&Value)), Table);
  while (Bucket)
  {
    counted_string *Element = &Bucket->Element;

    if (StringsMatch(Element, Value))
    {
      Result = Element;
      break;
    }
    else
    {
      Bucket = Bucket->Next;
    }
  }

  return Result;
}

link_internal parser*
Get(parser_hashtable *Table, counted_string Value)
{
  parser *Result = {};

  auto *Bucket = GetHashBucket(umm(Hash(&Value)), Table);
  while (Bucket)
  {
    parser *Element = &Bucket->Element;

    if (StringsMatch(Element->Tokens->Filename, Value))
    {
      Result = Element;
      break;
    }
    else
    {
      Bucket = Bucket->Next;
    }
  }

  return Result;
}

link_internal macro_def*
GetByName(macro_def_hashtable *Table, counted_string Name)
{
  macro_def *Result = {};

  macro_def_linked_list_node *Bucket = GetHashBucket(umm(Hash(&Name)), Table);
  while (Bucket)
  {
    macro_def *M = &Bucket->Element;

    if (StringsMatch(M->NameT->Value, Name))
    {
      Result = M;
      break;
    }
    else
    {
      Bucket = Bucket->Next;
    }
  }

  return Result;
}






link_internal void
DefineMacro(parse_context *Ctx, parser *Parser, macro_def *Macro)
{
  TIMED_FUNCTION();

  memory_arena *Memory = Ctx->Memory;

  RequireToken(Parser, CT_PreprocessorDefine);

  c_token_cursor TmpTokens = {};
  parser InstanceBody = MakeParser(&TmpTokens);

  InstanceBody.Tokens->Start = Parser->Tokens->At;
  InstanceBody.Tokens->At = Parser->Tokens->At;
  EatUntilExcluding(Parser, CTokenType_Newline);
  InstanceBody.Tokens->End = Parser->Tokens->At;

  // We have to copy tokens because ExpandMacro is written with the assumption
  // that the valid macro body tokens will not be erased and will not have
  // undergone macro expansion. That assumption is invalidated if we don't copy
  // tokens.
  //
  // TODO(Jesse): It actually seems to me that we shouldn't have to duplicate
  // because why would the preprocessor ever run on the actual macro definition?
  // I'd be more on-board with this statement in "ExpandMacro"
  //
  parser *InstanceParser = DuplicateParserTokens(&InstanceBody, Memory);

  RequireToken(InstanceParser, CToken(CT_MacroLiteral, Macro->NameT->Value));

  //
  // Classify as keyword or function
  //

  if (OptionalTokenRaw(InstanceParser, CTokenType_OpenParen))
  {
    Macro->Type = type_macro_function;

    if (OptionalToken(InstanceParser, CTokenType_CloseParen))
    {
      // No arguments
    }
    else
    {
      // Parse named arguments

      u32 ArgCount = 1 + CountTokensBeforeNext(InstanceParser, CTokenType_Comma, CTokenType_CloseParen);
      Macro->NamedArguments = CountedStringBuffer(ArgCount, Memory);

      for ( u32 ArgIndex = 0;
            ArgIndex < ArgCount-1;
            ++ArgIndex)
      {
        counted_string ArgName = RequireToken(InstanceParser, CTokenType_Identifier).Value;
        Macro->NamedArguments.Start[ArgIndex] = ArgName;
        RequireToken(InstanceParser, CTokenType_Comma);
      }

      if (PeekToken(InstanceParser).Type == CTokenType_Identifier)
      {
        counted_string ArgName = RequireToken(InstanceParser, CTokenType_Identifier).Value;
        Macro->NamedArguments.Start[Macro->NamedArguments.Count-1] = ArgName;
      }

      if (OptionalToken(InstanceParser, CTokenType_Ellipsis))
      {
        Macro->Variadic = True;
        Macro->NamedArguments.Count -= 1;
      }

      RequireToken(InstanceParser, CTokenType_CloseParen);
    }
  }
  else
  {
    Macro->Type = type_macro_keyword;
  }

  InstanceParser->Tokens->Start = InstanceParser->Tokens->At;

#if 0
  // Sanity check
  while (c_token *T = PeekTokenRawPointer(InstanceParser))
  {
    switch (T->Type)
    {

      case CT_MacroLiteral:
      case CTokenType_Newline: { InvalidCodePath(); } break;
      default: { PopTokenRaw(InstanceParser); } break;
    }
  }
  Assert(InstanceParser->Tokens->At == InstanceParser->Tokens->End);
#endif

  if (Macro->Body.Start && TokenCursorsMatch(&Macro->Body, InstanceParser->Tokens))
  {
    // Redefining a macro with the same body is valid and has no effect from
    // what I can tell.
    //
    // TODO(Jesse, memory, memory-leak): When we have streaming arena allocations rewind the arena here
    /* Info("Macro redef'd with matching body."); */
  }
  else
  {
    Macro->Body = *InstanceParser->Tokens;
  }

  Rewind(&Macro->Body);
}

link_internal void
SkipToEndOfCursor(c_token_cursor *At, c_token_cursor *ToSkip)
{
  InvalidCodePath();
#if 0
  if (ToSkip->Next)
  {
    SinglyLinkedListSwapInplace(At, ToSkip->Next);
  }
  else
  {
    ToSkip->At = ToSkip->End;
    SinglyLinkedListSwapInplace(At, ToSkip);
  }
#endif
}

link_internal b32
MacroShouldBeExpanded(parser *Parser, c_token *T, macro_def *ThisMacro, macro_def *ExpandingMacro)
{
  Assert(PeekTokenPointer(Parser) == T);
  Assert(T->Type == CTokenType_Identifier);
  Assert(StringsMatch(T->Value, ThisMacro->NameT->Value));

  b32 Result = False;

  if (ThisMacro->IsExpanding == False)
  {
    switch (ThisMacro->Type)
    {
      case type_macro_function:
      {
        if (PeekToken(Parser, 1).Type == CTokenType_OpenParen)
        {
          Result = True;
        }
      } break;

      case type_macro_keyword:
      {
        if (ExpandingMacro && StringsMatch(ExpandingMacro->NameT->Value, ThisMacro->NameT->Value))
        {
          // TODO(Jesse): This is the path that's preventing self-referential macros
          // from expanding, but we should take this out in favor of the code at
          // @mark_keyword_macros_self_referential.
          //
          /* Info("Prevented recursive macro expansion of (%S)",  ExpandingMacro->Name); */

          // NOTE(Jesse): Pretty sure this _SHOULD_ be a dead path after adding (b32 IsExpanding) back into macros
          // Unfortunately, that doesn't appear to be the case..
        }
        else
        {
          Result = True;
        }
      } break;

      InvalidDefaultCase;
    }
  }


  return Result;
}

link_internal macro_def *
IdentifierShouldBeExpanded(parse_context *Ctx, parser *Parser, c_token *T, macro_def *ExpandingMacro)
{
  Assert(T->Type == CTokenType_Identifier);

  macro_def *Result = False;

  if (macro_def *ThisMacro = GetMacroDef(Ctx, T->Value))
  {
    if (MacroShouldBeExpanded(Parser, T, ThisMacro, ExpandingMacro))
    {
      Result = ThisMacro;
    }
  }

  return Result;
}

link_internal macro_def *
TryTransmuteIdentifierToMacro(parse_context *Ctx, parser *Parser, c_token *T, macro_def *ExpandingMacro)
{
  Assert(T->Type == CTokenType_Identifier);

  macro_def *Result = IdentifierShouldBeExpanded(Ctx, Parser, T, ExpandingMacro);
  if (Result)
  {
    T->Type = CT_MacroLiteral;
    T->Macro.Def = Result;
    Assert(T->Macro.Expansion == 0);
  }

  return Result;
}

link_internal b32
TryTransmuteIdentifierToken(c_token *T)
{
  Assert(T->Type == CT_PreprocessorPaste_InvalidToken);

  b32 Result = True;

  for (u32 CharIndex = 0;
      Result && CharIndex < T->Value.Count;
      ++CharIndex)
  {
    char C = T->Value.Start[CharIndex];
    Result &= IsAlphaNumeric(C) || C == '_';
  }

  if (Result)
  {
    T->Type = CTokenType_Identifier;
  }

  return Result;
}

// TODO(Jesse, preprocessor, correctness): Implement me.
link_internal b32
TryTransmuteNumericToken(c_token *T)
{
  Assert(T->Type == CT_PreprocessorPaste_InvalidToken);

#if BUG_NUMERIC_PASTE
  NotImplemented;
#endif

  b32 Result = T->Type != CT_PreprocessorPaste_InvalidToken;
  return Result;
}

// TODO(Jesse, preprocessor, correctness): Implement me.
link_internal b32
TryTransmuteOperatorToken(c_token *T)
{
  TIMED_FUNCTION();

  Assert(T->Type == CT_PreprocessorPaste_InvalidToken);

  b32 Result = T->Type != CT_PreprocessorPaste_InvalidToken;
  return Result;
}

link_internal b32
RunPreprocessor(parse_context *Ctx, parser *Parser, parser *Parent, memory_arena *Memory)
{
  TIMED_FUNCTION();

  memory_arena *TempMemory = GetTranArena(); //AllocateArena();

  c_token *LastT = 0;
  while (TokensRemain(Parser))
  {
    // TODO(Jesse)(speed): The only thing in this loop that we decides we can't
    // use PopTokenPointer is that DefineMacro wants to require the #define
    // keyword .. surely we're clever enough to get around that and still be
    // safe at runtime.
    c_token *T = PeekTokenPointer(Parser);
    switch (T->Type)
    {
      case CTokenType_Poof:
      {
        RequireToken(Parser, T->Type);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      case CT_PreprocessorPragma:
      {
        RequireToken(Parser, T->Type);

        if (OptionalToken(Parser, CToken(CSz("once"))))
        {
          if (GetByValue(&Ctx->Datatypes.FilesParsed, T->Filename))
          {
            // TODO(Jesse, tags: memory_leak): Free all memory
            Parser->Tokens->At = Parser->Tokens->Start;
            Parser->Tokens->End = Parser->Tokens->Start;
            T = 0;
          }
          else
          {
            Insert(T->Filename, &Ctx->Datatypes.FilesParsed, Memory);
          }
        }

        if (T)
        {
          EatUntilExcluding(Parser, CTokenType_Newline);
          EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
        }

      } break;

      case CT_PreprocessorInclude:
      case CT_PreprocessorIncludeNext:
      {
        RequireToken(Parser, T->Type);
        c_token *InsertedCodeT = PopTokenPointer(Parser);
        Assert(InsertedCodeT->Type == CT_InsertedCode);
        Assert(InsertedCodeT->Down == 0);

        EraseToken(T);
        EraseToken(InsertedCodeT);

        /* InsertedCodeT->Down = Allocate(c_token_cursor, Memory, 1); */
        /* c_token_cursor *Inc = ResolveInclude(Ctx, Parser, T); */
        /* *InsertedCodeT->Down = *Inc; */

        InsertedCodeT->Down = ResolveInclude(Ctx, Parser, T);
        if (InsertedCodeT->Down)
        {
          /* InsertedCodeT->Down->Up.Up = Allocate(c_token_cursor, Memory, 1); */
          /* *InsertedCodeT->Down->Up.Up = *Parser->Tokens; */
          InsertedCodeT->Down->Up.Tokens = Parser->Tokens;
          InsertedCodeT->Down->Up.At = InsertedCodeT;
          Assert(InsertedCodeT->Down->Up.At->Type == CT_InsertedCode);
          Assert(InsertedCodeT->Down->At == InsertedCodeT->Down->Start);
        }

        /* EatSpacesTabsEscapedNewlinesAndComments(Parser); */

        /* RequireToken(Parser, T->Type); */

        /* EraseBetweenExcluding(Parser, T, Parser->Tokens->At); */

        // TODO(Jesse): Pretty sure this can be EatUntilExcluding(Reuslt, CTokenType_Newline)
        // although when I tried it it failed for some reason.  TBD.
        b32 Continue = True;
        while (Continue)
        {
          c_token *Peek = PeekTokenRawPointer(Parser);

          if (IsWhitespace(Peek) || IsComment(Peek))
          {
            RequireTokenRaw(Parser, Peek->Type); // @optimize_call_advance_instead_of_being_dumb

            if (Peek->Type == CTokenType_CommentSingleLine ||
                Peek->Type == CTokenType_Newline)
            {
              Continue = False;
            }
          }
          else
          {
            Continue = False;
          }
        }

      } break;

      case CTokenType_Identifier:
      {
        if (Ctx)
        {
          if (macro_def *Macro = TryTransmuteIdentifierToMacro(Ctx, Parser, T, 0))
          {
            AdvanceTo(Parser, T);
            parser *Expanded = ExpandMacro(Ctx, Parser, Macro, Memory, TempMemory, True);
            if (Expanded->ErrorCode)
            {
              ParseInfoMessage( Parser,
                                FormatCountedString(GetTranArena(), CSz("While Expanding %S"), T->Value),
                                T);
            }
            else
            {
              EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
              T->Macro.Expansion = Expanded->Tokens;
              T->Macro.Def = Macro;
              Expanded->Tokens->Up.Tokens = Parser->Tokens;
              Expanded->Tokens->Up.At = T;
              Assert(Parser->Tokens->Source != TokenCursorSource_IntermediateRepresentaton);
              Assert(Expanded->Tokens->Up.At->Type == CT_MacroLiteral);
            }
          }
          else
          {
            RequireToken(Parser, CToken(T->Value));
          }
        }
        else
        {
          RequireToken(Parser, CToken(T->Value));
        }
      } break;

      case CT_MacroLiteral:
      {
        InvalidCodePath();
      } break;

      case CT_PreprocessorDefine:
      {
        c_token *MacroNameT = PeekTokenPointer(Parser, 1);

        if (Ctx)
        {
          macro_def *Macro = GetByName(&Ctx->Datatypes.Macros, MacroNameT->Value);

          if (Macro)
          {
            if (!Macro->Undefed)
            {
#if 1
              parser *PrevParser = Get(&Ctx->ParserHashtable, Macro->NameT->Filename);
              // NOTE(Jesse): 
              if (PrevParser)
              {
                ParseInfoMessage( PrevParser,
                                  CSz("Previous definition here"),
                                  Macro->NameT);
              }
              else
              {
                ParseInfoMessage( Parser,
                                  CSz("Previous definition on CLI."),
                                  MacroNameT);
              }

              ParseWarn( Parser,
                         ParseWarnCode_MacroRedefined,
                         FormatCountedString(GetTranArena(), CSz("Macro (%S) already defined"), MacroNameT->Value),
                         MacroNameT);
#endif
            }
            else
            {
            }

            *Macro = {};
            Macro->NameT = MacroNameT;
          }
          else
          {
            macro_def_linked_list_node *MacroNode = AllocateProtection(macro_def_linked_list_node, Ctx->Memory, 1, False);
            Macro = &MacroNode->Element;

            Macro->NameT = MacroNameT;
            Insert(MacroNode, &Ctx->Datatypes.Macros);
          }

          MacroNameT->Type = CT_MacroLiteral;
          MacroNameT->Macro.Def = Macro;

          DefineMacro(Ctx, Parser, Macro);

          EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
          RequireTokenRaw(Parser, CTokenType_Newline);
        }

      } break;

      case CT_PreprocessorUndef:
      {
        RequireToken(Parser, T->Type);
        counted_string MacroName = PopToken(Parser).Value;
        if (Ctx)
        {
          macro_def *Macro1 = GetMacroDef(Ctx, MacroName);
          if (Macro1) { Macro1->Undefed = True; }
        }
        EatUntilExcluding(Parser, CTokenType_Newline);
        EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
        RequireTokenRaw(Parser, CTokenType_Newline);
      } break;

      case CT_PreprocessorIf:
      case CT_PreprocessorElif:
      {
        RequireToken(Parser, T->Type);
        if (ResolveMacroConstantExpression(Ctx, Parser, Memory, TempMemory, 0, False))
        {
          // @optimize_call_advance_instead_of_being_dumb
          c_token *RewindT = PeekTokenRawPointer(Parser);
          EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
          Assert(T->Erased);
          EatIfBlock(Parser, PreserveTokens);
          EraseAllRemainingIfBlocks(Parser);
          RewindTo(Parser, RewindT);
        }
        else
        {
          EraseBetweenExcluding(Parser, T, Parser->Tokens->At);
          EatIfBlock(Parser, EraseTokens);
        }
      } break;

      case CT_PreprocessorIfDefined:
      {
        RequireToken(Parser, T->Type);
        c_token *DefineValue = PopTokenPointer(Parser);

        EraseToken(T);
        EraseToken(DefineValue);

        if ( GetMacroDef(Ctx, DefineValue->Value) )
        {
          EatIfBlock(Parser, PreserveTokens);
          EraseAllRemainingIfBlocks(Parser);
          RewindTo(Parser, T);
        }
        else
        {
          EatIfBlock(Parser, EraseTokens);
        }
      } break;

      case CT_PreprocessorIfNotDefined:
      {
        RequireToken(Parser, T->Type);
        c_token *DefineValue = PopTokenPointer(Parser);

        EraseToken(T);
        EraseToken(DefineValue);

        if ( ! GetMacroDef(Ctx, DefineValue->Value) )
        {
          EatIfBlock(Parser, PreserveTokens);
          EraseAllRemainingIfBlocks(Parser);
          RewindTo(Parser, T);
        }
        else
        {
          EatIfBlock(Parser, EraseTokens);
        }
      } break;

      // If we hit this case it must be the last one in an #if #elif chain because when
      // we hit a true clause we eat the rest of the clauses.
      case CT_PreprocessorElse:
      case CT_PreprocessorEndif:
      {
        EraseToken(T);
      } break;

      /* case CTokenType_Extern: */
      /* { */
      /*   RequireToken(Parser, T->Type); */
      /*   RuntimeBreak(); */
      /* } break; */

      default:
      {
        RequireToken(Parser, T->Type);
      } break;
    }
  }

  Assert (Parser->Tokens->Up.Tokens == 0);
  Rewind(Parser->Tokens);

  /* Ensure(VaporizeArena(TempMemory)); */

  return (Parser->ErrorCode == ParseErrorCode_None);
}

link_internal parser *
PreprocessedParserForFile(parse_context *Ctx, counted_string Filename, token_cursor_source Source, parser *Parent)
{
  TIMED_FUNCTION();

  parser *Result = ParserForFile(Ctx, Filename, Source, Ctx->Memory);

  if (Result && Result->ErrorCode == ParseErrorCode_None)
  {
    Insert(*Result, &Ctx->ParserHashtable, Ctx->Memory);
    if (RunPreprocessor(Ctx, Result, Parent, Ctx->Memory))
    {
      /* c_token_cursor Tmp = *Result->Tokens; */
      /* Result->Tokens = Allocate(c_token_cursor, Ctx->Memory, 1); */
      /* *Result->Tokens = Tmp; */
      // All good
    }
    else
    {
      Warn("Error encountered while running preprocessor on file (%S)", Filename);
    }
  }

  return Result;
}

#if 0
// TODO(Jesse id: 302, tags: id_301)
link_internal parser*
GetByFilename(parser_stream* Stream, counted_string Filename)
{
  TIMED_FUNCTION();

  parser* Result = 0;
  ITERATE_OVER(Stream)
  {
    parser *Parser = GET_ELEMENT(Iter);
    if (StringsMatch(Parser->Filename, Filename))
    {
      Result = Parser;
      break;
    }
  }

  return Result;
}
#endif

link_internal counted_string
ResolveIncludePath(parse_context *Ctx, parser *Parser, c_token *T, counted_string PartialPath, b32 IsIncludeNext, b32 IsRelative)
{
  TIMED_FUNCTION();

  Assert(IsValidForCursor(Parser->Tokens, T));

  counted_string Result = {};

  if (PartialPath.Count == 0)
  {
    ParseError(Parser, CSz("Include path must be specified"), T);
    return Result;
  }

  if (IsRelative)
  {
    if (IsIncludeNext)
    {
      ParseError(Parser, CSz("Relative includes not supported with 'include_next'"), T);
      return Result;
    }

    c_token_cursor *Current = Parser->Tokens;
    while (Current)
    {
      Assert(Current->Filename.Count);
      PartialPath = StripQuotes(PartialPath);
      counted_string CurrentFilepath = Concat(Dirname(Current->Filename), PartialPath, GetTranArena());
      if (FileExists(CurrentFilepath))
      {
        Result = CurrentFilepath;
        break;
      }
      else
      {
        Current = Current->Up.Tokens;
      }
    }
  }

  if (Result.Count == 0)
  {
    counted_string_cursor *IncludePaths = Ctx->IncludePaths;
    if (IncludePaths)
    {
      Assert(Result.Start == 0);
      {
        for ( u32 PrefixIndex = 0;
              PrefixIndex < Count(IncludePaths);
              ++PrefixIndex )
        {
          counted_string PrefixPath = IncludePaths->Start[PrefixIndex];
          counted_string FullPath = Concat(PrefixPath, PartialPath, GetTranArena()); // TODO(Jesse id: 304): only do this work once

          if (FileExists(FullPath))
          {
            if (IsIncludeNext)
            {
              IsIncludeNext = False;
            }
            else
            {
              Result = FullPath;
              break;
            }
          }
        }

      }
    }

    if ( ! Result.Count )
    {
      if (FileExists(PartialPath))
      {
        if (IsIncludeNext)
        {
          IsIncludeNext = False;
        }
        else
        {
          Result = PartialPath;
        }
      }
    }
  }

  return Result;
}

link_internal c_token_cursor *
ResolveInclude(parse_context *Ctx, parser *Parser, c_token *T)
{
  TIMED_FUNCTION();

  if (const char *INCLUDE_ENV = PlatformGetEnvironmentVar("INCLUDE", GetTranArena()))
  {
    Warn("poof does not support the environemnt variable INCLUDE (%s)", INCLUDE_ENV);
  }

  Assert(IsValidForCursor(Parser->Tokens, T));

  c_token_cursor *Result = {};
  b32 IsIncludeNext = (T->Type == CT_PreprocessorIncludeNext);
  b32 IsRelative = T->Flags & CTFlags_RelativeInclude;
  counted_string FinalIncludePath = ResolveIncludePath(Ctx, Parser, T, T->IncludePath, IsIncludeNext, IsRelative);

  if (FinalIncludePath.Count)
  {
    Info("Including (%S)", FinalIncludePath);
    parser *IncludeParser = PreprocessedParserForFile(Ctx, FinalIncludePath, TokenCursorSource_Include, Parser);
    if (IncludeParser)
    {
      // TODO(Jesse)(memory_leak) This leaks the parser struct but it's such
      // small overhead I'm not going to worry about it right now.  It's easy
      // to fix but I'm tired and don't feel like it.
      // @leak_parser_struct
      Result = IncludeParser->Tokens;
    }
  }
  else
  {
    if (IsIncludeNext)
    {
      Warn("Unable to resolve include_next for file : (%S)", T->IncludePath);
    } 
    else
    {
      Warn("Unable to resolve include for file : (%S)", T->IncludePath);
    }

  }

  return Result;
}




//
// Looks like this pretty much delineates the parser from the actual implementation of poof
//




d_union_member
DUnionMember(counted_string Name, counted_string Type, d_union_flags Flags)
{
  d_union_member Result = {};
  Result.Name = Name;
  Result.Type = Type;
  Result.Flags = Flags;
  return Result;
}

void
PushMember(d_union_decl* dUnion, c_token MemberIdentifierToken, d_union_flags Flags, memory_arena* Memory)
{
  Assert(MemberIdentifierToken.Type == CTokenType_Identifier);
  d_union_member Member = DUnionMember(MemberIdentifierToken.Value, MemberIdentifierToken.Value, Flags);
  Push(&dUnion->Members, Member);
}

link_internal counted_string
GenerateEnumDef(d_union_decl* dUnion, memory_arena* Memory)
{
  TIMED_FUNCTION();

  counted_string Result = FormatCountedString(Memory, CSz("enum %S_type\n{\n  type_%S_noop,\n"), dUnion->Name, dUnion->Name);

  for (d_union_member_iterator Iter = Iterator(&dUnion->Members);
      IsValid(&Iter);
      Advance(&Iter))
  {
    d_union_member* Member = &Iter.At->Element;
    Result = Concat(Result, FormatCountedString(Memory, CSz("  type_%S,\n"), Member->Type), Memory);
  }

  Result = Concat(Result, CS("};\n\n"), Memory);
  return Result;
}

link_internal counted_string
GenerateStructDef(parse_context *Ctx, d_union_decl* dUnion, memory_arena* Memory)
{
  TIMED_FUNCTION();

  counted_string UnionName = dUnion->Name;
  counted_string TagType = dUnion->CustomEnumType.Count ?
    dUnion->CustomEnumType :
    FormatCountedString(Memory, CSz("%S_type"), UnionName);

  counted_string Result = FormatCountedString(Memory, CSz("struct %S\n{\n  enum %S Type;\n"),
      UnionName, TagType);

  ITERATE_OVER(&dUnion->CommonMembers.Members)
  {
    declaration* Member = GET_ELEMENT(Iter);
    Assert(Member->Type == type_variable_decl);
    Result = Concat( Result,
                      FormatCountedString( Memory,
                                           CSz("  %S %S;\n"),
                                           GetTypeNameFor(Ctx, Member, Memory),
                                           GetNameForDecl(Member)),
                      Memory);
  }

  b32 ValidMemberFound = False;
  for (d_union_member_iterator Iter = Iterator(&dUnion->Members);
      IsValid(&Iter);
      Advance(&Iter))
  {
    d_union_member* Member = &Iter.At->Element;
    if (Member->Flags != d_union_flag_enum_only)
    {
      if (!ValidMemberFound)
      {
        Result = Concat(Result, CS("\n  union\n  {\n"), Memory);
        ValidMemberFound = True;
      }
      Result = Concat(Result, FormatCountedString(Memory, CSz("    struct %S %S;\n"), Member->Type, Member->Name), Memory);
    }
  }
  if (ValidMemberFound)
  {
    Result = Concat(Result, CS("  };\n"), Memory);
  }

  Result = Concat(Result, CS("};\n\n"), Memory);

  return Result;
}

poof(
  func gen_stream_getter(type_datatype Type, type_poof_symbol Key)
  {
    link_internal Type.name*
    Get(Type.name.to_capital_case)By(Key)( (Type.name)_stream* Stream, Type.member(0, (M) { M.type }) Key )
    {
      (Type.name) *Result = 0;
      ITERATE_OVER(Stream)
      {
        Type.name* T = GET_ELEMENT(Iter);
        if (AreEqual(T->(Key), Key))
        {
          Result = T;
          break;
        }
      }

      return Result;
    }
  }
)

poof( gen_stream_getter(type_def, {Alias}) )
#include <generated/gen_stream_getter_type_def_822865913.h>



poof( gen_stream_getter(enum_decl, {NameT}) )
#include <generated/gen_stream_getter_enum_decl_689333910.h>


#if 0
// TODO(Jesse id: 301, tags: metaprogramming):  These functions are super repetitive, generate them!
link_internal enum_decl*
GetEnumDeclByName(enum_decl_stream* ProgramEnums, counted_string EnumType)
{
  TIMED_FUNCTION();

  enum_decl* Result = 0;
  for (auto Iter = Iterator(ProgramEnums);
      IsValid(&Iter);
      Advance(&Iter))
  {
    enum_decl* Enum = &Iter.At->Element;
    if (StringsMatch(Enum->Name, EnumType))
    {
      Result = Enum;
      break;
    }
  }

  return Result;
}
#endif

link_internal function_decl*
GetFunctionByName(function_decl_stream* Functions, counted_string FuncName)
{
  TIMED_FUNCTION();

  function_decl *Result = {};
  ITERATE_OVER(Functions)
  {
    auto *Func = GET_ELEMENT(Iter);
    if (Func->NameT)
    {
      /* Info("Comparing function (%S) against (%S)", Func->NameT->Value, FuncName); */
      if (StringsMatch(Func->NameT->Value, FuncName))
      {
        /* Info("Matched"); */
        Result = Func;
        break;
      }
    }
  }

  return Result;
}
link_internal compound_decl*
GetStructByType(compound_decl_stream* ProgramStructs, counted_string StructType)
{
  TIMED_FUNCTION();

  compound_decl* Result = 0;
  for (compound_decl_iterator Iter = Iterator(ProgramStructs);
      IsValid(&Iter);
      Advance(&Iter))
  {
    compound_decl* Struct = &Iter.At->Element;
    if (Struct->Type)
    {
      if (StringsMatch(Struct->Type->Value, StructType))
      {
        Result = Struct;
        break;
      }
    }
  }

  return Result;
}

#if 0
link_internal datatype
GetDatatypeByTypeSpec(program_datatypes *Datatypes, type_spec *Type)
{
  datatype_hashtable *DHash = &Datatypes->DatatypeHashtable;

  umm HashValue = Hash(Type);
  /* datatype */

}
#endif



link_internal umm
Hash(datatype *D)
{
  cs Name = GetNameForDatatype(D, GetTranArena());
  umm Result = umm(Hash(&Name));
  return Result;
}

global_variable datatype
NullDatatype = {};

link_internal datatype *
GetDatatypeByName( datatype_hashtable *Table, cs QueryName )
{
  datatype *Result = {};

  auto *Bucket = GetHashBucket(umm(Hash(&QueryName)), Table);
  while (Bucket)
  {
    auto E = &Bucket->Element;
    cs EName = GetNameForDatatype(E, GetTranArena());

    if (AreEqual(EName, QueryName))
    {
      Result = E;
      break;
    }
    else
    {
      Bucket = Bucket->Next;
    }
  }

  if (Result == 0) { Result = &NullDatatype; }
  return Result;
}

link_internal datatype *
GetDatatypeByName(program_datatypes* Datatypes, counted_string Name)
{
  datatype *Result = GetDatatypeByName(&Datatypes->DatatypeHashtable, Name);
  return Result;
}

// NOTE(Jesse): This function isn't quite an exact duplicate, but it's close
// enough that we could probably merge all three with a templating thing
// @duplicate_DatatypeIs_Decl
link_internal b32
DatatypeIsVariableDecl(datatype *Data)
{
  b32 Result = False;
  switch (Data->Type)
  {
    case type_datatype_noop:
    {
      // TODO(Jesse): ?
      Assert(false);
      /* InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT); */
    } break;

    case type_primitive_def:
    case type_enum_member:
    {
    } break;

    case type_type_def:
    {
      // NOTE(Jesse): This could be .. a variable .. questionmark?
      NotImplemented;
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      switch (Decl->Type)
      {
        case type_declaration_noop:
        case type_compound_decl:
        case type_enum_decl:
        {
        } break;

        case type_function_decl:
        {
          // NOTE(Jesse): This could be .. a variable .. questionmark?
          NotImplemented;
        } break;

        case type_variable_decl:
        {
          Result = True;
        } break;
      }
    }
  }
  return Result;
}

link_internal b32
DatatypeIsFunction(parse_context *Ctx, parser *Scope, datatype *Data, c_token *MetaOperatorT)
{
  b32 Result = False;
  switch (Data->Type)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_primitive_def:
    {
    } break;

    case type_type_def:
    {
      type_def *TDef = SafeAccess(type_def, Data);
      datatype *DT = ResolveToBaseType(Ctx, TDef->Type);
      Result = DatatypeIsFunction(Ctx, Scope, DT, MetaOperatorT);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);

      switch(Decl->Type)
      {
        case type_declaration_noop:
        {
          // TODO(Jesse): ?
          InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT);
        } break;

        case type_function_decl:
        {
          Result = True;
        } break;

        case type_enum_decl:
        case type_compound_decl:
        {
        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);
          Result = (VDecl->Type.Indirection.IsFunction || VDecl->Type.Indirection.IsFunctionPtr);
          /* datatype DT = ResolveToBaseType(Ctx, VDecl->Type); */
          /* Result = DatatypeIsFunction(Ctx, Scope, &DT, MetaOperatorT); */
        } break;
      }

    } break;
  }

  return Result;
}

// This is an _almost_ exact duplicate of DatatypeIsCompoundDecl.  The only difference
// is which switch statement we set the result in.  How do we fix?
//
// @duplicate_DatatypeIs_Decl
link_internal function_decl *
DatatypeIsFunctionDecl(parse_context *Ctx, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  function_decl *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_primitive_def:
    {
    } break;

    case type_type_def:
    {
      type_def *TDef = SafeAccess(type_def, Data);
      datatype *DT = ResolveToBaseType(Ctx, TDef->Type);
      Result = DatatypeIsFunctionDecl(Ctx, DT, Scope, MetaOperatorT);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);

      switch(Decl->Type)
      {
        case type_declaration_noop:
        {
          // TODO(Jesse): ?
          InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT);
        } break;

        case type_function_decl:
        {
          Result = SafeAccess(function_decl, Decl);
        } break;

        case type_enum_decl:
        case type_compound_decl:
        {
        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);
          datatype *DT = ResolveToBaseType(Ctx, VDecl->Type);
          Result = DatatypeIsFunctionDecl(Ctx, DT, Scope, MetaOperatorT);
        } break;
      }

    } break;
  }

  return Result;
}

// @duplicate_DatatypeIs_Decl
link_internal compound_decl *
DatatypeIsCompoundDecl(parse_context *Ctx, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  compound_decl *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_primitive_def:
    {
    } break;

    case type_type_def:
    {
      /* // NOTE(Jesse): Pretty sure this path is roughly the following, but I didn't test it. */
      /* NotImplemented; */

      type_def *TDef = SafeAccess(type_def, Data);
      datatype *DT = ResolveToBaseType(Ctx, TDef);
      Result = DatatypeIsCompoundDecl(Ctx, DT, Scope, MetaOperatorT);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);

      switch(Decl->Type)
      {
        case type_declaration_noop:
        {
          // TODO(Jesse): ?
          if (Scope) { InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT); }
        } break;

        case type_enum_decl:
        case type_function_decl:
        {
        } break;

        case type_compound_decl:
        {
          Result = SafeAccess(compound_decl, Decl);

        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);
          datatype *DT = ResolveToBaseType(Ctx, VDecl->Type);
          /* Assert(DatatypeIsVariableDecl(&DT) == False); */
          Result = DatatypeIsCompoundDecl(Ctx, DT, Scope, MetaOperatorT);
        } break;
      }

    } break;
  }

  return Result;
}

link_internal function_decl *
TryCastToFunctionDecl(parse_context *Ctx, datatype *Datatype)
{
  function_decl *Result = DatatypeIsFunctionDecl(Ctx, Datatype);
  return Result;
}

link_internal compound_decl *
TryCastToCompoundDecl(parse_context *Ctx, datatype *Datatype)
{
  compound_decl *Result = DatatypeIsCompoundDecl(Ctx, Datatype);
  return Result;
}

link_internal enum_decl *
TryCastToEnumDecl(parse_context *Ctx, datatype *Datatype)
{
  enum_decl *Result = {};

  tswitch (Datatype)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_primitive_def:
    case type_type_def:
    {
    } break;

    tmatch(declaration, Datatype, Decl);
    {

      tswitch(Decl)
      {
        case type_declaration_noop:
        case type_function_decl:
        case type_compound_decl:
        case type_variable_decl:
        {
        } break;

        tmatch(enum_decl, Decl, Enum);
        {
          Result = Enum;
        }
      }

    }

  }

  return Result;
}

link_internal enum_decl
GetEnumDeclByName( parse_context *Ctx, counted_string Name )
{
  program_datatypes *Data = &Ctx->Datatypes;
  enum_decl Result = {};

  datatype *Datatype = GetDatatypeByName(Data, Name);

  switch (Datatype->Type)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_primitive_def:
    {
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeCast(declaration, Datatype);
      switch (Decl->Type)
      {
        InvalidCase(type_declaration_noop);

        case type_function_decl:
        case type_compound_decl:
        case type_variable_decl:
        {
        } break;

        case type_enum_decl:
        {
          Result = *SafeCast(enum_decl, Decl);
        } break;
      }
    } break;

    case type_type_def:
    {
      datatype *Resolved = ResolveToBaseType(Data, Datatype);
      enum_decl *Enum = TryCastToEnumDecl(Ctx, Resolved);
      if (Enum) Result = *Enum;
    } break;
  }

  return Result;
}

d_union_decl
ParseDiscriminatedUnion(parse_context *Ctx, parser* Parser, program_datatypes* Datatypes, c_token *NameT, memory_arena* Memory)
{
  TIMED_FUNCTION();

  d_union_decl dUnion = {};

  dUnion.Name = NameT->Value;
  dUnion.Members.Memory = Memory;

  if (c_token *EnumTypeT = OptionalToken(Parser, CTokenType_Identifier))
  {
    dUnion.CustomEnumType = EnumTypeT->Value;

    enum_decl EnumDef = GetEnumDeclByName(Ctx, dUnion.CustomEnumType);
    if (EnumDef.NameT)
    {
      Assert(EnumDef.NameT->Value.Count);

      ITERATE_OVER(&EnumDef.Members)
      {
        enum_member *Field = GET_ELEMENT(Iter);
        counted_string MemberName = Concat(Concat(dUnion.Name, CS("_"), Memory), Field->NameT->Value, Memory);
        PushMember(&dUnion, CToken(MemberName), d_union_flag_none, Memory);
      }
    }
    else
    {
      PoofTypeError( Parser,
                     ParseErrorCode_UndefinedDatatype,
                     CSz("Couldn't find definition for custom enum type"),
                     EnumTypeT);
    }
  }

  RequireToken(Parser, CTokenType_OpenBrace);

  b32 Complete = False;
  while (c_token *Interior = PeekTokenPointer(Parser))
  {
    if (Complete) break;

    switch (Interior->Type)
    {
      case CTokenType_Identifier:
      {
        RequireToken(Parser, Interior);

        d_union_flags Flags = {};
        if ( OptionalToken(Parser, CToken(ToString(enum_only))) )
        {
          Flags = d_union_flag_enum_only;
        }

        OptionalToken(Parser, CTokenType_Comma);

        PushMember(&dUnion, *Interior, Flags, Memory);
      } break;

      case CTokenType_CloseBrace:
      {
        RequireToken(Parser, CTokenType_CloseBrace);
        Complete = True;
      } break;

      default:
      {
        PoofTypeError( Parser,
                       ParseErrorCode_DUnionParse,
                       CSz("Unexpected token encountered while parsing d_union"),
                       Interior);
        Complete = True;
      } break;
    }
  }

  if (OptionalToken(Parser, CTokenType_Comma))
  {
    dUnion.CommonMembers = ParseStructBody(Ctx, NameT, 0);
  }

  return dUnion;
}

link_internal b32
DoDebugWindow(const char** ArgStrings, u32 ArgCount)
{
  b32 Result = False;
  for (u32 ArgIndex = 1;
      ArgIndex < ArgCount;
      ++ArgIndex)
  {
    counted_string Arg = CS(ArgStrings[ArgIndex]);
    if (StringsMatch(CS("-d"), Arg) ||
        StringsMatch(CS("--do-debug-window"), Arg) )
    {
      Result = True;
    }
  }

  return Result;
}

link_internal counted_string
PopArgString(const char** ArgStrings, u32 ArgStringCount, u32* ArgIndex)
{
  counted_string Result = {};

  if (*ArgIndex+1 < ArgStringCount)
  {
    *ArgIndex = *ArgIndex + 1;
    Result = CS(ArgStrings[*ArgIndex]);
  }
  else
  {
    Assert(*ArgIndex == ArgStringCount-1);
    Error("Argument required for '%s'.", ArgStrings[*ArgIndex]);
  }

  return Result;
}

// TODO(Jesse): Barf
template <typename stream_t, typename element_t>inline stream_t
AllocateBuffer(u32 Count, memory_arena* Memory)
{
  element_t* Start = Allocate(element_t, Memory, Count);
  stream_t Result = {
    .Start = Start,
    .At = Start,
    .End = Start + Count,
  };

  return Result;
}

link_internal arguments
ParseArgs(const char** ArgStrings, u32 ArgCount, parse_context *Ctx, memory_arena* Memory)
{
  arguments Result = {
    .Outpath      = CSz("."),
    .Files        = AllocateBuffer<counted_string_cursor, counted_string>((u32)ArgCount, Memory),
    .IncludePaths = AllocateBuffer<counted_string_cursor, counted_string>((u32)ArgCount, Memory),
    False, False, False
  };

#if 0
  counted_string PrevArg = {};
  for ( u32 ArgIndex = 0;
        ArgIndex < ArgCount;
        ++ArgIndex)
  {
    counted_string Arg = CS(ArgStrings[ArgIndex]);
    b32 PrevArgWasInclude = StringsMatch(PrevArg, CSz("-I")) || StringsMatch(PrevArg, CSz("--include-path"));

    if (PrevArgWasInclude) { LogDirect("\""); }
    LogDirect("%S", Arg);
    if (PrevArgWasInclude) { LogDirect("\""); }
    LogDirect(" ");

    PrevArg = Arg;
  }
  LogDirect("\n");
#endif

  for ( u32 ArgIndex = 1;
            ArgIndex < ArgCount;
          ++ArgIndex)
  {
    counted_string Arg = CS(ArgStrings[ArgIndex]);

    if (StringsMatch(CSz("##"), Arg))
    {
      break;
    }
    else if (StringsMatch(CSz("-h"), Arg) ||
             StringsMatch(CSz("--help"), Arg) )
    {
      log_level PrevLL = Global_LogLevel;
      Global_LogLevel = LogLevel_Verbose;

      Result.HelpTextPrinted = True;
      PrintToStdout(CSz(
"\n"
" -- Overview: poof C metaprogramming compiler --\n"
"\n"
" `poof` is a metaprogramming environment for the C and C++ languages\n"
"\n"
" poof aims to be unintrusive, but powerful.  Users of the language write `poof` \n"
" code inline in their C or C++ source files.  The generated code is then output \n"
" to header files which are included inline immediately after the `poof` code.   \n"
"\n"
" `poof` parses a small, currently undocumented, subset of C++, which may stabilize \n"
" and become well-defined in the future.  For the moment it parses operator         \n"
" overloads, classes (w/ constructors, destructors, virtual functions..) and        \n"
" simple templates.\n"
"\n"
" Usage: poof [--options] <src_file>\n"
"\n"
" -- Options --\n"
"\n"
" -h       | --help               : display this help text\n"
"\n"
" -I <dir> | --include-path <dir> : add dir to the include search path\n"
"\n"
" -D name  | --define name        : analogous to #define name=1  Custom values (ie -D name=2) are, for the moment, unsupported\n"
"\n"
" -c0      | --colors-off         : disable color escape codes in console output\n"
"\n"
"          | --no-normalize-output-whitespace : disable normalizing whitespace.  Useful to emit whitespace-sensitive languages.\n"
"\n"
" --log-level <LogLevel_Value>    : One of "));

DumpValidLogLevelOptions();

PrintToStdout(CSz(
"\n"
"\n"
" -- Experimental Options --\n"
"\n"
" -d       | --do-debug-window    : open performance debug window.  Requires https://github.com/scallyw4g/bonsai_debug to be built.\n"
"                                   this is currently not well supported, or documented, and will likely crash `poof`\n"
"\n"));

      Global_LogLevel = PrevLL;
    }
    else if (StringsMatch(CSz("-d"), Arg) ||
             StringsMatch(CSz("--do-debug-window"), Arg) )
    {
      Result.DoDebugWindow = True;
    }
    else if ( StringsMatch(CSz("--no-normalize-output-whitespace"), Arg) )
    {
      Result.DoNotNormalizeWhitespace = True;
    }
    else if ( StringsMatch(CSz("-I"), Arg) ||
              StringsMatch(CSz("--include-path"), Arg) )
    {
      counted_string Include = PopArgString(ArgStrings, ArgCount, &ArgIndex);

      // TODO(Jesse): Can we pass arguments that fuck this up?  ie. can we get
      // a 0-length string here somehow?
      if (LastChar(Include) != '/')
      {
        Include = Concat(Include, CSz("/"), Memory ); // Make sure we end with a '/'
      }

      Info("Include path added : (%S)", Include);
      Push(Include, &Result.IncludePaths);

    }
    else if ( StringsMatch(CS("-D"), Arg) ||
              StringsMatch(CS("--define"), Arg) )
    {
      if (ArgIndex+1 < ArgCount)
      {
        ArgIndex += 1;
        counted_string MacroName = CS(ArgStrings[ArgIndex]);

        if (Contains(CSz("="), MacroName))
        {
          Warn("Custom define values are currently unsupported.  Please use `--define NAME` or `-D NAME` to set NAME=1");
        }

        macro_def_linked_list_node *MacroNode = Allocate_macro_def_linked_list_node(Ctx->Memory);
        macro_def *Macro = &MacroNode->Element;

        Macro->Type = type_macro_keyword;
        c_token *MacroNameT = Allocate(c_token, Memory, 1);

        Macro->NameT = MacroNameT;
        Macro->NameT->Type = CT_MacroLiteral;
        Macro->NameT->Value = MacroName;

        CTokenCursor(&Macro->Body, 1, Memory, CSz("<CLI>"), TokenCursorSource_CommandLineOption, {0,0});

        Macro->Body.Start[0] = CToken(1u, CSz("1"));

        Insert(MacroNode, &Ctx->Datatypes.Macros);
      }
      else
      {
        Error("Macro name required when using (%S) switch", Arg);
      }

    }
#if 0
    // NOTE(Jesse): This has to come after the above -D path
    else if ( StartsWith(Arg, CSz("-D")) )
    {
      macro_def *NewMacro = Push(&Ctx->Datatypes.Macros, { .Name = MacroNameT->Value }, Ctx->Memory);
      NewMacro->Type = type_macro_keyword;
      counted_string Name = Substring(Arg, 2);
      NewMacro->Name = Name;
    }
#endif
    else if ( StringsMatch(CS("--log-level"), Arg) )
    {
      // Handled in SetupStdout
      ArgIndex += 1; // Skip the switch value
    }
    else if ( StringsMatch(CS("-c0"), Arg) ||
              StringsMatch(CS("--colors-off"), Arg) )
    {
      // Handled in SetupStdout
    }
    else if ( StringsMatch(CS("-o"), Arg) ||
              StringsMatch(CS("--output-path"), Arg) )
    {
      Result.Outpath = PopArgString(ArgStrings, ArgCount, &ArgIndex);
      if (LastChar(Result.Outpath) != '/')
      {
        Result.Outpath = Concat(Result.Outpath, CSz("/"), Memory );
      }
    }
    else if ( StartsWith(Arg, CSz("-")) )
    {
      Error("Unknown Switch %S", Arg);
    }
    else
    {
      Push(Arg, &Result.Files);
    }
  }

  TruncateToCurrentElements(&Result.Files);
  Rewind(&Result.Files);

  TruncateToCurrentElements(&Result.IncludePaths);
  Rewind(&Result.IncludePaths);

  Info("%d total include paths added", (u32)Count(&Result.IncludePaths));

  return Result;
}

global_variable random_series TempFileEntropy = {};

link_internal b32
RewriteOriginalFile(parser *Parser, counted_string OutputPath, counted_string Filename, memory_arena* Memory)
{
  TIMED_FUNCTION();
  b32 Result = False;

  native_file TempFile = GetTempFile(&TempFileEntropy, Memory);
  if (TempFile.Handle)
  {
    Rewind(Parser->Tokens);
    Assert(Parser->Tokens->At == Parser->Tokens->Start);

    b32 FileWritesSucceeded = True;

    umm TotalTokens = TotalElements(Parser->Tokens);

    c_token *Start = Parser->Tokens->Start;
    for (umm TokenIndex = 0; TokenIndex < TotalTokens; ++TokenIndex)
    {
      c_token *T = Start + TokenIndex;

      // TODO(Jesse): This should probably work differently..
      //
      // Output nothing for the special tokens we insert for #includes
      if (T->Type == CT_InsertedCode)
      {
        continue;
      }

      if (T->Value.Count)
      {
        FileWritesSucceeded &= WriteToFile(&TempFile, T->Value);
      }
      else
      {
        FileWritesSucceeded &= WriteToFile(&TempFile, CS((const char*)&T->Type, 1));
      }

      // The original token can be anything; the file could end with something
      // that's not a newline, in which case we have to write our own out.
      if (T->Type == CT_PoofInsertedCode)
      {
        if (T->CodeToInsert.Start)
        {
          if (!StringsMatch(T->Value, CSz("\n"))) { FileWritesSucceeded &= WriteToFile(&TempFile, CSz("\n")); }
          FileWritesSucceeded &= WriteToFile(&TempFile, T->CodeToInsert);
        }
      }
    }

    FileWritesSucceeded &= CloseFile(&TempFile);

    if (FileWritesSucceeded)
    {
      if (Rename(TempFile.Path, Filename))
      {
        Result = True;
      }
      else
      {
        Error("Renaming tempfile: %S -> %S", TempFile.Path, Filename);
      }
    }
    else
    {
      Error("Writing to tempfile: %S", TempFile.Path);
    }
  }
  else
  {
    Error("Opening tempfile: %S", TempFile.Path);
  }

  return Result;
}

link_internal b32
Output( cs SourceFilenameAndLineNumber,
        cs Code,
        cs OutputFilename,
        memory_arena *Memory,
        output_mode Mode = Output_NoOverwrite )
{
  TIMED_FUNCTION();
  Mode = Output_Unsafe;
  b32 Result = False;

  native_file TempFile = GetTempFile(&TempFileEntropy, Memory);
  if (TempFile.Handle)
  {
    b32 FileWritesSucceeded  = WriteToFile(&TempFile, CSz("// "));
        FileWritesSucceeded &= WriteToFile(&TempFile, SourceFilenameAndLineNumber);
        FileWritesSucceeded &= WriteToFile(&TempFile, CS("\n\n"));
        FileWritesSucceeded &= WriteToFile(&TempFile, Code);
        FileWritesSucceeded &= WriteToFile(&TempFile, CS("\n"));
        FileWritesSucceeded &= CloseFile(&TempFile);

    if (FileWritesSucceeded)
    {
      if (Mode == Output_NoOverwrite)
      {
        if (FileExists(OutputFilename))
        {
          counted_string FileContents = ReadEntireFileIntoString(OutputFilename, Memory);
          if (StringsMatch(Trim(Code), Trim(FileContents)))
          {
            Info("File contents matched output for %S", OutputFilename);
          }
          else
          {
            Error("File contents didn't match for %S", OutputFilename);
            Error("TODO(Jesse): Should probably emit to a similar filname with an incremented extension or something..");
          }
        }
        else if (Rename(TempFile.Path, OutputFilename))
        {
          Info("Generated and output %S", OutputFilename);
          Result = True;
        }
        else
        {
          Error("Renaming tempfile: %S -> %S", TempFile.Path, OutputFilename);
        }
      }
      else
      {
        if (Rename(TempFile.Path, OutputFilename))
        {
          Info("Generated and output %S", OutputFilename);
          Result = True;
        }
        else
        {
          Error("Renaming tempfile: %S -> %S", TempFile.Path, OutputFilename);
        }
      }
    }
    else
    {
      Error("Writing to tempfile: %S", TempFile.Path);
    }
  }
  else
  {
    Error("Opening tempfile: %S", TempFile.Path);
  }

  return Result;
}

link_internal void
DumpStringStreamToConsole(counted_string_stream* Stream)
{
  for (counted_string_iterator Iter = Iterator(Stream);
      IsValid(&Iter);
      Advance(&Iter))
  {
    counted_string Message = Iter.At->Element;
    DebugChars("%S\n", Message);
  }
}

link_internal compound_decl
StructDef(c_token *StructNameT) // , counted_string Sourcefile)
{
  compound_decl Result = {
    .Type = StructNameT,
    {}, {}
    /* .DefinedInFile = Sourcefile */
  };

  return Result;
}

link_internal parser
GetBodyTextForNextScope(parser *Parser, memory_arena *Memory)
{
  // TODO(Jesse, immediate): This should return c_token_cursor .. I think ..
  parser BodyText = {};

  c_token_cursor *Tokens = AllocateProtection(c_token_cursor, Memory, 1, False);
  BodyText.Tokens = Tokens;

  c_token_cursor *StartTokens = Parser->Tokens;
  c_token *Start = PeekTokenPointer(Parser);

  EatBetween(Parser, CTokenType_OpenBrace, CTokenType_CloseBrace);

  if (Parser->ErrorCode == ParseErrorCode_None)
  {
    // TODO(Jesse, immediate): This is janky as fuck and should work..
    // differently.  I'm not entirely sure _what_ should be different, but the
    // assertions and -1 +1 business below is a symptom of the return value of
    // this function having to be a slice into an existing c_token_cursor.  This
    // holds true without issue (at the moment) for the interpreter code, but the
    // C++ parsing code can encounter situations where the token after End was on
    // a different cursor, so we do these sketchy hacks.  There are also
    // situations where this fails completely.. ie if a function is closed by a
    // macro.  So..  what we should do in that situation is unclear.
    //
    // The complete failure case is documented at:
    //
    // BUG_BODY_SCOPE_ACROSS_C_TOKEN_CURSOR_BOUNDARY
    //
    c_token *End = PeekTokenRawPointer(Parser, -1);

    Assert(End->Type == CTokenType_CloseBrace);
    Assert(TokenValidFor(StartTokens, Start));
    Assert(TokenValidFor(StartTokens, End));

    CTokenCursor(Tokens, Start, (umm)(End-Start) + 1, Start->Filename, TokenCursorSource_BodyText, {});

    TrimFirstToken(&BodyText, CTokenType_OpenBrace);
    /* TrimLeadingWhitespace(&BodyText); */
    if (FirstNonNBSPToken(&BodyText).Type == CTokenType_Newline)
    {
      TrimNBSPUntilNewline(&BodyText);
    }

    TrimLastToken(&BodyText, CTokenType_CloseBrace);
    if (LastNonNBSPToken(&BodyText).Type == CTokenType_Newline)
    {
      TrimTrailingNBSP(&BodyText);
    }

    Rewind(BodyText.Tokens);

    Assert(BodyText.Tokens->At == BodyText.Tokens->Start);
  }

  return BodyText;
}

link_internal type_indirection_info
ParseIndirectionInfo(parser *Parser, type_spec *TypeSpec)
{
  type_indirection_info Result = {};

  b32 Done = False;

  while (TokensRemain(Parser) && Done == False)
  {
    c_token T = PeekToken(Parser);

    switch (T.Type)
    {
      case CTokenType_OpenParen:
      {
        Result.IsFunction = True;
        RequireToken(Parser, CTokenType_OpenParen);
        if (OptionalToken(Parser, CTokenType_Star))
        {
          Result.IsFunctionPtr = True;
        }

        Assert(TypeSpec);
        TypeSpec->DatatypeToken = OptionalToken(Parser, CTokenType_Identifier);

        if (PeekToken(Parser).Type == CTokenType_OpenParen)
           { EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen); }
        RequireToken(Parser, CTokenType_CloseParen);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
        Done = True;
      } break;

      case CTokenType_Volatile:
      {
        RequireToken(Parser, CTokenType_Volatile);
        Result.VolatileValue = True;
      } break;

      case CTokenType_LogicalAnd:
      {
        Assert(Result.ReferenceLevel == 0);
        RequireToken(Parser, CTokenType_LogicalAnd);
        Result.ReferenceLevel += 2;
      } break;

      case CTokenType_Ampersand:
      {
        RequireToken(Parser, CTokenType_Ampersand);
        ++Result.ReferenceLevel;
      } break;

      case CTokenType_Star:
      {
        RequireToken(Parser, CTokenType_Star);
        ++Result.IndirectionLevel;
      } break;

      case CTokenType_Const:
      {
        RequireToken(Parser, CTokenType_Const);
        Result.ConstPointer = True;
      } break;

      case CTokenType_Arrow: // Structs can have members with the same name as a type,
      case CTokenType_Dot:   // so these are valid
      case CTokenType_Comma: // This gets called on macro-functions too, so this is valid
      case CTokenType_CloseParen: // Valid closing token during a cast
      case CTokenType_Identifier:
      case CT_NameQualifier:
      case CTokenType_OperatorKeyword: // Finish parsing the return type of an operator funciton
      case CTokenType_OpenBracket: // Parsing an un-named argument followed by [], such as in `void myfunc(char[42])`
      case CTokenType_Ellipsis:
      case CTokenType_Semicolon:
      {
        Done = True;
      } break;

      default: { Done = True; }

      /* InvalidDefaultWhileParsing(Parser, CSz("Unexpected token while parsing indirection and reference levels")); */
    }
  }

  return Result;
}

link_internal b32
IsTypeIdentifier(counted_string TypeName, program_datatypes *Datatypes)
{
  b32 Result = GetDatatypeByName(Datatypes, TypeName)->Type != type_datatype_noop;
  return Result;
}

link_internal b32
TryEatTemplateParameterList(parser *Parser, program_datatypes *Datatypes)
{
  b32 ValidTemplateList = PeekToken(Parser).Type == CTokenType_LT;
  b32 Done = False;
  u32 Lookahead = 1; // Skip the initial <

  while (ValidTemplateList && !Done)
  {
    c_token TemplateParamListTestT = PeekToken(Parser, (s32)Lookahead);

    switch (TemplateParamListTestT.Type)
    {
      case CTokenType_Const:
      case CTokenType_Static:
      case CTokenType_Volatile:
      case CTokenType_Long:
      case CTokenType_Unsigned:
      case CTokenType_Signed:
      case CTokenType_Bool:
      case CTokenType_Auto:
      case CTokenType_Void:
      case CTokenType_Double:
      case CTokenType_Float:
      case CTokenType_Char:
      case CTokenType_Int:
      case CTokenType_Short:
      case CTokenType_Comma:
      case CTokenType_Star:
      case CTokenType_Ampersand:
      {
      } break;

      case CTokenType_Identifier:
      {
        if (!IsTypeIdentifier(TemplateParamListTestT.Value, Datatypes))
        {
          ValidTemplateList = False;
          break;
        }
      } break;

      case CTokenType_GT:
      {
        Done = True;
      } break;

      default:
      {
        ValidTemplateList = False;
      } break;

    }

    ++Lookahead;
  }

  if (ValidTemplateList)
  {
    EatBetween(Parser, CTokenType_LT, CTokenType_GT);
  }

  return ValidTemplateList;
}

link_internal b32
IsConstructorOrDestructorName(counted_string ClassName, counted_string FnName)
{
  b32 Result = StringsMatch(ClassName, FnName);
  return Result;
}

link_internal b32
IsConstructorOrDestructorName(c_token *ClassNameT, c_token *FNameT)
{
  b32 Result = False;
  if (ClassNameT && FNameT)
  {
    Assert(ClassNameT != FNameT);
    Result = StringsMatch(ClassNameT->Value, FNameT->Value);
  }

  return Result;
}

link_internal b32
IsConstructorOrDestructorName(counted_string *ClassName, counted_string *FnName)
{
  b32 Result = StringsMatch(ClassName, FnName);
  return Result;
}

link_internal b32
IsConstructorOrDestructorName(c_token *T)
{
  b32 Result = False;
  if (T)
  {
    if (T->Type) { Assert(T->Type == CTokenType_Identifier); }
    Result = T->QualifierName && StringsMatch(T->QualifierName->Value, T->Value);
  }
  return Result;
}

link_internal void
ParseLongness(parser *Parser, type_spec *Result)
{
  RequireToken(Parser, CTokenType_Long);
  SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Long);

  if (OptionalToken(Parser, CTokenType_Double)) { SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Double); }
  else
  {
    if (OptionalToken(Parser, CTokenType_Long)) { SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Long_Long); }
    if (OptionalToken(Parser, CTokenType_Int)) { SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Int); }
  }
}

link_internal c_token *
EatNameQualifiers(parser *Parser)
{
  c_token *Result = {};
  while (c_token *T = PeekTokenPointer(Parser))
  {
    if (T->Type == CT_NameQualifier)
    {
      Result = RequireTokenPointer(Parser, T);
      RequireToken(Parser, CT_ScopeResolutionOperator);
    }
    else
    {
      /* Assert(T->Type == CTokenType_Identifier); */
      break;
    }
  }
  return Result;
}

link_internal b32
TryEatAttributes(parser *Parser)
{
  b32 Result = False;
  while (OptionalToken(Parser, CT_KeywordAttribute))
  {
    EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
    Result = True;
  }
  return Result;
}

link_internal counted_string
StringFromTokenSpan(parser *Parser, c_token *StartToken, c_token *EndToken, memory_arena *Memory)
{
  Assert(IsValidForCursor(Parser->Tokens, StartToken));
  Assert(IsValidForCursor(Parser->Tokens, EndToken));
  NotImplemented;

  counted_string Result = {};
  return Result;
}

link_internal void
ParsePrimitivesAndQualifiers(parser *Parser, type_spec *Result)
{
  b32 Done = False;
  while (!Done)
  {
    c_token T = PeekToken(Parser);

    switch (T.Type)
    {
      case CT_KeywordAttribute:
      {
        // Type attribute
        TryEatAttributes(Parser);
      } break;

      case CTokenType_At:
      {
        NotImplemented;

        /* RequireToken(Parser, CTokenType_At); */
        /* Result->IsMetaTemplateVar = True; */
      } break;

      case CTokenType_ThreadLocal:
      {
        RequireToken(Parser, CTokenType_ThreadLocal);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_ThreadLocal);
      } break;

      case CTokenType_Const:
      {
        if (Result->Qualifier & TypeQual_Const)
        {
          Done = True;
        }
        else
        {
          RequireToken(Parser, CTokenType_Const);
          SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Const);
        }
      } break;

      case CT_Keyword_Noreturn:
      {
        RequireToken(Parser, CT_Keyword_Noreturn);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Noreturn);
      } break;
      case CT_Keyword_Virtual:
      {
        RequireToken(Parser, CT_Keyword_Virtual);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Virtual);
      } break;
      case CT_Keyword_Explicit:
      {
        RequireToken(Parser, CT_Keyword_Explicit);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Explicit);
      } break;
      case CT_Keyword_Constexpr:
      {
        RequireToken(Parser, CT_Keyword_Constexpr);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Constexpr);
      } break;
      case CTokenType_Static:
      {
        RequireToken(Parser, CTokenType_Static);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Static);
      } break;
      case CTokenType_Volatile:
      {
        RequireToken(Parser, CTokenType_Volatile);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Volatile);
      } break;
      case CTokenType_Signed:
      {
        RequireToken(Parser, T);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Signed);
      } break;
      case CTokenType_Unsigned:
      {
        RequireToken(Parser, T);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Unsigned);
      } break;
      case CTokenType_Enum:
      {
        RequireToken(Parser, CTokenType_Enum);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Enum);
      } break;
      case CT_Keyword_Class:
      {
        RequireToken(Parser, CT_Keyword_Class);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Class);
      } break;
      case CTokenType_Struct:
      {
        RequireToken(Parser, CTokenType_Struct);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Struct);
      } break;
      case CTokenType_Union:
      {
        RequireToken(Parser, CTokenType_Union);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Union);
      } break;

      case CTokenType_TemplateKeyword:
      {
        RequireToken(Parser, CTokenType_TemplateKeyword);
        string_from_parser Builder = StartStringFromParser(Parser);
        EatBetween(Parser, CTokenType_LT, CTokenType_GT);
        Result->TemplateSource = FinalizeStringFromParser(&Builder);
      } break;

      case CTokenType_Extern:
      {
        RequireToken(Parser, T.Type);
        Result->Linkage = linkage_extern;
        if ( OptionalToken(Parser, CToken(CTokenType_StringLiteral, CSz("\"C\""))) )
        {
          Result->Linkage = linkage_extern_c;
        }
        else if ( OptionalToken(Parser, CToken(CTokenType_StringLiteral, CSz("\"C++\""))) )
        {
          Result->Linkage = linkage_extern_cpp;
        }
      } break;

      case CTokenType_Inline:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Inline);
      } break;

      case CTokenType_Long:
      {
        RequireToken(Parser, CTokenType_Long);
        if (Result->Qualifier & TypeQual_Long)
        {
          SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Long_Long);
        }
        else
        {
          SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Long);
        }
      } break;

      case CTokenType_Short:
      {
        RequireToken(Parser, CTokenType_Short);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Short);
      } break;

      case CTokenType_Int:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Int);
      } break;

      case CTokenType_Double:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Double);
      } break;
      case CTokenType_Float:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Float);
      } break;
      case CTokenType_Char:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Char);
      } break;
      case CTokenType_Void:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Void);
      } break;
      case CTokenType_Bool:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Bool);
      } break;

      case CTokenType_Auto:
      {
        RequireToken(Parser, T.Type);
        SetBitfield(type_qualifier, Result->Qualifier, TypeQual_Auto);
      } break;

      case CT_NameQualifier:
      {
        Result->QualifierNameT = EatNameQualifiers(Parser);
      } break;

      default:
      {
        Done = True;
      } break;
    }

    continue;
  }
}

link_internal b32
IsPrimitiveType(type_spec *Type)
{
  u32 Mask =
    TypeQual_Auto      |  // Questionable that auto belongs here
    TypeQual_Bool      |
    TypeQual_Signed    |
    TypeQual_Unsigned  |
    TypeQual_Char      |
    TypeQual_Long      |
    TypeQual_Int       |
    TypeQual_Long_Long |
    TypeQual_Double    |
    TypeQual_Short     |
    TypeQual_Void      |
    TypeQual_Float;

  b32 Result = (Type->Qualifier & Mask) > 0;
  return Result;
}

#if 1
global_variable c_token Global_DefaultNameToken =
{
  CTokenType_Identifier,
  CTFlags_None,
  {CSz("(anonymous)")},
  {CSz("(anonymous)")},
  0,
  False,
  {{}},
};
#endif

link_internal type_spec
ParseTypeSpecifier(parse_context *Ctx, c_token *StructNameT = 0)
{
  TIMED_FUNCTION();
  type_spec Result = {};
  parser *Parser = Ctx->CurrentParser;

  ParsePrimitivesAndQualifiers(Parser, &Result);

  if (IsPrimitiveType(&Result))
  {
  }
  else if (PeekToken(Parser).Type == CTokenType_OperatorKeyword)
  {
  }
  else
  {
    // NOTE(Jesse): This is super tortured but it's for differentiating between
    // parsing constructors in structs ie:
    //
    // struct foo { foo() {} };
    //
    // and constructors declared in the global namespace ie:
    //
    // foo::foo() {}
    //
    // The idea is that we don't want to eat the actual constructor function
    // name token because it's annoying to have to check if we're parsing a
    // constructor function before calling ParseIndirectionInfo().
    //
    // Without these checks we eat the constructor name in the former case then
    // ParseIndirectionInfo() thinks it's parsing a function pointer.
    //

    b32 ParsingConstructorInsideStruct = False;

    c_token *NameT = PeekTokenPointer(Parser);
    c_token *OpenParen = PeekTokenPointer(Parser, 1);
    if (OpenParen && StructNameT && NameT)
    {
      if (OpenParen->Type == CTokenType_OpenParen)
      {
        if (IsConstructorOrDestructorName(StructNameT, NameT))
        {
          Result.QualifierNameT = StructNameT; // NOTE(Jesse): Pretty gross, but so is C++, so..
          ParsingConstructorInsideStruct = True;
        }
      }
    }

    // NOTE(Jesse): If we were parsing a foo::foo() constructor the
    // Result.NameQualifierT had already been set, so we compare that here.
    b32 ParsingConstructor = ParsingConstructorInsideStruct;
    if (IsConstructorOrDestructorName(Result.QualifierNameT, NameT))
    {
      ParsingConstructor |= True;
    }

    // This means we're parsing either a variable of function delc.  I think.
    if (ParsingConstructor == False)
    {
      if (c_token *TypeName = OptionalToken(Parser, CTokenType_Identifier))
      {
        Result.DatatypeToken = TypeName;
      }
    }
  }

  TryEatAttributes(Parser); // Value Attribute

  // Template parameters
  if (PeekToken(Parser).Type == CTokenType_LT)
  {
    EatBetween(Parser, CTokenType_LT, CTokenType_GT);
  }

  Result.Indirection = ParseIndirectionInfo(Parser, &Result);

  return Result;
}

link_internal ast_node*
ParseInitializerList(parser *Parser, memory_arena *Memory)
{
  ast_node* Result = {};
  ast_node_initializer_list *Node = AllocateAndCastTo(ast_node_initializer_list, &Result, Memory);
  EatBetween(Parser, CTokenType_OpenBrace, CTokenType_CloseBrace);
  return Result;
}

link_internal variable_decl
FinalizeVariableDecl(parse_context *Ctx, type_spec *TypeSpec, c_token *IdentifierToken = 0)
{
  parser *Parser = Ctx->CurrentParser;

  variable_decl Result = {};

  Result.Type = *TypeSpec;

  if (IdentifierToken == 0)
  {
    IdentifierToken = OptionalToken(Parser, CTokenType_Identifier);
  }

  if (IdentifierToken)
  {
    Result.NameT = IdentifierToken;

    TryEatAttributes(Parser); // Possibly garbage.  At least some of the code that calls this eats these already

    MaybeParseStaticBuffers(Ctx, Parser, &Result.StaticBufferSize);

    if (OptionalToken(Parser, CTokenType_Equals))
    {
      if (PeekToken(Parser).Type == CTokenType_OpenBrace)
      {
        Result.Value = ParseInitializerList(Ctx->CurrentParser, Ctx->Memory);
      }
      else
      {
        ParseExpression(Ctx, &Result.Value);
      }
    }
    else if ( PeekToken(Parser).Type == CTokenType_OpenBrace )
    {
      Result.Value = ParseInitializerList(Parser, Ctx->Memory);
    }
    else if ( OptionalToken(Parser, CTokenType_Colon) )
    {
      Result.StrictBitWidth = RequireToken(Parser, CTokenType_IntLiteral);
    }
  }
  else
  {
    // Handles unnamed arguments : void foo(char[42]);
    if ( PeekToken(Parser).Type == CTokenType_OpenBracket )
    {
      ParseExpression(Ctx, &Result.StaticBufferSize);
    }
  }

  return Result;
}

link_internal void
MaybeParseDeleteOrDefault(parser *Parser, function_decl *Result)
{
  auto Type = Result->Type ;
  Assert(Type == function_type_constructor ||
         Type == function_type_destructor ||
         Type == function_type_operator );

  if (OptionalToken(Parser, CTokenType_Equals))
  {
    Result->ImplIsDefault = (b32)(OptionalToken(Parser, CTokenType_Default) != 0);
    Result->ImplIsDeleted = (b32)(OptionalToken(Parser, CT_Keyword_Delete) != 0);

    if (! (Result->ImplIsDefault || Result->ImplIsDeleted) )
    {
      ParseError(Parser, CSz("Expected either keyword : 'delete' or 'default'") );
    }

    if ( Result->ImplIsDefault && Result->ImplIsDeleted )
    {
      ParseError(Parser,
          FormatCountedString(GetTranArena(), CSz("Cannot delete and default implementation of %S %S"), ToString(Type), Result->NameT->Value) );
    }
  }
}

link_internal void
MaybeParseAttributes(parser *Parser)
{
  if (PeekToken(Parser).Type == CT_KeywordAttribute)
  {
    // void FunctionName( arg A1, arg, A2) __attribute__((whatever));
    //
    TryEatAttributes(Parser);
    RequireToken(Parser, CTokenType_Semicolon);
  }
}

link_internal function_decl
ParseFunctionParameterList(parse_context *Ctx, type_spec *ReturnType, c_token *FuncNameT, function_type Type, c_token *StructNameT = 0)
{
  parser *Parser = Ctx->CurrentParser;

  RequireToken(Parser, CTokenType_OpenParen);

  function_decl Result = {};

  Result.Type = Type;
  Result.ReturnType = *ReturnType;
  Result.NameT = FuncNameT;
  Result.Args.Memory = &Global_PermMemory;

  // Function definition args
  b32 DoneParsingArguments = PeekToken(Parser) == CToken(CTokenType_CloseParen);

  switch (Type)
  {
    case function_type_constructor:
    case function_type_operator:
    case function_type_normal:
    {
      // TODO(Jesse, correctness): Is this valid for constructors?
      if ( PeekToken(Parser).Type == CTokenType_Void &&
           PeekToken(Parser, 1).Type == CTokenType_CloseParen )
      {
        RequireToken(Parser, CTokenType_Void);
        DoneParsingArguments = True;
      }

      while ( !DoneParsingArguments && TokensRemain(Parser) )
      {
        type_spec TypeSpec = ParseTypeSpecifier(Ctx);
        variable_decl Arg = FinalizeVariableDecl(Ctx, &TypeSpec);
        Push(&Result.Args, Arg);

        if (!OptionalToken(Parser, CTokenType_Comma))
        {
          DoneParsingArguments = True;
        }

        if (OptionalToken(Parser, CTokenType_Ellipsis))
        {
          Result.IsVariadic = True;
          DoneParsingArguments = True;
        }

        continue;
      }

      RequireToken(Parser, CTokenType_CloseParen); // Ending paren for arguments
    } break;

    case function_type_noop:
    {
      InternalCompilerError( Parser, CSz("ParseFunctionParameterList called with function_type_noop!"), FuncNameT);
    } break;

    case function_type_destructor:
    {
      InternalCompilerError( Parser, CSz("ParseFunctionParameterList called on function_type_destructor!"), FuncNameT );
    } break;

  }

  MaybeParseAttributes(Parser);

  return Result;
}

link_internal void
MaybeParseStaticBuffers(parse_context *Ctx, parser *Parser, ast_node **Dest)
{
  ast_node_expression *Current = {};

  if ( PeekToken(Parser).Type == CTokenType_OpenBracket )
  {
    Current = AllocateAndCastTo(ast_node_expression, Dest, Ctx->Memory);
  }

  while ( OptionalToken(Parser, CTokenType_OpenBracket) )
  {
    ParseExpression(Ctx, Current);
    RequireToken(Parser, CTokenType_CloseBracket);

    if ( PeekToken(Parser).Type == CTokenType_OpenBracket )
    {
      Current->Next = Allocate(ast_node_expression, Ctx->Memory, 1);
    }
    Current = Current->Next;
  }
}

link_internal function_decl
FinalizeFunctionDecl(parse_context *Ctx, type_spec *TypeSpec, c_token *FuncNameT, function_type Type)
{
  function_decl Result = {};
  Result = ParseFunctionParameterList(Ctx, TypeSpec, FuncNameT, Type);
  Result.Body = MaybeParseFunctionBody(Ctx->CurrentParser, Ctx->Memory);

  return Result;
}

link_internal function_decl
FinalizeOperatorFunction(parse_context *Ctx, type_spec *TypeSpec)
{
  parser *Parser = Ctx->CurrentParser;

  function_decl Result = {};

  c_token *OperatorNameT = RequireOperatorToken(Parser);
  if ( PeekToken(Parser).Type == CTokenType_OpenParen )
  {
    Result = ParseFunctionParameterList(Ctx, TypeSpec, OperatorNameT, function_type_operator);
    MaybeParseDeleteOrDefault(Parser, &Result);
    Result.Body = MaybeParseFunctionBody(Parser, Ctx->Memory);
  }

  return Result;
}

link_internal u64
ResolveConstantExpression(parser *Parser, ast_node *Node)
{
  u64 Result = 0;
  if (Node)
  {
    switch(Node->Type)
    {
      case type_ast_node_expression:
      {
        auto Expr = SafeAccess(ast_node_expression, Node);
        if (Expr->Next)
        {
          PoofNotImplementedError(Parser, CSz("ResolveConstantExpression currently unable to resolve compound expressions."), PeekTokenPointer(Parser));
        }

        switch (Expr->Value->Type)
        {
          case type_ast_node_literal:
          {
            auto Literal = SafeAccess(ast_node_literal, Expr->Value);
            switch (Literal->Token.Type)
            {
              case CTokenType_IntLiteral:
              {
                Result = Literal->Token.UnsignedValue;
              } break;

              InvalidDefaultWhileParsing(Parser,
                FormatCountedString( GetTranArena(),
                  CSz("Invalid literal type passed to ResolveConstantExpression.  Type was (%S)"), ToString(Literal->Token.Type) ) );
            }
          } break;

          InvalidDefaultWhileParsing(Parser,
            FormatCountedString( GetTranArena(),
              CSz("Invalid expression type passed to ResolveConstantExpression.  Type was (%S)"), ToString(Expr->Value->Type) )
          );
        }

      } break;

      InvalidDefaultWhileParsing(Parser,
        FormatCountedString( GetTranArena(),
          CSz("Invalid node type passed to ResolveConstantExpression.  Type was (%S)"), ToString(Node->Type) )
      );
    }
  }

  return Result;
}

#if 0
// NOTE(Jesse): This function is not meant to parse struct-member specific
// functions such as constructors and destructors.  It parses variables or free functions
link_internal declaration
ParseFunctionOrVariableDecl(parse_context *Ctx, type_spec *TypeSpec, type_indirection_info *Indirection)
{
  parser *Parser = Ctx->CurrentParser;

  declaration Result = {};

  if (OptionalToken(Parser, CTokenType_OperatorKeyword))
  {
    Result.Type = type_function_decl;
    Result.function_decl = FinalizeOperatorFunction(Ctx, TypeSpec);

  }
  else if (Indirection->IsFunction || Indirection->IsFunctionPtr)
  {
    // TODO(Jesse): We know this statically when we call this function so it should go outside.
    Result.Type = type_variable_decl;
    Result.variable_decl.Type = *TypeSpec;
    Result.variable_decl.Indirection = *Indirection;
  }
  else
  {
    c_token *DeclNameToken = RequireTokenPointer(Parser, CTokenType_Identifier);
    if (OptionalToken(Parser, CTokenType_LT))
    {
      EatUntilIncluding(Parser, CTokenType_GT);
    }

    b32 IsFunction = PeekToken(Parser).Type == CTokenType_OpenParen;
    if ( IsFunction )
    {
      Result.Type = type_function_decl;
      Result.function_decl = FinalizeFunctionDecl(Ctx, TypeSpec, DeclNameToken, function_type_normal);
    }
    else // Variable decl
    {
      Result.Type = type_variable_decl;
      Result.variable_decl = FinalizeVariableDecl(Ctx, TypeSpec, Indirection, DeclNameToken);
    }
  }

  // TODO(Jesse id: 323, tags: id_321)
  /* Assert(Result.Type); */
  return Result;
}
#endif

link_internal s64
ApplyOperator(parser *Parser, s64 LHS, c_token_type OperatorType, s64 RHS)
{
  s64 Result = 0;
  switch (OperatorType)
  {
    case CTokenType_LT:
    {
      Result = LHS < RHS;
    } break;

    case CTokenType_GT:
    {
      Result = LHS > RHS;
    } break;

    case CTokenType_Plus:
    {
      Result = LHS + RHS;
    } break;

    case CTokenType_Minus:
    {
      Result = LHS - RHS;
    } break;

    case CTokenType_Star:
    {
      Result = LHS * RHS;
    } break;

    case CTokenType_FSlash:
    {
      Result = LHS / RHS;
    } break;

    // TODO(Jesse id: 345): How do we differentiate between arithmetic/logical shifts here?
    case CTokenType_LeftShift:
    {
      if (RHS < 64)
      {
        Result = LHS << RHS;
      }
      else
      {
        ParseError(Parser, CSz("Attempted to left-shift by more than 64 bits!"));
      }
    } break;

    // TODO(Jesse id: 346, tags: id_345)
    case CTokenType_RightShift:
    {
      Result = LHS >> RHS;
    } break;

    case CTokenType_LessEqual:
    {
      Result = LHS <= RHS;
    } break;

    case CTokenType_GreaterEqual:
    {
      Result = LHS >= RHS;
    } break;

    case CTokenType_AreEqual:
    {
      Result = LHS == RHS;
    } break;

    case CTokenType_NotEqual:
    {
      Result = LHS != RHS;
    } break;

    case CTokenType_PlusEquals:
    {
      Result = LHS + RHS;
    } break;

    case CTokenType_MinusEquals:
    {
      Result = LHS - RHS;
    } break;

    case CTokenType_TimesEquals:
    {
      Result = LHS * RHS;
    } break;

    case CTokenType_DivEquals:
    {
      Result = LHS / RHS;
    } break;

    case CTokenType_ModEquals:
    {
      Result = LHS % RHS;
    } break;

    case CTokenType_AndEquals:
    {
      Result = LHS && RHS;
    } break;

    case CTokenType_OrEquals:
    {
      Result = LHS || RHS;
    } break;

    case CTokenType_XorEquals:
    {
      Result = LHS ^ RHS;
    } break;

    case CTokenType_Increment:
    {
      Assert(RHS == 0);
      Result = LHS+1;
    } break;

    case CTokenType_Decrement:
    {
      Assert(RHS == 0);
      Result = LHS - 1;
    } break;

    case CTokenType_LogicalAnd:
    {
      Result = LHS && RHS;
    } break;

    case CTokenType_LogicalOr:
    {
      Result = LHS || RHS;
    } break;

    case CTokenType_Ampersand:
    {
      Result = LHS & RHS;
    } break;

    case CTokenType_Pipe:
    {
      Result = LHS | RHS;
    } break;

    case CTokenType_Hat:
    {
      Result = LHS ^ RHS;
    } break;

    InvalidDefaultWhileParsing(Parser, CSz("Invalid operator type passed to ApplyOperator."));
  }


  return Result;
}

#if 0
link_internal b32
IsNextTokenMacro(parse_context *Ctx, parser *Parser)
{
  c_token *T = PeekTokenRawPointer(Parser);
  b32 Result = (GetMacroDef(Ctx, T->Value) != 0);
  return Result;
}
#endif

// This function is confusing as fuck.  Sorry about that.  It's written
// recursively because when we encounter a macro literal we either had to call
// ResolveMacroConstantExpression again on the resulting expanded parser, or
// track each macro expansion and where we're at in each.  That seemed onerous
// and annoying, so I did it the 'lazy' recursive way.
//
link_internal s64
ResolveMacroConstantExpression(parse_context *Ctx, parser *Parser, memory_arena *PermMemory, memory_arena *TempMemory, s64 PreviousValue, b32 LogicalNotNextValue)
{
  TIMED_FUNCTION();

  s64 Result = PreviousValue;

  EatSpacesTabsAndEscapedNewlines(Parser);

  if (c_token *T = PeekTokenRawPointer(Parser)) // Has to be raw because newlines delimit the end of a macro expression
  {
    switch (T->Type)
    {
      case CT_MacroLiteral:
      {
        if (T->Erased)
        {
          RequireTokenRaw(Parser, T);

          if (T->Macro.Def->Type == type_macro_function)
          {
            c_token Next = PeekTokenRaw(Parser);
            if (Next.Type == CTokenType_OpenParen)
            {
              EatBetweenRaw(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
            }
          }
          Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
        }
        else
        {
          ParseError(Parser, CSz("Invalid MacroLiteral encountered."));
        }
      } break;

      case CTokenType_Question:
      {
        RequireTokenRaw(Parser, T);
        s64 Operator = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
        if (Operator)
        {
          Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
        }
        else
        {
          EatUntilIncluding(Parser, CTokenType_Colon);
          Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
        }
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
      } break;

      case CTokenType_Bang:
      {
        RequireTokenRaw(Parser, CTokenType_Bang);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue ? False : True);
     } break;

      case CTokenType_Identifier:
      {
        if (OptionalTokenRaw(Parser, CToken(CSz("defined"))))
        {
          EatSpacesTabsAndEscapedNewlines(Parser);
          c_token *NextToken = PeekTokenRawPointer(Parser);
          switch (NextToken->Type)
          {
            case CT_MacroLiteral:
            case CTokenType_Identifier:
            {
              // TODO(Jesse, tags: correctness): Should this path set the next token to CT_MacroLiteral
              c_token *PotentialMacroToken = PeekTokenRawPointer(Parser);
              macro_def *M = GetMacroDef(Ctx, PotentialMacroToken->Value);
              b32 NextTokenIsMacro = (M && !M->Undefed);
              RequireTokenRaw(Parser, PotentialMacroToken->Type); // @optimize_call_advance_instead_of_being_dumb

              Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, LogicalNotNextValue ? !NextTokenIsMacro : NextTokenIsMacro, False);
            } break;

            case CTokenType_OpenParen:
            {
              u32 ParenCount = 0;
              while (PeekTokenRaw(Parser).Type == CTokenType_OpenParen )
              {
                RequireTokenRaw(Parser, CTokenType_OpenParen);
                EatSpacesTabsAndEscapedNewlines(Parser);
                ++ParenCount;
              }

              // TODO(Jesse, tags: correctness): Should this path has to set the next token to CT_MacroLiteral
              c_token *PotentialMacroToken = PeekTokenRawPointer(Parser);
              macro_def *M = GetMacroDef(Ctx, PotentialMacroToken->Value);
              b32 NextTokenIsMacro = (M && !M->Undefed);
              RequireTokenRaw(Parser, PotentialMacroToken->Type); // @optimize_call_advance_instead_of_being_dumb

              while (ParenCount--)
              {
                EatSpacesTabsAndEscapedNewlines(Parser);
                RequireTokenRaw(Parser, CTokenType_CloseParen);
              }

              Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, LogicalNotNextValue ? !NextTokenIsMacro : NextTokenIsMacro, False);
            } break;

            case CT_PreprocessorHasInclude:
            case CT_PreprocessorHasIncludeNext:
            {
              RequireTokenRaw(Parser, NextToken);
              Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, LogicalNotNextValue ? 0 : 1, False);
            } break;

            InvalidDefaultError( Parser,
                                 FormatCountedString_(GetTranArena(), CSz(" ResolveMacroConstantExpression failed : Invalid %S(%S)"), ToString(NextToken->Type), NextToken->Value ),
                                 NextToken );
          }
        }
        else
        {
          // TODO(Jesse)(correctness): Call TryTransmuteIdentifierToMacro() here?
          macro_def *Macro = GetMacroDef(Ctx, T->Value);
          if (Macro)
          {
            c_token *pT = PeekTokenRawPointer(Parser);
            pT->Type = CT_MacroLiteral;
            pT->Macro.Def = Macro;
            Assert(pT->Macro.Expansion == 0);

            // TODO(Jesse): Is it actually even possible for this to fail?
            parser *Expanded = ExpandMacro(Ctx, Parser, Macro, PermMemory, TempMemory, False, True);
            if (Expanded->ErrorCode)
            {
              ParseInfoMessage( Parser,
                                FormatCountedString(GetTranArena(), CSz("While Expanding %S"), pT->Value),
                                pT);
            }
            else
            {
              s64 MacroExpansion = ResolveMacroConstantExpression(Ctx, Expanded, PermMemory, TempMemory, Result, LogicalNotNextValue);
              Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, MacroExpansion, False);
            }
          }
          else if (!Macro || Macro->Undefed)
          {
            RequireTokenRaw(Parser, CTokenType_Identifier);
            Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, LogicalNotNextValue ? 1 : 0, False);
          }
          else
          {
            InvalidCodePath();
          }
        }
      } break;

      case CTokenType_CharLiteral:
      case CTokenType_IntLiteral:
      {
        RequireTokenRaw(Parser, T->Type);

        s64 ThisValue = LogicalNotNextValue ? !T->as_s64 : T->as_s64;

        if (T->Flags & CTFlags_Signed)
        {
          s64 OperationResult = ApplyOperator(Parser, PreviousValue, CTokenType_Plus, ThisValue);
          Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, OperationResult, False);
        }
        else
        {
          Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, ThisValue, False);
        }

      } break;

      case CTokenType_OpenParen:
      {
        RequireTokenRaw(Parser, T->Type);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, False);
        if (LogicalNotNextValue)
        {
          Result = !Result;
        }
        EatSpacesTabsAndEscapedNewlines(Parser);
        RequireTokenRaw(Parser, CTokenType_CloseParen);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, False);
      } break;

      case CTokenType_Plus:
      case CTokenType_Minus:
      case CTokenType_Star:
      case CTokenType_FSlash:
      case CTokenType_LT:
      case CTokenType_GT:
      case CTokenType_LeftShift:
      case CTokenType_RightShift:
      case CTokenType_LessEqual:
      case CTokenType_GreaterEqual:
      case CTokenType_AreEqual:
      case CTokenType_NotEqual:
      case CTokenType_PlusEquals:
      case CTokenType_MinusEquals:
      case CTokenType_TimesEquals:
      case CTokenType_DivEquals:
      case CTokenType_ModEquals:
      case CTokenType_AndEquals:
      case CTokenType_OrEquals:
      case CTokenType_XorEquals:
      case CTokenType_Increment:
      case CTokenType_Decrement:
      case CTokenType_LogicalAnd:
      case CTokenType_LogicalOr:
      case CTokenType_Ampersand:
      case CTokenType_Pipe:
      case CTokenType_Hat:
      {
        RequireTokenRaw(Parser, T->Type);
        c_token_type OperatorToApply = T->Type;
        s64 NextValue = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, False);
        s64 OperationResult = ApplyOperator(Parser, Result, OperatorToApply, NextValue);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, OperationResult, False);
      } break;

      case CTokenType_CommentMultiLine:
      case CTokenType_EscapedNewline:
      {
        RequireTokenRaw(Parser, T->Type);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, Result, LogicalNotNextValue);
      } break;

      case CTokenType_CommentSingleLine:
      case CTokenType_Newline:
      case CTokenType_CloseParen:
      case CTokenType_Colon:
      {
        // We're done
      } break;

      case CT_PreprocessorHasInclude:
      case CT_PreprocessorHasIncludeNext:
      {
        RequireTokenRaw(Parser, T);

        RequireToken(Parser, CTokenType_OpenParen);

        b32 IsRelative;
        c_token *PathT = 0;
        counted_string Path = {};
        if (PeekToken(Parser).Type == CTokenType_LT)
        {
          IsRelative = False;
          PathT = T;
          Path = EatBetween_Str(Parser, CTokenType_LT, CTokenType_GT);
          Path = StripBookends(Path, '<', '>');
        }
        else
        {
          IsRelative = True;
          PathT = PopTokenPointer(Parser);
          Path = PathT->Value;
        }

        RequireToken(Parser, CTokenType_CloseParen);

        b32 IsIncludeNext = (T->Type == CT_PreprocessorHasIncludeNext);
        counted_string Inc = ResolveIncludePath(Ctx, Parser, PathT, Path, IsIncludeNext, IsRelative);

        b32 Val = (Inc.Start != 0);
        Result = ResolveMacroConstantExpression(Ctx, Parser, PermMemory, TempMemory, LogicalNotNextValue ? !Val : Val, False);

      } break;

      InvalidDefaultError( Parser,
                           FormatCountedString_(GetTranArena(), CSz("ResolveMacroConstantExpression failed : Invalid %S(%S)"), ToString(T->Type), T->Value ),
                           T);
    }
  }

  return Result;
}

link_internal c_token *
EraseAllRemainingIfBlocks(parser *Parser)
{
  c_token *StartToken = PeekTokenRawPointer(Parser);
  c_token *Result = StartToken;

  if (Parser->ErrorCode == ParseErrorCode_None)
  {
    b32 Done = 0;
    while (Result && !Done)
    {
      Result = EatIfBlock(Parser, EraseTokens);

      switch (Result->Type)
      {
        case CT_PreprocessorElse:
        case CT_PreprocessorElif:
        {
          RequireTokenRaw(Parser, Result);
        } break;

        case CT_PreprocessorEndif:
        {
          Done = True;
        } break;

        InvalidDefaultError(Parser, CSz("Got an invalid token while attempting to preprocess #if block"), StartToken);
      }
    }

    if (Result)
    {
      Assert(Result->Type == CT_PreprocessorEndif);
    }
    else
    {
      ParseError( Parser,
                  FormatCountedString(GetTranArena(), CSz("Unable to find closing token for %S."), ToString(CT_PreprocessorIf)),
                  StartToken);
    }
  }

  return Result;
}

link_internal c_token *
EatIfBlock(parser *Parser, erase_token_mode EraseMode)
{
  c_token *StartToken = PeekTokenRawPointer(Parser);
  c_token *Result = StartToken;

  if (Parser->ErrorCode == ParseErrorCode_None)
  {
    u32 Depth = 0;
    while (Result)
    {
      if (Result->Type == CT_PreprocessorIf ||
          Result->Type == CT_PreprocessorIfNotDefined ||
          Result->Type == CT_PreprocessorIfDefined)
      {
        ++Depth;
      }

      if ( Result->Type == CT_PreprocessorElse ||
           Result->Type == CT_PreprocessorElif ||
           Result->Type == CT_PreprocessorEndif )
      {
        if (Depth == 0)
        {
          break;
        }

        if (Result->Type == CT_PreprocessorEndif)
        {
          --Depth;
        }
      }

      if (EraseMode == EraseTokens)
      {
        Result->Erased = True;
      }

      // @optimize_call_advance_instead_of_being_dumb
      RequireTokenRaw(Parser, *Result);
      Result = PeekTokenRawPointer(Parser);
    }

    if (!Result)
    {
      ParseError( Parser,
                  FormatCountedString(GetTranArena(), CSz("Unable to find closing token for %S."), ToString(CT_PreprocessorIf)),
                  StartToken );
    }
  }

  /* Assert(Result == Parser->Tokens->At); */
  return Result;
}

#if 0
link_internal counted_string
ParseIfDefinedValue(parser *Parser)
{
  u32 NumOpenParens = 0;
  while (OptionalToken(Parser, CTokenType_OpenParen))
  {
    ++NumOpenParens;
  }

  counted_string Result = RequireToken(Parser, CTokenType_Identifier).Value;

  while (NumOpenParens--)
  {
    RequireToken(Parser, CTokenType_CloseParen);
  }

  return Result;
}
#endif

link_internal macro_def *
GetMacroDef(parse_context *Ctx, counted_string DefineValue)
{
  /* TIMED_FUNCTION(); */
  macro_def *Macro = GetByName(&Ctx->Datatypes.Macros, DefineValue);

  macro_def *Result = 0;
  if ( Macro &&
      !Macro->Undefed )
  {
    Result = Macro;
  }
  return Result;
}

#if 0
link_internal macro_def *
GetMacroDef(parse_context *Ctx, counted_string *DefineValue)
{
  macro_def *Result = GetMacroDef(Ctx, *DefineValue);
  return Result;
}
#endif

link_internal b32
IsImplementation(function_decl *Func)
{
  b32 Result = Func->Body.Tokens != 0;
  return Result;
}

link_internal b32
IsDefaultImplementation(function_decl *Func)
{
  b32 Result = Func->ImplIsDefault;
  return Result;
}

link_internal b32
IsDeletedImplementation(function_decl *Func)
{
  b32 Result = Func->ImplIsDeleted;
  return Result;
}

link_internal parser
MaybeParseFunctionBody(parser *Parser, memory_arena *Memory)
{
  parser Result = {};
  if (PeekToken(Parser).Type == CTokenType_OpenBrace)
  {
    Result = GetBodyTextForNextScope(Parser, Memory);
  }
  return Result;
}

link_internal void
ParseStructMemberOperatorFn(parse_context *Ctx, declaration *Result, c_token *StructNameT)
{
  NotImplemented;
}

link_internal void
ParseStructMemberConstructorFn(parse_context *Ctx, type_spec *TypeSpec, declaration *Result, c_token *ConstructorNameT)
{
  parser *Parser = Ctx->CurrentParser;

  c_token *StructNameT = TypeSpec->QualifierNameT;

  Assert(StructNameT);
  Assert(ConstructorNameT);

  if (IsConstructorOrDestructorName(StructNameT, ConstructorNameT))
  {
    Result->Type = type_function_decl;
    type_spec ReturnType = {};
    Result->function_decl = ParseFunctionParameterList(Ctx, &ReturnType, ConstructorNameT, function_type_constructor, StructNameT);

    if (OptionalToken(Parser, CTokenType_Colon) )
    {
      while (OptionalToken(Parser, CTokenType_Identifier))
      {
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
        OptionalToken(Parser, CTokenType_Comma);
      }
    }

    MaybeParseDeleteOrDefault(Parser, &Result->function_decl);
    MaybeParseAttributes(Parser);
    Result->function_decl.Body = MaybeParseFunctionBody(Parser, Ctx->Memory);

    if (IsImplementation(&Result->function_decl) ||
        IsDefaultImplementation(&Result->function_decl) ||
        IsDeletedImplementation(&Result->function_decl))
    {
      /* Push(&Ctx->Datatypes.Functions, Result->function_decl, Ctx->Memory); */
    }
#if 0
    else
    {
      ParseError(Parser,
          CSz("Constructor function must have a body"),
          ConstructorNameT);
    }
#endif
  }
  else
  {
    ParseError(Parser,
        FormatCountedString(GetTranArena(), CSz("Constructor (%S) name must match the struct name (%S)"), ConstructorNameT->Value, StructNameT->Value),
        ConstructorNameT );
  }
}

link_internal void
ParseStructMemberDestructorFn(parse_context *Ctx, declaration *Result, c_token *StructNameT)
{
  Assert(Result->Type == type_declaration_noop);

  parser *Parser = Ctx->CurrentParser;
  RequireToken(Parser, CTokenType_Tilde);

  c_token *FuncNameT = RequireTokenPointer(Parser, CTokenType_Identifier);
  type_spec ReturnType = {};

  if (IsConstructorOrDestructorName(StructNameT, FuncNameT))
  {
    c_token *GotOpen = OptionalToken(Parser, CTokenType_OpenParen);
    OptionalToken(Parser, CTokenType_Void);
    c_token *GotClose = OptionalToken(Parser, CTokenType_CloseParen);
    if ( GotOpen && GotClose )
    {
    }
    else
    {
      ParseError(Parser, FormatCountedString(GetTranArena(), CSz("Destructor (%S) must have an empty parameter list!"), FuncNameT->Value) );
    }
  }
  else
  {
    ParseError(Parser, FormatCountedString(GetTranArena(), CSz("Destructor (%S) name must match the struct name (%S)"), FuncNameT->Value, StructNameT->Value ) );
  }

  Result->Type = type_function_decl;
  Result->function_decl.Type = function_type_destructor;

  MaybeParseDeleteOrDefault(Parser, &Result->function_decl);
  Result->function_decl.Body = MaybeParseFunctionBody(Parser, Ctx->Memory);
}

link_internal b32
ParsingConstructor(parser *Parser, type_spec *TypeSpec)
{
  b32 Result = False;
  if (TypeSpec->QualifierNameT)
  {
    c_token *NextT = PeekTokenPointer(Parser);
    if (IsConstructorOrDestructorName(TypeSpec->QualifierNameT, NextT))
    {
      Result = True;
    }
  }
  return Result;
}

link_internal function_decl
FinalizeVirtualFunctionDecl(parse_context *Ctx, parser *Parser, type_spec *TypeSpec)
{
  function_decl Result = {};

  c_token *FuncNameT = RequireTokenPointer(Parser, CTokenType_Identifier);

  // TODO(Jesse, correctness): I think this might actually belong in the else block below.
  Result = FinalizeFunctionDecl(Ctx, TypeSpec, FuncNameT, function_type_normal);

  if (OptionalToken(Parser, CTokenType_Equals))
  {
    RequireToken(Parser, CToken(0u)); // TODO(Jesse): is this legit, or can we see other values here?
  }
  else
  {
    OptionalToken(Parser, CT_Keyword_Override);
  }

  return Result;
}

link_internal declaration
ParseStructMember(parse_context *Ctx, c_token *StructNameT)
{
  TIMED_FUNCTION();
  parser *Parser = Ctx->CurrentParser;

  declaration Result = {};

  c_token *T = PeekTokenPointer(Parser);
  if (T)
  {
    switch(T->Type)
    {
      case CTokenType_Tilde: // Destructor
      {
        ParseStructMemberDestructorFn(Ctx, &Result, StructNameT);
      } break;

      case CTokenType_Enum:
      {
        RequireToken(Parser, T->Type);
        type_spec TypeSpec = ParseTypeSpecifier(Ctx);
        Result.Type = type_variable_decl;
        Result.variable_decl = FinalizeVariableDecl(Ctx, &TypeSpec);
      } break;

      case CT_NameQualifier:
      {
        EatNameQualifiers(Parser);
      } [[fallthrough]];

      case CTokenType_Union:
      case CTokenType_Struct:

      case CT_Keyword_Virtual:
      case CT_Keyword_Constexpr:
      case CT_Keyword_Explicit:
      case CTokenType_OperatorKeyword:

      case CTokenType_ThreadLocal:
      case CTokenType_Const:
      case CTokenType_Static:
      case CTokenType_Volatile:
      case CTokenType_Void:
      case CTokenType_Long:
      case CTokenType_Bool:
      case CTokenType_Auto:
      case CTokenType_Double:
      case CTokenType_Float:
      case CTokenType_Char:
      case CTokenType_Int:
      case CTokenType_Short:
      case CTokenType_Unsigned:
      case CTokenType_Signed:
      case CTokenType_Identifier:
      {
        type_spec TypeSpec = ParseTypeSpecifier(Ctx, StructNameT);

        if (ParsingConstructor(Parser, &TypeSpec))
        {
          c_token *FuncNameT = RequireTokenPointer(Parser, CTokenType_Identifier);
          ParseStructMemberConstructorFn(Ctx, &TypeSpec, &Result, FuncNameT);
        }
        else if (TypeSpec.Qualifier & TypeQual_Virtual)
        {
          Result.Type = type_function_decl;
          Result.function_decl = FinalizeVirtualFunctionDecl(Ctx, Parser, &TypeSpec);
        }
        else // operator, regular function, or variable decl
        {
          Result = FinalizeDeclaration(Ctx, Parser, &TypeSpec);
        }
      } break;

      case CTokenType_Semicolon:
      case CTokenType_CloseBrace:
      {
        /* RequireToken(Parser, T->Type); */

        // Done parsing struct member, or finished with the struct
      } break;

      InvalidDefaultWhileParsing(Parser,
          FormatCountedString(GetTranArena(), CSz("Unexpected token encountered while parsing struct %S"), StructNameT->Value));
    }
  }

  return Result;
}

link_internal counted_string
ConcatTokensUntil(parser* Parser, c_token_type Close, memory_arena* Memory)
{
  NotImplemented;

  counted_string Result = {};
#if 0
  // TODO(Jesse  id: 225, tags: todos, easy): Rewrite with string_from_parser
  string_builder Builder = {};
  while (Remaining(Parser) && PeekTokenRaw(Parser).Type != Close)
  {
    Append(&Builder, PopTokenRaw(Parser).Value);
  }
  counted_string Result = Finalize(&Builder, Memory);
#endif
  return Result;
}

link_internal counted_string
ConcatTokensUntilNewline(parser* Parser, memory_arena* Memory)
{
  counted_string Result = ConcatTokensUntil(Parser, CTokenType_Newline, Memory);
  return Result;
}

#if 0
link_internal declaration_stream
MembersOfType(compound_decl* Struct, counted_string MemberType, memory_arena *Memory)
{
  declaration_stream Result = {Memory, 0, 0, 0};
  if (MemberType.Start)
  {
    ITERATE_OVER(&Struct->Members)
    {
      declaration *Member = GET_ELEMENT(Iter);
      switch (Member->Type)
      {
        case type_variable_decl:
        {
          if (StringsMatch(Member->variable_decl.Type.DatatypeToken->Value, MemberType))
          {
            Push(&Result, *Member);
          }
        } break;

        InvalidDefaultCase;
      }
    }
  }
  else
  {
    Result = Struct->Members;
  }

  return Result;
}
#endif

link_internal comma_separated_decl
ParseCommaSeperatedDecl(parse_context *Ctx)
{
  parser *Parser = Ctx->CurrentParser;
  comma_separated_decl Result = {};

  TryEatAttributes(Parser);

  Result.Indirection = ParseIndirectionInfo(Parser, 0);

  Result.NameT = RequireTokenPointer(Parser, CTokenType_Identifier);

  if ( OptionalToken(Parser, CTokenType_OpenBracket) )
  {
    ParseExpression(Ctx, &Result.StaticBufferSize );
    RequireToken(Parser, CTokenType_CloseBracket);
  }

  if ( OptionalToken(Parser, CTokenType_Equals) )
  {
    if (PeekToken(Parser).Type == CTokenType_OpenBrace)
    {
      Result.Value = ParseInitializerList(Ctx->CurrentParser, Ctx->Memory);
    }
    else
    {
      ParseExpression(Ctx, &Result.Value);
    }
  }

  return Result;
}

link_internal b32
MaybeEatAdditionalCommaSeperatedNames(parse_context *Ctx)
{
  parser *Parser = Ctx->CurrentParser;

  b32 Result = False;
  while (OptionalToken(Parser, CTokenType_Comma))
  {
    ParseCommaSeperatedDecl(Ctx);
    Result = True;
  }

  return Result;
}

link_internal b32
MaybeEatVisibilityQualifier(parser *Parser)
{
  b32 Result = False;
  c_token *T = PeekTokenPointer(Parser);
  if ( T &&
     ( T->Type == CT_Keyword_Public  ||
       T->Type == CT_Keyword_Private ||
       T->Type == CT_Keyword_Protected )
    )
  {
    RequireToken(Parser, T);
    RequireToken(Parser, CTokenType_Colon);
    Result = True;
  }

  return Result;
}

link_internal b32
MaybeEatStaticAssert(parser *Parser)
{
  b32 Result = False;
  if (OptionalToken(Parser, CT_StaticAssert))
  {
    EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
    RequireToken(Parser, CTokenType_Semicolon);
    Result = True;
  }
  return Result;
}

link_internal b32
IsAnonymous(compound_decl *Decl)
{
  b32 Result = (Decl->Type == 0);
  return Result;
}

link_internal compound_decl
ParseStructBody(parse_context *Ctx, c_token *StructNameT, poof_tag_block_array *Tags)
{
  TIMED_FUNCTION();
  parser *Parser = Ctx->CurrentParser;
  compound_decl Result = StructDef(StructNameT); //, Parser->Filename);

  RequireToken(Parser, CTokenType_OpenBrace);

  b32 Continue = True;
  while (Continue)
  {
    declaration *StoredMember = {};

    if (MaybeEatStaticAssert(Parser)) { continue; }
    if (MaybeEatVisibilityQualifier(Parser)) { continue; }

    /* poof_tag_block_array StructTags; */
    if (Tags)
    {
      TryParsePoofKeywordAndTagList(Parser, Tags);
    }

    declaration Member = ParseStructMember(Ctx, Result.Type);

    datatype *AnonymousDecl = {};
    switch (Member.Type)
    {
      case type_function_decl:
      {
        if (PeekToken(Parser).Type == '{')
        {
          Member.function_decl.Body = GetBodyTextForNextScope(Parser, Ctx->Memory);
        }
        StoredMember = Push(&Result.Members, Member);
      } break;

      case type_variable_decl:
      {
        StoredMember = Push(&Result.Members, Member);
      } break;

      case type_compound_decl:
      {
        // NOTE(Jesse): This is a pretty gross hack to make sure variables
        // declared after an anonymous compound decl can get back to their
        // declaration somehow. ie.
        //
        // struct
        // {
        //   int foo;
        // } bar, *baz;
        //
        if (IsAnonymous(&Member.compound_decl))
        {
          Info("Pushed anonymous compound decl in (%S)", StructNameT->Value);
          AnonymousDecl = Insert(Datatype(&Member), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
        }

        StoredMember = Push(&Result.Members, Member);
      } break;

      /* { */
      /*   // TODO(Jesse): Track these either on the struct def or in the global */
      /*   // hashtable?  Currently there isn't a reason to do this, but when we */
      /*   // start doing static analysis this'll come up PDQ. */
      /*   // */
      /*   // Actually there is a reason to do this already : @snap_pointer_to_struct_member_struct_decl */
      /* } break; */

      default: { Continue = False; } break;
    }

    if (Continue == False) break;

    switch (Member.Type)
    {
      case type_function_decl:
      {
        OptionalToken(Parser, CTokenType_Semicolon);
      } break;

      case type_variable_decl:
      case type_compound_decl:
      {
        if (OptionalToken(Parser, CTokenType_Semicolon)) //  int foo;
        {
        }
        else //  Parse comma-separated definitions .. ie:
             //  `int *foo, bar[42], baz = 0;`     or
             //  `struct { int foo; } bar, *baz;`
        {

          if (Member.Type == type_variable_decl)
          {
            RequireToken(Parser, CTokenType_Comma);
          }

          while (OptionalToken(Parser, CTokenType_Semicolon) == 0)
          {
            comma_separated_decl Decl = ParseCommaSeperatedDecl(Ctx);

            if (Ctx->CurrentParser->ErrorCode) break;

            switch (Member.Type)
            {
              case type_variable_decl:
              {
                auto Var = SafeAccessObjPtr(variable_decl, Member);

                Var->NameT            = Decl.NameT;
                Var->Type.Indirection = Decl.Indirection;
                Var->StaticBufferSize = Decl.StaticBufferSize;
                Var->Value = Decl.Value;

                StoredMember = Push(&Result.Members, Member);
              } break;

              case type_compound_decl:
              {
                auto StructDecl = SafeAccessObjPtr(compound_decl, Member);

                declaration TmpMember = {};
                TmpMember.Type = type_variable_decl;

                // TODO(Jesse): The compound_decl here is all stored on the stack
                // .. we've got to store it in a hashtable somewhere such that
                // we can snap pointers to it here.
                //
                // @snap_pointer_to_struct_member_struct_decl
                // TmpMember.variable_decl.TypeSpec.Datatype = Datatype(StructDecl);

                TmpMember.variable_decl.NameT            = Decl.NameT;
                TmpMember.variable_decl.Type.Indirection = Decl.Indirection;
                TmpMember.variable_decl.StaticBufferSize = Decl.StaticBufferSize;
                TmpMember.variable_decl.Value            = Decl.Value;
                TmpMember.variable_decl.Value            = Decl.Value;

                if (AnonymousDecl)
                {
                  Assert(AnonymousDecl->Type == type_declaration);
                  Assert(AnonymousDecl->declaration.Type == type_compound_decl);
                  Assert(IsAnonymous(&AnonymousDecl->declaration.compound_decl));
                }
                TmpMember.variable_decl.Type.BaseType    = AnonymousDecl;

                StoredMember = Push(&Result.Members, TmpMember);
              } break;

              case type_enum_decl:
              {
                InternalCompilerError(Parser, CSz("Somehow parsed an enum while parsing comma seperated values"), PeekTokenPointer(Parser));
              } break;

              case type_function_decl:
              {
                InternalCompilerError(Parser, CSz("Somehow parsed a function while parsing comma seperated values"), PeekTokenPointer(Parser));
              } break;

              case type_declaration_noop:
              {
                InternalCompilerError(Parser, CSz("Yikes"), PeekTokenPointer(Parser));
              } break;
            }

            OptionalToken(Parser, CTokenType_Comma);
          }
        }

      } break;

      default: { Continue = False; } break;
    }

    // Parse tags for the member
    TryParsePoofKeywordAndTagList(Parser, &StoredMember->Tags);
  }

  RequireToken(Parser, CTokenType_CloseBrace);

  return Result;
}

link_internal void
ParseEnumBody(parse_context *Ctx, parser *Parser, enum_decl *Enum)
{
  RequireToken(Parser, CTokenType_OpenBrace);

  Assert(Enum->Members.Memory == 0);
  Enum->Members.Memory = &Global_PermMemory;

  b32 Done = False;
  while (!Done && TokensRemain(Parser))
  {
    if (OptionalToken(Parser, CTokenType_Poof))
    {
      EatBetween(Parser, c_token_type('('), c_token_type(')'));
    }

    if (c_token *NameT = OptionalToken(Parser, CTokenType_Identifier))
    {
      enum_member Field = {};
      Field.NameT = NameT;

      if (OptionalToken(Parser, CTokenType_Equals))
      {
        Field.Expr = ParseExpression(Ctx);
      }

      TryParsePoofKeywordAndTagList(Parser, &Field.Tags);

      Push(&Enum->Members, Field);

      if(OptionalToken(Parser, CTokenType_Comma))
      {
        if (OptionalToken(Parser, CTokenType_CloseBrace))
        {
          Done = True;
        }
      }
      else
      {
        RequireToken(Parser, CTokenType_CloseBrace);
        Done = True;
      }
    }
    else
    {
      break;
    }
  }

  return;
}

link_internal enum_decl
ParseEnum(parse_context *Ctx, type_spec *TypeSpec)
{
  TIMED_FUNCTION();

  parser *Parser = Ctx->CurrentParser;
  program_datatypes *Datatypes = &Ctx->Datatypes;

  c_token *EnumNameT = TypeSpec->DatatypeToken;
  /* counted_string EnumName = EnumNameT ? EnumNameT->Value : CSz("(anonymous)"); */

  enum_decl Enum = {};
  Enum.NameT = EnumNameT ? EnumNameT : &Global_DefaultNameToken;

  ParseEnumBody(Ctx, Parser, &Enum);

  return Enum;
}

link_internal b32
IsTypeQualifier(c_token T)
{
  b32 Result = False;
  switch (T.Type)
  {
    case CTokenType_LT:
    case CTokenType_Ampersand:
    case CTokenType_Star:
    { Result = True; } break;

    default : {} break;
  }

  return Result;
}

link_internal void
OptionalPrefixOperator(parser *Parser)
{
  c_token T = PeekToken(Parser);
  if ( T.Type == CTokenType_Increment ||
       T.Type == CTokenType_Decrement ||
       T.Type == CTokenType_Ampersand ||
       T.Type == CTokenType_Star
       )
  {
    PopToken(Parser);
  }
}

link_internal void
OptionalPostfixOperator(parser *Parser)
{
  c_token T = PeekToken(Parser);
  if ( T.Type == CTokenType_Increment ||
       T.Type == CTokenType_Decrement)
  {
    PopToken(Parser);
  }
}

link_internal void
ParseAndPushTypedef(parse_context *Ctx)
{
  parser *Parser = Ctx->CurrentParser;

  type_spec Type = ParseTypeSpecifier(Ctx);

  c_token *AliasT = {};
  counted_string Alias = {};

  if (Type.Indirection.IsFunction)
  {
    Alias = Type.DatatypeToken->Value;
  }
  else
  {
    if (PeekToken(Parser).Type == CTokenType_OpenBrace)
    {
      if (Type.Qualifier & TypeQual_Struct)
      {
        compound_decl S = ParseStructBody(Ctx, 0, 0);
        S.Members.Memory = &Global_PermMemory;
        // @dup_typedefd_anon_thingy_name_token
        comma_separated_decl Decl = ParseCommaSeperatedDecl(Ctx);
        S.Type = Decl.NameT;
        Insert(Datatype(&S), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
      }
      else if (Type.Qualifier & TypeQual_Enum)
      {
        enum_decl Enum = {};
        ParseEnumBody(Ctx, Parser, &Enum);
        // @dup_typedefd_anon_thingy_name_token
        comma_separated_decl Decl = ParseCommaSeperatedDecl(Ctx);
        Enum.NameT = Decl.NameT;

        Insert(Datatype(&Enum), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
      }
      else
      {
        InvalidCodePath();
      }

      TryEatAttributes(Parser);

      MaybeEatAdditionalCommaSeperatedNames(Ctx);

      return;
    }
    else
    {
      AliasT = RequireTokenPointer(Parser, CTokenType_Identifier);
      Alias = AliasT->Value;

      if (PeekToken(Parser).Type == CTokenType_OpenParen)
      {
        // TODO(Jesse): This is pretty half-baked and probably should be represented
        // differently.  I just hacked it in here to get typedef'd funcs to parse.
        Type.Indirection.IsFunction = True;
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      }
    }

  }

  MaybeEatAdditionalCommaSeperatedNames(Ctx);

  TryEatAttributes(Parser);

  if ( OptionalToken(Parser, CTokenType_OpenBracket) )
  {
    // Assert(False); // Pretty sure this path should be dead
    //
    // Turns out this codepath isn't dead.  Just braindead.
    //
    // lol
    variable_decl Tmp = {};
    ParseExpression(Ctx, &Tmp.StaticBufferSize );
    RequireToken(Parser, CTokenType_CloseBracket);
  }

  RequireToken(Parser, CTokenType_Semicolon);

  Assert(Alias.Count);
  type_def Typedef = {
    Alias,
    Type,
  };

  if (Type.Indirection.IsFunction || Type.Indirection.IsFunctionPtr)
  {
    function_decl Func = {};
    Func.Type = function_type_normal;
    Func.ReturnType = Type;

    if (AliasT)
    {
      Func.NameT = AliasT;
    }
    else
    {
      Func.NameT = Type.DatatypeToken;
    }

    Assert(Func.NameT);

    Info("Pushing function decl (%S)", Func.NameT ? Func.NameT->Value : CSz("anonymous"));
    Insert(Datatype(&Func), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
  }
  else
  {
    Info("Pushing typedef decl (%S) -> (%S)", PrintTypeSpec(&Typedef.Type, GetTranArena()), Typedef.Alias);
    Insert(Datatype(&Typedef), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);

#if BONSAI_INTERNAL
    {
      /* datatype *D = GetDatatypeByName(&Ctx->Datatypes.DatatypeHashtable, Typedef.Alias); */
      /* Assert(D->Type == type_type_def); */
      /* Assert(D->type_deb */
    }
#endif
    /* Push(&Ctx->Datatypes.Typedefs, Typedef); */
  }
}

link_internal void
ParseTypedef(parse_context *Ctx)
{
  TIMED_FUNCTION();

  parser *Parser = Ctx->CurrentParser;

  RequireToken(Parser, CTokenType_Typedef);

  // @optimize_call_advance_instead_of_being_dumb
  c_token *Peek = PeekTokenPointer(Parser);
  if ( Peek->Type == CTokenType_Struct || Peek->Type == CTokenType_Union )
  {
    RequireToken(Parser, Peek);

    if (PeekToken(Parser).Type == CTokenType_OpenBrace)
    {
      compound_decl S = ParseStructBody(Ctx, 0, 0);

      comma_separated_decl Decl = ParseCommaSeperatedDecl(Ctx);
      S.Type = Decl.NameT;
      TryEatAttributes(Parser);

      MaybeEatAdditionalCommaSeperatedNames(Ctx);

      while (OptionalToken(Parser, CTokenType_Comma))
      {
        comma_separated_decl Alias = ParseCommaSeperatedDecl(Ctx);
      }

      RequireToken(Parser, CTokenType_Semicolon);

      Insert(Datatype(&S), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
      /* Push(&Ctx->Datatypes.Structs, S); */
    }
    else if ( PeekToken(Parser, 0).Type == CTokenType_Identifier &&
              PeekToken(Parser, 1).Type == CTokenType_OpenBrace )
    {
      comma_separated_decl Decl = ParseCommaSeperatedDecl(Ctx);
      compound_decl S = ParseStructBody(Ctx, Decl.NameT, 0);

      /* while (OptionalToken(Parser, CTokenType_Comma)) */
      /* { */
      /*   ParseCommaSeperatedDecl(Ctx); */
      /*   Result = True; */
      /* } */

      /* comma_separated_decl ActualName = */ ParseCommaSeperatedDecl(Ctx);
      MaybeEatAdditionalCommaSeperatedNames(Ctx);

      RequireToken(Parser, CTokenType_Semicolon);
      Insert(Datatype(&S), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
      /* Push(&Ctx->Datatypes.Structs, S); */
    }
    else
    {
      ParseAndPushTypedef(Ctx);
    }
  }
  else if (Peek->Type == CTokenType_Enum)
  {
    ParseAndPushTypedef(Ctx);
  }
  else
  {
    ParseAndPushTypedef(Ctx);
  }

  return;
}

link_internal function_decl*
GetByName(counted_string Name, function_decl_stream* Stream)
{
  TIMED_FUNCTION();

  function_decl *Result = {};
  ITERATE_OVER(Stream)
  {
    auto *Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->NameT->Value, Name))
    {
      Result = Current;
      break;
    }
  }

  return Result;
}

link_internal ast_node_variable_def*
GetByTypeName(counted_string Name, ast_node_variable_def_stream* Stream)
{
  ast_node_variable_def* Result = {};
  ITERATE_OVER(Stream)
  {
    ast_node_variable_def* Current = GET_ELEMENT(Iter);
    if (Current->Decl.NameT)
    {
      if (StringsMatch(Current->Decl.NameT->Value, Name))
      {
        Result = Current;
        break;
      }
    }
  }
  return Result;
}

link_internal ast_node *
ParseFunctionArgument(parser *Parser, memory_arena *Memory, function_decl_stream *FunctionPrototypes);

link_internal ast_node_statement *
ParseAllStatements(parse_context *Ctx);

link_internal void
ParseSingleStatement(parse_context *Ctx, ast_node_statement *Result)
{
  parser *Parser = Ctx->CurrentParser;

  b32 Done = False;
  while (!Done && TokensRemain(Parser))
  {
    c_token *T = PeekTokenPointer(Parser);
    switch (T->Type)
    {
      case CTokenType_IntLiteral:
      case CTokenType_DoubleLiteral:
      case CTokenType_FloatLiteral:
      {
        Result->LHS = ParseExpression(Ctx);
      } break;

      case CTokenType_Identifier:
      {
        if (Result->LHS)
        {
          DebugPrint(Result);
          InternalCompilerError(Parser, CSz("Unable to parse L-value, LHS of expression already set!"), T);
        }

        Result->LHS = ParseExpression(Ctx);
      } break;

      case CTokenType_Equals:
      {
        RequireToken(Parser, T->Type);
        if (PeekToken(Parser).Type == CTokenType_OpenBrace)
        {
          // Initializer List
          EatBetween(Parser, CTokenType_OpenBrace, CTokenType_CloseBrace);
        }
        else
        {
          Assert(!Result->RHS);
          Result->RHS = AllocateProtection(ast_node_statement, Ctx->Memory, 1, False);
          ParseSingleStatement(Ctx, Result->RHS);
        }
        Done = True;
      } break;

      case CTokenType_For:
      {
        RequireToken(Parser, T->Type);
#if 0
        RequireToken(Parser, CTokenType_OpenParen);
        Result->Next = AllocateProtection(ast_node_statement, Memory, 1, False);
        ParseSingleStatement(Parser, Memory, Datatypes, Result->Next);
        ParseExpression(Ctx);
        RequireToken(Parser, CTokenType_Semicolon);
        ParseExpression(Ctx);
        RequireToken(Parser, CTokenType_CloseParen);
#else
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
#endif
        Done = True;
      } break;

      case CTokenType_If:
      case CTokenType_While:
      case CTokenType_Switch:
      {
        RequireToken(Parser, T->Type);
        Result->LHS = ParseExpression(Ctx);
        Done = True;
      } break;

      case CTokenType_Case:
      {
        RequireToken(Parser, CTokenType_Case);
        Result->LHS = ParseExpression(Ctx);
        RequireToken(Parser, CTokenType_Colon);
        Done = True;
      } break;

      case CTokenType_Default:
      {
        RequireToken(Parser, CTokenType_Default);
        RequireToken(Parser, CTokenType_Colon);
      } break;

      // What do we do with these?
      case CTokenType_Else:
      case CTokenType_Continue:
      {
        RequireToken(Parser, T->Type);
      } break;

      case CTokenType_Return:
      {
        RequireToken(Parser, T->Type);
        Result->LHS = ParseExpression(Ctx);
      } break;

      case CTokenType_OpenBrace:
      {
        RequireToken(Parser, CTokenType_OpenBrace);
        Result->RHS = ParseAllStatements(Ctx);
        Done = True;
      } break;

      case CTokenType_ThreadLocal:
      case CTokenType_Const:
      case CTokenType_Static:
      case CTokenType_Volatile:
      case CTokenType_Void:
      case CTokenType_Bool:
      case CTokenType_Auto:
      case CTokenType_Double:
      case CTokenType_Float:
      case CTokenType_Char:
      case CTokenType_Int:
      case CTokenType_Short:
      case CTokenType_Unsigned:
      case CTokenType_Signed:

      case CTokenType_CharLiteral:
      case CTokenType_StringLiteral:

      case CTokenType_Minus:
      case CTokenType_LeftShift:
      case CTokenType_RightShift:
      case CTokenType_Increment:
      case CTokenType_Decrement:

      case CTokenType_Bang:
      case CTokenType_Ampersand:
      case CTokenType_Tilde:
      case CTokenType_Star:
      case CTokenType_OpenParen:
      {
        if (Result->LHS)
        {
          DebugPrint(Result);
          InternalCompilerError(Parser, CSz("Unable to parse L-value, LHS of expression already set!"), T);
        }
        else
        {
          Result->LHS = ParseExpression(Ctx);
        }
        Done = True;
      } break;

      case CTokenType_Colon:
      case CTokenType_CloseParen:
      case CTokenType_Break:
      case CTokenType_CloseBrace:
      case CTokenType_Semicolon:
      {
        RequireToken(Parser, T->Type);
        Done = True;
      } break;

      case CTokenType_Goto:
      {
        RequireToken(Parser, T->Type);
        RequireToken(Parser, CTokenType_Identifier);
      } break;

      case CTokenType_Poof:
      {
        RequireToken(Parser, T->Type);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      case CTokenType_Asm:
      {
        RequireToken(Parser, T->Type);
        OptionalToken(Parser, CTokenType_Volatile);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      InvalidDefaultError(Parser, CSz("Invalid token encountered while parsing statement."), T);
    }
  }
}

link_internal ast_node_statement*
ParseSingleStatement(parse_context *Ctx)
{
  ast_node_statement *Result = AllocateProtection(ast_node_statement, Ctx->Memory, 1, False);
  ParseSingleStatement(Ctx, Result);
  return Result;
}

link_internal b32
IsAtBeginning(parser *Parser)
{
  b32 Result = Parser->Tokens->At == Parser->Tokens->Start && Parser->Tokens->Up.Tokens == 0;
  return Result;
}

link_internal ast_node_statement*
ParseAllStatements(parse_context *Ctx)
{
  ast_node_statement *Result = 0;

  ast_node_statement **Current = &Result;

  parser *Parser = Ctx->CurrentParser;

  Assert(IsAtBeginning(Parser));

  while ( TokensRemain(Ctx->CurrentParser) )
  {
    Assert(*Current == 0);

    *Current = ParseSingleStatement(Ctx);
    while (*Current) { Current = &(*Current)->Next; } // Walk to the end of the statement chain.
  }

  return Result;
}

link_internal ast_node*
ParseAllStatements_ast_node(parse_context *Ctx)
{
  ast_node *Result = Allocate(ast_node, Ctx->Memory, 1);
  Result->Type = type_ast_node_statement;

  ast_node_statement *Foo = &Result->ast_node_statement;
  ast_node_statement **Current = &Foo;

  while ( TokensRemain(Ctx->CurrentParser) )
  {
    Assert(*Current == 0);

    *Current = ParseSingleStatement(Ctx);
    while (*Current) { Current = &(*Current)->Next; } // Walk to the end of the statement chain.
  }

  return Result;
}

link_internal ast_node*
ParseFunctionCall(parse_context *Ctx, counted_string FunctionName);

link_internal void
ParseTypeSpecifierNode(parse_context *Ctx, ast_node_expression *Result, datatype *Data = 0)
{
  ast_node_type_specifier *Node = AllocateAndCastTo(ast_node_type_specifier, &Result->Value, Ctx->Memory);

  Node->TypeSpec = ParseTypeSpecifier(Ctx);

  if (Data)
  {
    Node->Datatype = *Data;
    Assert(Node->Datatype.Type != type_datatype_noop);
  }
  else
  {
    c_token *DatatypeToken = Node->TypeSpec.DatatypeToken;
    if (DatatypeToken && DatatypeToken->Type == CTokenType_Identifier)
    {
      // TODO(Jesse): Save a pointer here probably..
      Node->Datatype = *GetDatatypeByName(&Ctx->Datatypes, Node->TypeSpec.DatatypeToken->Value);
      if (Node->Datatype.Type == type_datatype_noop)
      {
        // Type-checking failed.
      }
    }
    else
    {
      // Primitive type
    }
  }

  Node->Name = ParseExpression(Ctx);

  if (Node->Name->Value)
  {
    switch (Node->Name->Value->Type)
    {
      case type_ast_node_function_call: // Constructor bonsai_function
      case type_ast_node_symbol:        // Regular variable definition
      {
      } break;

      // This case should go away once we can check what local varaibles are defined for the scope we're parsing
      case type_ast_node_access:
      {
        BUG("Skipped symbol resolution in ParseTypeSpecifierNode. @not_pushing_variable_decls ");
      } break;

      InvalidDefaultWhileParsing(Ctx->CurrentParser, CSz("Invalid syntax following type-specifier."));
    }
  }
  else
  {
    // Cast or macro bonsai_function call
  }
}

link_internal b32
ParseSymbol(parse_context *Ctx, ast_node_expression* Result, b32 SymbolIsNeverTypeSpecifier = True)
{
  parser *Parser = Ctx->CurrentParser;

  b32 PushedParser = False;

  c_token T = PeekToken(Parser);
  Assert(T.Type == CTokenType_Identifier);

#if 0
  macro_def *Macro = GetByName(&Ctx->Datatypes.Macros, T.Value);
  if (Macro)
  {
    c_token MacroNameToken = RequireToken(Parser, CTokenType_Identifier);
    switch (Macro->Type)
    {
      case type_macro_keyword:
      {
        PushParser(Parser, Macro->Parser, parser_push_type_macro);
        PushedParser = True;
      } break;

      case type_macro_function:
      {
        if (PeekToken(Parser).Type == CTokenType_OpenParen)
        {
          Assert(Result->Value == 0);
          Result->Value = ParseFunctionCall(Ctx, MacroNameToken.Value);
        }
        else
        {
          ast_node_symbol *Node = AllocateAndCastTo(ast_node_symbol, &Result->Value, Ctx->Memory);
          Node->Token = MacroNameToken;
        }
      } break;

      case type_macro_noop: { InvalidCodePath(); } break;
    }

    return PushedParser;
  }
#endif

  if ( PeekToken(Parser, 1).Type  == CTokenType_OpenParen )
  {
    c_token SymbolToken = RequireToken(Parser, CTokenType_Identifier);
    Assert(!Result->Value);
    Result->Value = ParseFunctionCall(Ctx, SymbolToken.Value);
  }
  else
  {
    datatype *TestDatatype = GetDatatypeByName(&Ctx->Datatypes, T.Value);
    if ( SymbolIsNeverTypeSpecifier || TestDatatype->Type == type_datatype_noop )
    {
      ast_node_symbol *Node = AllocateAndCastTo(ast_node_symbol, &Result->Value, Ctx->Memory);
      Node->Token = RequireToken(Parser, CTokenType_Identifier);
    }
    else
    {
      ParseTypeSpecifierNode(Ctx, Result, TestDatatype);
    }
  }

  return PushedParser;
}

link_internal void
ParseExpression(parse_context *Ctx, ast_node_expression* Result)
{
  parser *Parser = Ctx->CurrentParser;

  b32 ParsePushedParser = False;
  do
  {
    ParsePushedParser = False;
    c_token *T = PeekTokenPointer(Parser);
    switch (T->Type)
    {
      case CTokenType_Poof:
      {
        RequireToken(Parser, CTokenType_Poof);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      case CTokenType_IntLiteral:
      case CTokenType_DoubleLiteral:
      case CTokenType_FloatLiteral:
      {
        ast_node_literal *Node = AllocateAndCastTo(ast_node_literal, &Result->Value, Ctx->Memory);
        Node->Token = RequireToken(Parser, T->Type);
      } break;

      case CTokenType_Struct:
      case CTokenType_Enum:
      case CTokenType_Union:
      {
        RequireToken(Parser, T->Type);
      } [[fallthrough]];

      case CTokenType_Identifier:
      {
        ParsePushedParser = ParseSymbol(Ctx, Result, False);
      } break;

      case CTokenType_Arrow:
      case CTokenType_Dot:
      {
        ast_node_access *Node = AllocateAndCastTo(ast_node_access, &Result->Value, Ctx->Memory);
        Node->AccessType = RequireToken(Parser, T->Type);

        Node->Symbol = AllocateProtection(ast_node_expression, Ctx->Memory, 1, False);
        ParsePushedParser = ParseSymbol(Ctx, Node->Symbol);
      } break;

      case CTokenType_ThreadLocal:
      case CTokenType_Const:
      case CTokenType_Static:
      case CTokenType_Volatile:
      case CTokenType_Void:
      case CTokenType_Long:
      case CTokenType_Bool:
      case CTokenType_Auto:
      case CTokenType_Double:
      case CTokenType_Float:
      case CTokenType_Char:
      case CTokenType_Int:
      case CTokenType_Short:
      case CTokenType_Unsigned:
      case CTokenType_Signed:
      {
        ParseTypeSpecifierNode(Ctx, Result);
      } break;

      case CTokenType_OpenParen:
      {
        RequireToken(Parser, T->Type);
        ast_node_parenthesized *Node = AllocateAndCastTo(ast_node_parenthesized, &Result->Value, Ctx->Memory);
        Node->Expr = ParseExpression(Ctx);
        RequireToken(Parser, CTokenType_CloseParen);

        if (Node->Expr->Value)
        {
          if (Node->Expr->Value->Type == type_ast_node_type_specifier)
          {
            Node->IsCast = True;
            Node->CastValue = ParseExpression(Ctx);
          }
        }
        else
        {
          PoofSyntaxError(Parser, CSz("Parens must enclose a valid expression!"), T);
        }


      } break;

      case CTokenType_Ampersand:
      case CTokenType_Tilde:
      case CTokenType_Hat:
      case CTokenType_LT:
      case CTokenType_GT:
      case CTokenType_Bang:
      case CTokenType_LeftShift:
      case CTokenType_RightShift:
      case CTokenType_LessEqual:
      case CTokenType_GreaterEqual:
      case CTokenType_AreEqual:
      case CTokenType_NotEqual:
      case CTokenType_PlusEquals:
      case CTokenType_MinusEquals:
      case CTokenType_TimesEquals:
      case CTokenType_DivEquals:
      case CTokenType_ModEquals:
      case CTokenType_AndEquals:
      case CTokenType_OrEquals:
      case CTokenType_XorEquals:
      case CTokenType_Increment:
      case CTokenType_Decrement:
      case CTokenType_LogicalAnd:
      case CTokenType_LogicalOr:
      case CTokenType_Percent:
      case CTokenType_Pipe:
      case CTokenType_Plus:
      case CTokenType_Minus:
      case CTokenType_FSlash:
      case CTokenType_Star:
      case CTokenType_OpenBracket:
      {
        Assert(!Result->Value);
        ast_node_operator* Node = AllocateAndCastTo(ast_node_operator, &Result->Value, Ctx->Memory);
        Node->Token = RequireToken(Parser, T->Type);
        Node->Operand = ParseExpression(Ctx);
        if (T->Type == CTokenType_OpenBracket)
          { RequireToken(Parser, CTokenType_CloseBracket); }
      } break;

      case CTokenType_Question:
      {
        ast_node_operator* Node = AllocateAndCastTo(ast_node_operator, &Result->Value, Ctx->Memory);
        Node->Token = RequireToken(Parser, T->Type);
        Node->Operand = ParseExpression(Ctx);
        RequireToken(Parser, CTokenType_Colon);
        ParseExpression(Ctx); // TODO(Jesse id: 260): Currently ignoring the second half of a ternary .. we should probably not do this
      } break;

      case CTokenType_CharLiteral:
      case CTokenType_StringLiteral:
      {
        Assert(!Result->Value);
        ast_node_literal *Node = AllocateAndCastTo(ast_node_literal, &Result->Value, Ctx->Memory);
        Node->Token = RequireToken(Parser, T->Type);

        while ( PeekToken(Parser).Type == CTokenType_StringLiteral ||
                PeekToken(Parser).Type == CTokenType_Identifier )
        {
          c_token NextT = PopToken(Parser);
          switch (NextT.Type)
          {
            case CTokenType_StringLiteral:
            {
              Node->Token = CToken(CTokenType_StringLiteral, Concat(Node->Token.Value, NextT.Value, Ctx->Memory)); // TODO(Jesse, id: 264, tags: memory_leak): This should use a string builder
            } break;

            case CTokenType_Identifier:
            {
              NotImplemented;

              // TODO(Jesse id: 264): Once we have proper macro expansion, this can be expanded and concatenated to the string as well.
              macro_def *Macro = GetByName(&Ctx->Datatypes.Macros, NextT.Value);
              switch(Macro->Type)
              {
                case type_macro_keyword:
                {
                } break;

                case type_macro_function:
                {
                  EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
                } break;

                case type_macro_noop: { InvalidCodePath(); } break;
              }
            } break;

            InvalidDefaultWhileParsing(Parser, CSz("Expected macro identifier or string literal."));
          }
        }
      } break;

      case CTokenType_Colon:
      case CTokenType_Semicolon:
      case CTokenType_Comma:
      case CTokenType_CloseBracket:
      case CTokenType_CloseParen:
      case CTokenType_Equals:
      {
      } break;

      InvalidDefaultWhileParsing(Parser, CSz("Invalid token while parsing expression."));
    }
  } while (ParsePushedParser);

  if (NextTokenIsOperator(Parser))
  {
    Assert(!Result->Next);
    Result->Next = ParseExpression(Ctx);
  }
}

// TODO(Jesse id: 316): These paths are exactly the same .. they should probably use the same API ..?
link_internal ast_node_expression*
ParseExpression(parse_context *Ctx)
{
  ast_node_expression *Result = AllocateProtection(ast_node_expression, Ctx->Memory, 1, False);
  ParseExpression(Ctx, Result);
  return Result;
}

link_internal void
ParseExpression(parse_context *Ctx, ast_node **Result)
{
  ast_node_expression *Node = AllocateAndCastTo(ast_node_expression, Result, Ctx->Memory);
  ParseExpression(Ctx, Node);
}

#if 0
link_internal type_spec
ReduceToTypeSpec(ast_node* InputNode, ast_node_variable_def_stream *Locals)
{
  type_spec Result = {};

  ast_node* Current = InputNode;

  while (Current)
  {
    switch (Current->Type)
    {
      InvalidDefaultCase;

      case type_ast_node_symbol:
      {
        ast_node_symbol *Symbol = SafeCast(ast_node_symbol, Current);
        Assert(Symbol->Token.Type == CTokenType_Identifier);
        ast_node_variable_def *LocalVarDecl = GetByTypeName(Symbol->Token.Value, Locals);

        if (ResultType->Name.Count)
        {
          if (LocalVarDecl)
          {
            if (StringsMatch(ResultType->Name, LocalVarDecl->Decl.Type.Name))
            {
            }
            else
            {
              Error("Type mismatch");
              RuntimeBreak();
            }
          }
          else
          {
            Error("Unable to find local variable decl");
          }
        }
        else
        {
          if (LocalVarDecl)
          {
            ResultType->Name = LocalVarDecl->Decl.Type.Name;
          }
          else
          {
            Error("Unable to find local variable decl");
          }
        }

      } break;

      case type_ast_node_function_call:
      {
        ast_node_function_call *Node = SafeCast(ast_node_function_call, Current);
      } return;

      case type_ast_node_scope:
      {
      } break;

      case type_ast_node_ignored:
      {
      } break;
    }
  }

  return Result;
}
#endif

link_internal ast_node*
ParseFunctionCall(parse_context *Ctx, cs FunctionName)
{
  parser *Parser = Ctx->CurrentParser;

  ast_node *Result = {};
  ast_node_function_call *Node = AllocateAndCastTo(ast_node_function_call, &Result, Ctx->Memory);

  Node->Name = FunctionName;
  Node->Args.Memory = &Global_PermMemory;

  datatype *D = GetDatatypeByName(&Ctx->Datatypes, Node->Name);
  Node->Prototype = TryCastToFunctionDecl(Ctx, D);

  // TODO(Jesse): Emit error here?
  if (Node->Prototype == 0)
  {
    // For now, because we don't have a sizeof token, we have to settle for a warning
    // @add_sizeof_keyword
    Warn("Couldn't find definition for function (%S)", FunctionName);
  }

  RequireToken(Parser, CTokenType_OpenParen);

  b32 Done = False;
  while (!Done)
  {
    ast_node_expression Arg = {};
    ParseExpression(Ctx, &Arg);
    Push(&Node->Args, Arg);

    if(OptionalToken(Parser, CTokenType_Comma))
    {
    }
    else
    {
      RequireToken(Parser, CTokenType_CloseParen);
      Done = True;
    }
  }


  return Result;
}

link_internal declaration
FinalizeDeclaration(parse_context *Ctx, parser *Parser, type_spec *TypeSpec)
{
  TIMED_FUNCTION();
  declaration Result = {};

  b32 IsConstructor = ParsingConstructor(Parser, TypeSpec);

  if (OptionalToken(Parser, CT_ScopeResolutionOperator))
  {
    NotImplemented;
  }

  TryParsePoofKeywordAndTagList(Parser, &Result.Tags);

  // We know anything with a name is either a variable or function
  if (c_token *NameT = OptionalToken(Parser, CTokenType_Identifier))
  {
    if (PeekToken(Parser).Type == CTokenType_LT)
    {
      EatBetween(Parser, CTokenType_LT, CTokenType_GT);
    }

    if (PeekToken(Parser).Type == CTokenType_OpenParen)
    {
      Result.Type = type_function_decl;
      function_type Type = IsConstructor ? function_type_constructor : function_type_normal;

      // TODO(Jesse): Fix this function such that it records indirection info from return value.
      // Result.function_decl = FinalizeFunctionDecl(Ctx, &TypeSpec, &Indirection, NameT, Type);
      Result.function_decl = FinalizeFunctionDecl(Ctx, TypeSpec, NameT, Type);
    }
    else
    {
      Result.Type = type_variable_decl;
      Result.variable_decl = FinalizeVariableDecl(Ctx, TypeSpec, NameT); // Globally-scoped variable : `struct foo = { .bar = 1 }`
    }

  }
  else if (OptionalToken(Parser, CTokenType_OperatorKeyword))
  {
    Result.Type = type_function_decl;
    Result.function_decl = FinalizeOperatorFunction(Ctx, TypeSpec);
  }
  else if ( TypeSpec->Qualifier & TypeQual_Struct ||
            TypeSpec->Qualifier & TypeQual_Class   )
  {
    if ( PeekToken(Parser).Type == CTokenType_OpenBrace ||
         PeekToken(Parser).Type == CTokenType_Colon      )
    {

      // TODO(Jesse): Is this actually valid for structs, or should structs and
      // unions take the same path?
      if (OptionalToken(Parser, CTokenType_Colon))
      {
        EatUntilExcluding(Parser, CTokenType_OpenBrace);
      }

      Result.Type = type_compound_decl;
      Result.compound_decl = ParseStructBody(Ctx, TypeSpec->DatatypeToken, &Result.Tags);
    }
    else
    {
      // struct foo;
      //
      // template<typename foo>
      // struct bar;
    }
  }
  else if (TypeSpec->Qualifier & TypeQual_Union) // union { ... }
  {
    Result.Type = type_compound_decl;
    Result.compound_decl = ParseStructBody(Ctx, TypeSpec->DatatypeToken, &Result.Tags);
    Result.compound_decl.IsUnion = True;
  }
  else if (TypeSpec->Qualifier & TypeQual_Enum) // enum { ... }
  {
    Result.Type = type_enum_decl;
    Result.enum_decl = ParseEnum(Ctx, TypeSpec);
  }
  else if (IsConstructor)  // my_thing::my_thing(...) {...}
  {
    // TODO(Jesse): Maybe this should be handled above ?
    Result.Type = type_function_decl;

    c_token *ConstructorNameT = RequireTokenPointer(Parser, CTokenType_Identifier);
    if (PeekToken(Parser).Type == CTokenType_LT)
    {
      EatBetween(Parser, CTokenType_LT, CTokenType_GT);
    }

    Result.function_decl = ParseFunctionParameterList( Ctx,
                                                       TypeSpec,
                                                       ConstructorNameT,
                                                       function_type_constructor,
                                                       TypeSpec->QualifierNameT);

    Result.function_decl.Body = MaybeParseFunctionBody(Parser, Ctx->Memory);
  }
  else if ( TypeSpec->Indirection.IsFunction || TypeSpec->Indirection.IsFunctionPtr )
  {
    // TODO(Jesse, correctness, cleanup): Should this not get hit for regular
    // functions too ..??  Or does this path just never get hit?
    Result.Type = type_variable_decl;
    Result.variable_decl.Type = *TypeSpec;
  }
  else
  {
    ParseError( Parser,
                ParseErrorCode_MalformedType,
                CSz("Malformed type specifier near here."),
                PeekTokenPointer(Parser, -1));
  }

  return Result;
}

link_internal declaration
ParseDeclaration(parse_context *Ctx)
{
  TIMED_FUNCTION();
  parser *Parser = Ctx->CurrentParser;

  type_spec TypeSpec = ParseTypeSpecifier(Ctx);
  declaration Result = FinalizeDeclaration(Ctx, Parser, &TypeSpec);

  return Result;
}

link_internal void
ParseDatatypes(parse_context *Ctx, parser *Parser)
{
  TIMED_FUNCTION();

  Assert(Parser == Ctx->CurrentParser);

  program_datatypes* Datatypes = &Ctx->Datatypes;
  memory_arena* Memory = Ctx->Memory;

  while ( c_token *T = PeekTokenPointer(Parser) )
  {
    switch(T->Type)
    {
      case CT_KeywordPragma:
      case CTokenType_Poof:
      {
        RequireToken(Parser, T->Type);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      case CT_StaticAssert:
      {
        RequireToken(Parser, CT_StaticAssert);
        EatBetween(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
        RequireToken(Parser, CTokenType_Semicolon);
      } break;

      case CTokenType_Semicolon:
      {
        RequireToken(Parser, CTokenType_Semicolon);
      } break;

      case CTokenType_TemplateKeyword:
      {
        RequireToken(Parser, CTokenType_TemplateKeyword);
        EatBetween(Parser, CTokenType_LT, CTokenType_GT);
      } break;

      case CTokenType_Typedef:
      {
        ParseTypedef(Ctx);
      } break;

      case CTokenType_Using:
      {
        EatUntilIncluding(Parser, CTokenType_Semicolon);
      } break;


      case CT_NameQualifier:
      case CTokenType_Struct:
      case CTokenType_Enum:
      case CTokenType_Union:
      case CT_Keyword_Class:

      case CT_Keyword_Noreturn:

      case CTokenType_OperatorKeyword:
      case CT_Keyword_Explicit:
      case CT_Keyword_Constexpr:
      case CTokenType_Extern:
      case CTokenType_Inline:
      case CTokenType_ThreadLocal:
      case CTokenType_Const:
      case CTokenType_Static:
      case CTokenType_Volatile:
      case CTokenType_Void:
      case CTokenType_Long:
      case CTokenType_Bool:
      case CTokenType_Auto:
      case CTokenType_Double:
      case CTokenType_Float:
      case CTokenType_Char:
      case CTokenType_Int:
      case CTokenType_Short:
      case CTokenType_Unsigned:
      case CTokenType_Signed:
      case CTokenType_Identifier:
      {
        TIMED_NAMED_BLOCK(TopLevelDeclParse);
        declaration Decl = ParseDeclaration(Ctx);

        if (Decl.Type)
        {
          datatype DeclDT = Datatype(&Decl);
          // TODO(Jesse)(not_pushing_variable_decls): Enable this, and disable the other ones at @not_pushing_variable_decls
          //
          // Enabling this uncovers a bug in type-checking, also noted with @not_pushing_variable_decls
          //
          // There might also be another/other bugs because after that happened
          // I was hitting invalid case in that same switch statement.
          //
          /* datatype *DTPointer = Insert(DeclDT, &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory); */

          switch (Decl.Type)
          {
            InvalidCase(type_declaration_noop); // We just checked if this was set.

            case type_enum_decl:
            {
              // @not_pushing_variable_decls
              Insert(DeclDT, &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);
              Info("Pushing enum decl (%S)", GetNameForDatatype(&DeclDT, Ctx->Memory));
              /* Push(&Ctx->Datatypes, Decl.enum_decl); */
            } break;

            case type_function_decl:
            {
              // @not_pushing_variable_decls
              Insert(DeclDT, &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);

              function_decl Function = Decl.function_decl;
              Info("Pushing function decl (%S)", Function.NameT ? Function.NameT->Value : CSz("anonymous"));
              /* Insert(Datatype(&Decl), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory); */
              /* Push(&Ctx->Datatypes.Functions, Decl.function_decl); // Free function */
            } break;


            case type_compound_decl:
            {
              // @not_pushing_variable_decls
              datatype *Inserted = Insert(DeclDT, &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory);

              compound_decl StructOrUnion = Decl.compound_decl;

              Info( "Pushing %S decl (%S)",
                    StructOrUnion.IsUnion ? CSz("union") : CSz("struct"),
                    StructOrUnion.Type ? StructOrUnion.Type->Value : CSz("(anonymous)") );

              /* if (StringsMatch(StructOrUnion.Type->Value, CSz("terminal_colors"))) */
              /* { */
              /*   RuntimeBreak(); */
              /* } */

              /* Insert(Datatype(&StructOrUnion), &Ctx->Datatypes.DatatypeHashtable, Ctx->Memory); */
              /* Push(&Ctx->Datatypes.Structs, StructOrUnion); */

              if (OptionalToken(Parser, CTokenType_Semicolon))
              {
              }
              else
              {
                comma_separated_decl Var = ParseCommaSeperatedDecl(Ctx);
                MaybeEatAdditionalCommaSeperatedNames(Ctx);
                RequireToken(Parser, CTokenType_Semicolon);
              }
            } break;

            case type_variable_decl:
            {
              // NOTE(Jesse): If we ever start tracking globally defined variables
              // this'll matter, but for the moment it doesn't
              variable_decl *VD = SafeAccess(variable_decl, &Decl);
              /* VD->Type.Datatype = DTPointer; */

              /* DebugPrint(Decl); */
              /* datatype BaseType = ResolveToBaseType(Ctx, &DeclDT); */
              /* DebugPrint("---------------------\n"); */
              /* DebugPrint(&DeclDT); */
              /* DebugPrint(" :: basetype :: \n"); */
              /* DebugPrint(&BaseType); */
              /* DebugPrint("---------------------\n"); */

              MaybeEatAdditionalCommaSeperatedNames(Ctx);

              if (OptionalToken(Parser, CTokenType_Semicolon) == False)
              {
                ParseError_ExpectedSemicolonEqualsCommaOrOpenBrace(Parser, PeekTokenPointer(Parser));
              }
            } break;

          }
        }
        else
        {
          // This hits when we parse something like `struct foo;`
          //
          // We never do anything with pre-declarations like that, so the
          // parser just completely ignores them.
        }

      } break;

      case CT_Keyword_Namespace:
      {
        RequireToken(Parser, CT_Keyword_Namespace);
        RequireToken(Parser, CTokenType_Identifier);
        RequireToken(Parser, CTokenType_OpenBrace);
      } break;

      // Closing namespace
      //
      // NOTE(Jesse): Pretty jank-tacular, but whatever.  Hopefully this code
      // goes away soon.
      //
      case CTokenType_CloseBrace:
      {
        RequireToken(Parser, CTokenType_CloseBrace);
      } break;

      case CT_KeywordAttribute:
      {
        TryEatAttributes(Parser);
      } break;


      InvalidDefaultError(Parser, CSz("Unexpected token encountered while scanning for datatypes"), T);
    }

    continue;
  }

  return;
}

link_internal parser_cursor
AllocateTokenizedFiles(u32 Count, memory_arena* Memory)
{
  parser* Start = Allocate(parser, Memory, Count);
  parser_cursor Result = {
    .Start = Start,
    .At = Start,
    .End = Start + Count,
  };

  return Result;
}

/* link_internal tuple_cs_cs_buffer */
/* GoGoGadgetMetaprogramming(parse_context* Ctx, todo_list_info* TodoInfo); */

link_internal counted_string
FlushOutputToDisk( parse_context *Ctx,
                   cs OutputForThisParser,
                   cs NewFilename,
                   cs SourceFileAndLineNumber,
                   todo_list_info* TodoInfo,
                   memory_arena* Memory,
                   b32 IsInlineCode = False,
                   b32 OmitInclude = False,
                   b32 CodeFragment = False)
{
  TIMED_FUNCTION();
  parser *Parser = Ctx->CurrentParser;

  // NOTE(Jesse): It can be a semicolon too
  // TODO(Jesse): I _think_ the semicolon thing is fixed and this assertion shouldn't fire anymore
  //
  /* Assert(PeekToken(Parser, -1).Type == CTokenType_CloseParen); */

  if (Parser->ErrorCode)
  {
    Warn("Parse error encountered, not flushing code generated in (%S) to disk.", Parser->Tokens->Filename);
    return {};
  }

  counted_string OutputPath = {};

  EatUntilIncluding(Parser, CTokenType_Newline);

  /* EatNBSP(Parser); */

  /* parser PrevParserState = *Parser; */
  /* c_token_cursor PrevTokens = *Parser->Tokens; */
  c_token *LastTokenBeforeNewline = PeekTokenPointer(Parser, -1);

  c_token *T = PeekTokenRawPointer(Parser);
  if (T && T->Type == CT_PreprocessorInclude)
  {
    c_token IncludeT = RequireTokenRaw(Parser, CT_PreprocessorInclude);
    counted_string IncludePath = IncludeT.IncludePath;

    if (IncludeT.Flags & CTFlags_RelativeInclude) { IncludePath = StripQuotes(IncludePath); }

    /* Info("INCLUDE PATH %S", IncludePath); */
    /* OutputPath = Concat(Ctx->Args.Outpath, Basename(IncludePath), Memory); */
    OutputPath = IncludePath;
  }

  if (OmitInclude && T && T->Type == CTokenType_CommentSingleLine)
  {
    OutputPath = T->Value;
    Frontcate(&OutputPath, 3);
    /* Info("OmitInclude (%S)", OutputPath); */
  }

  cs CodeToInsert = {};
  if (OutputPath.Start == False)
  {
    Assert(PeekTokenRaw(Parser, -1).Type == CTokenType_Newline);

    OutputPath = Concat(Ctx->Args.Outpath, NewFilename, Memory);
    Assert(LastTokenBeforeNewline->CodeToInsert.Start == 0);
    Assert(LastTokenBeforeNewline->CodeToInsert.Count == 0);

    if (OmitInclude)
    {
      if (T)
      {
        if (T->Type != CTokenType_CommentSingleLine)
        {
          CodeToInsert = Concat(CSz("// "), OutputPath, Memory);
        }
      }
      else
      {
        CodeToInsert = Concat(CSz("// "), OutputPath, Memory);
      }
    }
    else
    {
      CodeToInsert = Concat(CSz("#include <"), OutputPath, CSz(">"), Memory);
    }
  }

  if (CodeToInsert.Count)
  {
    // NOTE(Jesse): Keep the value intact so we can still print it
    LastTokenBeforeNewline->Type = CT_PoofInsertedCode;
    LastTokenBeforeNewline->CodeToInsert = CodeToInsert;
  }


  Output(SourceFileAndLineNumber, OutputForThisParser, OutputPath, Memory);
  parser *OutputParse = ParserForAnsiStream(Ctx, AnsiStream(OutputForThisParser, OutputPath), TokenCursorSource_MetaprogrammingExpansion, Ctx->Memory);

#if 1
  if (IsInlineCode)
  {
    // TODO(Jesse, id: 226, tags: metaprogramming, output): Should we handle this differently?l
    Info("Not parsing inlined code (%S)", OutputPath);
  }
  else if (OmitInclude)
  {
    Info("Skipping parsing code for function marked `omit_include` (%S)", OutputPath);
  }
  else if (CodeFragment)
  {
    Info("Skipping parsing code for function marked `code_fragment` (%S)", OutputPath);
  }
  else
  {
    // NOTE(Jesse): This is pretty tortured.. maybe remove Ctx->CurrentParser..
    // it's technically unnecessary but kinda entrenched at this point..
    parser *OldParser = Ctx->CurrentParser;
    Ctx->CurrentParser = OutputParse;
    RunPreprocessor(Ctx, OutputParse, OldParser, Memory);
    ParseDatatypes(Ctx, OutputParse);
    Ctx->CurrentParser = OldParser;
  }
#endif

  /* PushParser(Ctx->CurrentParser, OutputParse, parser_push_type_include); */
  /* GoGoGadgetMetaprogramming(Ctx, TodoInfo); */

  return OutputPath;
}

link_internal meta_func_arg*
StreamContains(meta_func_arg_stream* Stream, counted_string Match)
{
  meta_func_arg* Result = {};
  ITERATE_OVER(Stream)
  {
    meta_func_arg* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Match, Match))
    {
      Result = Current;
      break;
    }
  }

  return Result;
}

link_internal meta_func*
StreamContains(meta_func_stream* Stream, counted_string Name)
{
  meta_func* Result = {};
  ITERATE_OVER(Stream)
  {
    meta_func* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Name, Name))
    {
      Result = Current;
      break;
    }
  }

  if (Result)
  {
    Result->Body.ErrorCode = ParseErrorCode_None;
  }

  return Result;
}

link_internal counted_string*
StreamContains(counted_string_stream* Stream, counted_string Name)
{
  counted_string* Result = {};
  ITERATE_OVER(Stream)
  {
    counted_string* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current, Name))
    {
      Result = Current;
      break;
    }
  }

  return Result;
}

link_internal person*
StreamContains(person_stream* People, counted_string Name)
{
  person* Result = {};
  ITERATE_OVER(People)
  {
    person* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Name, Name))
    {
      Result = Current;
      break;
    }
  }

  return Result;
}

link_internal tagged_counted_string_stream*
StreamContains(tagged_counted_string_stream_stream* Stream, counted_string Tag)
{
  tagged_counted_string_stream* Result = {};
  ITERATE_OVER(Stream)
  {
    tagged_counted_string_stream* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Tag, Tag))
    {
      Result = Current;
      break;
    }
  }
  return Result;
}

link_internal todo*
StreamContains(todo_stream* Todos, counted_string TodoId)
{
  todo* Result = {};
  ITERATE_OVER(Todos)
  {
    todo* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Id, TodoId))
    {
      Result = Current;
      break;
    }
  }
  return Result;
}

link_internal tag*
StreamContains(tag_stream* TodoLists, counted_string Tag)
{
  tag* Result = {};
  ITERATE_OVER(TodoLists)
  {
    tag* Current = GET_ELEMENT(Iter);
    if (StringsMatch(Current->Tag, Tag))
    {
      Result = Current;
      break;
    }
  }
  return Result;
}

link_internal todo*
UpdateTodo(todo_stream* Stream, todo Todo, memory_arena* Memory)
{
  todo* Result = StreamContains(Stream, Todo.Id);
  if (Result)
  {
    Result->Value = Todo.Value;
    Result->FoundInCodebase = Todo.FoundInCodebase;
  }
  else
  {
    Push(Stream, Todo);
    Result = StreamContains(Stream, Todo.Id);
  }
  return Result;
}

link_internal tag*
GetExistingOrCreate(tag_stream* Stream, counted_string Tag, memory_arena* Memory)
{
  tag* Result = StreamContains(Stream, Tag);
  if (!Result)
  {
    tag NewTag = {};
    NewTag.Tag = Tag ;

    Push(Stream, NewTag);
    Result = StreamContains(Stream, Tag);
  }
  return Result;
}

link_internal person*
GetExistingOrCreate(person_stream* People, counted_string PersonName, memory_arena* Memory)
{
  person* Person = StreamContains(People, PersonName);
  if (!Person)
  {
    person NewPerson = {};
    NewPerson.Name = PersonName;

    Push(People, NewPerson);
    Person = StreamContains(People, PersonName);
  }
  return Person;
}

static u32 LargestIdFoundInFile = 0;

link_internal todo
Todo(counted_string Id, counted_string Value, b32 FoundInCodebase)
{
  todo Result = { .Id = Id, .Value = Value, .FoundInCodebase = FoundInCodebase };
  return Result;
}

#if 0
link_internal person_stream
ParseAllTodosFromFile(counted_string Filename, memory_arena* Memory)
{
  TIMED_FUNCTION();

  InvalidCodePath();

  person_stream People = {};

  parser *Parser = ParserForFile(0, Filename, TokenCursorSource_Unknown, Memory);

  if (Parser)
  {
    while (TokensRemain(Parser))
    {
      RequireToken(Parser, CTokenType_Hash);
      counted_string PersonName = RequireToken(Parser, CTokenType_Identifier).Value;

      person* Person = GetExistingOrCreate(&People, PersonName, Memory);
      while (OptionalToken(Parser, CT_PreprocessorPaste))
      {
        counted_string TagName = RequireToken(Parser, CTokenType_Identifier).Value;

        tag* Tag = GetExistingOrCreate(&Person->Tags, TagName, Memory);
        while (OptionalToken(Parser, CTokenType_Minus))
        {
          RequireToken(Parser, CTokenType_Hash);
          c_token TodoId = RequireToken(Parser, CTokenType_IntLiteral);

          LargestIdFoundInFile = Max(LargestIdFoundInFile, (u32)TodoId.UnsignedValue);
          counted_string TodoValue = Trim(ConcatTokensUntilNewline(Parser, Memory));
          todo NewTodo = Todo(TodoId.Value, TodoValue, False);
          Push(&Tag->Todos, NewTodo);
          EatWhitespace(Parser);
        }

        EatWhitespace(Parser);
      }

      EatWhitespace(Parser);
    }
  }

  return People;
}
#endif

link_internal counted_string
GetResolvedTypeNameForDatatype(parse_context *Ctx, datatype *Data, memory_arena *Memory);

link_internal cs
ToString(parse_context *Ctx, meta_func_arg *Arg, memory_arena *Memory)
{
  cs Result = {};
  switch (Arg->Type)
  {
    InvalidCase(type_meta_func_arg_noop);

    case type_datatype:
    {
      /* Result = GetTypeNameFor(Ctx, &Arg->datatype, TypedefResoultion_DoNotResolveTypedefs, Memory); */
      Result = GetNameForDatatype(&Arg->datatype, Memory);
    } break;

    case type_poof_index:
    {
      Result = FormatCountedString(Memory, CSz("%u"), Arg->poof_index.Index);
    } break;

    case type_poof_symbol:
    {
      // TODO(Jesse): What should we do here?  Symbols can be really big and
      // contain spaces.  This gets used (at the moment) to generate output
      // filenames so we don't want to expand the whole thing.. TBD.
      /* NotImplemented; */
      Result = FormatCountedString(Memory, CSz("%lu"), Hash(Arg->poof_symbol.Value));
    } break;
  }
  return Result;
}

link_internal counted_string
GenerateOutfileNameFor(parse_context *Ctx, meta_func *Func, meta_func_arg_buffer *Args, memory_arena *Memory, cs Modifier = {})
{
  string_builder OutfileBuilder = StringBuilder(AllocateArena());
  Append(&OutfileBuilder, Func->Name);
  Append(&OutfileBuilder, CSz("_"));
  for (u32 ArgIndex = 0; ArgIndex < Args->Count; ++ArgIndex)
  {
    meta_func_arg *Arg = Args->Start + ArgIndex;
    Append(&OutfileBuilder, ToString(Ctx, Arg, Memory));
    if ( ArgIndex+1 != Args->Count )
    {
      Append(&OutfileBuilder, CSz("_"));
    }
  }
  Append(&OutfileBuilder, CSz(".h"));
  counted_string Result = Finalize(&OutfileBuilder, Memory);
  return Result;
}

link_internal counted_string
GenerateOutfileNameFor(counted_string Name, counted_string DatatypeName, memory_arena* Memory, counted_string Modifier = {})
{
  string_builder OutfileBuilder = {};
  Append(&OutfileBuilder, Name);
  Append(&OutfileBuilder, CSz("_"));
  Append(&OutfileBuilder, DatatypeName);
  if (Modifier.Count)
  {
    Append(&OutfileBuilder, CSz("_"));
    Append(&OutfileBuilder, Modifier);
  }
  Append(&OutfileBuilder, CSz(".h"));
  counted_string Result = Finalize(&OutfileBuilder, Memory);

  return Result;
}

link_internal counted_string
GetTypeNameFor(compound_decl *CDecl)
{
  counted_string Result = {};
  /* if (CDecl->Type) */
  /* { */
  /*   Result = CDecl->Type->Value; */
  /* } */
  /* else */
  {
    Result = CDecl->IsUnion ? CSz("union") :
                              CSz("struct");
  }
  return Result;
}

link_internal counted_string
GetTypeNameFor(parse_context *Ctx, declaration* Decl, memory_arena *Memory)
{
  counted_string Result = {};

  switch (Decl->Type)
  {
    case type_declaration_noop: { InvalidCodePath(); } break;

    case type_variable_decl:
    {
      auto VDecl = SafeAccess(variable_decl, Decl);
      Result = PrintTypeSpec(&VDecl->Type, Memory);
    } break;

    case type_function_decl:
    {
      // Very questionable what we do for this ..?
      NotImplemented;
      Result = CSz("(function)");
    } break;

    case type_compound_decl:
    {
      compound_decl *CDecl = SafeAccess(compound_decl, Decl);
      Result = GetTypeNameFor(CDecl);
    } break;

    case type_enum_decl:
    {
      Result = CSz("enum");
    } break;
  }

  return Result;
}

link_internal c_token *
GetNameTokenForDecl(declaration* Decl)
{
  c_token *Result = {};

  switch (Decl->Type)
  {
    InvalidCase(type_declaration_noop);

    case type_function_decl:
    {
      Result = Decl->function_decl.NameT;
    } break;

    case type_variable_decl:
    {
      Result = Decl->variable_decl.NameT;
    } break;

    case type_compound_decl:
    {
      Result = Decl->compound_decl.Type;
    } break;

    case type_enum_decl:
    {
      Result = Decl->enum_decl.NameT;
    } break;
  }

  return Result;
}

link_internal cs
GetNameForDecl(declaration *Decl)
{
  cs Result = CSz("(null)");
  if (c_token *NameT = GetNameTokenForDecl(Decl))
  {
    Result = NameT->Value;
  }
  return Result;
}

link_internal counted_string
ApplyTransformations(meta_transform_op Transformations, counted_string Input, memory_arena* Memory)
{
  counted_string Result = Input;
  switch (Transformations)
  {
    case to_capital_case:
    {
      Result = ToCapitalCase(Result, Memory);
    } break;
    case to_snake_case:
    {
      Result = ToSnakeCase(Input, Memory);
    } break;
    case to_lowercase:
    {
      Result = ToLowerCase(Result, Memory);
    } break;
    case to_uppercase:
    {
      Result = ToUpperCase(Result, Memory);
    } break;
    case strip_single_prefix:
    {
      Result = StripPrefix(Result, 0);
    } break;
    case strip_all_prefix:
    {
      Result = StripPrefixesUntilDoubleUnderscore(Result);
    } break;

    InvalidDefaultCase;
  }

  return Result;
}

link_internal counted_string
EscapeDoubleQuotes(counted_string S, memory_arena* Memory)
{
  string_builder B = {};
  for (u32 CharIndex = 0;
      CharIndex < S.Count;
      ++CharIndex)
  {
    if (S.Start[CharIndex] == '"')
    {
      Append(&B, CSz("\\"));
    }
    Append(&B, CS(S.Start+CharIndex, 1));
  }

  counted_string Result = Finalize(&B, Memory);
  return Result;
}

link_internal void
AppendAndEscapeInteriorOfDoubleQuotedString(string_builder* Builder, counted_string S)
{
  if (S.Count >= 2 &&
      S.Start[0] == '"' &&
      S.Start[S.Count-1] == '"')
  {
    S.Count -= 2;
    ++S.Start;
    Append(Builder, CSz("\""));
    Append(Builder, EscapeDoubleQuotes(S, Builder->Chunks.Memory));
    Append(Builder, CSz("\""));
  }
  else
  {
    Append(Builder, EscapeDoubleQuotes(S, Builder->Chunks.Memory));
  }
}

link_internal meta_transform_op
ParseTransformations(parser* Scope)
{
  meta_transform_op Result = {};

  while (OptionalToken(Scope, CTokenType_Dot))
  {
    c_token *NextOpT = PopTokenPointer(Scope);
    meta_transform_op NextOp = MetaTransformOp(NextOpT->Value);
    if (NextOp != meta_transform_op_noop)
    {
      Result = (meta_transform_op)(Result | NextOp);
    }
    else
    {
      PoofTypeError(Scope,
                    ParseErrorCode_InvalidMetaTransformOp,
                    CSz("Unknown transformer encountered"),
                    NextOpT);
    }
  }

  return Result;
}

link_internal meta_func_arg
MetaFuncArg(parse_context *Ctx, poof_tag Tag, cs Match)
{
  Assert(Tag.Value.Count);

  meta_func_arg Result = {};
  Result.Match = Match;

  if (IsNumeric(Tag.Value))
  {
    Result.Type = type_poof_index;
    Result.poof_index = PoofIndex(ToU32(Tag.Value), 0);
  }
  else
  {
    datatype *D = GetDatatypeByName(&Ctx->Datatypes, Tag.Value);
    if (D->Type)
    {
      Result.Type = type_datatype;
      Result.datatype = *D;
    }
  }

  if (Result.Type == type_meta_func_arg_noop)
  {
    Result.Type = type_poof_symbol;
    Result.poof_symbol.Value = Tag.Value;
  }

  return Result;
}

link_internal meta_func_arg
ReplacementPattern(counted_string Match, meta_func_arg *Arg)
{
  meta_func_arg Result = *Arg;
  Result.Match = Match;
  return Result;
}

link_internal meta_func_arg
ReplacementPattern(counted_string Match, poof_symbol Symbol)
{
  meta_func_arg Result = MetaFuncArg(Symbol, Match);
  Result.Match = Match;
  return Result;
}

link_internal meta_func_arg
ReplacementPattern(counted_string Match, poof_index Index)
{
  meta_func_arg Result = MetaFuncArg(Index, Match);
  Result.Match = Match;
  return Result;
}

link_internal meta_func_arg
ReplacementPattern(counted_string Match, datatype Data)
{
  meta_func_arg Result = MetaFuncArg(Data, Match);
  Result.Match = Match;
  return Result;
}

#if 0
link_internal meta_func_arg_stream
CopyStream(meta_func_arg_stream* Stream, memory_arena* Memory)
{
  meta_func_arg_stream Result = {};
  ITERATE_OVER(Stream)
  {
    meta_func_arg* Element = GET_ELEMENT(Iter);
    Push(&Result, *Element);
  }
  return Result;
}
#endif

link_internal counted_string
PrintTypeSpec(type_spec *TypeSpec, memory_arena *Memory)
{
  counted_string Result = {};

  if (TypeSpec->DatatypeToken)
  {
    Result = TypeSpec->DatatypeToken->Value;
  }
  else if (TypeSpec->BaseType)
  {
    // TODO(Jesse): This is actually a symptom of a much deeper issue.  TypeSpecs
    // should probably always just have a BaseType pointer instead of a DatatypeToken?
    //
    // @base_type_hack
    //
    // Seems like we might actually just want to have typespecs basically be datatypes ??
    //
    Result = CSz("this_is_a_bug"); // GetTypeNameFor(Ctx, TypeSpec->BaseType, Memory);
  }
  else
  {
    string_builder Builder = {};
    poof(
      func (type_qualifier Enum)
      {
        (Enum.map_values(EnumVal)
        {
          if (TypeSpec->Qualifier & (EnumVal.name))
          {
            Append(&Builder, CSz("(EnumVal.name.strip_all_prefix.to_lowercase) "));
          }
        })
      }
    )
#include <generated/anonymous_function_type_qualifier_fPa8h41Z.h>

    Result = Finalize(&Builder, Memory);
  }

  Assert(Result.Count);

  return Result;
}

link_internal cs
ToString(type_spec *T, memory_arena *Memory)
{
  cs Result = PrintTypeSpec(T, Memory);
  return Result;
}

link_internal void
DebugPrint(type_spec *TypeSpec, u32 Depth)
{
  DebugPrint(PrintTypeSpec(TypeSpec, GetTranArena()), Depth);
  DebugPrint("\n");
}

link_internal counted_string
GetValueForDatatype(program_datatypes *Datatypes, datatype *Data, memory_arena *Memory)
{
  counted_string Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    {
      InvalidCodePath();
    } break;

    case type_primitive_def:
    {
      NotImplemented;
    } break;

    case type_type_def:
    {
      auto Resolved = ResolveToBaseType(Datatypes, Data);
      Result = GetValueForDatatype(Datatypes, Resolved, Memory);
    } break;

    case type_enum_member:
    {
      enum_member *EM = SafeAccess(enum_member, Data);
      Result = PrintAstNode(EM->Expr, Memory);
    } break;


    case type_declaration:
    {
      declaration *Member = SafeAccess(declaration, Data);
      switch (Member->Type)
      {
        case type_enum_decl:
        {
          // What should we do here?
          NotImplemented;
          Result = CSz("(enum decl)");
        } break;

        case type_declaration_noop:
        {
          InvalidCodePath();
        } break;

        case type_compound_decl:
        {
          Result = CSz("(value unsupported for compound declaration types)");
        } break;

        case type_function_decl:
        {
          Result = ToString(&Member->function_decl.ReturnType, Memory);
        } break;

        case type_variable_decl:
        {
          variable_decl *Var = SafeAccess(variable_decl, Member);
          Result = PrintAstNode(Var->Value, Memory);
        } break;
      }

    } break;
  }

  return Result;
}

link_internal c_token *
GetNameTokenForDatatype(datatype *Data)
{
  c_token *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    {
      // NOTE(Jesse): This is a valid case when the datatype is undefined!
      //
      // Such as when we've queried for a type occuring in the CRT but didn't
      // traverse the include graph
    } break;

    case type_declaration:
    {
      Result = GetNameTokenForDecl(&Data->declaration);
    } break;

    case type_enum_member:
    {
      Result = Data->enum_member.NameT;
    } break;

    case type_primitive_def:
    {
      Result = Data->primitive_def.TypeSpec.DatatypeToken;
    } break;

    case type_type_def:
    {
      Result = Data->type_def.Type.DatatypeToken;
    } break;
  }

  return Result;

}

link_internal cs
GetNameForDatatype(datatype *Data, memory_arena *Memory)
{
  cs Result = CSz("(anonymous)");
  switch (Data->Type)
  {
    // NOTE(Jesse): I decieded that nobody should ever ask for a name of an
    // undefined datatype.  The caller is responsible for checking that they
    // got a datatype back from their query!
    InvalidCase(type_datatype_noop);

    case type_type_def:
    {
      Result = Data->type_def.Alias;
    } break;

    case type_enum_member:
    case type_declaration:
    {
      if (c_token *NameT = GetNameTokenForDatatype(Data))
      {
        Result = NameT->Value;
      }
    } break;

    case type_primitive_def:
    {
      Result = PrintTypeSpec(&Data->primitive_def.TypeSpec, Memory);
    } break;
  }

  return Result;
}

link_internal cs
GetFilenameForDatatype(datatype *Data)
{
  c_token *NameT = GetNameTokenForDatatype(Data);
  cs Result = NameT->Filename;
  return Result;
}

link_internal u32
GetLineNumberForDatatype(datatype *Data)
{
  c_token *NameT = GetNameTokenForDatatype(Data);
  u32 Result = NameT->LineNumber;
  return Result;
}

link_internal type_indirection_info *
GetIndirectionInfoForDatatype(program_datatypes *Datatypes, datatype *Data)
{
  type_indirection_info *Result = {};
  switch (Data->Type)
  {
    // I guess ..?
    InvalidCase(type_datatype_noop);

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      switch (Decl->Type)
      {
        InvalidCase(type_declaration_noop);

        case type_function_decl:
        case type_compound_decl:
        case type_enum_decl:
        {
        } break;

        case type_variable_decl:
        {
          Result = &Decl->variable_decl.Type.Indirection;
        } break;
      }
    } break;

    case type_enum_member:
    {
    } break;

    case type_primitive_def:
    {
      Result = &Data->primitive_def.TypeSpec.Indirection;
    } break;

    case type_type_def:
    {
      auto Resolved = ResolveToBaseType(Datatypes, Data);
      Result = GetIndirectionInfoForDatatype(Datatypes, Resolved);
    } break;

  }

  return Result;
}

link_internal counted_string
GetTypeTypeForDatatype(datatype *Data, memory_arena *Memory)
{
  counted_string Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    case type_enum_member:
    case type_type_def:
    case type_primitive_def:
    {
      Result = ToString(Data->Type);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      string_builder Builder = {};

      Append(&Builder, ToString(Data->Type));
      Append(&Builder, CSz(" & "));
      Append(&Builder, ToString(Decl->Type));

      Result = Finalize(&Builder, Memory);

    } break;
  }

  return Result;
}

link_internal counted_string
GetTypeNameFor(parse_context *Ctx, datatype *Data, typedef_resolution_behavior TDResBehavior, memory_arena *Memory)
{
  counted_string Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop: { InvalidCodePath(); } break;

    case type_type_def:
    {
      type_def *TD = SafeAccess(type_def, Data);
      if (TDResBehavior == TypedefResoultion_ResolveTypedefs)
      {
        Result = PrintTypeSpec(&TD->Type, Memory);
      }
      else
      {
        Assert(TDResBehavior == TypedefResoultion_DoNotResolveTypedefs);
        Result = TD->Alias;
      }
    } break;

    case type_primitive_def:
    {
      NotImplemented;
    } break;

    case type_enum_member:
    {
      // This is actually wrong.. shouldn't we print the _name_ of the enum the
      // member belongs to?
      // NotImplemented;
      Result = Data->enum_member.NameT->Value;
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      Result = GetTypeNameFor(Ctx, Decl, Memory);
    } break;
  }

  return Result;
}

link_internal ast_node*
DatatypeStaticBufferSize(parse_context *Ctx, parser *Scope, datatype *Data, c_token *MetaOperatorT)
{
  ast_node *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop:
    {
      // TODO(Jesse): ?
      InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT);
    } break;

    case type_type_def:
    case type_enum_member:
    case type_primitive_def:
    {
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);

      switch(Decl->Type)
      {
        case type_declaration_noop:
        {
          // TODO(Jesse): ?
          InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT);
        } break;

        case type_function_decl:
        case type_enum_decl:
        case type_compound_decl:
        {
        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);
          Result = VDecl->StaticBufferSize;
        } break;
      }

    } break;
  }

  return Result;
}

link_internal cs
ToString(type_indirection_info *Indirection, memory_arena *Memory)
{
  u32 CharsNeeded = Indirection->IndirectionLevel;
  cs Result = CountedString(CharsNeeded, Memory);

  if (Indirection->ReferenceLevel)
  {
    // NOTE(Jesse): I'm just not going to deal with interleaving the references
    // and pointers correctly for now, because I never use references.
    Warn("Not printing references; unsupported.");
  }

  RangeIterator_t(u32, CharIndex, CharsNeeded)
  {
    Cast(char*, &Result.Start[CharIndex])[0] = '*';
  }

  return Result;
}

link_internal cs
PrintIndirection(program_datatypes *Datatypes, datatype *Data, memory_arena *Memory)
{
  cs Result = {};
  type_indirection_info *Indirection = GetIndirectionInfoForDatatype(Datatypes, Data);
  if (Indirection) { Result = ToString(Indirection, Memory); }
  return Result;
}



link_internal cs
ToString( parse_context *Ctx, parser *Scope, datatype *Data, c_token *MetaOperatorT, memory_arena *Memory)
{
  string_builder Builder = {};

  cs TypeType = GetTypeNameFor(Ctx, Data, TypedefResoultion_ResolveTypedefs, Memory);
  cs TypeName = GetNameForDatatype(Data, Memory);

  /* cs TypeType = GetTypeTypeForDatatype(Data, Memory); */
  /* cs StaticBufferSize = PrintAstNode(DatatypeStaticBufferSize(Ctx, Scope, Data, MetaOperatorT), Memory); */

  cs Indirection = PrintIndirection(&Ctx->Datatypes, Data, Memory);

  cs Result = Concat(TypeType, CSz(" "), Indirection, TypeName, Memory);
  return Result;
}

link_internal b32
DatatypeIsArray(parse_context *Ctx, parser *Scope, datatype *Data, c_token *MetaOperatorT)
{
  b32 Result = (DatatypeStaticBufferSize(Ctx, Scope, Data, MetaOperatorT) != 0);
  return Result;
}

link_internal b32
TypeSpecIsPointer(type_spec *Type)
{
  type_indirection_info *Indirection = &Type->Indirection;
  b32 Result = ((Indirection->ConstPointer + Indirection->IndirectionLevel + Indirection->IsFunctionPtr) > 0);
  return Result;
}

link_internal poof_tag_block_array *
GetTagsFromDatatype(parse_context *Ctx, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  poof_tag_block_array *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop: { if (Scope) { InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT); } } break;


    case type_primitive_def:
    case type_type_def:
    {
    } break;

    case type_enum_member:
    {
      enum_member *Enum = SafeAccess(enum_member, Data);
      Result = &Enum->Tags;
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      Result = &Decl->Tags;
    } break;
  }

  return Result;
}

link_internal poof_tag
GetTagFromDatatype(parse_context *Ctx, cs TagName, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  poof_tag Result = {};
  
  if (poof_tag_block_array *Tags = GetTagsFromDatatype(Ctx, Data, Scope, MetaOperatorT))
  {
    IterateOver(Tags, Tag, TagIndex)
    {
      if (AreEqual(Tag->Name, TagName))
      {
        Result = *Tag;
        break;
      }
    }
  }

  return Result;
}

link_internal b32
DatatypeHasTag(parse_context *Ctx, cs TagName, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  b32 Result = GetTagFromDatatype(Ctx, TagName, Data, Scope, MetaOperatorT).Name.Count > 0;
  return Result;
}

link_internal b32
DatatypeIsPointer(parse_context *Ctx, datatype *Data, parser *Scope = 0, c_token *MetaOperatorT = 0)
{
  b32 Result = False;
  switch (Data->Type)
  {
    case type_datatype_noop: { if (Scope) { InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT); } } break;
    case type_enum_member: {} break;

    case type_primitive_def:
    {
      primitive_def *Prim = SafeCast(primitive_def, Data);
      Result = TypeSpecIsPointer(&Prim->TypeSpec);
    } break;

    case type_type_def:
    {
      type_def *TDef = &SafeAccessPtr(type_def, Data);
      Result = TypeSpecIsPointer(&TDef->Type);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);

      switch(Decl->Type)
      {
        case type_declaration_noop: { if (Scope) { InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorT); } } break;

        case type_enum_decl:
        case type_compound_decl:
        case type_function_decl:
        {
        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);
          /* datatype *Resolved = ResolveToBaseType(Ctx, VDecl->Type); */
          /* Result = TypeSpecIsPointer(&VDecl->Type); */
          Result = TypeSpecIsPointer(&VDecl->Type);
        } break;
      }

    } break;
  }

  return Result;
}

link_internal declaration_stream*
GetMembersFor(parse_context *Ctx, declaration *Decl)
{
  declaration_stream *Result = {};
  switch(Decl->Type)
  {
    case type_compound_decl:
    {
      compound_decl *Anon = SafeAccess(compound_decl, Decl);
      Result = &Anon->Members;
    } break;

    case type_variable_decl:
    {
      variable_decl *VDecl = SafeAccess(variable_decl, Decl);
      datatype *DT = ResolveToBaseType(Ctx, VDecl->Type);
      /* Assert(DatatypeIsVariableDecl(DT) == False); */
      auto Compound = DatatypeIsCompoundDecl(Ctx, DT);
      if (Compound) { Result = &Compound->Members; }
    } break;

    default: {} break;;
  }

  return Result;
}

link_internal declaration_stream*
GetMembersFor(parse_context *Ctx, datatype *Data)
{
  declaration_stream *Result = {};
  switch (Data->Type)
  {
    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      Result = GetMembersFor(Ctx, Decl);
    } break;

    case type_type_def:
    {
      // TODO(Jesse): Resolve typedef here?
      /* NotImplemented; */
    } break;

    default:
    {
    } break;
  }

  return Result;
}

// This resolves typedefs and tells us if we've got a primitive type, complex type, or undefined type
//
// This is the primal function
//
link_internal datatype *
ResolveToBaseType(program_datatypes *Datatypes, type_def *TD)
{
  datatype *Result = {};

  if (TD->Type.DatatypeToken)
  {
    datatype *Resolved = GetDatatypeByName(Datatypes, TD->Type.DatatypeToken->Value);
    if (Resolved->Type == type_datatype_noop)
    {
      // either undefined or a primitive.  what do we do?
      //
    }
    else if (Resolved->Type == type_type_def)
    {
      type_def *ResolvedTD = SafeAccess(type_def, Resolved);
      Result = ResolveToBaseType(Datatypes, ResolvedTD);
    }
    else
    {
      Result = Resolved;
    }
  }
  else
  {

#if 0
    // This is defaulting to a function or primitive type.. although I feel
    // like it should never be a function here ..?
    Result = Datatype(TD->Type);
    NotImplemented;
#else
    /* Leak("Leaking datatype allocation during ResolveToBaseType"); */
    Result = Allocate(datatype, GetTranArena(), 1);
    *Result = Datatype(TD->Type);
#endif
  }

  if (Result == 0) { Result = &NullDatatype; }

  return Result;
}

link_internal datatype *
ResolveToBaseType(program_datatypes *DataHash, datatype *Data)
{
  datatype *Result = {};
  switch (Data->Type)
  {
    case type_datatype_noop: { InvalidCodePath(); } break;

    case type_type_def:
    {
      type_def *TDef = SafeAccess(type_def, Data);
      Result = ResolveToBaseType(DataHash, TDef->Type);
    } break;

    case type_enum_member:
    case type_primitive_def:
    {
      Result = Data;
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, Data);
      switch (Decl->Type)
      {
        case type_declaration_noop: { InvalidCodePath(); } break;

        case type_variable_decl:
        {
          auto VDecl = SafeAccess(variable_decl, Decl);
          Result = ResolveToBaseType(DataHash, VDecl->Type);
        } break;

        case type_compound_decl:
        case type_function_decl:
        case type_enum_decl:
        {
          Result = Data;
        } break;

      }

    } break;
  }

  if (Result == 0) { Result = &NullDatatype; }
  return Result;
}


link_internal datatype*
ResolveToBaseType(program_datatypes *DataHash, type_spec TypeSpec)
{
  datatype *Result = {};

  if ( TypeSpec.BaseType )
  {
    Assert(TypeSpec.BaseType->Type == type_declaration);
    Result = TypeSpec.BaseType;
  }
  else if ( TypeSpec.DatatypeToken &&
            TypeSpec.DatatypeToken->Type == CTokenType_Identifier )
  {
    Result = GetDatatypeByName(DataHash, TypeSpec.DatatypeToken->Value);
    if (Result->Type == type_type_def)
    {
      type_def *ResolvedTD = SafeAccess(type_def, Result);
      Result = ResolveToBaseType(DataHash, ResolvedTD);
    }
  }
  else if ( TypeSpec.Indirection.IsFunction ||
            TypeSpec.Indirection.IsFunctionPtr )
  {
    /* NotImplemented; */
    /* Result = Datatype(TypeSpec); */
    /* Leak("Leaking datatype allocation"); */
    Result = Allocate(datatype, GetTranArena(), 1);
    *Result = Datatype(TypeSpec);
  }
  else
  {
    /* NotImplemented; */
    /* Result = Datatype(TypeSpec); */
    /* Leak("Leaking datatype allocation"); */
    Result = Allocate(datatype, GetTranArena(), 1);
    *Result = Datatype(TypeSpec);
  }

  if (Result == 0) { Result = &NullDatatype; }
  return Result;
}

link_internal datatype*
ResolveToBaseType(parse_context *Ctx, type_spec TypeSpec)
{
  datatype *Result = ResolveToBaseType(&Ctx->Datatypes, TypeSpec);
  Assert(Result);
  return Result;
}

link_internal datatype*
ResolveToBaseType(parse_context *Ctx, datatype *Datatype)
{
  datatype *Result = ResolveToBaseType(&Ctx->Datatypes, Datatype);
  Assert(Result);
  return Result;
}

link_internal datatype*
ResolveToBaseType(parse_context *Ctx, type_def *Typedef)
{
  datatype *Result = ResolveToBaseType(&Ctx->Datatypes, Typedef);
  Assert(Result);
  return Result;
}

link_internal b32
TryParsePoofTag(parser *Parser, poof_tag *Tag)
{
  b32 Result = False;

  if (OptionalToken(Parser, CTokenType_At))
  {
    c_token TagToken = RequireToken(Parser, CTokenType_Identifier);
    Tag->Name = TagToken.Value;

    if (PeekToken(Parser).Type == CTokenType_OpenParen)
    {
      Tag->Value = EatBetween_Str(Parser, CTokenType_OpenParen, CTokenType_CloseParen);
      Tag->Value = StripBookends(Tag->Value, '(', ')');
    }

    Result = True;
  }

  return Result;
}

link_internal b32
TryParsePoofKeywordAndTagList(parser *Parser, poof_tag_block_array *Tags)
{
  if (Tags->Memory == 0)
  {
    Tags->Memory = &Global_PermMemory;
  }

  c_token *T = PeekTokenPointer(Parser);

  b32 Got = False;
  if (T && T->Type == CTokenType_Poof)
  {
    RequireToken(Parser, T);

    if (PeekToken(Parser, 1).Type == '@')
    {
      RequireTokenPointer(Parser, CTokenType_OpenParen);
      poof_tag Tag = {};
      while(TryParsePoofTag(Parser, &Tag))
      {
        Info("Got Tag %S(%S)", Tag.Name, Tag.Value);
        Push(Tags, &Tag);
        Got = True;
      }
      RequireTokenPointer(Parser, CTokenType_CloseParen); // Closes poof( )

      // If we didn't get a tag but got a poof keyword something's wrong
      Assert(Got);
    }
    else
    {
      // Did not get an @ tag; this must be a function call, eat it.
      EatBetween(Parser, c_token_type('('), c_token_type(')'));
    }
  }

  return Got;
}

link_internal b32
ParseMetaFuncTags(parser *Parser, meta_func *Func)
{
  b32 OmitInclude = {};
  if (PeekToken(Parser).Type == CTokenType_At)
  {
    c_token *ErrorT = PeekTokenPointer(Parser);

    poof_tag Tag = {};
    while (TryParsePoofTag(Parser, &Tag))
    {

      if (AreEqual(Tag.Name, CSz("omit_include")))
      {
        Func->OmitInclude = True;
      }
      else if (AreEqual(Tag.Name, CSz("code_fragment")))
      {
        Func->CodeFragment = True;
      }
      else
      {
        PoofTagError(Parser, FSz("Unsupported tag value on poof func (%S).  Wanted (@omit_include) or (@code_fragment), got (%S)", Func->Name, Tag.Name), ErrorT);
      }
    }

  }
  return OmitInclude;
}


link_internal datatype *
ResolveNameToDatatype(parse_context *Ctx, parser *Scope, c_token *Token, meta_func_arg_buffer *Args, cs NameToResolve)
{
  datatype *Result = {};
  if (meta_func_arg* A0 = GetByMatch(Args, NameToResolve))
  {
    if (A0->Type == type_datatype)
    {
      Result = &A0->datatype;
    }
    else
    {
      PoofTypeError(Scope, ParseErrorCode_InvalidArgumentType, FSz("Attempted to resolve (%S) to non-datatype member (%S)", NameToResolve, ToString(A0->Type)), Token);
    }
  }
  else
  {
    Result = GetDatatypeByName(&Ctx->Datatypes, NameToResolve);
  }

  return Result;
}

#include <poof/execute.h>
link_internal b32
IsMetaprogrammingOutput(counted_string Filename, counted_string OutputDirectory)
{
  b32 Result = Contains(Filename, OutputDirectory);
  return Result;
}

#if 1
link_internal counted_string_stream
ParseDatatypeList(parse_context *Ctx, parser* Parser, program_datatypes* Datatypes, tagged_counted_string_stream_stream* NameLists, memory_arena* Memory)
{
  counted_string_stream Result = CountedStringStream(Memory);
  while (PeekToken(Parser).Type == CTokenType_Identifier)
  {
    c_token *NameT = RequireTokenPointer(Parser, CTokenType_Identifier);
    counted_string DatatypeName = NameT->Value;

    datatype *DT = GetDatatypeByName(Datatypes, DatatypeName);

    compound_decl *Struct              = TryCastToCompoundDecl(Ctx, DT);
    enum_decl     *Enum                = TryCastToEnumDecl(Ctx, DT);
    tagged_counted_string_stream *List = StreamContains(NameLists, DatatypeName);

    if (Struct || Enum)
    {
      Push(&Result, DatatypeName);
    }
    else if (List)
    {
      ConcatStreams(&Result, &List->Stream, Memory);
    }
    else
    {
      PoofTypeError(Parser,
                    ParseErrorCode_InvalidName,
                    FormatCountedString( GetTranArena(),
                                         CSz("(%S) could not be resolved to a struct name, enum name, or named_list"),
                                         DatatypeName),
                    NameT);
    }

    OptionalToken(Parser, CTokenType_Comma);
  }

  return Result;
}
#endif

// NOTE(Jesse): This parses the argument list for a function call instance
link_internal b32
ParseAndTypecheckArgument(parse_context *Ctx, parser *Parser, meta_func_arg *ParsedArg, meta_func_arg *ArgDef, meta_func_arg_buffer *CurrentScope)
{
  c_token *Peek = PeekTokenPointer(Parser);
  if (Peek)
  {
    ParsedArg->Match = ArgDef->Match;
    switch (ArgDef->Type)
    {
      InvalidCase(type_meta_func_arg_noop);

      case type_datatype:
      {
        c_token *Token = PopTokenPointer(Parser);
        if (Token->Type == CTokenType_Identifier)
        {
          datatype *Datatype = GetDatatypeByName(&Ctx->Datatypes, Token->Value);
          if (Datatype->Type)
          {
            ParsedArg->Type = type_datatype;
            ParsedArg->datatype = *Datatype;
          }
          else if (meta_func_arg *ScopedArg = GetByMatch(CurrentScope, Token->Value))
          {
            Assert(ScopedArg->Type == type_datatype || ScopedArg->Type == type_poof_symbol);
            *ParsedArg = *ScopedArg;
            ParsedArg->Match = ArgDef->Match;
          }
          else
          {
            PoofTypeError( Parser,
                           ParseErrorCode_UndefinedDatatype,
                           FormatCountedString( GetTranArena(),
                                                CSz("Could not find datatype or local variable for (%S)."),
                                                Token->Value ),
                           Token );
          }
        }
        else
        {
          PoofTypeError( Parser,
                         ParseErrorCode_PoofTypeError,
                         FormatCountedString( GetTranArena(),
                                              CSz("Expected (%S), got (%S)."),
                                              ToString(CTokenType_Identifier),
                                              ToString(Token->Type) ),
                         Token );
        }
      } break;

      case type_poof_index:
      {
        c_token *Token = PopTokenPointer(Parser);
        switch (Token->Type)
        {
          case CTokenType_IntLiteral:
          {
            ParsedArg->Match = ArgDef->Match;
            ParsedArg->Type = type_poof_index;
            ParsedArg->poof_index = PoofIndex(u32(Token->as_u64), 0);
          } break;

          case CTokenType_Identifier:
          {
            // TODO(Jesse): Look up into current scope to see if we've got an index
            // type with the same name
            NotImplemented;
          } break;

          // TODO(Jesse): Emit a type error here?
          InvalidDefaultCase;
        }
      } break;

      case type_poof_symbol:
      {
        cs Value = EatBetweenExcluding_Str(Parser, CTokenType_OpenBrace, CTokenType_CloseBrace);
        ParsedArg->Match = ArgDef->Match;
        ParsedArg->Type = type_poof_symbol;
        ParsedArg->poof_symbol = PoofSymbol(Value);
      } break;
    }
  }
  else
  {
    // TODO(Jesse)(error-message): Could make this error quite a bit better..
    PoofTypeError( Parser,
                   ParseErrorCode_InvalidArgumentCount,
                   CSz("Not enough arguments."),
                   Peek );
  }

  OptionalToken(Parser, CTokenType_Comma);
  b32 Result = (ParsedArg->Type != type_meta_func_arg_noop);
  return Result;
}

// NOTE(Jesse): This parses the argument list for a function definition
link_internal b32
ParseMetaFuncDefArg(parser *Parser, meta_func_arg_stream *Stream)
{
  cs Match = {};
  cs Type = {};

  c_token *FirstT = RequireTokenPointer(Parser, CTokenType_Identifier);
  if (c_token *SecondT = OptionalToken(Parser, CTokenType_Identifier))
  {
    Type = FirstT->Value;
    Match = SecondT->Value;
  }
  else
  {
    Match = FirstT->Value;
  }

  meta_func_arg Arg = {};
  Arg.Match = Match;

  if (Type.Start)
  {
    meta_func_arg_type ArgT = MetaFuncArgType(Type);
    switch (ArgT)
    {
      case type_meta_func_arg_noop:
      {
        ParseError(Parser, ParseErrorCode_InvalidArgumentType, CSz("Invalid Type Token"), FirstT);
      } break;

      case type_datatype:
      case type_poof_index:
      case type_poof_symbol:
      {
        Arg.Type = ArgT;
      } break;
    }
  }
  else
  {
    // Default to datatype if no type specified
    Arg.Type = type_datatype;
  }

  Push(Stream, Arg);

  b32 Result = (OptionalToken(Parser, CTokenType_Comma) != 0);
  return Result;
}

link_internal meta_func
ParseMapMetaFunctionInstance(parser* Parser, cs FuncName, memory_arena *Memory)
{
  TIMED_FUNCTION();

  RequireToken(Parser, CTokenType_OpenParen);
  c_token T1 = RequireToken(Parser, CTokenType_Identifier);
  c_token *T2 = {};
  if (OptionalToken(Parser, CTokenType_Comma)) { T2 = RequireTokenPointer(Parser, CTokenType_Identifier); }

  RequireToken(Parser, CTokenType_CloseParen);
  parser Body = GetBodyTextForNextScope(Parser, Memory);



  u32 ArgCount = T2 ? 2 : 1;
  meta_func_arg_buffer Args = MetaFuncArgBuffer(ArgCount, Memory);

  Args.Start[0] = ReplacementPattern(T1.Value, Datatype());
  if (T2) { SetLast(&Args, ReplacementPattern(T2->Value, PoofIndex(0,0))); }

  meta_func Func = {
    .Name = FuncName,
    .Args = Args,
    .Body = Body,
    False, False
  };

  return Func;
}

link_internal meta_func
ParseMetaFunctionDef(parser* Parser, cs FuncName, memory_arena *Memory)
{
  TIMED_FUNCTION();

  RequireToken(Parser, CTokenType_OpenParen);
  meta_func_arg_stream ArgStream = MetaFuncArgStream(AllocateArena());

  while (ParseMetaFuncDefArg(Parser, &ArgStream));
  RequireToken(Parser, CTokenType_CloseParen);

  meta_func Func = MetaFunc(FuncName, Compact(&ArgStream, Memory));

  ParseMetaFuncTags(Parser, &Func);

  Func.Body = GetBodyTextForNextScope(Parser, Memory);


  return Func;
}

// TODO(Jesse): Rewrite this so it doesn't suck
//
// Or maybe just delete it?
#if 0
link_internal void
RemoveAllMetaprogrammingOutputRecursive(const char * OutputPath)
{

  WIN32_FIND_DATA fdFile;
  HANDLE hFind = NULL;

  char sPath[2048];

  //Specify a file mask. *.* = We want everything!
  sprintf(sPath, "%s\\*.*", OutputPath);

  if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
  {
      printf("Path not found: [%s]\n", OutputPath);
  }

  do
  {
      //Find first file will always return "."
      //    and ".." as the first two directories.
      if(strcmp(fdFile.cFileName, ".") != 0
              && strcmp(fdFile.cFileName, "..") != 0)
      {
          //Build up our file path using the passed in
          //  [OutputPath] and the file/foldername we just found:
          sprintf(sPath, "%s\\%s", OutputPath, fdFile.cFileName);

          //Is the entity a File or Folder?
          if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
          {
              printf("Descending into Directory: %s\n", sPath);
              RemoveAllMetaprogrammingOutputRecursive(sPath);
          }
          else
          {
              printf("Removeing File: %s\n", sPath);
              Ensure(Remove(CS(sPath)));
          }
      }
  }
  while(FindNextFile(hFind, &fdFile));

  FindClose(hFind);

#if 0
  for (u32 ParserIndex = 0;
      ParserIndex < Count(ParsedFiles);
      ++ParserIndex)
  {
    parser* Parser = ParsedFiles->Start+ParserIndex;
    if (IsMetaprogrammingOutput(Parser->Filename, Args->Outpath))
    {
      Info("Removing %S", Parser->Filename);
      Remove(Parser->Filename);
      continue;
    }
  }
#endif

}
#endif

#if 0
link_internal counted_string
ParseMultiLineTodoValue(parser* Parser, memory_arena* Memory)
{
  string_builder Builder = {};

  while (PeekTokenRaw(Parser).Type != CTokenType_CommentMultiLineEnd)
  {
    c_token T = PopTokenRaw(Parser);

    if (T.Type == CTokenType_Newline)
    {
      EatWhitespace(Parser);
      if (OptionalToken(Parser, CTokenType_Star))
      {
        EatWhitespace(Parser);
      }
      Append(&Builder, CSz(" "));
    }
    else
    {
      Append(&Builder, T.Value);
    }

    continue;
  }

  Ensure( PopTokenRaw(Parser).Type == CTokenType_CommentMultiLineEnd );

  counted_string Result = Trim(Finalize(&Builder, Memory));
  return Result;
}
#endif

void
ParseComment_old_dead_deprecated()
{
#if 0
        c_token CommentStartToken = PopTokenRaw(Parser);
        c_token FirstInteriorT = PeekToken(Parser);
        if ( StringsMatch(FirstInteriorT.Value, CSz("TODO")) )
        {
          RequireToken(Parser, CToken(CSz("TODO")));

          if (OptionalToken(Parser, CTokenType_OpenParen))
          {
            counted_string PersonName = RequireToken(Parser, CTokenType_Identifier).Value;
            counted_string_stream TodoTags = {};
            b32 GotAnyTags = False;

            counted_string IdValue = {};
            OptionalToken(Parser, CTokenType_Comma);

            b32 GeneratedNewId = False;
            if (OptionalToken(Parser, CToken(CSz("id"))))
            {
              RequireToken(Parser, CTokenType_Colon);
              IdValue = RequireToken(Parser, CTokenType_IntLiteral).Value;
            }
            else
            {
              Assert(!IdValue.Count);
              GeneratedNewId = True;
              IdValue = CS(++LargestIdFoundInFile);
              /* Push(CToken(CS(" id: ")), &Parser->OutputTokens); */
              /* Push(CToken(IdValue),     &Parser->OutputTokens); */
            }

            OptionalToken(Parser, CTokenType_Comma);

            if (StringsMatch(PeekToken(Parser).Value, CSz("tags")))
            {
              if (GeneratedNewId)
              {
                /* Push(CToken(CTokenType_Comma), &Parser->OutputTokens); */
              }

              RequireToken(Parser, CToken(CSz("tags")));
              GotAnyTags = True;
              RequireToken(Parser, CTokenType_Colon);
              Push(&TodoTags, RequireToken(Parser, CTokenType_Identifier).Value, Memory);

              while (OptionalToken(Parser, CTokenType_Comma))
              {
                Push(&TodoTags, RequireToken(Parser, CTokenType_Identifier).Value, Memory);
              }
            }

            if (!GotAnyTags)
            {
              counted_string Tag = CSz("untagged");
              Push(&TodoTags, Tag, Memory);
            }

            RequireToken(Parser, CTokenType_CloseParen);
            OptionalToken(Parser, CTokenType_Colon);

            counted_string TodoValue = {};
            if ( T.Type == CTokenType_CommentSingleLine )
            {
              string_from_parser Builder = StartStringFromParser(Parser);
                EatUntilIncluding(Parser, CTokenType_Newline);
              TodoValue = FinalizeStringFromParser(&Builder);

              TodoValue = Trim(TodoValue);
            }
            else
            {
              TodoValue = ParseMultiLineTodoValue(Parser, Memory);
            }

            person* Person = GetExistingOrCreate(People, PersonName, Memory);

            ITERATE_OVER(&TodoTags)
            {
              counted_string* TodoTag = GET_ELEMENT(Iter);
              tag* Tag = GetExistingOrCreate(&Person->Tags, *TodoTag, Memory);
              UpdateTodo(&Tag->Todos, Todo(IdValue, TodoValue, True), Memory);
            }

          }
        }
        else
        {
          EatComment(Parser, CommentStartToken.Type);
        }
#endif
}

link_internal b32
ParseAndTypeCheckArgs(parse_context *Ctx, parser *Parser, c_token *FunctionT, meta_func *Func, meta_func_arg_buffer *ArgInstances, meta_func_arg_buffer *ArgsInScope, memory_arena *Memory)
{
  b32 TypeCheckPassed = True;

  parser ArgParser = EatBetweenExcluding_Parser(Parser, CTokenType_OpenParen, CTokenType_CloseParen, Memory);

  *ArgInstances = MetaFuncArgBuffer(Func->Args.Count, Memory);
  for (u32 ArgIndex = 0; ArgIndex < Func->Args.Count; ++ArgIndex)
  {
    meta_func_arg *ArgDef = Func->Args.Start + ArgIndex;
    if (ParseAndTypecheckArgument(Ctx, &ArgParser, ArgInstances->Start+ArgIndex, ArgDef, ArgsInScope))
    {
    }
    else
    {
      // NOTE(Jesse): Need this because the errors emitted from
      // ParseAndTypecheckArgument don't contain the whole parse context, just
      // the arguments.  Also, we have to propagate the error code to the main
      // parser.
      ParseError( Parser,
                  ArgParser.ErrorCode,
                  FormatCountedString( GetTranArena(),
                                       CSz("Could not parse args for (%S)."),
                                       FunctionT->Value ),
                  FunctionT );

      TypeCheckPassed = False;
      return TypeCheckPassed;
    }
  }

  if (c_token *T = PeekTokenPointer(&ArgParser))
  {
    TypeCheckPassed = False;
    ParseInfoMessage( &ArgParser, CSz("Function had too many arugments"), FunctionT);
  }

  return TypeCheckPassed;
}

link_internal counted_string
CallFunction(parse_context *Ctx, c_token *FunctionT, meta_func *Func, meta_func_arg_buffer *ArgInstances, memory_arena *Memory, umm *Depth)
{
  cs Result = {};
  if (Ctx->CallStackDepth < 128)
  {
    ++Ctx->CallStackDepth;
    Result = Execute(Func, ArgInstances, Ctx, Memory, Depth);
    --Ctx->CallStackDepth;

    if (Func->Body.ErrorCode)
    {
      Result.Start = 0;
      Result.Count = 0;
      /* ParseInfoMessage( Ctx->CurrentParser, */
      /*                   FormatCountedString(GetTranArena(), */
      /*                                       CSz("Unable to generate code for (func %S)."), Func->Name), FunctionT); */
    }
  }
  else
  {
    PoofError(Ctx->CurrentParser,
              ParseErrorCode_StackOverflow,
              FormatCountedString( GetTranArena(),
                                   CSz("Stack overflow when trying to execute (%S). Poof has a stack frame limit of (%u) frames."),
                                   Func->Name,
                                   Ctx->CallStackDepth-1),
              FunctionT);
  }

  return Result;
}

link_internal tuple_cs_cs_buffer
GoGoGadgetMetaprogramming(parse_context* Ctx, todo_list_info* TodoInfo)
{
  TIMED_FUNCTION();

  tuple_cs_cs_buffer_builder Builder = {};
  Builder.Chunks.Memory = &Global_PermMemory;

  program_datatypes *Datatypes   = &Ctx->Datatypes;
  memory_arena *Memory           = Ctx->Memory;

  parser *Parser = Ctx->CurrentParser;
  Assert(IsAtBeginning(Parser));
  while (c_token *T = PeekTokenPointer(Parser))
  {
    switch( T->Type )
    {
      case CTokenType_CommentMultiLine:
      case CTokenType_CommentSingleLine:
      {
        Assert(False);
        c_token CommentToken = RequireTokenRaw(Parser, T->Type);
        /* ParseComment_old_dead_deprecated(Parser); */
        /* EatComment(Parser, CommentStartToken.Type); */

      } break;

      case CTokenType_Poof:
      {
        c_token MetaToken = RequireToken(Parser, CTokenType_Poof);
        RequireToken(Parser, CTokenType_OpenParen);

        if (OptionalToken(Parser, CTokenType_Ellipsis)) // Just ignore poof tags containing (...) .. It's probably the '#define poof(...)' thing
        {
          RequireToken(Parser, CTokenType_CloseParen);
        }
        else if (OptionalToken(Parser, CTokenType_CloseParen))
        {
          // We're closing an empty `poof()` statement.
          // TODO(Jesse): Should we emit a warning here?
        }
        else
        {
          poof_tag Tag = {};

          // Skip Tags, we've already parsed them and attached them to their datastructures
          while (PeekToken(Parser).Type == CTokenType_At)
          {
            while (TryParsePoofTag(Parser, &Tag));
            RequireToken(Parser, CTokenType_CloseParen);
          }

          // TODO(jesse): This is weird af.
          if (Tag.Name.Start == 0)
          {
            c_token *DirectiveT = RequireTokenPointer(Parser, CTokenType_Identifier);
            metaprogramming_directive Directive = MetaprogrammingDirective(DirectiveT->Value);
            ExecuteMetaprogrammingDirective(Ctx, Directive, DirectiveT, &Builder);
          }
        }

      } break;

      default:
      {
        PopToken(Parser);
      } break;
    }

    continue;
  }

  tuple_cs_cs_buffer Result = Finalize(&Builder, Memory);
  return Result;
}

link_internal void
WriteTodosToFile(person_stream* People, memory_arena* Memory)
{
  random_series Rng = {.Seed = 123125};
  native_file TempFile = GetTempFile(&Rng, Memory);
  b32 AllWritesSucceeded = TempFile.Handle ? True : False;
  if (AllWritesSucceeded)
  {
    ITERATE_OVER_AS(person, People)
    {
      person* Person = GET_ELEMENT(personIter);
      AllWritesSucceeded &= WriteToFile(&TempFile, CSz("# "));
      AllWritesSucceeded &= WriteToFile(&TempFile, Person->Name);
      AllWritesSucceeded &= WriteToFile(&TempFile, CSz("\n"));
      ITERATE_OVER(&Person->Tags)
      {
        tag* Tag = GET_ELEMENT(Iter);

        todo_iterator InnerIter = Iterator(&Tag->Todos);
        if (IsValid(&InnerIter))
        {
          AllWritesSucceeded &= WriteToFile(&TempFile, CSz("  ## "));
          AllWritesSucceeded &= WriteToFile(&TempFile, Tag->Tag);
          AllWritesSucceeded &= WriteToFile(&TempFile, CSz("\n"));

          for (;
              IsValid(&InnerIter);
              Advance(&InnerIter))
          {
            todo* Todo = GET_ELEMENT(InnerIter);
            if (Todo->FoundInCodebase)
            {
              AllWritesSucceeded &= WriteToFile(&TempFile, CSz("    - #"));
              AllWritesSucceeded &= WriteToFile(&TempFile, Todo->Id);
              AllWritesSucceeded &= WriteToFile(&TempFile, CSz(" "));
              AllWritesSucceeded &= WriteToFile(&TempFile, Todo->Value);
              AllWritesSucceeded &= WriteToFile(&TempFile, CSz("\n"));
            }
          }

          AllWritesSucceeded &= WriteToFile(&TempFile, CSz("\n"));
        }
      }
    }
  }
  else
  {
    Error("Opening Tempfile");
  }

  AllWritesSucceeded &= CloseFile(&TempFile);

  if (AllWritesSucceeded)
  {
    Rename(TempFile.Path, CSz("todos.md"));
  }

  return;
}

link_internal void WalkAst(ast_node* Ast);
link_internal void WalkAst(ast_node_statement* Ast);

link_internal void
WalkAst(ast_node_expression* Ast)
{
  if (Ast)
  {
    WalkAst(Ast->Value);
    WalkAst(Ast->Next);
  }
}

link_internal void
WalkAst(ast_node_statement* Ast)
{
  if (Ast)
  {
    WalkAst(Ast->LHS);
    WalkAst(Ast->RHS);
    WalkAst(Ast->Next);
  }
}

link_internal void
WalkAst(ast_node* Ast)
{
  if (Ast)
  {
    switch(Ast->Type)
    {
      case type_ast_node_function_call:
      {
        auto Child = SafeAccess(ast_node_function_call, Ast);
      } break;

      case type_ast_node_statement:
      {
        auto Child = SafeAccess(ast_node_statement, Ast);
        WalkAst(Child);
      } break;

      case type_ast_node_variable_def:
      {
        auto Child = SafeAccess(ast_node_variable_def, Ast);
        WalkAst(Child->Value);
      } break;

      case type_ast_node_access:
      {
        auto Child = SafeAccess(ast_node_access, Ast);
        WalkAst(Child->Symbol);
      } break;

      case type_ast_node_expression:
      {
        auto Child = SafeAccess(ast_node_expression, Ast);
        WalkAst(Child);
      } break;

      case type_ast_node_parenthesized:
      {
        auto Child = SafeAccess(ast_node_parenthesized, Ast);
        WalkAst(Child->Expr);
        if (Child->IsCast)
        {
          Assert(Child->CastValue);
          WalkAst(Child->CastValue);
        }
        else
        {
          Assert(!Child->CastValue);
        }
      } break;

      case type_ast_node_operator:
      {
        auto Child = SafeAccess(ast_node_operator, Ast);
        WalkAst(Child->Operand);
      } break;

      case type_ast_node_return:
      {
        auto Child = SafeAccess(ast_node_return, Ast);
        WalkAst(Child->Value);
      } break;

      case type_ast_node_initializer_list:
      case type_ast_node_symbol:
      case type_ast_node_type_specifier:
      case type_ast_node_literal:
      {
      } break;

      case type_ast_node_noop:
      {
        InvalidCodePath();
      } break;

      InvalidDefaultCase;
    }
  }
}

#ifndef EXCLUDE_PREPROCESSOR_MAIN

/* link_internal void */
/* PrintHashtable(parser_hashtable *Table) */
/* { */
/*   for (u32 BucketIndex = 0; BucketIndex < Table->Size; ++BucketIndex) */
/*   { */
/*     auto Bucket = Table->Elements[BucketIndex]; */
/*     while (Bucket) */
/*     { */
/*       DebugLine("bucket(%u) --------------------------------------------------------------------------------------------------------------------------", BucketIndex); */
/*       DebugPrint(&Bucket->Element, 0); */
/*       Bucket = Bucket->Next; */
/*     } */
/*   } */
/* } */

link_internal void
PrintHashtable(datatype_hashtable *Table)
{
  for (u32 BucketIndex = 0; BucketIndex < Table->Size; ++BucketIndex)
  {
    auto Bucket = Table->Elements[BucketIndex];
    if (Bucket) { DebugLine("bucket(%u) --------------------------------------------------------------------------------------------------------------------------", BucketIndex); }
    while (Bucket)
    {
      DebugPrint(&Bucket->Element, 0);
      Bucket = Bucket->Next;
    }
  }
}


void
ScanForMutationsAndOutput(parser *Parser, counted_string OutputPath, memory_arena *Memory)
{
  TIMED_FUNCTION();

  c_token_cursor *Tokens = Parser->Tokens;

  umm TotalTokens = TotalElements(Tokens);

  b32 NeedsToBeOverwritten = false;

  c_token *Start = Tokens->Start;
  for (umm TokenIndex = 0; TokenIndex < TotalTokens; ++TokenIndex)
  {
    c_token *T = Start + TokenIndex;

    if (StringsMatch(T->Value, CSz("bonsai_function")))
    {
      T->Value = CSz("link_internal");
      NeedsToBeOverwritten = true;
    }

    // This is how we signal that we've got a `poof` statement without an
    // include directive following it.  This system should probably be fleshed
    // out to support more arbitrary output types, but this is fine for now.
    if (T->Type == CT_PoofInsertedCode)
    {
      NeedsToBeOverwritten = true;
      break;
    }
  }

  if (NeedsToBeOverwritten)
  {
    RewriteOriginalFile(Parser, OutputPath, Parser->Tokens->Filename, Memory);
  }
}

link_external const char *
DoPoofForWeb(char *zInput, umm InputLen)
{
  const char *Result = {};

  counted_string Input = CS(zInput, InputLen);

  SetTerminalColorsOff();

  memory_arena tmpMemory = {};
  memory_arena *Memory = &tmpMemory;

  /* AllocateAndInitThreadStates(Memory); */

  parse_context CtxObj = AllocateParseContext(Memory);
  parse_context *Ctx = &CtxObj;

  parser *Parser = ParserForAnsiStream(Ctx, AnsiStream(Input), TokenCursorSource_RootFile, Ctx->Memory);
  Ctx->CurrentParser = Parser;


  if (RunPreprocessor(Ctx, Parser, 0, Memory))
  {
    FullRewind(Ctx->CurrentParser);
    ParseDatatypes(Ctx, Parser);

    FullRewind(Ctx->CurrentParser);
    tuple_cs_cs_buffer OutputBuffer = GoGoGadgetMetaprogramming(Ctx, 0);

    if (Parser->ErrorCode == ParseErrorCode_None)
    {
      string_builder Builder = {};
      for (u32 TupleIndex = 0; TupleIndex < OutputBuffer.Count; ++TupleIndex)
      {
        counted_string Filename = OutputBuffer.E[TupleIndex].E0;
        counted_string Code = OutputBuffer.E[TupleIndex].E1;

        Append(&Builder, CSz("// "));
        Append(&Builder, Filename);
        Append(&Builder, CSz("\n\n\n"));

        Append(&Builder, Code);
        Append(&Builder, CSz("\n\n"));
      }

      counted_string S = Finalize(&Builder, Memory, True);
      Result = S.Start;
    }
    else
    {
      string_builder Builder = {};
      Append(&Builder, CSz("//\n// ERROR\n//\n\n"));
      ITERATE_OVER(&Global_ErrorStream)
      {
        counted_string *E = GET_ELEMENT(Iter);
        Append(&Builder, *E);
      }
      Result = Finalize(&Builder, Memory, True).Start;
      Deallocate(&Global_ErrorStream);
    }
  }

  return Result;
}

s32
main(s32 ArgCount_, const char** ArgStrings)
{
  u32 ArgCount = (u32)ArgCount_;
  SetupStdout(ArgCount, ArgStrings);

  memory_arena Memory_ = {};
  memory_arena* Memory = &Memory_;
  Memory->NextBlockSize = Megabytes(256);

  application_api AppApi = {};
  bonsai_stdlib Stdlib = {};
  bonsai_init_flags InitFlags = {};

  Global_Stdlib = &Stdlib;

  /* AllocateAndInitThreadStates(Memory); */

  parse_context Ctx = AllocateParseContext(Memory);
  Ctx.Args = ParseArgs(ArgStrings, ArgCount, &Ctx, Memory);

  Ctx.MetaFunctions.Memory = &Global_PermMemory;

#if BONSAI_DEBUG_SYSTEM_API
  if (Ctx.Args.DoDebugWindow)
  {
    InitFlags = bonsai_init_flags(InitFlags|BonsaiInit_OpenWindow|BonsaiInit_InitDebugSystem);

    /* GetDebugState()->OpenAndInitializeDebugWindow(); */
    /* GetDebugState()->FrameBegin(True, True); */

  }
#endif
  /* Info("%d", InitFlags); */

  Ensure( InitializeBonsaiStdlib( InitFlags, &AppApi, &Stdlib, Memory) );

  /* DEBUG_REGISTER_ARENA(Memory); */
  /* DEBUG_REGISTER_ARENA(&Global_PermMemory); */

  /* AllocateAndInitThreadStates(Memory); */


  if (Ctx.Args.HelpTextPrinted)
  {
    return CLI_SUCCESS_EXIT_CODE;
  }

  TryCreateDirectory(TMP_DIR_ROOT);
  TryCreateDirectory(Ctx.Args.Outpath);


  b32 Success = False;
  if (ArgCount > 1)
  {
    // TODO(Jesse): Make ParseArgs operate on the parse context directly?
    Ctx.IncludePaths = &Ctx.Args.IncludePaths;

    todo_list_info TodoInfo = {};

    /* todo_list_info TodoInfo = { */
    /*   .People = ParseAllTodosFromFile(CSz("todos.md"), Memory), */
    /* }; */

    Assert(TotalElements(&Ctx.Args.Files) == 1);
    Assert(Ctx.Args.Files.Start == Ctx.Args.Files.At);
    counted_string ParserFilename = Ctx.Args.Files.Start[0];

    Info("Poofing %S", ParserFilename);

    umm ParserFilenameHash = umm(Hash(&ParserFilename));
    TempFileEntropy.Seed = ParserFilenameHash;

    parser *Parser = PreprocessedParserForFile(&Ctx, ParserFilename, TokenCursorSource_RootFile, 0);

#if BONSAI_DEBUG_SYSTEM_API
    if (Ctx.Args.DoDebugWindow) { MAIN_THREAD_ADVANCE_DEBUG_SYSTEM(GetDt()); }
#endif

    if (Parser->ErrorCode == ParseErrorCode_None)
    {
      /* RemoveAllMetaprogrammingOutputRecursive(GetNullTerminated(Ctx.Args.Outpath)); */

      // TODO(Jesse): Do we actually need this?
      Ctx.CurrentParser = Parser;
      FullRewind(Ctx.CurrentParser);

      ParseDatatypes(&Ctx, Parser);

      /* PrintHashtable(&Ctx.ParserHashtable); */
      /* PrintHashtable(&Ctx.Datatypes.DatatypeHashtable); */

      FullRewind(Ctx.CurrentParser);

#if BONSAI_DEBUG_SYSTEM_API
    if (Ctx.Args.DoDebugWindow) { MAIN_THREAD_ADVANCE_DEBUG_SYSTEM(GetDt()); }
#endif

      GoGoGadgetMetaprogramming(&Ctx, &TodoInfo);

      auto Table = &Ctx.ParserHashtable;
      for (u32 BucketIndex = 0; BucketIndex < Table->Size; ++BucketIndex)
      {
        auto Bucket = Table->Elements[BucketIndex];
        while (Bucket)
        {
          ScanForMutationsAndOutput(&Bucket->Element, Ctx.Args.Outpath, Memory);
          Bucket = Bucket->Next;
        }
      }

#if 0
      if (Parser->Valid)
      {
        Assert(Parser->Tokens->At == Parser->Tokens->Start)
        if (Output(Parser->Tokens, Concat(Parser->Filename, CSz("_tmp"), Memory), Memory))
        {
          LogSuccess("Output '%S'.", Parser->Filename);
        }
        else
        {
          Erorr("Failed writing to '%S'", Parser->Filename);
        }
      }
#endif

      if (Parser->ErrorCode == ParseErrorCode_None)
      {
        Success = True;
      }
    }
    else
    {
      Warn("Failed tokenizing file (%S)", ParserFilename);
    }

#if 0
    ITERATE_OVER(&Ctx.Datatypes.Functions)
    {
      function_decl *Func = GET_ELEMENT(Iter);

      if (Func->ReturnType.TemplateSource.Count) { continue; }

      PushParser(&Ctx.Parser, Func->Body, parser_push_type_root);
      Func->Ast = ParseAllStatements(&Ctx);
      WalkAst(Func->Ast);

      Ctx.Parser.Depth = 0;
    }
#endif

    /* WriteTodosToFile(&TodoInfo.People, Memory); */

  }
  else
  {
    Warn("No input files specified, exiting.");
  }


#if BONSAI_DEBUG_SYSTEM_API
  // BootstrapDebugSystem is behind a flag, or it could have failed.
  if (Ctx.Args.DoDebugWindow && GetDebugState())
  {
    debug_state *DebugState = GetDebugState();
    DebugState->UIType = DebugUIType_CallGraph;
    /* DebugState->DisplayDebugMenu = True; */
    DebugState->DebugDoScopeProfiling = False;

    MAIN_THREAD_ADVANCE_DEBUG_SYSTEM(GetDt());

    heap_allocator Heap = InitHeap(Gigabytes(1), False);
    renderer_2d Ui = {};

    SetRenderer(&Ui);

    v2 MouseP, MouseDP, ScreenDim;

    PlatformMakeRenderContextCurrent(&Stdlib.Os);
    InitRenderer2D(&Ui, &Heap, Memory, &Stdlib.Plat.MouseP, &Stdlib.Plat.MouseDP, &Stdlib.Plat.ScreenDim, &Stdlib.Plat.Input);

    r32 Dt = 1.f;

    // Do a frame to just toggle the UI on.  Pretty dumb.. but whatever.
    /* DEBUG_FRAME_BEGIN(&Ui, Dt, True, False); */
    /* DEBUG_FRAME_END(Dt); */
    UiFrameEnd(&Ui);

    UNPACK_STDLIB(&Stdlib);

    while (Os->ContinueRunning)
    {

      // TODO(Jesse): Make a stdlib function that wraps all this stuff up
      // @stdlib_frame_preamble
      ResetInputForFrameStart(&Plat->Input, 0);

      // @stdlib_frame_preamble
      v2 LastMouseP = Plat->MouseP;
      ProcessOsMessages(&Stdlib.Os, &Stdlib.Plat);
      Plat->MouseDP = LastMouseP - Plat->MouseP;

      // @stdlib_frame_preamble?
      Ui.Input = &Plat->Input;
      Ui.MouseP = &Plat->MouseP;
      Ui.MouseDP = &Plat->MouseDP;
      Ui.ScreenDim = &Plat->ScreenDim;


      UiFrameBegin(&Ui);

      if (Stdlib.Plat.Input.Escape.Clicked) { break; }

      Info("Hi");

      b32 Toggle = False, Toggle2 = False;
      if (Stdlib.Plat.Input.F1.Clicked) { Info("hi"); Toggle = True; }
      if (Stdlib.Plat.Input.F2.Clicked) { Info("hi2"); Toggle2 = True; }

      Info("%d", Stdlib.Plat.Input.F1.Clicked);

      /* DebugState->DisplayDebugMenu = True; */
      DEBUG_FRAME_BEGIN(&Ui, Dt, Toggle, Toggle2);

      PushTableStart(&Ui);
      PushTableEnd(&Ui);

      DEBUG_FRAME_END(Dt);
      {
        layout DefaultLayout = {};
        render_state RenderState = {};
        RenderState.Layout = &DefaultLayout;

        SetWindowZDepths(Ui.CommandBuffer);
        FlushCommandBuffer(&Ui, &RenderState, Ui.CommandBuffer, &DefaultLayout);
      }

      UiFrameEnd(&Ui);

      BonsaiSwapBuffers(&Stdlib.Os);

      auto GL = GetGL();

      GL->BindFramebuffer(GL_FRAMEBUFFER, 0);
      GL->ClearColor(0.2f, 0.2f, 0.2f, 1.f);
      GL->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /* Ensure(RewindArena(GetTranArena())); */
    }
  }
#endif

  /* TryDeleteDirectory(TMP_DIR_ROOT); */

  /* DebugPrint(Ctx); */

  s32 Result = Success ? CLI_SUCCESS_EXIT_CODE : CLI_FAILURE_EXIT_CODE ;
  return Result;
}
#endif
