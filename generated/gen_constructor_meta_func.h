// ./poof/poof.h:640:0

link_internal meta_func
MetaFunc(  cs   Name , c_token *  SourceToken , meta_func_arg_buffer   Args , parser   Body , meta_func_directive   Directives , cs   HeaderFormatString  )
{
  meta_func Reuslt = {
            .Name = Name

,        .SourceToken = SourceToken
,
        .Args = Args
,
        .Body = Body
,
        .Directives = Directives
,
        .HeaderFormatString = HeaderFormatString
  };

  
  return Reuslt;
}

