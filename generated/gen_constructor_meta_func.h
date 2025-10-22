// ./poof/poof.h:816:0


link_internal meta_func
MetaFunc(  cs   Name , meta_func_arg_buffer   Args , parser   Body , meta_func_directive   Directives , cs   HeaderFormatString  )
{
  meta_func Reuslt = {
            .Name = Name

,        .Args = Args
,
        .Body = Body
,
        .Directives = Directives
,
        .HeaderFormatString = HeaderFormatString
  };

  
  return Reuslt;
}

