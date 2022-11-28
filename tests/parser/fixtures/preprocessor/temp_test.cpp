


#define MacroFunction(a) a
#define self_including_macro_keyword MacroFunction(self_including_macro_keyword 42)
self_including_macro_keyword // should expand to "self_including_macro_keyword 42"

#define m1() m2()
#define m2() m1()
m2()

#define self_including_macro_function() self_including_macro_function()
self_including_macro_function()
