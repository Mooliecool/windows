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

// Metadata version: v1.0.0
.assembly extern legacy library mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .hash = (56 B0 E5 B4 68 C3 B0 16 1A A9 4F 94 EE 2A 49 F3   // V...h.....O..*I.
           49 92 22 46 )                                     // I."F
  .ver 2:0:3500:0
}
.assembly legacy library constrained
{

  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 

  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module constrained.exe
// MVID: {cf2e7a52-4916-e552-db0f-004a564757fb}
.imagebase 0x00400000
.file alignment 0x00001000
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000001    //  ILONLY
// Image base: 0x07C70000


// =============== CLASS MEMBERS DECLARATION ===================

.class public auto ansi beforefieldinit ConstrainedTests
       extends [mscorlib]System.Object
{
  .field private static int32 s_failed
  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    // Code size       346 (0x15a)
    .maxstack  2
    .locals init (class Container<valuetype ValType> V_0,
             class Container<class RefType> V_1,
             class Container<valuetype ValTypeNewImpl> V_2,
             valuetype ValTypeNewImpl V_3,
             class [mscorlib]System.Exception V_4,
             int32 V_5)
    .try
    {
      IL_0000:  ldc.i4.s   23
      IL_0002:  newobj     instance void ValType::.ctor(int32)
      IL_0007:  newobj     instance void class Container<valuetype ValType>::.ctor(!0)
      IL_000c:  stloc.0
      IL_000d:  newobj     instance void RefType::.ctor()
      IL_0012:  newobj     instance void class Container<class RefType>::.ctor(!0)
      IL_0017:  stloc.1
      IL_0018:  ldc.i4.3
      IL_0019:  newobj     instance void ValTypeNewImpl::.ctor(int32)
      IL_001e:  newobj     instance void class Container<valuetype ValTypeNewImpl>::.ctor(!0)
      IL_0023:  stloc.2
      IL_0024:  ldloca.s   V_3
      IL_0026:  initobj    ValTypeNewImpl
      IL_002c:  ldloc.1
      IL_002d:  callvirt   instance string class Container<class RefType>::ToStr()
      IL_0032:  ldstr      "RefType"
      IL_0037:  call       bool [mscorlib]System.String::op_Equality(string,
                                                                     string)
      IL_003c:  ldstr      "Ref.ToStr"
      IL_0041:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_0046:  ldloc.0
      IL_0047:  callvirt   instance string class Container<valuetype ValType>::ToStr()
      IL_004c:  ldstr      "ValType"
      IL_0051:  call       bool [mscorlib]System.String::op_Equality(string,
                                                                     string)
      IL_0056:  ldstr      "Val.ToStr"
      IL_005b:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_0060:  ldloc.2
      IL_0061:  callvirt   instance string class Container<valuetype ValTypeNewImpl>::ToStr()
      IL_0066:  ldstr      "Foo"
      IL_006b:  call       bool [mscorlib]System.String::op_Equality(string,
                                                                     string)
      IL_0070:  ldstr      "ValNew.ToStr"
      IL_0075:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_007a:  ldloc.1
      IL_007b:  callvirt   instance int32 class Container<class RefType>::DoCall()
      IL_0080:  ldc.i4.2
      IL_0081:  ceq
      IL_0083:  ldstr      "Ref.DoCall"
      IL_0088:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_008d:  ldloc.0
      IL_008e:  callvirt   instance int32 class Container<valuetype ValType>::DoCall()
      IL_0093:  ldc.i4.s   23
      IL_0095:  ceq
      IL_0097:  ldstr      "Val.DoCall"
      IL_009c:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00a1:  ldloc.2
      IL_00a2:  callvirt   instance int32 class Container<valuetype ValTypeNewImpl>::DoCall()
      IL_00a7:  ldc.i4.3
      IL_00a8:  ceq
      IL_00aa:  ldstr      "ValNew.DoCall"
      IL_00af:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00b4:  ldloc.1
      IL_00b5:  callvirt   instance bool class Container<class RefType>::Eq()
      IL_00ba:  ldstr      "Ref.Eq"
      IL_00bf:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00c4:  ldloc.0
      IL_00c5:  callvirt   instance bool class Container<valuetype ValType>::Eq()
      IL_00ca:  ldstr      "Val.Eq"
      IL_00cf:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00d4:  ldloc.2
      IL_00d5:  callvirt   instance bool class Container<valuetype ValTypeNewImpl>::Eq()
      IL_00da:  ldc.i4.0
      IL_00db:  ceq
      IL_00dd:  ldstr      "ValNew.Eq"
      IL_00e2:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00e7:  ldloc.1
      IL_00e8:  ldc.i4.1
      IL_00e9:  callvirt   instance int32 class Container<class RefType>::BigArgs(int32)
      IL_00ee:  ldc.i4     0xff1
      IL_00f3:  ceq
      IL_00f5:  ldstr      "Ref.BigArgs"
      IL_00fa:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_00ff:  ldloc.0
      IL_0100:  ldc.i4.2
      IL_0101:  callvirt   instance int32 class Container<valuetype ValType>::BigArgs(int32)
      IL_0106:  ldc.i4     0xff2
      IL_010b:  ceq
      IL_010d:  ldstr      "Val.BigArgs"
      IL_0112:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_0117:  ldloc.2
      IL_0118:  ldc.i4.3
      IL_0119:  callvirt   instance int32 class Container<valuetype ValTypeNewImpl>::BigArgs(int32)
      IL_011e:  ldc.i4     0xff3
      IL_0123:  ceq
      IL_0125:  ldstr      "ValNew.BigArgs"
      IL_012a:  call       void ConstrainedTests::Test(bool,
                                                       string)
      IL_012f:  leave.s    IL_014e

    }  // end .try
    catch [mscorlib]System.Exception 
    {
      IL_0131:  stloc.s    V_4
      IL_0133:  ldstr      "Caught Unexpected Exception"
      IL_0138:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_013d:  ldloc.s    V_4
      IL_013f:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_0144:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0149:  ldc.i4.1
      IL_014a:  stloc.s    V_5
      IL_014c:  leave.s    IL_0157

    }  // end handler
    IL_014e:  ldsfld     int32 ConstrainedTests::s_failed
    IL_0153:  stloc.s    V_5
    IL_0155:  br.s       IL_0157

    IL_0157:  ldloc.s    V_5
    IL_0159:  ret
  } // end of method ConstrainedTests::Main

  .method public hidebysig static void  Test(bool test,
                                             string msg) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  brfalse.s  IL_0015

    IL_0003:  ldstr      "PASSED "
    IL_0008:  ldarg.1
    IL_0009:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_000e:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0013:  br.s       IL_002b

    IL_0015:  ldstr      "FAILED "
    IL_001a:  ldarg.1
    IL_001b:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_0020:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0025:  ldc.i4.1
    IL_0026:  stsfld     int32 ConstrainedTests::s_failed
    IL_002b:  ret
  } // end of method ConstrainedTests::Test

  .method private hidebysig specialname rtspecialname static 
          void  .cctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     int32 ConstrainedTests::s_failed
    IL_0006:  ret
  } // end of method ConstrainedTests::.cctor

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method ConstrainedTests::.ctor

} // end of class ConstrainedTests

.class public auto ansi beforefieldinit Container<(IFoo) T>
       extends [mscorlib]System.Object
{
  .field private valuetype IntHolder ih
  .field private !0 a
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(!0 _a) cil managed
  {
    // Code size       71 (0x47)
    .maxstack  2
    IL_0000:  ldarg.0
    IL_0001:  ldflda     valuetype IntHolder class Container<!0>::ih
    IL_0006:  initobj    IntHolder
    IL_000c:  ldarg.0
    IL_000d:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0012:  ldarg.0
    IL_0013:  ldarg.1
    IL_0014:  stfld      !0 class Container<!0>::a
    IL_0019:  ldarg.0
    IL_001a:  ldflda     valuetype IntHolder class Container<!0>::ih
    IL_001f:  ldc.i4.s   15
    IL_0021:  stfld      int32 IntHolder::x
    IL_0026:  ldarg.0
    IL_0027:  ldflda     valuetype IntHolder class Container<!0>::ih
    IL_002c:  ldc.i4     0xf0
    IL_0031:  stfld      int32 IntHolder::y
    IL_0036:  ldarg.0
    IL_0037:  ldflda     valuetype IntHolder class Container<!0>::ih
    IL_003c:  ldc.i4     0xf00
    IL_0041:  stfld      int32 IntHolder::z
    IL_0046:  ret
  } // end of method Container::.ctor

  .method public hidebysig instance int32 
          DoCall() cil managed
  {
    // Code size       21 (0x15)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldflda      !0 class Container<!0>::a
    constrained. !0
    IL_000b:  callvirt   instance int32 IFoo::func()
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_0013

    IL_0013:  ldloc.0
    IL_0014:  ret
  } // end of method Container::DoCall

  .method public hidebysig instance string 
          ToStr() cil managed
  {
    // Code size       21 (0x15)
    .maxstack  1
    .locals init (string V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldflda      !0 class Container<!0>::a
    constrained. !0
    IL_000b:  callvirt   instance string [mscorlib]System.Object::ToString()
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_0013

    IL_0013:  ldloc.0
    IL_0014:  ret
  } // end of method Container::ToStr

  .method public hidebysig instance bool 
          Eq() cil managed
  {
    // Code size       32 (0x20)
    .maxstack  2
    .locals init (bool V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldflda      !0 class Container<!0>::a
    IL_000b:  ldarg.0
    IL_000c:  ldfld      !0 class Container<!0>::a
    IL_0011:  box        !0
    constrained. !0
    IL_0016:  callvirt   instance bool [mscorlib]System.Object::Equals(object)
    IL_001b:  stloc.0
    IL_001c:  br.s       IL_001e

    IL_001e:  ldloc.0
    IL_001f:  ret
  } // end of method Container::Eq

   .method public hidebysig instance int32 
          BigArgs(int32 x) cil managed
  {
    // Code size       34 (0x22)
    .maxstack  4
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldflda      !0 class Container<!0>::a
    IL_000b:  ldarg.0
    IL_000c:  ldfld      valuetype IntHolder class Container<!0>::ih
    IL_0011:  ldarg.0
    IL_0012:  ldfld      valuetype IntHolder class Container<!0>::ih
    IL_0017:  ldarg.1
    constrained. !0
    IL_0018:  callvirt   instance int32 IFoo::big(valuetype IntHolder,
                                                  valuetype IntHolder,
                                                  int32)
    IL_001d:  stloc.0
    IL_001e:  br.s       IL_0020

    IL_0020:  ldloc.0
    IL_0021:  ret
  } // end of method Container::BigArgs

} // end of class Container

.class interface public abstract auto ansi IFoo
{
  .method public hidebysig newslot abstract virtual 
          instance int32  func() cil managed
  {
  } // end of method IFoo::func

  .method public hidebysig newslot abstract virtual 
          instance int32  big(valuetype IntHolder ih1,
                              valuetype IntHolder ih2,
                              int32 x) cil managed
  {
  } // end of method IFoo::big

} // end of class IFoo

.class public sequential ansi sealed beforefieldinit ValType
       extends [mscorlib]System.ValueType
       implements IFoo
{
  .field public int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 x_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValType::x
    IL_0007:  ret
  } // end of method ValType::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  func() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 ValType::x
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method ValType::func

  .method public hidebysig instance int32 
          normal() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 ValType::x
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method ValType::normal

  .method public hidebysig newslot virtual final 
          instance int32  big(valuetype IntHolder ih1,
                              valuetype IntHolder ih2,
                              int32 x) cil managed
  {
    // Code size       22 (0x16)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarga.s   ih1
    IL_0002:  ldfld      int32 IntHolder::y
    IL_0007:  ldarga.s   ih2
    IL_0009:  ldfld      int32 IntHolder::z
    IL_000e:  add
    IL_000f:  ldarg.3
    IL_0010:  add
    IL_0011:  stloc.0
    IL_0012:  br.s       IL_0014

    IL_0014:  ldloc.0
    IL_0015:  ret
  } // end of method ValType::big

} // end of class ValType

.class public auto ansi beforefieldinit RefType
       extends [mscorlib]System.Object
       implements IFoo
{
  .field public int32 x
  .method public hidebysig newslot virtual final 
          instance int32  func() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 RefType::x
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method RefType::func

  .method public hidebysig newslot virtual final 
          instance int32  big(valuetype IntHolder ih1,
                              valuetype IntHolder ih2,
                              int32 x) cil managed
  {
    // Code size       22 (0x16)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarga.s   ih1
    IL_0002:  ldfld      int32 IntHolder::y
    IL_0007:  ldarga.s   ih2
    IL_0009:  ldfld      int32 IntHolder::z
    IL_000e:  add
    IL_000f:  ldarg.3
    IL_0010:  add
    IL_0011:  stloc.0
    IL_0012:  br.s       IL_0014

    IL_0014:  ldloc.0
    IL_0015:  ret
  } // end of method RefType::big

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.2
    IL_0002:  stfld      int32 RefType::x
    IL_0007:  ldarg.0
    IL_0008:  call       instance void [mscorlib]System.Object::.ctor()
    IL_000d:  ret
  } // end of method RefType::.ctor

} // end of class RefType

.class public sequential ansi sealed beforefieldinit ValTypeNewImpl
       extends [mscorlib]System.ValueType
       implements IFoo
{
  .field public int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 x_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValTypeNewImpl::x
    IL_0007:  ret
  } // end of method ValTypeNewImpl::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  func() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 ValTypeNewImpl::x
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method ValTypeNewImpl::func

  .method public hidebysig virtual instance string 
          ToString() cil managed
  {
    // Code size       10 (0xa)
    .maxstack  1
    .locals init (string V_0)
    IL_0000:  ldstr      "Foo"
    IL_0005:  stloc.0
    IL_0006:  br.s       IL_0008

    IL_0008:  ldloc.0
    IL_0009:  ret
  } // end of method ValTypeNewImpl::ToString

  .method public hidebysig virtual instance bool 
          Equals(object obj) cil managed
  {
    // Code size       6 (0x6)
    .maxstack  1
    .locals init (bool V_0)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0004

    IL_0004:  ldloc.0
    IL_0005:  ret
  } // end of method ValTypeNewImpl::Equals

  .method public hidebysig newslot virtual final 
          instance int32  big(valuetype IntHolder ih1,
                              valuetype IntHolder ih2,
                              int32 x) cil managed
  {
    // Code size       22 (0x16)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarga.s   ih1
    IL_0002:  ldfld      int32 IntHolder::y
    IL_0007:  ldarga.s   ih2
    IL_0009:  ldfld      int32 IntHolder::z
    IL_000e:  add
    IL_000f:  ldarg.3
    IL_0010:  add
    IL_0011:  stloc.0
    IL_0012:  br.s       IL_0014

    IL_0014:  ldloc.0
    IL_0015:  ret
  } // end of method ValTypeNewImpl::big

} // end of class ValTypeNewImpl

.class public sequential ansi sealed beforefieldinit IntHolder
       extends [mscorlib]System.ValueType
{
  .field public int32 x
  .field public int32 y
  .field public int32 z
} // end of class IntHolder


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
