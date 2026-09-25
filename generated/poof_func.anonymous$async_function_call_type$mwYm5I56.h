// callsite
// ./include/bonsai_stdlib/src/threadpool.cpp:131:0

// def (poof_func.anonymous)
// ./include/bonsai_stdlib/src/threadpool.cpp:131:0
{
  tmatch( compile_shader_pair_async_params, WrappedTask, FuncParams );
  ExecFunction(FuncParams);
} break;
{
  tmatch( finalize_and_flush_async_params, WrappedTask, FuncParams );
  ExecFunction(FuncParams);
} break;


