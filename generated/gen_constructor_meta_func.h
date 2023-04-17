link_internal meta_func
MetaFunc( counted_string Name , meta_func_arg_buffer Args , parser Body  )
{
  meta_func Reuslt = {
    .Name = Name,
    .Args = Args,
    .Body = Body
  };
  return Reuslt;
}

