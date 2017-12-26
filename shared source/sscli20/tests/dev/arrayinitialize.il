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

//  Test for non-trivial constructor calling path in Array.Initialize

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly arrayinitialize
{

  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module arrayinitialize.exe
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .class sequential ansi sealed nested private beforefieldinit Foo
         extends [mscorlib]System.ValueType
  {
  } // end of class Foo

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
  .class sequential ansi sealed nested private beforefieldinit Foo
         extends [mscorlib]System.ValueType
  {
    .field public int32 x
    .field public string y
    .field public object z
    .method private hidebysig specialname rtspecialname 
            instance void  .ctor() cil managed
    {

      .maxstack  2
      IL_0000:  ldarg.0
      IL_0001:  ldc.i4.1
      IL_0002:  stfld      int32 MainApp/Foo::x
      IL_0007:  ldarg.0
      IL_0008:  ldstr      "Hello World!"
      IL_000d:  stfld      string MainApp/Foo::y
      IL_0012:  ldarg.0
      IL_0013:  newobj     instance void [mscorlib]System.Object::.ctor()
      IL_0018:  stfld      object MainApp/Foo::z
      IL_001d:  ret
    } // end of method Foo::.ctor

  } // end of class Foo

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint

    .maxstack  2
    .locals init (valuetype MainApp/Foo[] V_0)
    IL_0000:  ldc.i4.3
    IL_0001:  newarr     MainApp/Foo
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  callvirt   instance void [mscorlib]System.Array::Initialize()
    IL_000d:  ldloc.0
    IL_000e:  ldc.i4.1
    IL_000f:  ldelema    MainApp/Foo
    IL_0014:  ldfld      string MainApp/Foo::y
    IL_0019:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_001e:  ret
  } // end of method MainApp::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {

    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

} // end of class MainApp


// =============================================================



