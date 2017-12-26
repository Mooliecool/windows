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

//  This tests calling a generic virtual method via a delegate.

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:1:3300:0
}
.assembly genvirt8
{
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module genvirt8.exe
// MVID: {e89bb067-bc2c-4732-9e57-876bc8e5ddc9}
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

.class private auto ansi sealed MyDelegate<([mscorlib]System.Object) U>
       extends [mscorlib]System.MulticastDelegate
{
} // end of class MyDelegate

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
    // Code size       31 (0x1f)
    .maxstack  3
    IL_0000:  ldstr      "C1<"
    IL_0005:  ldtoken    !0
    IL_000a:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_000f:  ldstr      ">.meth"
    IL_0014:  call       string [mscorlib]System.String::Concat(object,
                                                                object,
                                                                object)
    IL_0019:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_001e:  ret
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

.class private auto ansi sealed MyDelegate<([mscorlib]System.Object) U>
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native int 'method') runtime managed
  {
  } // end of method MyDelegate::.ctor

  .method public hidebysig virtual instance void 
          Invoke(string arg) runtime managed
  {
  } // end of method MyDelegate::Invoke

  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(string arg,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed
  {
  } // end of method MyDelegate::BeginInvoke

  .method public hidebysig newslot virtual 
          instance void  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed
  {
  } // end of method MyDelegate::EndInvoke

} // end of class MyDelegate

.class public auto ansi beforefieldinit M
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       32 (0x20)
    .maxstack  3
    .locals init (class C2 V_0,
             class MyDelegate<string> V_1)
    IL_0000:  newobj     instance void C2::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  dup
    IL_0008:  ldvirtftn  instance void class C1<int32>::meth<string>(!!0)
    IL_000e:  newobj     instance void class MyDelegate<string>::.ctor(object, native int)
    IL_0013:  stloc.1
    IL_0014:  ldloc.1
    IL_0015:  ldstr      "hello"
    IL_001a:  callvirt   instance void class MyDelegate<string>::Invoke(string)
    IL_001f:  ret
  } // end of method M::Main

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

//*********** DISASSEMBLY COMPLETE ***********************
