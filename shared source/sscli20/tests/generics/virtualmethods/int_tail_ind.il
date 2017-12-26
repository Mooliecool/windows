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

// This tests using ldloc/ldvirtftn/calli of a generic virtual method via
// an interface.

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:1:3300:0
}
.assembly genvirt4
{
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module genvirt4.exe
// MVID: {a9deb857-7f1e-467d-84e7-36a37de76f46}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x06ee0000
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class interface public abstract auto ansi I
{
} // end of class I

.class public auto ansi beforefieldinit C1
       extends [mscorlib]System.Object
       implements I
{
} // end of class C1

.class public auto ansi beforefieldinit C2
       extends C1
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

.class interface public abstract auto ansi I
{
  .method public hidebysig newslot virtual abstract 
          instance void  meth<([mscorlib]System.Object) T>(!!0) cil managed
  {
  } // end of method I::meth

} // end of class I

.class public auto ansi beforefieldinit C1
       extends [mscorlib]System.Object
       implements I
{
  .method public hidebysig newslot virtual 
          instance void  meth<([mscorlib]System.Object) T>(!!0) cil managed
  {
    // Code size       65 (0x41)
    .maxstack  3
    .locals init (object[] V_0)
    IL_0000:  ldc.i4.5
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "C1<"
    IL_000e:  stelem.ref
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.1
    IL_0011:  ldtoken    !!0
    IL_0016:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_001b:  stelem.ref
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.2
    IL_001e:  ldstr      ","
    IL_0023:  stelem.ref
    IL_0024:  ldloc.0
    IL_0025:  ldc.i4.3
    IL_0026:  ldarg.1
    IL_0027:  box        [mscorlib]System.Int32
    IL_002c:  stelem.ref
    IL_002d:  ldloc.0
    IL_002e:  ldc.i4.4
    IL_002f:  ldstr      ">.meth"
    IL_0034:  stelem.ref
    IL_0035:  ldloc.0
    IL_0036:  call       string [mscorlib]System.String::Concat(object[])
    IL_003b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0040:  ret
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
       extends C1
{
  .method public hidebysig virtual instance void 
          meth<([mscorlib]System.Object) T>(!!0) cil managed
  {
    // Code size       65 (0x41)
    .maxstack  3
    .locals init (object[] V_0)
    IL_0000:  ldc.i4.5
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "C2<"
    IL_000e:  stelem.ref
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.1
    IL_0011:  ldtoken    !!0
    IL_0016:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_001b:  stelem.ref
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.2
    IL_001e:  ldstr      ","
    IL_0023:  stelem.ref
    IL_0024:  ldloc.0
    IL_0025:  ldc.i4.3
    IL_0026:  ldarg.1
    IL_0027:  box        !!0
    IL_002c:  stelem.ref
    IL_002d:  ldloc.0
    IL_002e:  ldc.i4.4
    IL_002f:  ldstr      ">.meth"
    IL_0034:  stelem.ref
    IL_0035:  ldloc.0
    IL_0036:  call       string [mscorlib]System.String::Concat(object[])
    IL_003b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0040:  ret
  } // end of method C2::meth

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void class C1::.ctor()
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
    .maxstack  3
    .locals init (class C1 V_0, class I V_1)
    IL_0000:  newobj     instance void class C2::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  ldstr "hello"
    IL_000c:  callvirt   instance void class C1::meth<string>(!!0)
    IL_0011:  ldloc.0
    IL_0012:  stloc.1
    IL_0013:  ldloc.1
    IL_0014:  ldc.i4.7
              ldloc.1
              ldvirtftn   instance void class I::meth<int32>(!!0)
              calli	void(class I,int32)
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
