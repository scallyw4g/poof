// callsite
// ./include/bonsai_stdlib/src/threadpool.cpp:131:0

// def (poof_func.anonymous)
// ./include/bonsai_stdlib/src/threadpool.cpp:131:0
{
  tmatch( output_async_params, WrappedTask, FuncParams );
  ExecFunction(FuncParams);
} break;
{
  tmatch( compile_shader_pair_async_params, WrappedTask, FuncParams );
  ExecFunction(FuncParams);
} break;


