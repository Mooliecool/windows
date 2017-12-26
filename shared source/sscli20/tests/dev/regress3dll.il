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
  .hash = (98 31 71 95 B3 A4 51 E8 E2 0D 25 55 57 E7 6E 9E   // .1q...Q...%UW.n.
           54 8C EF C9 )                                     // T...
  .ver 1:0:3300:0
}

.assembly regress3dll
{
  .custom instance void [mscorlib]System.Reflection.AssemblyDelaySignAttribute::.ctor(bool) = ( 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module regress3dll.dll
.imagebase 0x10000000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi C
       extends [mscorlib]System.Object
{
} // end of class C


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================

//Global fields
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.field public static int16 xyzzy at D_00002088

// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi C
       extends [mscorlib]System.Object
{
  .method public static void  Doit() cil managed
  {

    .maxstack  2
    .locals init (class [mscorlib]System.Threading.Thread V_0)
    IL_0000:  ldnull
    IL_0001:  stloc.0
    IL_0002:  ldc.i4     0x6eb
    IL_0007:  stsfld     int16 xyzzy
    IL_000c:  ldnull
    IL_000d:  ldftn      void C::ts()
    IL_0013:  newobj     instance void [mscorlib]System.Threading.ThreadStart::.ctor(object,
                                                                                     native int)
    IL_0018:  newobj     instance void [mscorlib]System.Threading.Thread::.ctor(class [mscorlib]System.Threading.ThreadStart)
    IL_001d:  stloc.0
    IL_001e:  ldstr      "About to call Threading::Thread::Start"
    IL_0023:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0028:  ldloc.0
    IL_0029:  call       instance void [mscorlib]System.Threading.Thread::Start()
    IL_002e:  ret
  } // end of method C::Doit

  .method public static void  ts() cil managed
  {

    .maxstack  1
    IL_0000:  ldstr      "Entering C::ts"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_000a:  ldc.i4     0x3e8
    IL_000f:  call       void [mscorlib]System.Threading.Thread::Sleep(int32)
    IL_0014:  ldstr      "Leaving C::ts"
    IL_0019:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_001e:  ret
  } // end of method C::ts

  .method public specialname rtspecialname 
          instance void  .ctor() cil managed
  {

    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method C::.ctor

} // end of class C


// =============================================================

.data D_00002088 = bytearray (
                 FF 25)                                           // .%

