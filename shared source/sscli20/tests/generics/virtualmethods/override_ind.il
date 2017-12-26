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

//  This code is based on the disassmebly of override.cs,
//  with one callvirt instruction replaced by ldloc/ldvirtftn/calli.

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:1:3300:0
}
.assembly genvirt2
{
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module genvirt2.exe
// MVID: {04f14a2c-0054-4540-9dd1-17f69ceada31}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x06ee0000
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi beforefieldinit C1
       extends [mscorlib]System.Object
{
} // end of class C1

.class public auto ansi beforefieldinit C2
       extends C1
{
} // end of class C2

.class private auto ansi beforefieldinit M
       extends [mscorlib]System.Object
{
} // end of class M


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi beforefieldinit C1
       extends [mscorlib]System.Object
{
  .method public hidebysig newslot virtual 
          instance void  meth1<([mscorlib]System.Object) T>(int32 i) cil managed
  {
    // Code size       65 (0x41)
    .maxstack  3
    .locals init (object[] V_0)
    IL_0000:  ldc.i4.5
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "C1::meth1<"
    IL_000e:  stelem.ref
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.1
    IL_0011:  ldtoken    !!0
    IL_0016:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_001b:  stelem.ref
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.2
    IL_001e:  ldstr      ">("
    IL_0023:  stelem.ref
    IL_0024:  ldloc.0
    IL_0025:  ldc.i4.3
    IL_0026:  ldarg.1
    IL_0027:  box        [mscorlib]System.Int32
    IL_002c:  stelem.ref
    IL_002d:  ldloc.0
    IL_002e:  ldc.i4.4
    IL_002f:  ldstr      ")"
    IL_0034:  stelem.ref
    IL_0035:  ldloc.0
    IL_0036:  call       string [mscorlib]System.String::Concat(object[])
    IL_003b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0040:  ret
  } // end of method C1::meth1

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
       extends C1
{
  .method public hidebysig virtual instance void 
          meth1<([mscorlib]System.Object) T>(int32 i) cil managed
  {
    // Code size       65 (0x41)
    .maxstack  3
    .locals init (object[] V_0)
    IL_0000:  ldc.i4.5
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "C2::meth1<"
    IL_000e:  stelem.ref
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.1
    IL_0011:  ldtoken    !!0
    IL_0016:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_001b:  stelem.ref
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.2
    IL_001e:  ldstr      ">("
    IL_0023:  stelem.ref
    IL_0024:  ldloc.0
    IL_0025:  ldc.i4.3
    IL_0026:  ldarg.1
    IL_0027:  box        [mscorlib]System.Int32
    IL_002c:  stelem.ref
    IL_002d:  ldloc.0
    IL_002e:  ldc.i4.4
    IL_002f:  ldstr      ")"
    IL_0034:  stelem.ref
    IL_0035:  ldloc.0
    IL_0036:  call       string [mscorlib]System.String::Concat(object[])
    IL_003b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0040:  ret
  } // end of method C2::meth1

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void C1::.ctor()
    IL_0006:  ret
  } // end of method C2::.ctor

} // end of class C2

.class private auto ansi beforefieldinit M
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       73 (0x49)
    .maxstack  3
    .locals init (class C1 V_0,
             class C2 V_1)
    newobj     instance void C1::.ctor()
    stloc.0
    newobj     instance void C2::.ctor()
    stloc.1
    ldloc.0
    ldc.i4.s   42
    callvirt   instance void C1::meth1<string>(int32)

    ldloc.1
    ldc.i4.s   49
    ldloc.1
    ldvirtftn  instance void C1::meth1<string>(int32)
    calli      void(class C2, int32)

    ldloc.0
    ldc.i4.s   42
    callvirt   instance void C1::meth1<int32>(int32)
    ldloc.1
    ldc.i4.s   49
    callvirt   instance void C1::meth1<int32>(int32)
    ldloc.1
    stloc.0
    ldloc.0
    ldc.i4.s   53
    callvirt   instance void C1::meth1<string>(int32)
    ldloc.0
    ldc.i4.s   53
    callvirt   instance void C1::meth1<int32>(int32)
    ldstr      "Finished!"
    call       void [mscorlib]System.Console::WriteLine(string)
    ret
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
