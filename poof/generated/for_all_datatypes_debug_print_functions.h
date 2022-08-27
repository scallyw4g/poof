    bonsai_function void DebugPrint( terminal_colors S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("terminal_colors {\n", Depth);
      }

                                                          DebugPrint("counted_string Red {\n", Depth+2);
                DebugPrint(S.Red, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string BrightRed {\n", Depth+2);
                DebugPrint(S.BrightRed, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string DarkRed {\n", Depth+2);
                DebugPrint(S.DarkRed, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Blue {\n", Depth+2);
                DebugPrint(S.Blue, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string BrightBlue {\n", Depth+2);
                DebugPrint(S.BrightBlue, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Purple {\n", Depth+2);
                DebugPrint(S.Purple, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string BrightPurple {\n", Depth+2);
                DebugPrint(S.BrightPurple, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Green {\n", Depth+2);
                DebugPrint(S.Green, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string BrightGreen {\n", Depth+2);
                DebugPrint(S.BrightGreen, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Yellow {\n", Depth+2);
                DebugPrint(S.Yellow, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string BrightYellow {\n", Depth+2);
                DebugPrint(S.BrightYellow, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string White {\n", Depth+2);
                DebugPrint(S.White, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Grey {\n", Depth+2);
                DebugPrint(S.Grey, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( terminal_colors *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( native_file S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("native_file {\n", Depth);
      }

                            // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(FILE Handle)\n", Depth+2);

                                                    DebugPrint("counted_string Path {\n", Depth+2);
                DebugPrint(S.Path, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( native_file *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( XVisualInfo S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("XVisualInfo {\n", Depth);
      }

                            // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(Visual visual)\n", Depth+2);

                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(VisualID visualid)\n", Depth+2);

                                                    DebugPrint("s32 screen =", Depth+2);
                DebugPrint(S.screen, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 depth =", Depth+2);
                DebugPrint(S.depth, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 c_class =", Depth+2);
                DebugPrint(S.c_class, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 red_mask =", Depth+2);
                DebugPrint(S.red_mask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 green_mask =", Depth+2);
                DebugPrint(S.green_mask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 blue_mask =", Depth+2);
                DebugPrint(S.blue_mask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 colormap_size =", Depth+2);
                DebugPrint(S.colormap_size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 bits_per_rgb =", Depth+2);
                DebugPrint(S.bits_per_rgb, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( XVisualInfo *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( os S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("os {\n", Depth);
      }

                            // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(window Window)\n", Depth+2);

                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(display Display)\n", Depth+2);

                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(gl_context GlContext)\n", Depth+2);

                                                    DebugPrint("b32 ContinueRunning =", Depth+2);
                DebugPrint(S.ContinueRunning, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( os *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v2i S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v2i {\n", Depth);
      }

                                                          DebugPrint("s32 x =", Depth+2);
                DebugPrint(S.x, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 y =", Depth+2);
                DebugPrint(S.y, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v2i *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v2 S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v2 {\n", Depth);
      }

                                                          DebugPrint("r32 x =", Depth+2);
                DebugPrint(S.x, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r32 y =", Depth+2);
                DebugPrint(S.y, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v2 *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v3i S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v3i {\n", Depth);
      }

                                          // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                                    DebugPrint("s32 E =", Depth+2);
                DebugPrint(S.E, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v3i *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v3 S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v3 {\n", Depth);
      }

                                          // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                                    DebugPrint("r32 E =", Depth+2);
                DebugPrint(S.E, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v3 *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v4 S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v4 {\n", Depth);
      }

                                          // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                                    DebugPrint("r32 E =", Depth+2);
                DebugPrint(S.E, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v4 *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( voxel_position S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("voxel_position {\n", Depth);
      }

                                          // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                                    DebugPrint("s32 E =", Depth+2);
                DebugPrint(S.E, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( voxel_position *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( f32_reg S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("f32_reg {\n", Depth);
      }

                                                          DebugPrint("r32 F =", Depth+2);
                DebugPrint(S.F, 1);
                DebugPrint(";\n");



                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(__m128 Sse)\n", Depth+2);



      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( f32_reg *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( random_series S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("random_series {\n", Depth);
      }

                                                          DebugPrint("u64 Seed =", Depth+2);
                DebugPrint(S.Seed, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( random_series *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( input_event S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("input_event {\n", Depth);
      }

                                                          DebugPrint("b32 Clicked =", Depth+2);
                DebugPrint(S.Clicked, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Pressed =", Depth+2);
                DebugPrint(S.Pressed, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( input_event *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( input S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("input {\n", Depth);
      }

                                                          DebugPrint("input_event Escape {\n", Depth+2);
                DebugPrint(S.Escape, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event Space {\n", Depth+2);
                DebugPrint(S.Space, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event Shift {\n", Depth+2);
                DebugPrint(S.Shift, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event Ctrl {\n", Depth+2);
                DebugPrint(S.Ctrl, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event Alt {\n", Depth+2);
                DebugPrint(S.Alt, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F12 {\n", Depth+2);
                DebugPrint(S.F12, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F11 {\n", Depth+2);
                DebugPrint(S.F11, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F10 {\n", Depth+2);
                DebugPrint(S.F10, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F9 {\n", Depth+2);
                DebugPrint(S.F9, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F8 {\n", Depth+2);
                DebugPrint(S.F8, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F7 {\n", Depth+2);
                DebugPrint(S.F7, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F6 {\n", Depth+2);
                DebugPrint(S.F6, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F5 {\n", Depth+2);
                DebugPrint(S.F5, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F4 {\n", Depth+2);
                DebugPrint(S.F4, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F3 {\n", Depth+2);
                DebugPrint(S.F3, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F2 {\n", Depth+2);
                DebugPrint(S.F2, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event F1 {\n", Depth+2);
                DebugPrint(S.F1, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event RMB {\n", Depth+2);
                DebugPrint(S.RMB, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event LMB {\n", Depth+2);
                DebugPrint(S.LMB, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event MMB {\n", Depth+2);
                DebugPrint(S.MMB, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event W {\n", Depth+2);
                DebugPrint(S.W, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event A {\n", Depth+2);
                DebugPrint(S.A, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event S {\n", Depth+2);
                DebugPrint(S.S, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event D {\n", Depth+2);
                DebugPrint(S.D, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event Q {\n", Depth+2);
                DebugPrint(S.Q, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("input_event E {\n", Depth+2);
                DebugPrint(S.E, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( input *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( hotkeys S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("hotkeys {\n", Depth);
      }

                                                          DebugPrint("b32 Left =", Depth+2);
                DebugPrint(S.Left, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Right =", Depth+2);
                DebugPrint(S.Right, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Forward =", Depth+2);
                DebugPrint(S.Forward, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Backward =", Depth+2);
                DebugPrint(S.Backward, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Player_Fire =", Depth+2);
                DebugPrint(S.Player_Fire, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Player_Proton =", Depth+2);
                DebugPrint(S.Player_Proton, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Player_Spawn =", Depth+2);
                DebugPrint(S.Player_Spawn, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( hotkeys *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( work_queue S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("work_queue {\n", Depth);
      }

                                                          DebugPrint("u32 EnqueueIndex =", Depth+2);
                DebugPrint(S.EnqueueIndex, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 DequeueIndex =", Depth+2);
                DebugPrint(S.DequeueIndex, 1);
                DebugPrint(";\n");



                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(work_queue_entry Entries)\n", Depth+2);

                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(semaphore GlobalQueueSemaphore)\n", Depth+2);



      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( work_queue *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( memory_arena S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("memory_arena {\n", Depth);
      }

                                                          DebugPrint("u8 Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm NextBlockSize =", Depth+2);
                DebugPrint(S.NextBlockSize, 1);
                DebugPrint(";\n");



                                                    DebugPrint("memory_arena Prev {\n", Depth+2);
                DebugPrint(S.Prev, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( memory_arena *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( push_metadata S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("push_metadata {\n", Depth);
      }

                                                          DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm ArenaHash =", Depth+2);
                DebugPrint(S.ArenaHash, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm HeadArenaHash =", Depth+2);
                DebugPrint(S.HeadArenaHash, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm StructSize =", Depth+2);
                DebugPrint(S.StructSize, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm StructCount =", Depth+2);
                DebugPrint(S.StructCount, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 PushCount =", Depth+2);
                DebugPrint(S.PushCount, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( push_metadata *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( platform S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("platform {\n", Depth);
      }

                                                          DebugPrint("work_queue LowPriority {\n", Depth+2);
                DebugPrint(S.LowPriority, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("work_queue HighPriority {\n", Depth+2);
                DebugPrint(S.HighPriority, Depth+4);
                DebugPrint("}\n", Depth+2);



                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(semaphore QueueSemaphore)\n", Depth+2);

                      // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(thread_startup_params Threads)\n", Depth+2);

                                                    DebugPrint("v2 MouseP {\n", Depth+2);
                DebugPrint(S.MouseP, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v2 MouseDP {\n", Depth+2);
                DebugPrint(S.MouseDP, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("memory_arena Memory {\n", Depth+2);
                DebugPrint(S.Memory, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("r32 dt =", Depth+2);
                DebugPrint(S.dt, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 WindowWidth =", Depth+2);
                DebugPrint(S.WindowWidth, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 WindowHeight =", Depth+2);
                DebugPrint(S.WindowHeight, 1);
                DebugPrint(";\n");



                                                    DebugPrint("input Input {\n", Depth+2);
                DebugPrint(S.Input, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( platform *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( heap_allocation_block S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("heap_allocation_block {\n", Depth);
      }

                                                          DebugPrint("heap_allocation_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm PrevAllocationSize =", Depth+2);
                DebugPrint(S.PrevAllocationSize, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( heap_allocation_block *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( heap_allocator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("heap_allocator {\n", Depth);
      }

                                                          DebugPrint("heap_allocation_block Heap {\n", Depth+2);
                DebugPrint(S.Heap, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( heap_allocator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tagged_counted_string_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tagged_counted_string_stream {\n", Depth);
      }

                                                          DebugPrint("counted_string Tag {\n", Depth+2);
                DebugPrint(S.Tag, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tagged_counted_string_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( char_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("char_cursor {\n", Depth);
      }

                                                          DebugPrint("char  Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("char  At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("char  End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("memory_arena Memory {\n", Depth+2);
                DebugPrint(S.Memory, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( char_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( linei S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("linei {\n", Depth);
      }

                                                          DebugPrint("v3i MinP {\n", Depth+2);
                DebugPrint(S.MinP, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3i MaxP {\n", Depth+2);
                DebugPrint(S.MaxP, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( linei *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( line S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("line {\n", Depth);
      }

                                                          DebugPrint("v3 MinP {\n", Depth+2);
                DebugPrint(S.MinP, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 MaxP {\n", Depth+2);
                DebugPrint(S.MaxP, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( line *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( Quaternion S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("Quaternion {\n", Depth);
      }

                                          // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("struct (anonymous)\n", Depth+2);


                                                    DebugPrint("float  E =", Depth+2);
                DebugPrint(S.E, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( Quaternion *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( string_builder S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("string_builder {\n", Depth);
      }

                                                          DebugPrint("memory_arena Memory {\n", Depth+2);
                DebugPrint(S.Memory, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_stream Chunks {\n", Depth+2);
                DebugPrint(S.Chunks, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( string_builder *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( u32_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("u32_stream {\n", Depth);
      }

                                                          DebugPrint("u32 Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( u32_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( u8_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("u8_stream {\n", Depth);
      }

                                                          DebugPrint("u8 Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( u8_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ansi_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ansi_stream {\n", Depth);
      }

                                                          DebugPrint("const char  Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("const char  At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("const char  End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("counted_string Filename {\n", Depth+2);
                DebugPrint(S.Filename, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("text_encoding Encoding =", Depth+2);
                DebugPrint(S.Encoding, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ansi_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( r32_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("r32_stream {\n", Depth);
      }

                                                          DebugPrint("r32 Start =", Depth+2);
                DebugPrint(S.Start, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( r32_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( m4 S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("m4 {\n", Depth);
      }

                                                          DebugPrint("v4 E {\n", Depth+2);
                DebugPrint(S.E, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( m4 *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( m_nxn S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("m_nxn {\n", Depth);
      }

                                                          DebugPrint("r32 Elements =", Depth+2);
                DebugPrint(S.Elements, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 Rows =", Depth+2);
                DebugPrint(S.Rows, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 Columns =", Depth+2);
                DebugPrint(S.Columns, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( m_nxn *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( mutex S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("mutex {\n", Depth);
      }

                            // NOTE(Jesse): we've got no definition for this type.. print a placeholder
            DebugPrint("undefined(native_mutex M)\n", Depth+2);



      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( mutex *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( mutex_op_record S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("mutex_op_record {\n", Depth);
      }

                                                          DebugPrint("mutex Mutex {\n", Depth+2);
                DebugPrint(S.Mutex, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u64 Cycle =", Depth+2);
                DebugPrint(S.Cycle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("mutex_op Op =", Depth+2);
                DebugPrint(S.Op, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( mutex_op_record *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( mutex_op_array S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("mutex_op_array {\n", Depth);
      }

                                                          DebugPrint("u32 NextRecord =", Depth+2);
                DebugPrint(S.NextRecord, 1);
                DebugPrint(";\n");



                                                    DebugPrint("mutex_op_record Records {\n", Depth+2);
                DebugPrint(S.Records, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( mutex_op_array *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( rect2 S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("rect2 {\n", Depth);
      }

                                                          DebugPrint("v2 Min {\n", Depth+2);
                DebugPrint(S.Min, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v2 Max {\n", Depth+2);
                DebugPrint(S.Max, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( rect2 *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( aabb S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("aabb {\n", Depth);
      }

                                                          DebugPrint("v3 Center {\n", Depth+2);
                DebugPrint(S.Center, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 Radius {\n", Depth+2);
                DebugPrint(S.Radius, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( aabb *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ray S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ray {\n", Depth);
      }

                                                          DebugPrint("v3 Origin {\n", Depth+2);
                DebugPrint(S.Origin, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 Dir {\n", Depth+2);
                DebugPrint(S.Dir, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ray *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_property S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_property {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_property Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_property *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_token S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_token {\n", Depth);
      }

                                                          DebugPrint("xml_token_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("xml_property Property {\n", Depth+2);
                DebugPrint(S.Property, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_token *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_tag S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_tag {\n", Depth);
      }

                                                          DebugPrint("xml_token Open {\n", Depth+2);
                DebugPrint(S.Open, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag Parent {\n", Depth+2);
                DebugPrint(S.Parent, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag Sibling {\n", Depth+2);
                DebugPrint(S.Sibling, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_property Properties {\n", Depth+2);
                DebugPrint(S.Properties, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_property NextPropertySlot {\n", Depth+2);
                DebugPrint(S.NextPropertySlot, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_tag *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_token_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_token_stream {\n", Depth);
      }

                                                          DebugPrint("xml_token Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_token At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_token End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag_hashtable Hashes {\n", Depth+2);
                DebugPrint(S.Hashes, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_token_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_tag_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_tag_stream {\n", Depth);
      }

                                                          DebugPrint("xml_tag Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_tag_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_parsing_at_indicators S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_parsing_at_indicators {\n", Depth);
      }

                                                          DebugPrint("xml_tag LastClosedTag {\n", Depth+2);
                DebugPrint(S.LastClosedTag, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag CurrentlyOpenTag {\n", Depth+2);
                DebugPrint(S.CurrentlyOpenTag, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_parsing_at_indicators *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( shader_uniform S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("shader_uniform {\n", Depth);
      }

                                                          DebugPrint("shader_uniform_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("union (anonymous)\n", Depth+2);


                                                    DebugPrint("s32 ID =", Depth+2);
                DebugPrint(S.ID, 1);
                DebugPrint(";\n");



                                                    DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("shader_uniform Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( shader_uniform *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( shader S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("shader {\n", Depth);
      }

                                                          DebugPrint("u32 ID =", Depth+2);
                DebugPrint(S.ID, 1);
                DebugPrint(";\n");



                                                    DebugPrint("shader_uniform FirstUniform {\n", Depth+2);
                DebugPrint(S.FirstUniform, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( shader *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( texture S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("texture {\n", Depth);
      }

                                                          DebugPrint("u32 ID =", Depth+2);
                DebugPrint(S.ID, 1);
                DebugPrint(";\n");



                                                    DebugPrint("v2i Dim {\n", Depth+2);
                DebugPrint(S.Dim, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( texture *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( textured_2d_geometry_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("textured_2d_geometry_buffer {\n", Depth);
      }

                                                          DebugPrint("v3 Verts {\n", Depth+2);
                DebugPrint(S.Verts, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 Colors {\n", Depth+2);
                DebugPrint(S.Colors, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 UVs {\n", Depth+2);
                DebugPrint(S.UVs, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( textured_2d_geometry_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( untextured_2d_geometry_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("untextured_2d_geometry_buffer {\n", Depth);
      }

                                                          DebugPrint("v3 Verts {\n", Depth+2);
                DebugPrint(S.Verts, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 Colors {\n", Depth+2);
                DebugPrint(S.Colors, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( untextured_2d_geometry_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( untextured_3d_geometry_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("untextured_3d_geometry_buffer {\n", Depth);
      }

                                                          DebugPrint("v3 Verts {\n", Depth+2);
                DebugPrint(S.Verts, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v4 Colors {\n", Depth+2);
                DebugPrint(S.Colors, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 Normals {\n", Depth+2);
                DebugPrint(S.Normals, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 End =", Depth+2);
                DebugPrint(S.End, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 At =", Depth+2);
                DebugPrint(S.At, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( untextured_3d_geometry_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( gpu_mapped_element_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("gpu_mapped_element_buffer {\n", Depth);
      }

                                                          DebugPrint("u32 VertexHandle =", Depth+2);
                DebugPrint(S.VertexHandle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 NormalHandle =", Depth+2);
                DebugPrint(S.NormalHandle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 ColorHandle =", Depth+2);
                DebugPrint(S.ColorHandle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("untextured_3d_geometry_buffer Buffer {\n", Depth+2);
                DebugPrint(S.Buffer, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( gpu_mapped_element_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( framebuffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("framebuffer {\n", Depth);
      }

                                                          DebugPrint("u32 ID =", Depth+2);
                DebugPrint(S.ID, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 Attachments =", Depth+2);
                DebugPrint(S.Attachments, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( framebuffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( cycle_range S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("cycle_range {\n", Depth);
      }

                                                          DebugPrint("u64 StartCycle =", Depth+2);
                DebugPrint(S.StartCycle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 TotalCycles =", Depth+2);
                DebugPrint(S.TotalCycles, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( cycle_range *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( memory_arena_stats S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("memory_arena_stats {\n", Depth);
      }

                                                          DebugPrint("u64 Allocations =", Depth+2);
                DebugPrint(S.Allocations, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 Pushes =", Depth+2);
                DebugPrint(S.Pushes, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 TotalAllocated =", Depth+2);
                DebugPrint(S.TotalAllocated, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 Remaining =", Depth+2);
                DebugPrint(S.Remaining, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( memory_arena_stats *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( min_max_avg_dt S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("min_max_avg_dt {\n", Depth);
      }

                                                          DebugPrint("r64 Min =", Depth+2);
                DebugPrint(S.Min, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r64 Max =", Depth+2);
                DebugPrint(S.Max, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r64 Avg =", Depth+2);
                DebugPrint(S.Avg, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( min_max_avg_dt *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( debug_profile_scope S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("debug_profile_scope {\n", Depth);
      }

                                                          DebugPrint("u64 CycleCount =", Depth+2);
                DebugPrint(S.CycleCount, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 StartingCycle =", Depth+2);
                DebugPrint(S.StartingCycle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Expanded =", Depth+2);
                DebugPrint(S.Expanded, 1);
                DebugPrint(";\n");



                                                    DebugPrint("debug_profile_scope Sibling {\n", Depth+2);
                DebugPrint(S.Sibling, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_profile_scope Child {\n", Depth+2);
                DebugPrint(S.Child, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_profile_scope Parent {\n", Depth+2);
                DebugPrint(S.Parent, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( debug_profile_scope *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( unique_debug_profile_scope S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("unique_debug_profile_scope {\n", Depth);
      }

                                                          DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 CallCount =", Depth+2);
                DebugPrint(S.CallCount, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 TotalCycles =", Depth+2);
                DebugPrint(S.TotalCycles, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 MinCycles =", Depth+2);
                DebugPrint(S.MinCycles, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 MaxCycles =", Depth+2);
                DebugPrint(S.MaxCycles, 1);
                DebugPrint(";\n");



                                                    DebugPrint("debug_profile_scope Scope {\n", Depth+2);
                DebugPrint(S.Scope, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("unique_debug_profile_scope NextUnique {\n", Depth+2);
                DebugPrint(S.NextUnique, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( unique_debug_profile_scope *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( debug_scope_tree S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("debug_scope_tree {\n", Depth);
      }

                                                          DebugPrint("debug_profile_scope Root {\n", Depth+2);
                DebugPrint(S.Root, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_profile_scope WriteScope {\n", Depth+2);
                DebugPrint(S.WriteScope, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_profile_scope ParentOfNextScope {\n", Depth+2);
                DebugPrint(S.ParentOfNextScope, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u64 FrameRecorded =", Depth+2);
                DebugPrint(S.FrameRecorded, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( debug_scope_tree *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( debug_thread_state S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("debug_thread_state {\n", Depth);
      }

                                                          DebugPrint("memory_arena Memory {\n", Depth+2);
                DebugPrint(S.Memory, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("memory_arena MemoryFor_debug_profile_scope {\n", Depth+2);
                DebugPrint(S.MemoryFor_debug_profile_scope, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("push_metadata MetaTable {\n", Depth+2);
                DebugPrint(S.MetaTable, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_scope_tree ScopeTrees {\n", Depth+2);
                DebugPrint(S.ScopeTrees, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("debug_profile_scope FirstFreeScope {\n", Depth+2);
                DebugPrint(S.FirstFreeScope, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("mutex_op_array MutexOps {\n", Depth+2);
                DebugPrint(S.MutexOps, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 WriteIndex =", Depth+2);
                DebugPrint(S.WriteIndex, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 Pad =", Depth+2);
                DebugPrint(S.Pad, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( debug_thread_state *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( registered_memory_arena S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("registered_memory_arena {\n", Depth);
      }

                                                          DebugPrint("memory_arena Arena {\n", Depth+2);
                DebugPrint(S.Arena, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Expanded =", Depth+2);
                DebugPrint(S.Expanded, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( registered_memory_arena *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( selected_memory_arena S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("selected_memory_arena {\n", Depth);
      }

                                                          DebugPrint("umm ArenaHash =", Depth+2);
                DebugPrint(S.ArenaHash, 1);
                DebugPrint(";\n");



                                                    DebugPrint("umm HeadArenaHash =", Depth+2);
                DebugPrint(S.HeadArenaHash, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( selected_memory_arena *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( selected_arenas S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("selected_arenas {\n", Depth);
      }

                                                          DebugPrint("u32 Count =", Depth+2);
                DebugPrint(S.Count, 1);
                DebugPrint(";\n");



                                                    DebugPrint("selected_memory_arena Arenas {\n", Depth+2);
                DebugPrint(S.Arenas, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( selected_arenas *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( frame_stats S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("frame_stats {\n", Depth);
      }

                                                          DebugPrint("u64 TotalCycles =", Depth+2);
                DebugPrint(S.TotalCycles, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u64 StartingCycle =", Depth+2);
                DebugPrint(S.StartingCycle, 1);
                DebugPrint(";\n");



                                                    DebugPrint("r64 FrameMs =", Depth+2);
                DebugPrint(S.FrameMs, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( frame_stats *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( called_function S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("called_function {\n", Depth);
      }

                                                          DebugPrint("const char  Name =", Depth+2);
                DebugPrint(S.Name, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 CallCount =", Depth+2);
                DebugPrint(S.CallCount, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( called_function *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( debug_draw_call S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("debug_draw_call {\n", Depth);
      }

                                                          DebugPrint("const char  Caller =", Depth+2);
                DebugPrint(S.Caller, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 N =", Depth+2);
                DebugPrint(S.N, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 Calls =", Depth+2);
                DebugPrint(S.Calls, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( debug_draw_call *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( bitmap_image_header S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("bitmap_image_header {\n", Depth);
      }

                                                          DebugPrint("u32 SizeOfImageHeader =", Depth+2);
                DebugPrint(S.SizeOfImageHeader, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 WidthInPixels =", Depth+2);
                DebugPrint(S.WidthInPixels, 1);
                DebugPrint(";\n");



                                                    DebugPrint("s32 HeightInPixels =", Depth+2);
                DebugPrint(S.HeightInPixels, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u16 ColorPlanes =", Depth+2);
                DebugPrint(S.ColorPlanes, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u16 BPP =", Depth+2);
                DebugPrint(S.BPP, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 CompressionType =", Depth+2);
                DebugPrint(S.CompressionType, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 SizeInBytes =", Depth+2);
                DebugPrint(S.SizeInBytes, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 xPixelsPerMeter =", Depth+2);
                DebugPrint(S.xPixelsPerMeter, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 yPixelsPerMeter =", Depth+2);
                DebugPrint(S.yPixelsPerMeter, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 ColorMapsUsed =", Depth+2);
                DebugPrint(S.ColorMapsUsed, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 NumSignificantColors =", Depth+2);
                DebugPrint(S.NumSignificantColors, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 RedMask =", Depth+2);
                DebugPrint(S.RedMask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 GreenMask =", Depth+2);
                DebugPrint(S.GreenMask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 BlueMask =", Depth+2);
                DebugPrint(S.BlueMask, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 AlphaMask =", Depth+2);
                DebugPrint(S.AlphaMask, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( bitmap_image_header *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( bitmap_header S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("bitmap_header {\n", Depth);
      }

                                                          DebugPrint("u16 Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 FileSizeInBytes =", Depth+2);
                DebugPrint(S.FileSizeInBytes, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 Ignored =", Depth+2);
                DebugPrint(S.Ignored, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 OffsetToPixelData =", Depth+2);
                DebugPrint(S.OffsetToPixelData, 1);
                DebugPrint(";\n");



                                                    DebugPrint("bitmap_image_header Image {\n", Depth+2);
                DebugPrint(S.Image, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( bitmap_header *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( bitmap S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("bitmap {\n", Depth);
      }

                                                          DebugPrint("v2i Dim {\n", Depth+2);
                DebugPrint(S.Dim, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32_stream Pixels {\n", Depth+2);
                DebugPrint(S.Pixels, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( bitmap *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_expansion S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_expansion {\n", Depth);
      }

                                                          DebugPrint("c_token_cursor Expansion {\n", Depth+2);
                DebugPrint(S.Expansion, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def Def {\n", Depth+2);
                DebugPrint(S.Def, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_expansion *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token {\n", Depth);
      }

                                                          DebugPrint("c_token_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("counted_string Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Filename {\n", Depth+2);
                DebugPrint(S.Filename, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 LineNumber =", Depth+2);
                DebugPrint(S.LineNumber, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Erased =", Depth+2);
                DebugPrint(S.Erased, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u8 Flags =", Depth+2);
                DebugPrint(S.Flags, 1);
                DebugPrint(";\n");



                                    // NOTE(Jesse): there was an anonymous struct or union here
              DebugPrint("union (anonymous)\n", Depth+2);




      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( peek_result S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("peek_result {\n", Depth);
      }

                                                          DebugPrint("c_token_cursor Tokens {\n", Depth+2);
                DebugPrint(S.Tokens, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 DoNotDescend =", Depth+2);
                DebugPrint(S.DoNotDescend, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( peek_result *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_cursor_up S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_cursor_up {\n", Depth);
      }

                                                          DebugPrint("c_token_cursor Tokens {\n", Depth+2);
                DebugPrint(S.Tokens, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_cursor_up *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_cursor {\n", Depth);
      }

                                                          DebugPrint("token_cursor_source Source =", Depth+2);
                DebugPrint(S.Source, 1);
                DebugPrint(";\n");



                                                    DebugPrint("c_token Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token_cursor_up Up {\n", Depth+2);
                DebugPrint(S.Up, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Filename {\n", Depth+2);
                DebugPrint(S.Filename, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parser S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parser {\n", Depth);
      }

                                                          DebugPrint("parse_error_code ErrorCode =", Depth+2);
                DebugPrint(S.ErrorCode, 1);
                DebugPrint(";\n");



                                                    DebugPrint("c_token_cursor Tokens {\n", Depth+2);
                DebugPrint(S.Tokens, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parser *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( declaration_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("declaration_stream {\n", Depth);
      }

                                                          DebugPrint("declaration_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( declaration_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( compound_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("compound_decl {\n", Depth);
      }

                                                          DebugPrint("c_token Type {\n", Depth+2);
                DebugPrint(S.Type, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration_stream Members {\n", Depth+2);
                DebugPrint(S.Members, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 IsUnion =", Depth+2);
                DebugPrint(S.IsUnion, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( compound_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_indirection_info S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_indirection_info {\n", Depth);
      }

                                                          DebugPrint("b32 ConstPointer =", Depth+2);
                DebugPrint(S.ConstPointer, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 VolatileValue =", Depth+2);
                DebugPrint(S.VolatileValue, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 ReferenceLevel =", Depth+2);
                DebugPrint(S.ReferenceLevel, 1);
                DebugPrint(";\n");



                                                    DebugPrint("u32 IndirectionLevel =", Depth+2);
                DebugPrint(S.IndirectionLevel, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 IsFunction =", Depth+2);
                DebugPrint(S.IsFunction, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 IsFunctionPtr =", Depth+2);
                DebugPrint(S.IsFunctionPtr, 1);
                DebugPrint(";\n");



                                                    DebugPrint("c_token FunctionNameT {\n", Depth+2);
                DebugPrint(S.FunctionNameT, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_indirection_info *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_spec S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_spec {\n", Depth);
      }

                                                          DebugPrint("c_token QualifierNameT {\n", Depth+2);
                DebugPrint(S.QualifierNameT, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token DatatypeToken {\n", Depth+2);
                DebugPrint(S.DatatypeToken, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_qualifier Qualifier =", Depth+2);
                DebugPrint(S.Qualifier, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 HasTemplateArguments =", Depth+2);
                DebugPrint(S.HasTemplateArguments, 1);
                DebugPrint(";\n");



                                                    DebugPrint("counted_string TemplateSource {\n", Depth+2);
                DebugPrint(S.TemplateSource, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("linkage_type Linkage =", Depth+2);
                DebugPrint(S.Linkage, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_spec *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( variable_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("variable_decl {\n", Depth);
      }

                                                          DebugPrint("type_spec Type {\n", Depth+2);
                DebugPrint(S.Type, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node StaticBufferSize {\n", Depth+2);
                DebugPrint(S.StaticBufferSize, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("u32 StrictBitWidth =", Depth+2);
                DebugPrint(S.StrictBitWidth, 1);
                DebugPrint(";\n");



                                                    DebugPrint("type_indirection_info Indirection {\n", Depth+2);
                DebugPrint(S.Indirection, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( variable_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( function_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("function_decl {\n", Depth);
      }

                                                          DebugPrint("function_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("type_spec ReturnType {\n", Depth+2);
                DebugPrint(S.ReturnType, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token NameT {\n", Depth+2);
                DebugPrint(S.NameT, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("variable_decl_stream Args {\n", Depth+2);
                DebugPrint(S.Args, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 IsVariadic =", Depth+2);
                DebugPrint(S.IsVariadic, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 ImplIsDefault =", Depth+2);
                DebugPrint(S.ImplIsDefault, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 ImplIsDeleted =", Depth+2);
                DebugPrint(S.ImplIsDeleted, 1);
                DebugPrint(";\n");



                                                    DebugPrint("parser Body {\n", Depth+2);
                DebugPrint(S.Body, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node Ast {\n", Depth+2);
                DebugPrint(S.Ast, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( function_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_member S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_member {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression Expr {\n", Depth+2);
                DebugPrint(S.Expr, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_member *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_decl {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_member_stream Members {\n", Depth+2);
                DebugPrint(S.Members, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( primitive_def S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("primitive_def {\n", Depth);
      }

                                                          DebugPrint("type_spec TypeSpec {\n", Depth+2);
                DebugPrint(S.TypeSpec, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( primitive_def *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_union_member S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_union_member {\n", Depth);
      }

                                                          DebugPrint("counted_string Type {\n", Depth+2);
                DebugPrint(S.Type, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_union_flags Flags =", Depth+2);
                DebugPrint(S.Flags, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_union_member *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_def S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_def {\n", Depth);
      }

                                                          DebugPrint("type_spec Type {\n", Depth+2);
                DebugPrint(S.Type, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Alias {\n", Depth+2);
                DebugPrint(S.Alias, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 IsFunction =", Depth+2);
                DebugPrint(S.IsFunction, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_def *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_arg S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_arg {\n", Depth);
      }

                                                          DebugPrint("counted_string Match {\n", Depth+2);
                DebugPrint(S.Match, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("datatype Data {\n", Depth+2);
                DebugPrint(S.Data, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_arg *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_union_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_union_decl {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_union_member_stream Members {\n", Depth+2);
                DebugPrint(S.Members, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration_stream CommonMembers {\n", Depth+2);
                DebugPrint(S.CommonMembers, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string CustomEnumType {\n", Depth+2);
                DebugPrint(S.CustomEnumType, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_union_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def {\n", Depth);
      }

                                                          DebugPrint("macro_type Type =", Depth+2);
                DebugPrint(S.Type, 1);
                DebugPrint(";\n");



                                                    DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token_cursor Body {\n", Depth+2);
                DebugPrint(S.Body, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_buffer NamedArguments {\n", Depth+2);
                DebugPrint(S.NamedArguments, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 Variadic =", Depth+2);
                DebugPrint(S.Variadic, 1);
                DebugPrint(";\n");



                                                    DebugPrint("b32 Undefed =", Depth+2);
                DebugPrint(S.Undefed, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string ArgName {\n", Depth+2);
                DebugPrint(S.ArgName, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser Body {\n", Depth+2);
                DebugPrint(S.Body, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( todo S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("todo {\n", Depth);
      }

                                                          DebugPrint("counted_string Id {\n", Depth+2);
                DebugPrint(S.Id, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 FoundInCodebase =", Depth+2);
                DebugPrint(S.FoundInCodebase, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( todo *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tag S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tag {\n", Depth);
      }

                                                          DebugPrint("counted_string Tag {\n", Depth+2);
                DebugPrint(S.Tag, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("todo_stream Todos {\n", Depth+2);
                DebugPrint(S.Todos, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tag *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( person S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("person {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tag_stream Tags {\n", Depth+2);
                DebugPrint(S.Tags, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( person *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_expression S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_expression {\n", Depth);
      }

                                                          DebugPrint("ast_node Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_expression *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_statement S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_statement {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression LHS {\n", Depth+2);
                DebugPrint(S.LHS, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_statement RHS {\n", Depth+2);
                DebugPrint(S.RHS, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_statement Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_statement *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_function_call S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_function_call {\n", Depth);
      }

                                                          DebugPrint("counted_string Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("function_decl Prototype {\n", Depth+2);
                DebugPrint(S.Prototype, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression_stream Args {\n", Depth+2);
                DebugPrint(S.Args, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_function_call *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_type_specifier S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_type_specifier {\n", Depth);
      }

                                                          DebugPrint("datatype Datatype {\n", Depth+2);
                DebugPrint(S.Datatype, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_spec TypeSpec {\n", Depth+2);
                DebugPrint(S.TypeSpec, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_indirection_info Indirection {\n", Depth+2);
                DebugPrint(S.Indirection, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression Name {\n", Depth+2);
                DebugPrint(S.Name, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_type_specifier *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_variable_def S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_variable_def {\n", Depth);
      }

                                                          DebugPrint("datatype Type {\n", Depth+2);
                DebugPrint(S.Type, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("variable_decl Decl {\n", Depth+2);
                DebugPrint(S.Decl, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_variable_def *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_access S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_access {\n", Depth);
      }

                                                          DebugPrint("c_token AccessType {\n", Depth+2);
                DebugPrint(S.AccessType, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression Symbol {\n", Depth+2);
                DebugPrint(S.Symbol, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_access *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_parenthesized S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_parenthesized {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression Expr {\n", Depth+2);
                DebugPrint(S.Expr, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 IsCast =", Depth+2);
                DebugPrint(S.IsCast, 1);
                DebugPrint(";\n");



                                                    DebugPrint("ast_node_expression CastValue {\n", Depth+2);
                DebugPrint(S.CastValue, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_parenthesized *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_initializer_list S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_initializer_list {\n", Depth);
      }

                                                          DebugPrint("u32 Ignored =", Depth+2);
                DebugPrint(S.Ignored, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_initializer_list *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_operator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_operator {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression Operand {\n", Depth+2);
                DebugPrint(S.Operand, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token Token {\n", Depth+2);
                DebugPrint(S.Token, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_operator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_literal S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_literal {\n", Depth);
      }

                                                          DebugPrint("c_token Token {\n", Depth+2);
                DebugPrint(S.Token, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_literal *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_symbol S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_symbol {\n", Depth);
      }

                                                          DebugPrint("c_token Token {\n", Depth+2);
                DebugPrint(S.Token, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_symbol *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_return S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_return {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_return *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_predicated S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_predicated {\n", Depth);
      }

                                                          DebugPrint("c_token Token {\n", Depth+2);
                DebugPrint(S.Token, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression Predicate {\n", Depth+2);
                DebugPrint(S.Predicate, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_predicated *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( arguments S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("arguments {\n", Depth);
      }

                                                          DebugPrint("counted_string Outpath {\n", Depth+2);
                DebugPrint(S.Outpath, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_cursor Files {\n", Depth+2);
                DebugPrint(S.Files, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_cursor IncludePaths {\n", Depth+2);
                DebugPrint(S.IncludePaths, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("b32 DoDebugWindow =", Depth+2);
                DebugPrint(S.DoDebugWindow, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( arguments *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( program_datatypes S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("program_datatypes {\n", Depth);
      }

                                                          DebugPrint("datatype_hashtable DatatypeHashtable {\n", Depth+2);
                DebugPrint(S.DatatypeHashtable, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def_hashtable Macros {\n", Depth+2);
                DebugPrint(S.Macros, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_hashtable FilesParsed {\n", Depth+2);
                DebugPrint(S.FilesParsed, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl_stream Structs {\n", Depth+2);
                DebugPrint(S.Structs, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl_stream Enums {\n", Depth+2);
                DebugPrint(S.Enums, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("function_decl_stream Functions {\n", Depth+2);
                DebugPrint(S.Functions, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_def_stream Typedefs {\n", Depth+2);
                DebugPrint(S.Typedefs, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( program_datatypes *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( for_enum_constraints S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("for_enum_constraints {\n", Depth);
      }

                                                          DebugPrint("counted_string TypeName {\n", Depth+2);
                DebugPrint(S.TypeName, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string ValueName {\n", Depth+2);
                DebugPrint(S.ValueName, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( for_enum_constraints *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( body_text_constraints S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("body_text_constraints {\n", Depth);
      }

                                                          DebugPrint("counted_string MemberContains {\n", Depth+2);
                DebugPrint(S.MemberContains, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string TypeTag {\n", Depth+2);
                DebugPrint(S.TypeTag, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string TypeName {\n", Depth+2);
                DebugPrint(S.TypeName, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string ValueName {\n", Depth+2);
                DebugPrint(S.ValueName, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( body_text_constraints *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( todo_list_info S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("todo_list_info {\n", Depth);
      }

                                                          DebugPrint("person_stream People {\n", Depth+2);
                DebugPrint(S.People, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tagged_counted_string_stream_stream NameLists {\n", Depth+2);
                DebugPrint(S.NameLists, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( todo_list_info *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( string_from_parser S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("string_from_parser {\n", Depth);
      }

                                                          DebugPrint("parser Parser {\n", Depth+2);
                DebugPrint(S.Parser, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token StartToken {\n", Depth+2);
                DebugPrint(S.StartToken, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( string_from_parser *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parse_context S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parse_context {\n", Depth);
      }

                                                          DebugPrint("parser CurrentParser {\n", Depth+2);
                DebugPrint(S.CurrentParser, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("program_datatypes Datatypes {\n", Depth+2);
                DebugPrint(S.Datatypes, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_cursor IncludePaths {\n", Depth+2);
                DebugPrint(S.IncludePaths, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_stream MetaFunctions {\n", Depth+2);
                DebugPrint(S.MetaFunctions, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("memory_arena Memory {\n", Depth+2);
                DebugPrint(S.Memory, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("arguments Args {\n", Depth+2);
                DebugPrint(S.Args, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parse_context *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( comma_separated_decl S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("comma_separated_decl {\n", Depth);
      }

                                                          DebugPrint("c_token NameT {\n", Depth+2);
                DebugPrint(S.NameT, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_indirection_info Indirection {\n", Depth+2);
                DebugPrint(S.Indirection, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node StaticBufferSize {\n", Depth+2);
                DebugPrint(S.StaticBufferSize, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node Value {\n", Depth+2);
                DebugPrint(S.Value, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( comma_separated_decl *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_list S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_list {\n", Depth);
      }

                                                          DebugPrint("d_list Prev {\n", Depth+2);
                DebugPrint(S.Prev, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_list Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_list *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_cursor {\n", Depth);
      }

                                                          DebugPrint("counted_string Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("counted_string Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_stream {\n", Depth);
      }

                                                          DebugPrint("counted_string_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_iterator {\n", Depth);
      }

                                                          DebugPrint("counted_string_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tagged_counted_string_stream_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tagged_counted_string_stream_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("tagged_counted_string_stream Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tagged_counted_string_stream_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tagged_counted_string_stream_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tagged_counted_string_stream_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tagged_counted_string_stream_stream {\n", Depth);
      }

                                                          DebugPrint("tagged_counted_string_stream_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tagged_counted_string_stream_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tagged_counted_string_stream_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tagged_counted_string_stream_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tagged_counted_string_stream_iterator {\n", Depth);
      }

                                                          DebugPrint("tagged_counted_string_stream_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tagged_counted_string_stream_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tagged_counted_string_stream_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_tag_linked_list_node S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_tag_linked_list_node {\n", Depth);
      }

                                                          DebugPrint("xml_tag Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("xml_tag_linked_list_node Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_tag_linked_list_node *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( xml_tag_hashtable S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("xml_tag_hashtable {\n", Depth);
      }

                                                          DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("xml_tag_linked_list_node Elements {\n", Depth+2);
                DebugPrint(S.Elements, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( xml_tag_hashtable *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_buffer {\n", Depth);
      }

                                                          DebugPrint("counted_string Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("umm Count =", Depth+2);
                DebugPrint(S.Count, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_linked_list_node S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_linked_list_node {\n", Depth);
      }

                                                          DebugPrint("counted_string Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("counted_string_linked_list_node Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_linked_list_node *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( counted_string_hashtable S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("counted_string_hashtable {\n", Depth);
      }

                                                          DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("counted_string_linked_list_node Elements {\n", Depth+2);
                DebugPrint(S.Elements, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( counted_string_hashtable *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( v3_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("v3_cursor {\n", Depth);
      }

                                                          DebugPrint("v3 Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("v3 End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( v3_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_buffer {\n", Depth);
      }

                                                          DebugPrint("c_token Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("umm Count =", Depth+2);
                DebugPrint(S.Count, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_buffer_buffer S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_buffer_buffer {\n", Depth);
      }

                                                          DebugPrint("c_token_buffer Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("umm Count =", Depth+2);
                DebugPrint(S.Count, 1);
                DebugPrint(";\n");





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_buffer_buffer *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_buffer_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_buffer_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("c_token_buffer Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token_buffer_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_buffer_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_buffer_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_buffer_stream {\n", Depth);
      }

                                                          DebugPrint("c_token_buffer_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token_buffer_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_buffer_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( c_token_buffer_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("c_token_buffer_iterator {\n", Depth);
      }

                                                          DebugPrint("c_token_buffer_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("c_token_buffer_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( c_token_buffer_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parser_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parser_cursor {\n", Depth);
      }

                                                          DebugPrint("parser Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parser_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parser_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parser_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("parser Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parser_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parser_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parser_stream {\n", Depth);
      }

                                                          DebugPrint("parser_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parser_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( parser_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("parser_iterator {\n", Depth);
      }

                                                          DebugPrint("parser_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("parser_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( parser_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( compound_decl_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("compound_decl_cursor {\n", Depth);
      }

                                                          DebugPrint("compound_decl Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( compound_decl_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( compound_decl_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("compound_decl_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("compound_decl Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( compound_decl_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( compound_decl_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("compound_decl_stream {\n", Depth);
      }

                                                          DebugPrint("compound_decl_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( compound_decl_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( compound_decl_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("compound_decl_iterator {\n", Depth);
      }

                                                          DebugPrint("compound_decl_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("compound_decl_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( compound_decl_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( variable_decl_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("variable_decl_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("variable_decl Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("variable_decl_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( variable_decl_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( variable_decl_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("variable_decl_stream {\n", Depth);
      }

                                                          DebugPrint("variable_decl_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("variable_decl_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( variable_decl_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( variable_decl_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("variable_decl_iterator {\n", Depth);
      }

                                                          DebugPrint("variable_decl_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("variable_decl_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( variable_decl_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( function_decl_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("function_decl_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("function_decl Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("function_decl_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( function_decl_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( function_decl_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("function_decl_stream {\n", Depth);
      }

                                                          DebugPrint("function_decl_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("function_decl_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( function_decl_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( function_decl_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("function_decl_iterator {\n", Depth);
      }

                                                          DebugPrint("function_decl_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("function_decl_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( function_decl_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_member_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_member_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("enum_member Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_member_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_member_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_member_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_member_stream {\n", Depth);
      }

                                                          DebugPrint("enum_member_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_member_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_member_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_member_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_member_iterator {\n", Depth);
      }

                                                          DebugPrint("enum_member_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_member_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_member_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_decl_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_decl_cursor {\n", Depth);
      }

                                                          DebugPrint("enum_decl Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_decl_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_decl_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_decl_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("enum_decl Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_decl_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_decl_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_decl_stream {\n", Depth);
      }

                                                          DebugPrint("enum_decl_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_decl_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( enum_decl_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("enum_decl_iterator {\n", Depth);
      }

                                                          DebugPrint("enum_decl_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("enum_decl_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( enum_decl_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( declaration_cursor S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("declaration_cursor {\n", Depth);
      }

                                                          DebugPrint("declaration Start {\n", Depth+2);
                DebugPrint(S.Start, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration End {\n", Depth+2);
                DebugPrint(S.End, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( declaration_cursor *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( declaration_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("declaration_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("declaration Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( declaration_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( datatype_linked_list_node S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("datatype_linked_list_node {\n", Depth);
      }

                                                          DebugPrint("datatype Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("datatype_linked_list_node Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( datatype_linked_list_node *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( datatype_hashtable S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("datatype_hashtable {\n", Depth);
      }

                                                          DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("datatype_linked_list_node Elements {\n", Depth+2);
                DebugPrint(S.Elements, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( datatype_hashtable *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_union_member_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_union_member_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("d_union_member Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_union_member_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_union_member_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_union_member_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_union_member_stream {\n", Depth);
      }

                                                          DebugPrint("d_union_member_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_union_member_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_union_member_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( d_union_member_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("d_union_member_iterator {\n", Depth);
      }

                                                          DebugPrint("d_union_member_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("d_union_member_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( d_union_member_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_def_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_def_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("type_def Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_def_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_def_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_def_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_def_stream {\n", Depth);
      }

                                                          DebugPrint("type_def_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_def_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_def_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( type_def_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("type_def_iterator {\n", Depth);
      }

                                                          DebugPrint("type_def_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("type_def_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( type_def_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_arg_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_arg_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("meta_func_arg Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_arg_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_arg_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_arg_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_arg_stream {\n", Depth);
      }

                                                          DebugPrint("meta_func_arg_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_arg_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_arg_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_arg_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_arg_iterator {\n", Depth);
      }

                                                          DebugPrint("meta_func_arg_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_arg_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_arg_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("macro_def Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def_stream {\n", Depth);
      }

                                                          DebugPrint("macro_def_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def_iterator {\n", Depth);
      }

                                                          DebugPrint("macro_def_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def_linked_list_node S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def_linked_list_node {\n", Depth);
      }

                                                          DebugPrint("macro_def Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("macro_def_linked_list_node Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def_linked_list_node *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( macro_def_hashtable S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("macro_def_hashtable {\n", Depth);
      }

                                                          DebugPrint("umm Size =", Depth+2);
                DebugPrint(S.Size, 1);
                DebugPrint(";\n");



                                                    DebugPrint("macro_def_linked_list_node Elements {\n", Depth+2);
                DebugPrint(S.Elements, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( macro_def_hashtable *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("meta_func Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_stream {\n", Depth);
      }

                                                          DebugPrint("meta_func_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( meta_func_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("meta_func_iterator {\n", Depth);
      }

                                                          DebugPrint("meta_func_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("meta_func_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( meta_func_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( todo_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("todo_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("todo Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("todo_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( todo_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( todo_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("todo_stream {\n", Depth);
      }

                                                          DebugPrint("todo_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("todo_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( todo_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( todo_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("todo_iterator {\n", Depth);
      }

                                                          DebugPrint("todo_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("todo_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( todo_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tag_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tag_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("tag Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tag_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tag_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tag_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tag_stream {\n", Depth);
      }

                                                          DebugPrint("tag_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tag_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tag_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( tag_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("tag_iterator {\n", Depth);
      }

                                                          DebugPrint("tag_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("tag_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( tag_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( person_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("person_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("person Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("person_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( person_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( person_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("person_stream {\n", Depth);
      }

                                                          DebugPrint("person_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("person_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( person_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( person_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("person_iterator {\n", Depth);
      }

                                                          DebugPrint("person_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("person_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( person_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_expression_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_expression_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_expression_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_expression_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_expression_stream {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_expression_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_expression_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_expression_iterator {\n", Depth);
      }

                                                          DebugPrint("ast_node_expression_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_expression_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_expression_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_variable_def_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_variable_def_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("ast_node_variable_def Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_variable_def_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_variable_def_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_variable_def_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_variable_def_stream {\n", Depth);
      }

                                                          DebugPrint("ast_node_variable_def_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_variable_def_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_variable_def_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_variable_def_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_variable_def_iterator {\n", Depth);
      }

                                                          DebugPrint("ast_node_variable_def_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_variable_def_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_variable_def_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_stream_chunk S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_stream_chunk {\n", Depth);
      }

                                                          DebugPrint("ast_node Element {\n", Depth+2);
                DebugPrint(S.Element, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_stream_chunk Next {\n", Depth+2);
                DebugPrint(S.Next, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_stream_chunk *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_stream S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_stream {\n", Depth);
      }

                                                          DebugPrint("ast_node_stream_chunk FirstChunk {\n", Depth+2);
                DebugPrint(S.FirstChunk, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_stream_chunk LastChunk {\n", Depth+2);
                DebugPrint(S.LastChunk, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_stream *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( ast_node_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("ast_node_iterator {\n", Depth);
      }

                                                          DebugPrint("ast_node_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("ast_node_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( ast_node_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( declaration_iterator S, u32 Depth)
    {
      if (Depth == 0)
      {
        DebugPrint("declaration_iterator {\n", Depth);
      }

                                                          DebugPrint("declaration_stream Stream {\n", Depth+2);
                DebugPrint(S.Stream, Depth+4);
                DebugPrint("}\n", Depth+2);



                                                    DebugPrint("declaration_stream_chunk At {\n", Depth+2);
                DebugPrint(S.At, Depth+4);
                DebugPrint("}\n", Depth+2);





      if (Depth == 0)
      {
        DebugPrint("}\n", Depth);
      }
    }

    bonsai_function void DebugPrint( declaration_iterator *S, u32 Depth)
    {
      if (S) { DebugPrint(*S, Depth); }
      else { DebugPrint("ptr(0)\n", Depth); }
    }
    bonsai_function void DebugPrint( log_level EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case LogLevel_Undefined:
            {
              DebugPrint("LogLevel_Undefined ", Depth);
            } break;
            case LogLevel_Debug:
            {
              DebugPrint("LogLevel_Debug ", Depth);
            } break;
            case LogLevel_Normal:
            {
              DebugPrint("LogLevel_Normal ", Depth);
            } break;
            case LogLevel_Error:
            {
              DebugPrint("LogLevel_Error ", Depth);
            } break;
            case LogLevel_Shush:
            {
              DebugPrint("LogLevel_Shush ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( sign EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case Negative:
            {
              DebugPrint("Negative ", Depth);
            } break;
            case Zero:
            {
              DebugPrint("Zero ", Depth);
            } break;
            case Positive:
            {
              DebugPrint("Positive ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( memory_protection_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case MemoryProtection_Protected:
            {
              DebugPrint("MemoryProtection_Protected ", Depth);
            } break;
            case MemoryProtection_RW:
            {
              DebugPrint("MemoryProtection_RW ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( heap_allocation_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case AllocationType_Free:
            {
              DebugPrint("AllocationType_Free ", Depth);
            } break;
            case AllocationType_Reserved:
            {
              DebugPrint("AllocationType_Reserved ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( text_encoding EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case TextEncoding_ASCII:
            {
              DebugPrint("TextEncoding_ASCII ", Depth);
            } break;
            case TextEncoding_UTF8:
            {
              DebugPrint("TextEncoding_UTF8 ", Depth);
            } break;
            case TextEncoding_UTF16LE:
            {
              DebugPrint("TextEncoding_UTF16LE ", Depth);
            } break;
            case TextEncoding_UTF16BE:
            {
              DebugPrint("TextEncoding_UTF16BE ", Depth);
            } break;
            case TextEncoding_UTF32LE:
            {
              DebugPrint("TextEncoding_UTF32LE ", Depth);
            } break;
            case TextEncoding_UTF32BE:
            {
              DebugPrint("TextEncoding_UTF32BE ", Depth);
            } break;
            case TextEncoding_UTF7:
            {
              DebugPrint("TextEncoding_UTF7 ", Depth);
            } break;
            case TextEncoding_UTF1:
            {
              DebugPrint("TextEncoding_UTF1 ", Depth);
            } break;
            case TextEncoding_EBCDIC:
            {
              DebugPrint("TextEncoding_EBCDIC ", Depth);
            } break;
            case TextEncoding_CSCU:
            {
              DebugPrint("TextEncoding_CSCU ", Depth);
            } break;
            case TextEncoding_BOCU:
            {
              DebugPrint("TextEncoding_BOCU ", Depth);
            } break;
            case TextEncoding_GB18030:
            {
              DebugPrint("TextEncoding_GB18030 ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( mutex_op EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case MutexOp_Waiting:
            {
              DebugPrint("MutexOp_Waiting ", Depth);
            } break;
            case MutexOp_Aquired:
            {
              DebugPrint("MutexOp_Aquired ", Depth);
            } break;
            case MutexOp_Released:
            {
              DebugPrint("MutexOp_Released ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( xml_token_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case XmlTokenType_Unknown:
            {
              DebugPrint("XmlTokenType_Unknown ", Depth);
            } break;
            case XmlTokenType_Boolean:
            {
              DebugPrint("XmlTokenType_Boolean ", Depth);
            } break;
            case XmlTokenType_Float:
            {
              DebugPrint("XmlTokenType_Float ", Depth);
            } break;
            case XmlTokenType_Int:
            {
              DebugPrint("XmlTokenType_Int ", Depth);
            } break;
            case XmlTokenType_Property:
            {
              DebugPrint("XmlTokenType_Property ", Depth);
            } break;
            case XmlTokenType_Open:
            {
              DebugPrint("XmlTokenType_Open ", Depth);
            } break;
            case XmlTokenType_Close:
            {
              DebugPrint("XmlTokenType_Close ", Depth);
            } break;
            case XmlTokenType_Count:
            {
              DebugPrint("XmlTokenType_Count ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( shader_uniform_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case ShaderUniform_Undefined:
            {
              DebugPrint("ShaderUniform_Undefined ", Depth);
            } break;
            case ShaderUniform_M4:
            {
              DebugPrint("ShaderUniform_M4 ", Depth);
            } break;
            case ShaderUniform_V3:
            {
              DebugPrint("ShaderUniform_V3 ", Depth);
            } break;
            case ShaderUniform_S32:
            {
              DebugPrint("ShaderUniform_S32 ", Depth);
            } break;
            case ShaderUniform_U32:
            {
              DebugPrint("ShaderUniform_U32 ", Depth);
            } break;
            case ShaderUniform_R32:
            {
              DebugPrint("ShaderUniform_R32 ", Depth);
            } break;
            case ShaderUniform_Texture:
            {
              DebugPrint("ShaderUniform_Texture ", Depth);
            } break;
            case ShaderUniform_Light:
            {
              DebugPrint("ShaderUniform_Light ", Depth);
            } break;
            case ShaderUniform_Camera:
            {
              DebugPrint("ShaderUniform_Camera ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( debug_ui_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case DebugUIType_None:
            {
              DebugPrint("DebugUIType_None ", Depth);
            } break;
            case DebugUIType_PickedChunks:
            {
              DebugPrint("DebugUIType_PickedChunks ", Depth);
            } break;
            case DebugUIType_CallGraph:
            {
              DebugPrint("DebugUIType_CallGraph ", Depth);
            } break;
            case DebugUIType_CollatedFunctionCalls:
            {
              DebugPrint("DebugUIType_CollatedFunctionCalls ", Depth);
            } break;
            case DebugUIType_Memory:
            {
              DebugPrint("DebugUIType_Memory ", Depth);
            } break;
            case DebugUIType_Graphics:
            {
              DebugPrint("DebugUIType_Graphics ", Depth);
            } break;
            case DebugUIType_Network:
            {
              DebugPrint("DebugUIType_Network ", Depth);
            } break;
            case DebugUIType_DrawCalls:
            {
              DebugPrint("DebugUIType_DrawCalls ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( d_union_flags EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case d_union_flag_none:
            {
              DebugPrint("d_union_flag_none ", Depth);
            } break;
            case d_union_flag_enum_only:
            {
              DebugPrint("d_union_flag_enum_only ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( metaprogramming_directive EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case meta_directive_noop:
            {
              DebugPrint("meta_directive_noop ", Depth);
            } break;
            case enum_only:
            {
              DebugPrint("enum_only ", Depth);
            } break;
            case d_union:
            {
              DebugPrint("d_union ", Depth);
            } break;
            case named_list:
            {
              DebugPrint("named_list ", Depth);
            } break;
            case for_datatypes:
            {
              DebugPrint("for_datatypes ", Depth);
            } break;
            case func:
            {
              DebugPrint("func ", Depth);
            } break;
            case polymorphic_func:
            {
              DebugPrint("polymorphic_func ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( meta_arg_operator EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case meta_arg_operator_noop:
            {
              DebugPrint("meta_arg_operator_noop ", Depth);
            } break;
            case name:
            {
              DebugPrint("name ", Depth);
            } break;
            case type:
            {
              DebugPrint("type ", Depth);
            } break;
            case value:
            {
              DebugPrint("value ", Depth);
            } break;
            case map_values:
            {
              DebugPrint("map_values ", Depth);
            } break;
            case map_members:
            {
              DebugPrint("map_members ", Depth);
            } break;
            case is_enum:
            {
              DebugPrint("is_enum ", Depth);
            } break;
            case is_struct:
            {
              DebugPrint("is_struct ", Depth);
            } break;
            case is_union:
            {
              DebugPrint("is_union ", Depth);
            } break;
            case is_defined:
            {
              DebugPrint("is_defined ", Depth);
            } break;
            case is_compound:
            {
              DebugPrint("is_compound ", Depth);
            } break;
            case is_primitive:
            {
              DebugPrint("is_primitive ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( meta_transform_op EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case meta_transform_op_noop:
            {
              DebugPrint("meta_transform_op_noop ", Depth);
            } break;
            case to_capital_case:
            {
              DebugPrint("to_capital_case ", Depth);
            } break;
            case to_lowercase:
            {
              DebugPrint("to_lowercase ", Depth);
            } break;
            case strip_single_prefix:
            {
              DebugPrint("strip_single_prefix ", Depth);
            } break;
            case strip_all_prefix:
            {
              DebugPrint("strip_all_prefix ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( c_token_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case CTokenType_Unknown:
            {
              DebugPrint("CTokenType_Unknown ", Depth);
            } break;
            case CT_ControlChar_Start_of_Heading:
            {
              DebugPrint("CT_ControlChar_Start_of_Heading ", Depth);
            } break;
            case CT_ControlChar_Start_of_Text:
            {
              DebugPrint("CT_ControlChar_Start_of_Text ", Depth);
            } break;
            case CT_ControlChar_End_of_Text:
            {
              DebugPrint("CT_ControlChar_End_of_Text ", Depth);
            } break;
            case CT_ControlChar_End_of_Transmission:
            {
              DebugPrint("CT_ControlChar_End_of_Transmission ", Depth);
            } break;
            case CT_ControlChar_Enquiry:
            {
              DebugPrint("CT_ControlChar_Enquiry ", Depth);
            } break;
            case CT_ControlChar_Acknowledgement:
            {
              DebugPrint("CT_ControlChar_Acknowledgement ", Depth);
            } break;
            case CT_ControlChar_Bell:
            {
              DebugPrint("CT_ControlChar_Bell ", Depth);
            } break;
            case CT_ControlChar_Backspace:
            {
              DebugPrint("CT_ControlChar_Backspace ", Depth);
            } break;
            case CT_ControlChar_Vertical_Tab:
            {
              DebugPrint("CT_ControlChar_Vertical_Tab ", Depth);
            } break;
            case CT_ControlChar_Form_Feed:
            {
              DebugPrint("CT_ControlChar_Form_Feed ", Depth);
            } break;
            case CT_ControlChar_Shift_Out:
            {
              DebugPrint("CT_ControlChar_Shift_Out ", Depth);
            } break;
            case CT_ControlChar_Shift_In:
            {
              DebugPrint("CT_ControlChar_Shift_In ", Depth);
            } break;
            case CT_ControlChar_Data_Link_Escape:
            {
              DebugPrint("CT_ControlChar_Data_Link_Escape ", Depth);
            } break;
            case CT_ControlChar_Device_Control_1:
            {
              DebugPrint("CT_ControlChar_Device_Control_1 ", Depth);
            } break;
            case CT_ControlChar_Device_Control_2:
            {
              DebugPrint("CT_ControlChar_Device_Control_2 ", Depth);
            } break;
            case CT_ControlChar_Device_Control_3:
            {
              DebugPrint("CT_ControlChar_Device_Control_3 ", Depth);
            } break;
            case CT_ControlChar_Device_Control_4:
            {
              DebugPrint("CT_ControlChar_Device_Control_4 ", Depth);
            } break;
            case CT_ControlChar_Negative_Acknowledgement:
            {
              DebugPrint("CT_ControlChar_Negative_Acknowledgement ", Depth);
            } break;
            case CT_ControlChar_Synchronous_Idle:
            {
              DebugPrint("CT_ControlChar_Synchronous_Idle ", Depth);
            } break;
            case CT_ControlChar_End_of_Transmission_Block:
            {
              DebugPrint("CT_ControlChar_End_of_Transmission_Block ", Depth);
            } break;
            case CT_ControlChar_Cancel:
            {
              DebugPrint("CT_ControlChar_Cancel ", Depth);
            } break;
            case CT_ControlChar_End_of_Medium:
            {
              DebugPrint("CT_ControlChar_End_of_Medium ", Depth);
            } break;
            case CT_ControlChar_Substitute:
            {
              DebugPrint("CT_ControlChar_Substitute ", Depth);
            } break;
            case CT_ControlChar_Escape:
            {
              DebugPrint("CT_ControlChar_Escape ", Depth);
            } break;
            case CT_ControlChar_File_Separator:
            {
              DebugPrint("CT_ControlChar_File_Separator ", Depth);
            } break;
            case CT_ControlChar_Group_Separator:
            {
              DebugPrint("CT_ControlChar_Group_Separator ", Depth);
            } break;
            case CT_ControlChar_Record_Separator:
            {
              DebugPrint("CT_ControlChar_Record_Separator ", Depth);
            } break;
            case CT_ControlChar_Unit_Separator:
            {
              DebugPrint("CT_ControlChar_Unit_Separator ", Depth);
            } break;
            case CT_ControlChar_Delete:
            {
              DebugPrint("CT_ControlChar_Delete ", Depth);
            } break;
            case CTokenType_OpenBracket:
            {
              DebugPrint("CTokenType_OpenBracket ", Depth);
            } break;
            case CTokenType_CloseBracket:
            {
              DebugPrint("CTokenType_CloseBracket ", Depth);
            } break;
            case CTokenType_OpenBrace:
            {
              DebugPrint("CTokenType_OpenBrace ", Depth);
            } break;
            case CTokenType_CloseBrace:
            {
              DebugPrint("CTokenType_CloseBrace ", Depth);
            } break;
            case CTokenType_OpenParen:
            {
              DebugPrint("CTokenType_OpenParen ", Depth);
            } break;
            case CTokenType_CloseParen:
            {
              DebugPrint("CTokenType_CloseParen ", Depth);
            } break;
            case CTokenType_Dot:
            {
              DebugPrint("CTokenType_Dot ", Depth);
            } break;
            case CTokenType_Comma:
            {
              DebugPrint("CTokenType_Comma ", Depth);
            } break;
            case CTokenType_Semicolon:
            {
              DebugPrint("CTokenType_Semicolon ", Depth);
            } break;
            case CTokenType_Colon:
            {
              DebugPrint("CTokenType_Colon ", Depth);
            } break;
            case CTokenType_Hash:
            {
              DebugPrint("CTokenType_Hash ", Depth);
            } break;
            case CTokenType_At:
            {
              DebugPrint("CTokenType_At ", Depth);
            } break;
            case CTokenType_Dollar:
            {
              DebugPrint("CTokenType_Dollar ", Depth);
            } break;
            case CTokenType_Space:
            {
              DebugPrint("CTokenType_Space ", Depth);
            } break;
            case CTokenType_Tab:
            {
              DebugPrint("CTokenType_Tab ", Depth);
            } break;
            case CTokenType_Star:
            {
              DebugPrint("CTokenType_Star ", Depth);
            } break;
            case CTokenType_Ampersand:
            {
              DebugPrint("CTokenType_Ampersand ", Depth);
            } break;
            case CTokenType_SingleQuote:
            {
              DebugPrint("CTokenType_SingleQuote ", Depth);
            } break;
            case CTokenType_DoubleQuote:
            {
              DebugPrint("CTokenType_DoubleQuote ", Depth);
            } break;
            case CTokenType_Equals:
            {
              DebugPrint("CTokenType_Equals ", Depth);
            } break;
            case CTokenType_LT:
            {
              DebugPrint("CTokenType_LT ", Depth);
            } break;
            case CTokenType_GT:
            {
              DebugPrint("CTokenType_GT ", Depth);
            } break;
            case CTokenType_Plus:
            {
              DebugPrint("CTokenType_Plus ", Depth);
            } break;
            case CTokenType_Minus:
            {
              DebugPrint("CTokenType_Minus ", Depth);
            } break;
            case CTokenType_Percent:
            {
              DebugPrint("CTokenType_Percent ", Depth);
            } break;
            case CTokenType_Bang:
            {
              DebugPrint("CTokenType_Bang ", Depth);
            } break;
            case CTokenType_Hat:
            {
              DebugPrint("CTokenType_Hat ", Depth);
            } break;
            case CTokenType_Question:
            {
              DebugPrint("CTokenType_Question ", Depth);
            } break;
            case CTokenType_FSlash:
            {
              DebugPrint("CTokenType_FSlash ", Depth);
            } break;
            case CTokenType_BSlash:
            {
              DebugPrint("CTokenType_BSlash ", Depth);
            } break;
            case CTokenType_Tilde:
            {
              DebugPrint("CTokenType_Tilde ", Depth);
            } break;
            case CTokenType_Backtick:
            {
              DebugPrint("CTokenType_Backtick ", Depth);
            } break;
            case CTokenType_Pipe:
            {
              DebugPrint("CTokenType_Pipe ", Depth);
            } break;
            case CTokenType_Newline:
            {
              DebugPrint("CTokenType_Newline ", Depth);
            } break;
            case CTokenType_CarrigeReturn:
            {
              DebugPrint("CTokenType_CarrigeReturn ", Depth);
            } break;
            case CTokenType_EOF:
            {
              DebugPrint("CTokenType_EOF ", Depth);
            } break;
            case CTokenType_CommentSingleLine:
            {
              DebugPrint("CTokenType_CommentSingleLine ", Depth);
            } break;
            case CTokenType_CommentMultiLine:
            {
              DebugPrint("CTokenType_CommentMultiLine ", Depth);
            } break;
            case CTokenType_EscapedNewline:
            {
              DebugPrint("CTokenType_EscapedNewline ", Depth);
            } break;
            case CTokenType_Identifier:
            {
              DebugPrint("CTokenType_Identifier ", Depth);
            } break;
            case CTokenType_StringLiteral:
            {
              DebugPrint("CTokenType_StringLiteral ", Depth);
            } break;
            case CTokenType_CharLiteral:
            {
              DebugPrint("CTokenType_CharLiteral ", Depth);
            } break;
            case CTokenType_IntLiteral:
            {
              DebugPrint("CTokenType_IntLiteral ", Depth);
            } break;
            case CTokenType_DoubleLiteral:
            {
              DebugPrint("CTokenType_DoubleLiteral ", Depth);
            } break;
            case CTokenType_FloatLiteral:
            {
              DebugPrint("CTokenType_FloatLiteral ", Depth);
            } break;
            case CTokenType_Meta:
            {
              DebugPrint("CTokenType_Meta ", Depth);
            } break;
            case CT_Keyword_Override:
            {
              DebugPrint("CT_Keyword_Override ", Depth);
            } break;
            case CT_Keyword_Virtual:
            {
              DebugPrint("CT_Keyword_Virtual ", Depth);
            } break;
            case CT_Keyword_Noexcept:
            {
              DebugPrint("CT_Keyword_Noexcept ", Depth);
            } break;
            case CT_Keyword_Explicit:
            {
              DebugPrint("CT_Keyword_Explicit ", Depth);
            } break;
            case CT_Keyword_Constexpr:
            {
              DebugPrint("CT_Keyword_Constexpr ", Depth);
            } break;
            case CT_Keyword_Namespace:
            {
              DebugPrint("CT_Keyword_Namespace ", Depth);
            } break;
            case CT_Keyword_Class:
            {
              DebugPrint("CT_Keyword_Class ", Depth);
            } break;
            case CTokenType_Struct:
            {
              DebugPrint("CTokenType_Struct ", Depth);
            } break;
            case CTokenType_Enum:
            {
              DebugPrint("CTokenType_Enum ", Depth);
            } break;
            case CTokenType_Union:
            {
              DebugPrint("CTokenType_Union ", Depth);
            } break;
            case CTokenType_Typedef:
            {
              DebugPrint("CTokenType_Typedef ", Depth);
            } break;
            case CTokenType_Using:
            {
              DebugPrint("CTokenType_Using ", Depth);
            } break;
            case CTokenType_ThreadLocal:
            {
              DebugPrint("CTokenType_ThreadLocal ", Depth);
            } break;
            case CTokenType_Const:
            {
              DebugPrint("CTokenType_Const ", Depth);
            } break;
            case CTokenType_Static:
            {
              DebugPrint("CTokenType_Static ", Depth);
            } break;
            case CTokenType_Volatile:
            {
              DebugPrint("CTokenType_Volatile ", Depth);
            } break;
            case CTokenType_Long:
            {
              DebugPrint("CTokenType_Long ", Depth);
            } break;
            case CTokenType_Unsigned:
            {
              DebugPrint("CTokenType_Unsigned ", Depth);
            } break;
            case CTokenType_Signed:
            {
              DebugPrint("CTokenType_Signed ", Depth);
            } break;
            case CTokenType_Bool:
            {
              DebugPrint("CTokenType_Bool ", Depth);
            } break;
            case CTokenType_Auto:
            {
              DebugPrint("CTokenType_Auto ", Depth);
            } break;
            case CTokenType_Void:
            {
              DebugPrint("CTokenType_Void ", Depth);
            } break;
            case CTokenType_Double:
            {
              DebugPrint("CTokenType_Double ", Depth);
            } break;
            case CTokenType_Float:
            {
              DebugPrint("CTokenType_Float ", Depth);
            } break;
            case CTokenType_Char:
            {
              DebugPrint("CTokenType_Char ", Depth);
            } break;
            case CTokenType_Int:
            {
              DebugPrint("CTokenType_Int ", Depth);
            } break;
            case CTokenType_Short:
            {
              DebugPrint("CTokenType_Short ", Depth);
            } break;
            case CTokenType_Inline:
            {
              DebugPrint("CTokenType_Inline ", Depth);
            } break;
            case CTokenType_TemplateKeyword:
            {
              DebugPrint("CTokenType_TemplateKeyword ", Depth);
            } break;
            case CTokenType_OperatorKeyword:
            {
              DebugPrint("CTokenType_OperatorKeyword ", Depth);
            } break;
            case CT_Keyword_Public:
            {
              DebugPrint("CT_Keyword_Public ", Depth);
            } break;
            case CT_Keyword_Private:
            {
              DebugPrint("CT_Keyword_Private ", Depth);
            } break;
            case CT_Keyword_Protected:
            {
              DebugPrint("CT_Keyword_Protected ", Depth);
            } break;
            case CT_KeywordPragma:
            {
              DebugPrint("CT_KeywordPragma ", Depth);
            } break;
            case CT_KeywordAttribute:
            {
              DebugPrint("CT_KeywordAttribute ", Depth);
            } break;
            case CT_StaticAssert:
            {
              DebugPrint("CT_StaticAssert ", Depth);
            } break;
            case CTokenType_Extern:
            {
              DebugPrint("CTokenType_Extern ", Depth);
            } break;
            case CTokenType_Asm:
            {
              DebugPrint("CTokenType_Asm ", Depth);
            } break;
            case CTokenType_Goto:
            {
              DebugPrint("CTokenType_Goto ", Depth);
            } break;
            case CTokenType_Ellipsis:
            {
              DebugPrint("CTokenType_Ellipsis ", Depth);
            } break;
            case CTokenType_If:
            {
              DebugPrint("CTokenType_If ", Depth);
            } break;
            case CTokenType_Else:
            {
              DebugPrint("CTokenType_Else ", Depth);
            } break;
            case CTokenType_Break:
            {
              DebugPrint("CTokenType_Break ", Depth);
            } break;
            case CTokenType_Switch:
            {
              DebugPrint("CTokenType_Switch ", Depth);
            } break;
            case CTokenType_Case:
            {
              DebugPrint("CTokenType_Case ", Depth);
            } break;
            case CT_Keyword_Delete:
            {
              DebugPrint("CT_Keyword_Delete ", Depth);
            } break;
            case CTokenType_Default:
            {
              DebugPrint("CTokenType_Default ", Depth);
            } break;
            case CTokenType_For:
            {
              DebugPrint("CTokenType_For ", Depth);
            } break;
            case CTokenType_While:
            {
              DebugPrint("CTokenType_While ", Depth);
            } break;
            case CTokenType_Continue:
            {
              DebugPrint("CTokenType_Continue ", Depth);
            } break;
            case CTokenType_Return:
            {
              DebugPrint("CTokenType_Return ", Depth);
            } break;
            case CTokenType_LeftShift:
            {
              DebugPrint("CTokenType_LeftShift ", Depth);
            } break;
            case CTokenType_RightShift:
            {
              DebugPrint("CTokenType_RightShift ", Depth);
            } break;
            case CTokenType_LessEqual:
            {
              DebugPrint("CTokenType_LessEqual ", Depth);
            } break;
            case CTokenType_GreaterEqual:
            {
              DebugPrint("CTokenType_GreaterEqual ", Depth);
            } break;
            case CTokenType_AreEqual:
            {
              DebugPrint("CTokenType_AreEqual ", Depth);
            } break;
            case CTokenType_NotEqual:
            {
              DebugPrint("CTokenType_NotEqual ", Depth);
            } break;
            case CTokenType_PlusEquals:
            {
              DebugPrint("CTokenType_PlusEquals ", Depth);
            } break;
            case CTokenType_MinusEquals:
            {
              DebugPrint("CTokenType_MinusEquals ", Depth);
            } break;
            case CTokenType_TimesEquals:
            {
              DebugPrint("CTokenType_TimesEquals ", Depth);
            } break;
            case CTokenType_DivEquals:
            {
              DebugPrint("CTokenType_DivEquals ", Depth);
            } break;
            case CTokenType_ModEquals:
            {
              DebugPrint("CTokenType_ModEquals ", Depth);
            } break;
            case CTokenType_AndEquals:
            {
              DebugPrint("CTokenType_AndEquals ", Depth);
            } break;
            case CTokenType_OrEquals:
            {
              DebugPrint("CTokenType_OrEquals ", Depth);
            } break;
            case CTokenType_XorEquals:
            {
              DebugPrint("CTokenType_XorEquals ", Depth);
            } break;
            case CTokenType_Increment:
            {
              DebugPrint("CTokenType_Increment ", Depth);
            } break;
            case CTokenType_Decrement:
            {
              DebugPrint("CTokenType_Decrement ", Depth);
            } break;
            case CTokenType_LogicalAnd:
            {
              DebugPrint("CTokenType_LogicalAnd ", Depth);
            } break;
            case CTokenType_LogicalOr:
            {
              DebugPrint("CTokenType_LogicalOr ", Depth);
            } break;
            case CTokenType_Arrow:
            {
              DebugPrint("CTokenType_Arrow ", Depth);
            } break;
            case CT_ScopeResolutionOperator:
            {
              DebugPrint("CT_ScopeResolutionOperator ", Depth);
            } break;
            case CT_NameQualifier:
            {
              DebugPrint("CT_NameQualifier ", Depth);
            } break;
            case CT_MacroLiteral:
            {
              DebugPrint("CT_MacroLiteral ", Depth);
            } break;
            case CT_MacroLiteral_SelfRefExpansion:
            {
              DebugPrint("CT_MacroLiteral_SelfRefExpansion ", Depth);
            } break;
            case CT_PreprocessorPaste:
            {
              DebugPrint("CT_PreprocessorPaste ", Depth);
            } break;
            case CT_PreprocessorPaste_InvalidToken:
            {
              DebugPrint("CT_PreprocessorPaste_InvalidToken ", Depth);
            } break;
            case CT_PreprocessorInclude:
            {
              DebugPrint("CT_PreprocessorInclude ", Depth);
            } break;
            case CT_PreprocessorIncludeNext:
            {
              DebugPrint("CT_PreprocessorIncludeNext ", Depth);
            } break;
            case CT_PreprocessorIf:
            {
              DebugPrint("CT_PreprocessorIf ", Depth);
            } break;
            case CT_PreprocessorElse:
            {
              DebugPrint("CT_PreprocessorElse ", Depth);
            } break;
            case CT_PreprocessorElif:
            {
              DebugPrint("CT_PreprocessorElif ", Depth);
            } break;
            case CT_PreprocessorEndif:
            {
              DebugPrint("CT_PreprocessorEndif ", Depth);
            } break;
            case CT_PreprocessorIfDefined:
            {
              DebugPrint("CT_PreprocessorIfDefined ", Depth);
            } break;
            case CT_PreprocessorIfNotDefined:
            {
              DebugPrint("CT_PreprocessorIfNotDefined ", Depth);
            } break;
            case CT_PreprocessorDefine:
            {
              DebugPrint("CT_PreprocessorDefine ", Depth);
            } break;
            case CT_PreprocessorUndef:
            {
              DebugPrint("CT_PreprocessorUndef ", Depth);
            } break;
            case CT_PreprocessorPragma:
            {
              DebugPrint("CT_PreprocessorPragma ", Depth);
            } break;
            case CT_PreprocessorError:
            {
              DebugPrint("CT_PreprocessorError ", Depth);
            } break;
            case CT_PreprocessorWarning:
            {
              DebugPrint("CT_PreprocessorWarning ", Depth);
            } break;
            case CT_Preprocessor__VA_ARGS__:
            {
              DebugPrint("CT_Preprocessor__VA_ARGS__ ", Depth);
            } break;
            case CT_Preprocessor_Nuked:
            {
              DebugPrint("CT_Preprocessor_Nuked ", Depth);
            } break;
            case CT_InsertedCode:
            {
              DebugPrint("CT_InsertedCode ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( c_token_flags EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case CTFlags_None:
            {
              DebugPrint("CTFlags_None ", Depth);
            } break;
            case CTFlags_RelativeInclude:
            {
              DebugPrint("CTFlags_RelativeInclude ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( va_args_flags EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case va_args_flags_none:
            {
              DebugPrint("va_args_flags_none ", Depth);
            } break;
            case va_args_flags_expand_with_commas:
            {
              DebugPrint("va_args_flags_expand_with_commas ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( token_cursor_source EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case TokenCursorSource_Unknown:
            {
              DebugPrint("TokenCursorSource_Unknown ", Depth);
            } break;
            case TokenCursorSource_RootFile:
            {
              DebugPrint("TokenCursorSource_RootFile ", Depth);
            } break;
            case TokenCursorSource_Include:
            {
              DebugPrint("TokenCursorSource_Include ", Depth);
            } break;
            case TokenCursorSource_MacroExpansion:
            {
              DebugPrint("TokenCursorSource_MacroExpansion ", Depth);
            } break;
            case TokenCursorSource_MetaprogrammingExpansion:
            {
              DebugPrint("TokenCursorSource_MetaprogrammingExpansion ", Depth);
            } break;
            case TokenCursorSource_PasteOperator:
            {
              DebugPrint("TokenCursorSource_PasteOperator ", Depth);
            } break;
            case TokenCursorSource_CommandLineOption:
            {
              DebugPrint("TokenCursorSource_CommandLineOption ", Depth);
            } break;
            case TokenCursorSource_BodyText:
            {
              DebugPrint("TokenCursorSource_BodyText ", Depth);
            } break;
            case TokenCursorSource_IntermediateRepresentaton:
            {
              DebugPrint("TokenCursorSource_IntermediateRepresentaton ", Depth);
            } break;
            case TokenCursorSource_Count:
            {
              DebugPrint("TokenCursorSource_Count ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( parse_error_code EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case ParseErrorCode_None:
            {
              DebugPrint("ParseErrorCode_None ", Depth);
            } break;
            case ParseErrorCode_ExpectedSemicolonOrEquals:
            {
              DebugPrint("ParseErrorCode_ExpectedSemicolonOrEquals ", Depth);
            } break;
            case ParseErrorCode_StreamEndedUnexpectedly:
            {
              DebugPrint("ParseErrorCode_StreamEndedUnexpectedly ", Depth);
            } break;
            case ParseErrorCode_RequireTokenFailed:
            {
              DebugPrint("ParseErrorCode_RequireTokenFailed ", Depth);
            } break;
            case ParseErrorCode_InvalidTokenGenerated:
            {
              DebugPrint("ParseErrorCode_InvalidTokenGenerated ", Depth);
            } break;
            case ParseErrorCode_DUnionParse:
            {
              DebugPrint("ParseErrorCode_DUnionParse ", Depth);
            } break;
            case ParseErrorCode_UndefinedDatatype:
            {
              DebugPrint("ParseErrorCode_UndefinedDatatype ", Depth);
            } break;
            case ParseErrorCode_PoofTypeError:
            {
              DebugPrint("ParseErrorCode_PoofTypeError ", Depth);
            } break;
            case ParseErrorCode_InputStreamNull:
            {
              DebugPrint("ParseErrorCode_InputStreamNull ", Depth);
            } break;
            case ParseErrorCode_InternalCompilerError:
            {
              DebugPrint("ParseErrorCode_InternalCompilerError ", Depth);
            } break;
            case ParseErrorCode_Unknown:
            {
              DebugPrint("ParseErrorCode_Unknown ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( linkage_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case linkage_noop:
            {
              DebugPrint("linkage_noop ", Depth);
            } break;
            case linkage_extern:
            {
              DebugPrint("linkage_extern ", Depth);
            } break;
            case linkage_extern_c:
            {
              DebugPrint("linkage_extern_c ", Depth);
            } break;
            case linkage_extern_cpp:
            {
              DebugPrint("linkage_extern_cpp ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( type_qualifier EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case TypeQual_None:
            {
              DebugPrint("TypeQual_None ", Depth);
            } break;
            case TypeQual_Void:
            {
              DebugPrint("TypeQual_Void ", Depth);
            } break;
            case TypeQual_Auto:
            {
              DebugPrint("TypeQual_Auto ", Depth);
            } break;
            case TypeQual_Class:
            {
              DebugPrint("TypeQual_Class ", Depth);
            } break;
            case TypeQual_Struct:
            {
              DebugPrint("TypeQual_Struct ", Depth);
            } break;
            case TypeQual_Union:
            {
              DebugPrint("TypeQual_Union ", Depth);
            } break;
            case TypeQual_Enum:
            {
              DebugPrint("TypeQual_Enum ", Depth);
            } break;
            case TypeQual_Bool:
            {
              DebugPrint("TypeQual_Bool ", Depth);
            } break;
            case TypeQual_Inline:
            {
              DebugPrint("TypeQual_Inline ", Depth);
            } break;
            case TypeQual_ThreadLocal:
            {
              DebugPrint("TypeQual_ThreadLocal ", Depth);
            } break;
            case TypeQual_Static:
            {
              DebugPrint("TypeQual_Static ", Depth);
            } break;
            case TypeQual_Extern:
            {
              DebugPrint("TypeQual_Extern ", Depth);
            } break;
            case TypeQual_Const:
            {
              DebugPrint("TypeQual_Const ", Depth);
            } break;
            case TypeQual_Volatile:
            {
              DebugPrint("TypeQual_Volatile ", Depth);
            } break;
            case TypeQual_Signed:
            {
              DebugPrint("TypeQual_Signed ", Depth);
            } break;
            case TypeQual_Unsigned:
            {
              DebugPrint("TypeQual_Unsigned ", Depth);
            } break;
            case TypeQual_Char:
            {
              DebugPrint("TypeQual_Char ", Depth);
            } break;
            case TypeQual_Long:
            {
              DebugPrint("TypeQual_Long ", Depth);
            } break;
            case TypeQual_Int:
            {
              DebugPrint("TypeQual_Int ", Depth);
            } break;
            case TypeQual_Long_Long:
            {
              DebugPrint("TypeQual_Long_Long ", Depth);
            } break;
            case TypeQual_Double:
            {
              DebugPrint("TypeQual_Double ", Depth);
            } break;
            case TypeQual_Short:
            {
              DebugPrint("TypeQual_Short ", Depth);
            } break;
            case TypeQual_Float:
            {
              DebugPrint("TypeQual_Float ", Depth);
            } break;
            case TypeQual_Constexpr:
            {
              DebugPrint("TypeQual_Constexpr ", Depth);
            } break;
            case TypeQual_Explicit:
            {
              DebugPrint("TypeQual_Explicit ", Depth);
            } break;
            case TypeQual_Operator:
            {
              DebugPrint("TypeQual_Operator ", Depth);
            } break;
            case TypeQual_Virtual:
            {
              DebugPrint("TypeQual_Virtual ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( function_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case function_type_noop:
            {
              DebugPrint("function_type_noop ", Depth);
            } break;
            case function_type_constructor:
            {
              DebugPrint("function_type_constructor ", Depth);
            } break;
            case function_type_destructor:
            {
              DebugPrint("function_type_destructor ", Depth);
            } break;
            case function_type_operator:
            {
              DebugPrint("function_type_operator ", Depth);
            } break;
            case function_type_normal:
            {
              DebugPrint("function_type_normal ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( datatype_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case type_datatype_noop:
            {
              DebugPrint("type_datatype_noop ", Depth);
            } break;
            case type_declaration:
            {
              DebugPrint("type_declaration ", Depth);
            } break;
            case type_enum_member:
            {
              DebugPrint("type_enum_member ", Depth);
            } break;
            case type_type_def:
            {
              DebugPrint("type_type_def ", Depth);
            } break;
            case type_primitive_def:
            {
              DebugPrint("type_primitive_def ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( macro_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case type_macro_noop:
            {
              DebugPrint("type_macro_noop ", Depth);
            } break;
            case type_macro_keyword:
            {
              DebugPrint("type_macro_keyword ", Depth);
            } break;
            case type_macro_function:
            {
              DebugPrint("type_macro_function ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( output_mode EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case Output_NoOverwrite:
            {
              DebugPrint("Output_NoOverwrite ", Depth);
            } break;
            case Output_Unsafe:
            {
              DebugPrint("Output_Unsafe ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( parser_push_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case parser_push_type_noop:
            {
              DebugPrint("parser_push_type_noop ", Depth);
            } break;
            case parser_push_type_root:
            {
              DebugPrint("parser_push_type_root ", Depth);
            } break;
            case parser_push_type_include:
            {
              DebugPrint("parser_push_type_include ", Depth);
            } break;
            case parser_push_type_macro:
            {
              DebugPrint("parser_push_type_macro ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( erase_token_mode EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case PreserveTokens:
            {
              DebugPrint("PreserveTokens ", Depth);
            } break;
            case EraseTokens:
            {
              DebugPrint("EraseTokens ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( declaration_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case type_declaration_noop:
            {
              DebugPrint("type_declaration_noop ", Depth);
            } break;
            case type_enum_decl:
            {
              DebugPrint("type_enum_decl ", Depth);
            } break;
            case type_function_decl:
            {
              DebugPrint("type_function_decl ", Depth);
            } break;
            case type_compound_decl:
            {
              DebugPrint("type_compound_decl ", Depth);
            } break;
            case type_variable_decl:
            {
              DebugPrint("type_variable_decl ", Depth);
            } break;

      }
    }
    bonsai_function void DebugPrint( ast_node_type EnumValue, u32 Depth)
    {
      switch (EnumValue)
      {
                    case type_ast_node_noop:
            {
              DebugPrint("type_ast_node_noop ", Depth);
            } break;
            case type_ast_node_statement:
            {
              DebugPrint("type_ast_node_statement ", Depth);
            } break;
            case type_ast_node_access:
            {
              DebugPrint("type_ast_node_access ", Depth);
            } break;
            case type_ast_node_literal:
            {
              DebugPrint("type_ast_node_literal ", Depth);
            } break;
            case type_ast_node_expression:
            {
              DebugPrint("type_ast_node_expression ", Depth);
            } break;
            case type_ast_node_parenthesized:
            {
              DebugPrint("type_ast_node_parenthesized ", Depth);
            } break;
            case type_ast_node_operator:
            {
              DebugPrint("type_ast_node_operator ", Depth);
            } break;
            case type_ast_node_function_call:
            {
              DebugPrint("type_ast_node_function_call ", Depth);
            } break;
            case type_ast_node_return:
            {
              DebugPrint("type_ast_node_return ", Depth);
            } break;
            case type_ast_node_initializer_list:
            {
              DebugPrint("type_ast_node_initializer_list ", Depth);
            } break;
            case type_ast_node_symbol:
            {
              DebugPrint("type_ast_node_symbol ", Depth);
            } break;
            case type_ast_node_variable_def:
            {
              DebugPrint("type_ast_node_variable_def ", Depth);
            } break;
            case type_ast_node_type_specifier:
            {
              DebugPrint("type_ast_node_type_specifier ", Depth);
            } break;

      }
    }

