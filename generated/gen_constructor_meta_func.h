// ./poof/poof.h:787:0

link_internal meta_func
MetaFunc(  cs   Name , meta_func_arg_buffer   Args , parser   Body , b32   OmitInclude , b32   CodeFragment  )
{
  meta_func Reuslt = {
            .Name = Name

,        .Args = Args
,
        .Body = Body
,
        .OmitInclude = OmitInclude
,
        .CodeFragment = CodeFragment
  };

  
  return Reuslt;
}

