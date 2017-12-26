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

// VTableFixup Directory:
// No data.
.subsystem 0x00000003
.corflags 0x00000001
.assembly extern mscorlib
{
  .publickeytoken = (03 68 91 16 D3 A4 AE 33 )                         // .h.....3
  .hash = (33 81 76 16 24 A4 BE 33 98 15 01 8B 72 BD 29 F7   // 3.v.$..3....r.).
           2D 89 35 99 )                                     // -.5.
  .ver 1:0:2106:0
}
.assembly 'scope9-14' // as "scope9-14"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void ['mscorlib']System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                  bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 
.module 'scope9-14.exe'
// MVID: {7722B4E4-703A-40B2-A99F-087035006953}
.class public auto ansi A
       extends ['mscorlib']System.Object
{
  .field public static int32 i
  .field public int32 j
  .method public hidebysig specialname rtspecialname static 
          void .cctor() il managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldc.i4.s   10
    IL_0002:  stsfld     int32 A::i
    IL_0007:  ret
  } // end of method 'A::.cctor'

  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    // Code size       15 (0xf)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.s   11
    IL_0003:  stfld      int32 A::j
    IL_0008:  ldarg.0
    IL_0009:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_000e:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

.class public auto ansi TestClass
       extends ['mscorlib']System.Object
{
  .method public hidebysig static void Foo() il managed
  {
    // Code size       6 (0x6)
    .maxstack  8
    IL_0000:  call       void TestClass::bar()
    IL_0005:  ret
  } // end of method 'TestClass::Foo'

.method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       25 (0x19)
    .maxstack  2
    .locals init (int32 V_1)
    .try {
      .try {
               call       void TestClass::Foo()
               leave.s    EndTry
               ldstr      "TEST FAILED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.1
               stloc      V_1
               leave.s    EndTry
      }  // end try
      catch ['mscorlib']System.FieldAccessException
      {
               pop
               ldstr      "TEST PASSED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.0
               stloc      V_1
               leave.s    EndTry

      }  // end handler
      EndTry: leave.s EndIL
    }  // end try 
    catch ['mscorlib']System.Exception 
    {
        pop
        ldstr      "TEST FAILED"
        call       void ['mscorlib']System.Console::WriteLine(string)
        ldc.i4.1
        stloc      V_1
        leave.s    EndIL
    }  // end handler

    EndIL:    ldloc  V_1 
              call       void [mscorlib]System.Environment::set_ExitCode(int32)
              ret
  } // end of method 'TestClass::Main'


  .method private hidebysig static void  foo(int32& i) il managed
  {
    // Code size       4 (0x4)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.5
    IL_0002:  stind.i4
    IL_0003:  ret
  } // end of method 'TestClass::foo'

  .method private hidebysig static void bar() il managed
  {
    // Code size       29 (0x1d)
    .maxstack  1
    .locals init (class A V_0)
    IL_0000:  newobj     instance void A::.ctor()
    IL_0005:  stloc.0
    // IL_0006:  ldloc.0
    IL_0007:  ldsflda     int32 A::i
    IL_000c:  call       void TestClass::foo(int32&)
    //IL_0011:  ldloc.0
    IL_0012:  ldsfld      int32 A::i
    IL_0017:  call       void ['mscorlib']System.Console::WriteLine(int32)
    IL_001c:  ret
  } // end of method 'TestClass::bar'

  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
