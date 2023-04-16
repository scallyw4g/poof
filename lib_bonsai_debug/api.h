#pragma once

#include <x86intrin.h>
#include <assert.h>

struct v2;
struct debug_scope_tree;

#define CAssert(condition) static_assert((condition), #condition )
#define Assert(condition) assert(condition)
#define PI64 (3.1415926535897932384626433832795028841971693993)
#define PI32 ((float)PI64)

#define local_persist     static
#define global_variable   static
#define debug_global      static

// Internal linkage
#define link_internal     static

// External linkage (pointer is from another lib)
#define link_external     extern "C"

// Export linkage (expose function pointer)
#if BONSAI_WIN32
#define link_export __declspec(dllexport) link_external

#else
#define link_export link_external

#endif

#define True  (1)
#define False (0)

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))

// 8 Bit types
typedef char           s8;
typedef unsigned char  u8;

// 16 Bit types
typedef short          s16;
typedef unsigned short u16;

// 32 Bit types
typedef int          s32;
typedef unsigned int u32;
typedef float        r32;
typedef u32          b32;

typedef r32      radians;
typedef r32      degrees;

// 64 Bit types
typedef long long int           s64;
typedef unsigned long long int  u64;
typedef double                  r64;
#if BONSAI_EMCC
typedef u32                     umm;
#else
typedef u64                     umm;
#endif
typedef u64                     b64;

CAssert(sizeof(s64) == 8);
CAssert(sizeof(u64) == 8);
CAssert(sizeof(r64) == 8);
CAssert(sizeof(umm) == sizeof(void*));

CAssert(sizeof(s32) == 4);
CAssert(sizeof(u32) == 4);
CAssert(sizeof(r32) == 4);

CAssert(sizeof(s16) == 2);
CAssert(sizeof(u16) == 2);

CAssert(sizeof(s8) == 1);
CAssert(sizeof(u8) == 1);

#define s8_MAX  ((s8) 0x7f)
#define s16_MAX ((s16)0x7fff)
#define s32_MAX ((s32)0x7fffffff)
#define s64_MAX ((s64)0x7fffffffffffffff)

#define s8_MIN  ((s8) 0x80)
#define s16_MIN ((s16)0x8000)
#define s32_MIN ((s32)0x80000000)
#define s64_MIN ((s64)0x8000000000000000)

#define u8_MAX  (0xff)
#define u16_MAX (0xffff)
#define u32_MAX (0xffffffff)
#define u64_MAX (0xffffffffffffffff)

#define f32_MAX (1E+37f)
#define f32_MIN (1E-37f)

#if BONSAI_EMCC
#define umm_MAX u32_MAX
#define umm_MIN u32_MIN
#else
#define umm_MAX u64_MAX
#define umm_MIN u64_MIN
#endif

#if 0
// #include <stdint.h>
CAssert(s8_MAX  == INT8_MAX);
CAssert(s16_MAX == INT16_MAX);
CAssert(s32_MAX == INT32_MAX);
CAssert(s64_MAX == INT64_MAX);

CAssert(s8_MIN  == INT8_MIN);
CAssert(s16_MIN == INT16_MIN);
CAssert(s32_MIN == INT32_MIN);
CAssert(s64_MIN == INT64_MIN);

CAssert(u8_MAX  == UINT8_MAX);
CAssert(u16_MAX == UINT16_MAX);
CAssert(u32_MAX == UINT32_MAX);
CAssert(u64_MAX == UINT64_MAX);
#endif

struct counted_string
{
  umm Count;
  const char* Start; // TODO(Jesse, id: 94, tags: cleanup, open_question): Make this non-const?
};

typedef counted_string cs;
struct debug_state;
struct debug_profile_scope;
struct debug_thread_state;

struct input;
struct memory_arena;
struct mutex;
struct heap_allocator;



#define BONSAI_NO_ARENA (0xFFFFFFFFFFFFFFFF)
struct memory_record
{
  const char* Name;
  umm ArenaAddress;     // If this is set to BONSAI_NO_ARENA you can set "ArenaMemoryBlock" to a null-terminated string identifying the allocation site
  umm ArenaMemoryBlock; // @ArenaMemoryBlock-as-char-pointer
  umm StructSize;
  umm StructCount;
  u32 ThreadId;

  u32 PushCount;
};


typedef debug_scope_tree*    (*get_read_scope_tree_proc)(u32);
typedef debug_scope_tree*    (*get_write_scope_tree_proc)();
typedef void                 (*debug_clear_framebuffers_proc)          ();
typedef void                 (*debug_frame_end_proc)                   (v2 *MouseP, v2 *MouseDP, v2 ScreenDim, input *Input, r32 dt);
typedef void                 (*debug_frame_begin_proc)                 (b32, b32);
typedef void                 (*debug_register_arena_proc)              (const char*, memory_arena*, u32);
typedef void                 (*debug_worker_thread_advance_data_system)(void);
typedef void                 (*debug_main_thread_advance_data_system)  (r64);

typedef void                 (*debug_mutex_waiting_proc)               (mutex*);
typedef void                 (*debug_mutex_aquired_proc)               (mutex*);
typedef void                 (*debug_mutex_released_proc)              (mutex*);

typedef debug_profile_scope* (*debug_get_profile_scope_proc)           ();
typedef void*                (*debug_allocate_proc)                    (memory_arena*, umm, umm, const char*, s32 , const char*, umm, b32);
typedef void                 (*debug_register_thread_proc)             (u32);
typedef void                 (*debug_track_draw_call_proc)             (const char*, u32);
typedef debug_thread_state*  (*debug_get_thread_local_state)           (void);
typedef void                 (*debug_value)                            (r32, const char*);
typedef void                 (*debug_dump_scope_tree_data_to_console)  ();

typedef void                 (*debug_clear_memory_records_proc)          (memory_arena*);
typedef void                 (*debug_write_memory_record_proc)           (memory_record*);

typedef b32                  (*debug_open_window_proc)                 ();
typedef b32                  (*debug_redraw_window_proc)               ();


typedef debug_state*         (*get_debug_state_proc)  ();
typedef u64                  (*query_memory_requirements_proc)();
typedef get_debug_state_proc (*init_debug_system_proc)(debug_state *, u64 DebugStateSize);
typedef void                 (*patch_debug_lib_pointers_proc)(debug_state *);


struct debug_profile_scope
{
  u64 StartingCycle;
  u64 EndingCycle;
  const char* Name;

  b32 Expanded;

  debug_profile_scope* Sibling;
  debug_profile_scope* Child;
  debug_profile_scope* Parent;

  /* b64 Pad; */
};
// NOTE(Jesse): I thought maybe this would increase perf .. it had a negligible
// effect These structs are per-thread so there's no sense in having them
// cache-line sized.  Something that would probably speed this up is not storing
// the sibling/child/parent pointers and instead store a one-past-last index into
// a statically allocated buffer (instead of dynamically allocating new scopes
// every time we need another one)
/* CAssert(sizeof(debug_profile_scope) == CACHE_LINE_SIZE); */

struct debug_scope_tree
{
  debug_profile_scope *Root;

  debug_profile_scope **WriteScope;
  debug_profile_scope *ParentOfNextScope;

  u64 FrameRecorded;
};

enum debug_ui_type
{
  DebugUIType_None = 0,

  DebugUIType_CallGraph             = (1 << 1),
  DebugUIType_CollatedFunctionCalls = (1 << 2),
  DebugUIType_Memory                = (1 << 3),
  DebugUIType_Graphics              = (1 << 4),
  DebugUIType_Network               = (1 << 5),
  DebugUIType_DrawCalls             = (1 << 6)
};

struct debug_state
{
  b32 Initialized;
  u32 UIType = DebugUIType_None;

  u64 BytesBufferedToCard;
  b32 DebugDoScopeProfiling = True;

  u64 NumScopes;

  debug_clear_framebuffers_proc             ClearFramebuffers;
  debug_frame_end_proc                      FrameEnd;
  debug_frame_begin_proc                    FrameBegin;
  debug_register_arena_proc                 RegisterArena;
  debug_worker_thread_advance_data_system   WorkerThreadAdvanceDebugSystem;
  debug_main_thread_advance_data_system     MainThreadAdvanceDebugSystem;

  debug_mutex_waiting_proc                  MutexWait;
  debug_mutex_aquired_proc                  MutexAquired;
  debug_mutex_released_proc                 MutexReleased;

  debug_get_profile_scope_proc              GetProfileScope;
  debug_allocate_proc                       Debug_Allocate;
  debug_register_thread_proc                RegisterThread;

  debug_write_memory_record_proc            WriteMemoryRecord;
  debug_clear_memory_records_proc           ClearMemoryRecordsFor;


  debug_track_draw_call_proc                TrackDrawCall;
  debug_get_thread_local_state              GetThreadLocalState;
  /* debug_pick_chunk                          PickChunk; */
  /* debug_compute_pick_ray                    ComputePickRay; */
  debug_value                               DebugValue;
  debug_dump_scope_tree_data_to_console     DumpScopeTreeDataToConsole;

  debug_open_window_proc                    OpenAndInitializeDebugWindow;
  debug_redraw_window_proc                  ProcessInputAndRedrawWindow;

  b32 (*InitializeRenderSystem)(debug_state*, heap_allocator*);

  get_read_scope_tree_proc GetReadScopeTree;
  get_write_scope_tree_proc GetWriteScopeTree;

  // For the GameGeo
  camera *Camera;
  framebuffer GameGeoFBO;
  shader GameGeoShader;
  m4 ViewProjection;
  gpu_mapped_element_buffer GameGeo;
  shader DebugGameGeoTextureShader;


  // TODO(Jesse): Put this into some sort of debug_render struct such that
  // users of the library (externally) don't have to include all the rendering
  // code that the library relies on.
  //
  // NOTE(Jesse): This stuff has to be "hidden" at the end of the struct so the
  // external ABI is the same as the internal ABI until this point
#if DEBUG_LIB_INTERNAL_BUILD
  debug_ui_render_group UiGroup;

  untextured_3d_geometry_buffer LineMesh;

  selected_arenas *SelectedArenas;

  b32 TriggerRuntimeBreak;
  b32 DisplayDebugMenu;

  debug_profile_scope* HotFunction;

  debug_profile_scope FreeScopeSentinel;
  debug_thread_state *ThreadStates;

#define DEBUG_FRAMES_TRACKED (128)
  frame_stats Frames[DEBUG_FRAMES_TRACKED];

  u32 ReadScopeIndex;
  s32 FreeScopeCount;

#define REGISTERED_MEMORY_ARENA_COUNT (128)
  registered_memory_arena RegisteredMemoryArenas[REGISTERED_MEMORY_ARENA_COUNT];

#endif

};

#define GetDebugState() Global_DebugStatePointer
link_external debug_state *Global_DebugStatePointer;

struct debug_timed_function
{
  debug_profile_scope *Scope;
  debug_scope_tree *Tree;

  debug_timed_function(const char *Name)
  {
    this->Scope = 0;
    this->Tree = 0;

    debug_state *DebugState = GetDebugState();
    if (DebugState)
    {
      if (!DebugState->DebugDoScopeProfiling) return;

      ++DebugState->NumScopes;

      this->Scope = DebugState->GetProfileScope();
      this->Tree = DebugState->GetWriteScopeTree();

      if (this->Scope && this->Tree)
      {
        (*this->Tree->WriteScope) = this->Scope;
        this->Tree->WriteScope = &this->Scope->Child;

        this->Scope->Parent = this->Tree->ParentOfNextScope;
        this->Tree->ParentOfNextScope = this->Scope;

        this->Scope->Name = Name;
        this->Scope->StartingCycle = __rdtsc(); // Intentionally last
      }
    }

    return;
  }

  ~debug_timed_function()
  {
    debug_state *DebugState = GetDebugState();
    if (DebugState)
    {
      if (!DebugState->DebugDoScopeProfiling) return;
      if (!this->Scope) return;

      this->Scope->EndingCycle = __rdtsc(); // Intentionally first;

      Assert(this->Scope->EndingCycle > this->Scope->StartingCycle);

      // 'Pop' the scope stack
      this->Tree->WriteScope = &this->Scope->Sibling;
      this->Tree->ParentOfNextScope = this->Scope->Parent;
    }
  }

};

#define TIMED_FUNCTION() debug_timed_function FunctionTimer(__func__)
#define TIMED_NAMED_BLOCK(BlockName) debug_timed_function BlockTimer(BlockName)

#define TIMED_BLOCK(BlockName) { debug_timed_function BlockTimer(BlockName)
#define END_BLOCK(BlockName) } do {} while (0)

#define DEBUG_VALUE(Pointer) do {GetDebugState()->DebugValue(Pointer, #Pointer);} while (false)

#define DEBUG_FRAME_RECORD(...) DoDebugFrameRecord(__VA_ARGS__)
#define DEBUG_FRAME_END(a, b, c, d, e) do {GetDebugState()->FrameEnd(a, b, c, d, e);} while (false)
#define DEBUG_FRAME_BEGIN(bToggleMenu, bToggleProfile) do {GetDebugState()->FrameBegin(bToggleMenu, bToggleProfile);} while (false)

#if 1
void DebugTimedMutexWaiting(mutex *Mut);
void DebugTimedMutexAquired(mutex *Mut);
void DebugTimedMutexReleased(mutex *Mut);

#define TIMED_MUTEX_WAITING(Mut)  do {GetDebugState()->MutexWait(Mut);} while (false)
#define TIMED_MUTEX_AQUIRED(Mut)  do {GetDebugState()->MutexAquired(Mut);} while (false)
#define TIMED_MUTEX_RELEASED(Mut) do {GetDebugState()->MutexReleased(Mut);} while (false)
#endif

#define MAIN_THREAD_ADVANCE_DEBUG_SYSTEM(dt)               do {GetDebugState()->MainThreadAdvanceDebugSystem(dt);} while (false)
#define WORKER_THREAD_ADVANCE_DEBUG_SYSTEM()               do {GetDebugState()->WorkerThreadAdvanceDebugSystem();} while (false)

#define DEBUG_CLEAR_MEMORY_RECORDS_FOR(Arena)                do {GetDebugState()->ClearMemoryRecordsFor(Arena);} while (false)
#define DEBUG_TRACK_DRAW_CALL(CallingFunction, VertCount)  do {GetDebugState()->TrackDrawCall(CallingFunction, VertCount);} while (false)

#define DEBUG_REGISTER_VIEW_PROJECTION_MATRIX(ViewProjPtr) do {GetDebugState()->ViewProjection = ViewProjPtr;} while (false)
#define DEBUG_COMPUTE_PICK_RAY(ViewProjPtr)                do {GetDebugState()->ComputePickRay(ViewProjPtr);} while (false)
#define DEBUG_PICK_CHUNK(Chunk, ChunkAABB)                 do {GetDebugState()->PickChunk(Chunk, ChunkAABB);} while (false)

#if BONSAI_DEBUG_LIB_LOADER_API

#include <dlfcn.h>
#include <stdio.h>
#include <time.h>

#define BonsaiDebug_DefaultLibPath "lib_bonsai_debug/lib_bonsai_debug.so"
debug_state *Global_DebugStatePointer;

struct bonsai_debug_api
{
  query_memory_requirements_proc QueryMemoryRequirements;
  init_debug_system_proc         InitDebugState;
  patch_debug_lib_pointers_proc  BonsaiDebug_OnLoad;
};

global_variable r64 Global_LastDebugTime = 0;
r64 GetDt()
{
  r64 ThisTime = GetHighPrecisionClock();
  r64 Result = ThisTime - Global_LastDebugTime;
  Global_LastDebugTime = ThisTime;
  return Result;
}

bool
InitializeBootstrapDebugApi(void* DebugLib, bonsai_debug_api *Api)
{
  b32 Result = 1;

  Api->QueryMemoryRequirements = (query_memory_requirements_proc)dlsym(DebugLib, "QueryMemoryRequirements");
  Result &= (Api->QueryMemoryRequirements != 0);

  Api->InitDebugState = (init_debug_system_proc)dlsym(DebugLib, "InitDebugState");
  Result &= (Api->InitDebugState != 0);

  Api->BonsaiDebug_OnLoad = (patch_debug_lib_pointers_proc)dlsym(DebugLib, "BonsaiDebug_OnLoad");
  Result &= (Api->InitDebugState != 0);

  return Result;
}

void*
InitializeBonsaiDebug(const char* DebugLibName)
{
  void* DebugLib = dlopen(DebugLibName, RTLD_NOW);

  if (DebugLib)
  {
    printf("Library (%s) loaded!\n", DebugLibName);

    bonsai_debug_api DebugApi = {};
    if (InitializeBootstrapDebugApi(DebugLib, &DebugApi))
    {
      u64 BytesRequested = DebugApi.QueryMemoryRequirements();
      Global_DebugStatePointer = (debug_state*)calloc(BytesRequested, 1);

      DebugApi.BonsaiDebug_OnLoad(Global_DebugStatePointer);

      if (DebugApi.InitDebugState(Global_DebugStatePointer, BytesRequested))
      {
        printf("Success initializing lib_bonsai_debug\n");
      }
      else { printf("Error initializing lib_bonsai_debug\n"); DebugLib = 0; }
    }
    else { printf("Error initializing lib_bonsai_debug bootstrap API\n"); DebugLib = 0; }
  }
  else { printf("OpenLibrary Failed (%s)\n", dlerror()); DebugLib = 0; }

  return DebugLib;
}


#endif
