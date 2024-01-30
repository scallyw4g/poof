// ./poof/poof.h:769:0

link_internal meta_func
MetaFunc( counted_string Name , meta_func_arg_buffer Args , parser Body , b32 OmitInclude  )
{
  meta_func Reuslt = {
    .Name = Name,
    .Args = Args,
    .Body = Body,
    .OmitInclude = OmitInclude
  };
  return Reuslt;
}

