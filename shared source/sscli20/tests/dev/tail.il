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
.assembly tail
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module tail.exe
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
} // end of class MainApp


.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  sum(int32 val,
                                             int32 res) cil managed
  {
    // Code size       34 (0x22)
    .maxstack  3
    .locals init (int32 V_0, int64 V_1, int64 V_2, int64 V_3, int64 V_4, int64 V_5)
    IL_000a:  ldarg.0
    IL_000b:  ldc.i4.0
    IL_000c:  ble.s      IL_001C
    IL_000e:  ldarg.0
    IL_000f:  ldc.i4.1
    IL_0010:  sub
    IL_0011:  ldarg.1
    IL_0012:  ldarg.0
    IL_0013:  add
              tail.
    IL_0014:  call       int32 MainApp::sum(int32,
                                            int32)
              ret
    IL_001C:  ldarg.1
              ret
   
  } // end of method MainApp::sum

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       14 (0xe)
    .maxstack  2
    IL_0000:  ldc.i4     5000
    IL_0002:  ldc.i4.0
    IL_0003:  call       int32 MainApp::sum(int32,
                                            int32)
              dup
              call       void [mscorlib]System.Console::WriteLine(int32)
              ldc.i4     12502500
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
