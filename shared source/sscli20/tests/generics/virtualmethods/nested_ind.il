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

//  This code is based on the disassmebly of nested.exe,
//  with one callvirt instruction replaced by ldloc/ldvirtftn/calli.

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly inherit1
{
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module inherit1.exe
// MVID: {e2719197-0909-85c8-61a4-a5b1b89ec9a4}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x06ee0000
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi beforefieldinit C1<([mscorlib]System.Object) T>
       extends [mscorlib]System.Object
{
} // end of class C1

.class public auto ansi beforefieldinit C2
       extends class C1<int32>
{
} // end of class C2

.class public auto ansi beforefieldinit M
       extends [mscorlib]System.Object
{
} // end of class M


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi beforefieldinit C1<([mscorlib]System.Object) T>
       extends [mscorlib]System.Object
{
  .method public hidebysig newslot virtual 
          instance void  meth<([mscorlib]System.Object) U>(!!0 x) cil managed
  {
    // Code size       86 (0x56)
    .maxstack  3
    .locals init (object[] V_0)
    IL_0000:  ldc.i4.7
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "C1<"
    IL_000e:  stelem.ref
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.1
    IL_0011:  ldtoken    !0
    IL_0016:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_001b:  stelem.ref
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.2
    IL_001e:  ldstr      ">.meth<"
    IL_0023:  stelem.ref
    IL_0024:  ldloc.0
    IL_0025:  ldc.i4.3
    IL_0026:  ldtoken    !!0
    IL_002b:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_0030:  stelem.ref
    IL_0031:  ldloc.0
    IL_0032:  ldc.i4.4
    IL_0033:  ldstr      ","
    IL_0038:  stelem.ref
    IL_0039:  ldloc.0
    IL_003a:  ldc.i4.5
    IL_003b:  ldarg.1
    IL_003c:  box        !!0
    IL_0041:  stelem.ref
    IL_0042:  ldloc.0
    IL_0043:  ldc.i4.6
    IL_0044:  ldstr      ">"
    IL_0049:  stelem.ref
    IL_004a:  ldloc.0
    IL_004b:  call       string [mscorlib]System.String::Concat(object[])
    IL_0050:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0055:  ret
  } // end of method C1::meth

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method C1::.ctor

} // end of class C1

.class public auto ansi beforefieldinit C2
       extends class C1<int32>
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void class C1<int32>::.ctor()
    IL_0006:  ret
  } // end of method C2::.ctor

} // end of class C2

.class public auto ansi beforefieldinit M
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       23 (0x17)
    .maxstack  1
    .locals init (class C2 V_0)
    IL_0000:  ldstr      "hello"
    IL_0005:  call       void M::test<string>(!!0)
    IL_000a:  newobj     instance void C2::.ctor()
    IL_000f:  stloc.0
    IL_0010:  ldloc.0
    IL_0011:  call       void M::test<class C2>(!!0)
    IL_0016:  ret
  } // end of method M::Main

  .method public hidebysig static void  test<([mscorlib]System.Object) T>(!!0 x) cil managed
  {
    // Code size       14 (0xe)
    .maxstack  3
    .locals init (class C2 V_0)
      newobj     instance void C2::.ctor()
      stloc.0
      ldloc.0
      ldarg.0
      ldloc.0
      ldvirtftn   instance void class C1<int32>::meth<!!0>(!!0)
      calli	void(class C2, !!0)
      ret
  } // end of method M::test

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method M::.ctor

} // end of class M


// =============================================================

