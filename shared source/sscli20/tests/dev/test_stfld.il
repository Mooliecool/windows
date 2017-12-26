// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly t2
{
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module t2.exe
// MVID: {fda21826-efe2-619b-d14b-93c994a8303f}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x06f40000
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .class sequential ansi sealed nested public beforefieldinit foo
         extends [mscorlib]System.ValueType
  {
  } // end of class foo

} // end of class MainApp


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .class sequential ansi sealed nested public beforefieldinit foo
         extends [mscorlib]System.ValueType
  {
    .field public static int32 i
    .field public int32 j
  } // end of class foo

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       42 (0x2a)
    .maxstack  2
    .locals init (valuetype MainApp/foo V_0)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    MainApp/foo
    IL_0008:  ldloca.s   0
    IL_000a:  ldflda     int32 MainApp/foo::i
    IL_000f:  ldc.i4.s   60
              stind.i4
              ldloca.s   V_0
              ldc.i4.s   60
              stfld      int32 MainApp/foo::i
              ldloc      V_0
              ldfld      int32 MainApp/foo::i
              ldc.i4.s   60
    IL_0011:  beq.s      IL_001f

    IL_0013:  ldstr      "Fail"
    IL_0018:  call       void [mscorlib]System.Console::WriteLine(string)
              ldc.i4.1
              call       void [mscorlib]System.Environment::Exit(int32)
    IL_001d:  br.s       IL_0029

    IL_001f:  ldstr      "Pass"
    IL_0024:  call       void [mscorlib]System.Console::WriteLine(string)
              ldc.i4.0
              call       void [mscorlib]System.Environment::Exit(int32)
    IL_0029:  ret
  } // end of method MainApp::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

} // end of class MainApp


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
