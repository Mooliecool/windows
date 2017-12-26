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
  .hash = (4A 8E 79 E6 84 4B 62 BE 40 D9 2D 95 A0 BB 5D 09   // J.y..Kb.@.-...].
           39 AF 78 22 )                                     // 9.x"
  .ver 2:0:3500:0
}
.assembly legacy library readonly_fromcsilinline
{
  .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilationRelaxationsAttribute::.ctor(int32) = ( 01 00 08 00 00 00 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module readonly_fromcsilinline.exe
// MVID: {ab6c6ed6-808e-39d5-029d-5ea83e675381}
.imagebase 0x00400000
.file alignment 0x00001000
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000001    //  ILONLY
// Image base: 0x07CC0000


// =============== CLASS MEMBERS DECLARATION ===================

.class public auto ansi beforefieldinit ILASM
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Inline(string cmd) cil managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method ILASM::Inline

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method ILASM::.ctor

} // end of class ILASM

.class interface public abstract auto ansi IPoint
{
  .method public hidebysig newslot abstract virtual 
          instance void  setx(int32 x_) cil managed
  {
  } // end of method IPoint::setx

  .method public hidebysig newslot abstract virtual 
          instance void  sety(int32 y_) cil managed
  {
  } // end of method IPoint::sety

  .method public hidebysig newslot abstract virtual 
          instance void  setz(int32 z_) cil managed
  {
  } // end of method IPoint::setz

  .method public hidebysig newslot abstract virtual 
          instance int32  getx() cil managed
  {
  } // end of method IPoint::getx

  .method public hidebysig newslot abstract virtual 
          instance int32  gety() cil managed
  {
  } // end of method IPoint::gety

  .method public hidebysig newslot abstract virtual 
          instance int32  getz() cil managed
  {
  } // end of method IPoint::getz

} // end of class IPoint

.class public sequential ansi sealed beforefieldinit ValType
       extends [mscorlib]System.ValueType
       implements IPoint
{
  .field public int32 x
  .field public int32 y
  .field public int32 z
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 x_,
                               int32 y_,
                               int32 z_) cil managed
  {
    // Code size       22 (0x16)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValType::x
    IL_0007:  ldarg.0
    IL_0008:  ldarg.2
    IL_0009:  stfld      int32 ValType::y
    IL_000e:  ldarg.0
    IL_000f:  ldarg.3
    IL_0010:  stfld      int32 ValType::z
    IL_0015:  ret
  } // end of method ValType::.ctor

  .method public hidebysig newslot virtual final 
          instance void  setx(int32 x_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValType::x
    IL_0007:  ret
  } // end of method ValType::setx

  .method public hidebysig newslot virtual final 
          instance void  sety(int32 y_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValType::y
    IL_0007:  ret
  } // end of method ValType::sety

  .method public hidebysig newslot virtual final 
          instance void  setz(int32 z_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 ValType::z
    IL_0007:  ret
  } // end of method ValType::setz

  .method public hidebysig newslot virtual final 
          instance int32  getx() cil managed
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
  } // end of method ValType::getx

  .method public hidebysig newslot virtual final 
          instance int32  gety() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 ValType::y
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method ValType::gety

  .method public hidebysig newslot virtual final 
          instance int32  getz() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 ValType::z
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method ValType::getz

} // end of class ValType

.class public auto ansi beforefieldinit RefType
       extends [mscorlib]System.Object
       implements IPoint
{
  .field public int32 x
  .field public int32 y
  .field public int32 z
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 x_,
                               int32 y_,
                               int32 z_) cil managed
  {
    // Code size       28 (0x1c)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 RefType::x
    IL_000d:  ldarg.0
    IL_000e:  ldarg.2
    IL_000f:  stfld      int32 RefType::y
    IL_0014:  ldarg.0
    IL_0015:  ldarg.3
    IL_0016:  stfld      int32 RefType::z
    IL_001b:  ret
  } // end of method RefType::.ctor

  .method public hidebysig newslot virtual final 
          instance void  setx(int32 x_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 RefType::x
    IL_0007:  ret
  } // end of method RefType::setx

  .method public hidebysig newslot virtual final 
          instance void  sety(int32 y_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 RefType::y
    IL_0007:  ret
  } // end of method RefType::sety

  .method public hidebysig newslot virtual final 
          instance void  setz(int32 z_) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 RefType::z
    IL_0007:  ret
  } // end of method RefType::setz

  .method public hidebysig newslot virtual final 
          instance int32  getx() cil managed
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
  } // end of method RefType::getx

  .method public hidebysig newslot virtual final 
          instance int32  gety() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 RefType::y
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method RefType::gety

  .method public hidebysig newslot virtual final 
          instance int32  getz() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 RefType::z
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method RefType::getz

} // end of class RefType

.class public auto ansi beforefieldinit FixedStack<(IPoint) T>
       extends [mscorlib]System.Object
{
  .field public !0[] m_data
  .field public int32 m_top
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 size) cil managed
  {
    // Code size       26 (0x1a)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  newarr     !0
    IL_000d:  stfld      !0[] class FixedStack<!0>::m_data
    IL_0012:  ldarg.0
    IL_0013:  ldc.i4.0
    IL_0014:  stfld      int32 class FixedStack<!0>::m_top
    IL_0019:  ret
  } // end of method FixedStack::.ctor

  .method public hidebysig instance void 
          Push(!0 datum) cil managed
  {
    // Code size       30 (0x1e)
    .maxstack  4
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldarg.0
    IL_0007:  dup
    IL_0008:  ldfld      int32 class FixedStack<!0>::m_top
    IL_000d:  dup
    IL_000e:  stloc.0
    IL_000f:  ldc.i4.1
    IL_0010:  add
    IL_0011:  stfld      int32 class FixedStack<!0>::m_top
    IL_0016:  ldloc.0
    IL_0017:  ldarg.1
    IL_0018:  stelem     !0
    IL_001d:  ret
  } // end of method FixedStack::Push

  .method public hidebysig instance void 
          stobjTest(!0 obj) cil managed
  {
    // Code size       38 (0x26)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  ldarg.1
    IL_0004:  call       instance void class FixedStack<!0>::stobjVerifyPass(!0)
    .try
    {
      .try
      {
        IL_0009:  ldarg.0
        IL_000a:  ldarg.1
        IL_000b:  call       instance void class FixedStack<!0>::stobjVerifyFail(!0)
        IL_0010:  leave.s    IL_0017

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0012:  stloc.1
        IL_0013:  ldc.i4.1
        IL_0014:  stloc.0
        IL_0015:  leave.s    IL_0017

      }  // end handler
      IL_0017:  leave.s    IL_0025

    }  // end .try
    finally
    {
      IL_0019:  ldloc.0
      IL_001a:  ldstr      "stobjTest"
      IL_001f:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0024:  endfinally
    }  // end handler
    IL_0025:  ret
  } // end of method FixedStack::stobjTest

  .method private hidebysig instance void 
          stobjVerifyPass(!0 obj) cil managed
  {
    // Code size       244 (0xf4)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.0
    IL_0007:  ldelem     !0
    IL_000c:  box        !0
    IL_0011:  callvirt   instance int32 IPoint::getx()
    IL_0016:  ldc.i4.s   100
    IL_0018:  ceq
    IL_001a:  ldstr      "stobj1 x"
    IL_001f:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0024:  ldarg.0
    IL_0025:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_002a:  ldc.i4.0
    IL_002b:  ldelem     !0
    IL_0030:  box        !0
    IL_0035:  callvirt   instance int32 IPoint::gety()
    IL_003a:  ldc.i4.s   101
    IL_003c:  ceq
    IL_003e:  ldstr      "stobj1 y"
    IL_0043:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0048:  ldarg.0
    IL_0049:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_004e:  ldc.i4.0
    IL_004f:  ldelem     !0
    IL_0054:  box        !0
    IL_0059:  callvirt   instance int32 IPoint::getz()
    IL_005e:  ldc.i4.s   102
    IL_0060:  ceq
    IL_0062:  ldstr      "stobj1 z"
    IL_0067:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_006c:  ldarg.0
    IL_006d:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0072:  ldc.i4.0
    IL_0073:  ldelema    !0
    IL_0078:  ldarg.1
    IL_0079:  stobj      !0
    IL_007e:  ldarg.0
    IL_007f:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0084:  ldc.i4.0
    IL_0085:  ldelem     !0
    IL_008a:  box        !0
    IL_008f:  callvirt   instance int32 IPoint::getx()
    IL_0094:  ldc.i4     0x159
    IL_0099:  ceq
    IL_009b:  ldstr      "stobj2 x"
    IL_00a0:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00a5:  ldarg.0
    IL_00a6:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00ab:  ldc.i4.0
    IL_00ac:  ldelem     !0
    IL_00b1:  box        !0
    IL_00b6:  callvirt   instance int32 IPoint::gety()
    IL_00bb:  ldc.i4     0x15a
    IL_00c0:  ceq
    IL_00c2:  ldstr      "stobj2 y"
    IL_00c7:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00cc:  ldarg.0
    IL_00cd:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00d2:  ldc.i4.0
    IL_00d3:  ldelem     !0
    IL_00d8:  box        !0
    IL_00dd:  callvirt   instance int32 IPoint::getz()
    IL_00e2:  ldc.i4     0x15b
    IL_00e7:  ceq
    IL_00e9:  ldstr      "stobj2 z"
    IL_00ee:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00f3:  ret
  } // end of method FixedStack::stobjVerifyPass

  .method private hidebysig instance void 
          stobjVerifyFail(!0 obj) cil managed
  {
    // Code size       21 (0x15)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.0
    IL_0007:  readonly.
    IL_0009:  ldelema    !0
    IL_000e:  ldarg.1
    IL_000f:  stobj      !0
    IL_0014:  ret
  } // end of method FixedStack::stobjVerifyFail

  .method public hidebysig instance void 
          initobjTest() cil managed
  {
    // Code size       38 (0x26)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  call       instance void class FixedStack<!0>::initobjVerifyPass()
    .try
    {
      .try
      {
        IL_0008:  ldarg.0
        IL_0009:  call       instance void class FixedStack<!0>::initobjVerifyFail()
        IL_000e:  leave.s    IL_0017

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0010:  stloc.1
        IL_0011:  ldnull
        IL_0012:  stloc.1
        IL_0013:  ldc.i4.1
        IL_0014:  stloc.0
        IL_0015:  leave.s    IL_0017

      }  // end handler
      IL_0017:  leave.s    IL_0025

    }  // end .try
    finally
    {
      IL_0019:  ldloc.0
      IL_001a:  ldstr      "initobjTest"
      IL_001f:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0024:  endfinally
    }  // end handler
    IL_0025:  ret
  } // end of method FixedStack::initobjTest

  .method private hidebysig instance void 
          initobjVerifyPass() cil managed
  {
    // Code size       232 (0xe8)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.1
    IL_0007:  ldelem     !0
    IL_000c:  box        !0
    IL_0011:  callvirt   instance int32 IPoint::getx()
    IL_0016:  ldc.i4.s   101
    IL_0018:  ceq
    IL_001a:  ldstr      "initobj1 x"
    IL_001f:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0024:  ldarg.0
    IL_0025:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_002a:  ldc.i4.1
    IL_002b:  ldelem     !0
    IL_0030:  box        !0
    IL_0035:  callvirt   instance int32 IPoint::gety()
    IL_003a:  ldc.i4.s   102
    IL_003c:  ceq
    IL_003e:  ldstr      "initobj1 y"
    IL_0043:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0048:  ldarg.0
    IL_0049:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_004e:  ldc.i4.1
    IL_004f:  ldelem     !0
    IL_0054:  box        !0
    IL_0059:  callvirt   instance int32 IPoint::getz()
    IL_005e:  ldc.i4.s   103
    IL_0060:  ceq
    IL_0062:  ldstr      "initobj1 z"
    IL_0067:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_006c:  ldarg.0
    IL_006d:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0072:  ldc.i4.1
    IL_0073:  ldelema    !0
    IL_0078:  initobj    !0
    IL_007e:  ldarg.0
    IL_007f:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0084:  ldc.i4.1
    IL_0085:  ldelem     !0
    IL_008a:  box        !0
    IL_008f:  callvirt   instance int32 IPoint::getx()
    IL_0094:  ldc.i4.0
    IL_0095:  ceq
    IL_0097:  ldstr      "initobj2 x"
    IL_009c:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00a1:  ldarg.0
    IL_00a2:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00a7:  ldc.i4.1
    IL_00a8:  ldelem     !0
    IL_00ad:  box        !0
    IL_00b2:  callvirt   instance int32 IPoint::gety()
    IL_00b7:  ldc.i4.0
    IL_00b8:  ceq
    IL_00ba:  ldstr      "initobj2 y"
    IL_00bf:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00c4:  ldarg.0
    IL_00c5:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00ca:  ldc.i4.1
    IL_00cb:  ldelem     !0
    IL_00d0:  box        !0
    IL_00d5:  callvirt   instance int32 IPoint::getz()
    IL_00da:  ldc.i4.0
    IL_00db:  ceq
    IL_00dd:  ldstr      "initobj2 z"
    IL_00e2:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00e7:  ret
  } // end of method FixedStack::initobjVerifyPass

  .method private hidebysig instance void 
          initobjVerifyFail() cil managed
  {
    // Code size       21 (0x15)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.1
    IL_0007:  readonly.
    IL_0009:  ldelema    !0
    IL_000e:  initobj    !0
    IL_0014:  ret
  } // end of method FixedStack::initobjVerifyFail

  .method public hidebysig instance void 
          mkrefanyTest() cil managed
  {
    // Code size       38 (0x26)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  call       instance void class FixedStack<!0>::mkrefanyVerifyPass()
    .try
    {
      .try
      {
        IL_0008:  ldarg.0
        IL_0009:  call       instance void class FixedStack<!0>::mkrefanyVerifyFail()
        IL_000e:  leave.s    IL_0017

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0010:  stloc.1
        IL_0011:  ldnull
        IL_0012:  stloc.1
        IL_0013:  ldc.i4.1
        IL_0014:  stloc.0
        IL_0015:  leave.s    IL_0017

      }  // end handler
      IL_0017:  leave.s    IL_0025

    }  // end .try
    finally
    {
      IL_0019:  ldloc.0
      IL_001a:  ldstr      "mkrefanyTest"
      IL_001f:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0024:  endfinally
    }  // end handler
    IL_0025:  ret
  } // end of method FixedStack::mkrefanyTest

  .method private hidebysig instance void 
          mkrefanyVerifyPass() cil managed
  {
    // Code size       19 (0x13)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.2
    IL_0007:  ldelema    !0
    IL_000c:  mkrefany   !0
    IL_0011:  pop
    IL_0012:  ret
  } // end of method FixedStack::mkrefanyVerifyPass

  .method private hidebysig instance void 
          mkrefanyVerifyFail() cil managed
  {
    // Code size       21 (0x15)
    .maxstack  10
    IL_0000:  ldarg.0
    IL_0001:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0006:  ldc.i4.2
    IL_0007:  readonly.
    IL_0009:  ldelema    !0
    IL_000e:  mkrefany   !0
    IL_0013:  pop
    IL_0014:  ret
  } // end of method FixedStack::mkrefanyVerifyFail

  .method public hidebysig instance void 
          cpobjTest(!0 obj) cil managed
  {
    // Code size       40 (0x28)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  ldarg.1
    IL_0004:  call       instance void class FixedStack<!0>::cpobjVerifyPass(!0)
    .try
    {
      .try
      {
        IL_0009:  ldarg.0
        IL_000a:  ldarg.1
        IL_000b:  call       instance void class FixedStack<!0>::cpobjVerifyFail(!0)
        IL_0010:  leave.s    IL_0019

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0012:  stloc.1
        IL_0013:  ldnull
        IL_0014:  stloc.1
        IL_0015:  ldc.i4.1
        IL_0016:  stloc.0
        IL_0017:  leave.s    IL_0019

      }  // end handler
      IL_0019:  leave.s    IL_0027

    }  // end .try
    finally
    {
      IL_001b:  ldloc.0
      IL_001c:  ldstr      "cpobjTest"
      IL_0021:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0026:  endfinally
    }  // end handler
    IL_0027:  ret
  } // end of method FixedStack::cpobjTest

  .method private hidebysig instance void 
          cpobjVerifyPass(!0 obj) cil managed
  {
    // Code size       379 (0x17b)
    .maxstack  8
    .locals init (!0 V_0)
    IL_0000:  ldarg.1
    IL_0001:  stloc.0
    IL_0002:  ldarg.1
    IL_0003:  stloc.0
    IL_0004:  ldloc.0
    IL_0005:  box        !0
    IL_000a:  ldc.i4.1
    IL_000b:  callvirt   instance void IPoint::setx(int32)
    IL_0010:  ldloc.0
    IL_0011:  box        !0
    IL_0016:  ldc.i4.1
    IL_0017:  callvirt   instance void IPoint::sety(int32)
    IL_001c:  ldloc.0
    IL_001d:  box        !0
    IL_0022:  ldc.i4.1
    IL_0023:  callvirt   instance void IPoint::setz(int32)
    IL_0028:  ldloca     V_0
    IL_002c:  ldarg.0
    IL_002d:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0032:  ldc.i4.3
    IL_0033:  readonly.
    IL_0035:  ldelema    !0
    IL_003a:  cpobj      !0
    IL_003f:  ldloc.0
    IL_0040:  box        !0
    IL_0045:  callvirt   instance int32 IPoint::getx()
    IL_004a:  ldc.i4.s   103
    IL_004c:  ceq
    IL_004e:  ldstr      "cpobj1 x"
    IL_0053:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0058:  ldloc.0
    IL_0059:  box        !0
    IL_005e:  callvirt   instance int32 IPoint::gety()
    IL_0063:  ldc.i4.s   104
    IL_0065:  ceq
    IL_0067:  ldstr      "cpobj1 y"
    IL_006c:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0071:  ldloc.0
    IL_0072:  box        !0
    IL_0077:  callvirt   instance int32 IPoint::getz()
    IL_007c:  ldc.i4.s   105
    IL_007e:  ceq
    IL_0080:  ldstr      "cpobj1 z"
    IL_0085:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_008a:  ldloc.0
    IL_008b:  box        !0
    IL_0090:  ldc.i4.1
    IL_0091:  callvirt   instance void IPoint::setx(int32)
    IL_0096:  ldloc.0
    IL_0097:  box        !0
    IL_009c:  ldc.i4.1
    IL_009d:  callvirt   instance void IPoint::sety(int32)
    IL_00a2:  ldloc.0
    IL_00a3:  box        !0
    IL_00a8:  ldc.i4.1
    IL_00a9:  callvirt   instance void IPoint::setz(int32)
    IL_00ae:  ldarg.0
    IL_00af:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00b4:  ldc.i4.3
    IL_00b5:  ldelem     !0
    IL_00ba:  box        !0
    IL_00bf:  callvirt   instance int32 IPoint::getx()
    IL_00c4:  ldc.i4.s   103
    IL_00c6:  ceq
    IL_00c8:  ldstr      "cpobj2 x"
    IL_00cd:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00d2:  ldarg.0
    IL_00d3:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00d8:  ldc.i4.3
    IL_00d9:  ldelem     !0
    IL_00de:  box        !0
    IL_00e3:  callvirt   instance int32 IPoint::gety()
    IL_00e8:  ldc.i4.s   104
    IL_00ea:  ceq
    IL_00ec:  ldstr      "cpobj2 y"
    IL_00f1:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_00f6:  ldarg.0
    IL_00f7:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_00fc:  ldc.i4.3
    IL_00fd:  ldelem     !0
    IL_0102:  box        !0
    IL_0107:  callvirt   instance int32 IPoint::getz()
    IL_010c:  ldc.i4.s   105
    IL_010e:  ceq
    IL_0110:  ldstr      "cpobj2 z"
    IL_0115:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_011a:  ldarg.0
    IL_011b:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0120:  ldc.i4.3
    IL_0121:  ldelema    !0
    IL_0126:  ldloca     V_0
    IL_012a:  cpobj      !0
    IL_012f:  ldloc.0
    IL_0130:  box        !0
    IL_0135:  callvirt   instance int32 IPoint::getx()
    IL_013a:  ldc.i4.s   103
    IL_013c:  ceq
    IL_013e:  ldstr      "cpobj3 x"
    IL_0143:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0148:  ldloc.0
    IL_0149:  box        !0
    IL_014e:  callvirt   instance int32 IPoint::gety()
    IL_0153:  ldc.i4.s   104
    IL_0155:  ceq
    IL_0157:  ldstr      "cpobj3 y"
    IL_015c:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0161:  ldloc.0
    IL_0162:  box        !0
    IL_0167:  callvirt   instance int32 IPoint::getz()
    IL_016c:  ldc.i4.s   105
    IL_016e:  ceq
    IL_0170:  ldstr      "cpobj3 z"
    IL_0175:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_017a:  ret
  } // end of method FixedStack::cpobjVerifyPass

  .method private hidebysig instance void 
          cpobjVerifyFail(!0 obj) cil managed
  {
    // Code size       26 (0x1a)
    .maxstack  8
    .locals init (!0 V_0)
    IL_0000:  ldarg.1
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0008:  ldc.i4.3
    IL_0009:  readonly.
    IL_000b:  ldelema    !0
    IL_0010:  ldloca     V_0
    IL_0014:  cpobj      !0
    IL_0019:  ret
  } // end of method FixedStack::cpobjVerifyFail

  .method public hidebysig instance void 
          constrainedTest() cil managed
  {
    // Code size       35 (0x23)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    .try
    {
      .try
      {
        IL_0002:  ldarg.0
        IL_0003:  call       instance void class FixedStack<!0>::constrainedVerifyPass()
        IL_0008:  leave.s    IL_0011

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000a:  stloc.1
        IL_000b:  ldnull
        IL_000c:  stloc.1
        IL_000d:  ldc.i4.1
        IL_000e:  stloc.0
        IL_000f:  leave.s    IL_0011

      }  // end handler
      IL_0011:  leave.s    IL_0022

    }  // end .try
    finally
    {
      IL_0013:  ldloc.0
      IL_0014:  ldc.i4.0
      IL_0015:  ceq
      IL_0017:  ldstr      "constrainedTest"
      IL_001c:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0021:  endfinally
    }  // end handler
    IL_0022:  ret
  } // end of method FixedStack::constrainedTest

  .method private hidebysig instance void 
          constrainedVerifyPass() cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  8
    .locals init (int32 V_0)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  ldarg.0
    IL_0003:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0008:  ldc.i4.4
    IL_0009:  readonly.
    IL_000b:  ldelema    !0
    IL_0010:  constrained. !0
    IL_0016:  callvirt   instance int32 IPoint::getx()
    IL_001b:  stloc.0
    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.s   104
    IL_001f:  ceq
    IL_0021:  ldstr      "constrainedVerifyPass"
    IL_0026:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_002b:  ret
  } // end of method FixedStack::constrainedVerifyPass

  .method public hidebysig instance void 
          basicBlockTest() cil managed
  {
    // Code size       69 (0x45)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1,
             class [mscorlib]System.Security.VerificationException V_2)
    IL_0000:  ldarg.0
    IL_0001:  call       instance void class FixedStack<!0>::basicBlockPass()
    IL_0006:  ldc.i4.0
    IL_0007:  stloc.0
    .try
    {
      .try
      {
        IL_0008:  ldarg.0
        IL_0009:  call       instance void class FixedStack<!0>::basicBlockFail1()
        IL_000e:  leave.s    IL_0017

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0010:  stloc.1
        IL_0011:  ldnull
        IL_0012:  stloc.1
        IL_0013:  ldc.i4.1
        IL_0014:  stloc.0
        IL_0015:  leave.s    IL_0017

      }  // end handler
      IL_0017:  leave.s    IL_0025

    }  // end .try
    finally
    {
      IL_0019:  ldloc.0
      IL_001a:  ldstr      "basicBlockFail1"
      IL_001f:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0024:  endfinally
    }  // end handler
    IL_0025:  ldc.i4.0
    IL_0026:  stloc.0
    .try
    {
      .try
      {
        IL_0027:  ldarg.0
        IL_0028:  call       instance void class FixedStack<!0>::basicBlockFail2()
        IL_002d:  leave.s    IL_0036

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_002f:  stloc.2
        IL_0030:  ldnull
        IL_0031:  stloc.2
        IL_0032:  ldc.i4.1
        IL_0033:  stloc.0
        IL_0034:  leave.s    IL_0036

      }  // end handler
      IL_0036:  leave.s    IL_0044

    }  // end .try
    finally
    {
      IL_0038:  ldloc.0
      IL_0039:  ldstr      "basicBlockFail2"
      IL_003e:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0043:  endfinally
    }  // end handler
    IL_0044:  ret
  } // end of method FixedStack::basicBlockTest

  .method private hidebysig instance void 
          basicBlockFail1() cil managed
  {
    // Code size       40 (0x28)
    .maxstack  8
    .locals init (bool V_0)
    IL_0000:  ldc.i4.1
    IL_0001:  stloc.0
    IL_0002:  ldloc.0
    IL_0003:  brtrue.s   IL_0013

    IL_0005:  ldarg.0
    IL_0006:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_000b:  ldc.i4.5
    IL_000c:  ldelema    !0
    IL_0011:  br.s       IL_0021

    IL_0013:  ldarg.0
    IL_0014:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0019:  ldc.i4.6
    IL_001a:  readonly.
    IL_001c:  ldelema    !0
    IL_0021:  initobj    !0
    IL_0027:  ret
  } // end of method FixedStack::basicBlockFail1

  .method private hidebysig instance void 
          basicBlockFail2() cil managed
  {
    // Code size       40 (0x28)
    .maxstack  8
    .locals init (bool V_0)
    IL_0000:  ldc.i4.1
    IL_0001:  stloc.0
    IL_0002:  ldloc.0
    IL_0003:  brfalse.s  IL_0015

    IL_0005:  ldarg.0
    IL_0006:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_000b:  ldc.i4.6
    IL_000c:  readonly.
    IL_000e:  ldelema    !0
    IL_0013:  br.s       IL_0021

    IL_0015:  ldarg.0
    IL_0016:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_001b:  ldc.i4.5
    IL_001c:  ldelema    !0
    IL_0021:  initobj    !0
    IL_0027:  ret
  } // end of method FixedStack::basicBlockFail2

  .method private hidebysig instance void 
          basicBlockPass() cil managed
  {
    // Code size       123 (0x7b)
    .maxstack  8
    .locals init (bool V_0,
             int32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  stloc.0
    IL_0002:  ldc.i4.0
    IL_0003:  stloc.1
    IL_0004:  ldloc.0
    IL_0005:  brfalse.s  IL_0017

    IL_0007:  ldarg.0
    IL_0008:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_000d:  ldc.i4.5
    IL_000e:  readonly.
    IL_0010:  ldelema    !0
    IL_0015:  br.s       IL_0025

    IL_0017:  ldarg.0
    IL_0018:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_001d:  ldc.i4.6
    IL_001e:  readonly.
    IL_0020:  ldelema    !0
    IL_0025:  constrained. !0
    IL_002b:  callvirt   instance int32 IPoint::getx()
    IL_0030:  stloc.1
    IL_0031:  ldloc.1
    IL_0032:  ldc.i4.s   105
    IL_0034:  ceq
    IL_0036:  ldstr      "basicBlockPass"
    IL_003b:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0040:  ldloc.0
    IL_0041:  brtrue.s   IL_0053

    IL_0043:  ldarg.0
    IL_0044:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0049:  ldc.i4.5
    IL_004a:  readonly.
    IL_004c:  ldelema    !0
    IL_0051:  br.s       IL_005f

    IL_0053:  ldarg.0
    IL_0054:  ldfld      !0[] class FixedStack<!0>::m_data
    IL_0059:  ldc.i4.6
    IL_005a:  ldelema    !0
    IL_005f:  constrained. !0
    IL_0065:  callvirt   instance int32 IPoint::getx()
    IL_006a:  stloc.1
    IL_006b:  ldloc.1
    IL_006c:  ldc.i4.s   106
    IL_006e:  ceq
    IL_0070:  ldstr      "basicBlockPass"
    IL_0075:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_007a:  ret
  } // end of method FixedStack::basicBlockPass

} // end of class FixedStack

.class public auto ansi beforefieldinit ReadonlyTests
       extends [mscorlib]System.Object
{
  .field private static int32 s_failed
  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    // Code size       315 (0x13b)
    .maxstack  5
    .locals init (class FixedStack<valuetype ValType> V_0,
             int32 V_1,
             class FixedStack<class RefType> V_2,
             int32 V_3,
             class [mscorlib]System.Exception V_4,
             int32 V_5)
    .try
    {
      IL_0000:  ldstr      "****ValType Tests****"
      IL_0005:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_000a:  ldc.i4.s   100
      IL_000c:  newobj     instance void class FixedStack<valuetype ValType>::.ctor(int32)
      IL_0011:  stloc.0
      IL_0012:  ldc.i4.0
      IL_0013:  stloc.1
      IL_0014:  br.s       IL_0031

      IL_0016:  ldloc.0
      IL_0017:  ldloc.1
      IL_0018:  ldc.i4.s   100
      IL_001a:  add
      IL_001b:  ldloc.1
      IL_001c:  ldc.i4.s   101
      IL_001e:  add
      IL_001f:  ldloc.1
      IL_0020:  ldc.i4.s   102
      IL_0022:  add
      IL_0023:  newobj     instance void ValType::.ctor(int32,
                                                        int32,
                                                        int32)
      IL_0028:  callvirt   instance void class FixedStack<valuetype ValType>::Push(!0)
      IL_002d:  ldloc.1
      IL_002e:  ldc.i4.1
      IL_002f:  add
      IL_0030:  stloc.1
      IL_0031:  ldloc.1
      IL_0032:  ldc.i4.s   100
      IL_0034:  blt.s      IL_0016

      IL_0036:  ldloc.0
      IL_0037:  ldc.i4     0x159
      IL_003c:  ldc.i4     0x15a
      IL_0041:  ldc.i4     0x15b
      IL_0046:  newobj     instance void ValType::.ctor(int32,
                                                        int32,
                                                        int32)
      IL_004b:  callvirt   instance void class FixedStack<valuetype ValType>::stobjTest(!0)
      IL_0050:  ldloc.0
      IL_0051:  callvirt   instance void class FixedStack<valuetype ValType>::initobjTest()
      IL_0056:  ldloc.0
      IL_0057:  callvirt   instance void class FixedStack<valuetype ValType>::mkrefanyTest()
      IL_005c:  ldloc.0
      IL_005d:  ldc.i4     0x1c8
      IL_0062:  ldc.i4     0x1c9
      IL_0067:  ldc.i4     0x1ca
      IL_006c:  newobj     instance void ValType::.ctor(int32,
                                                        int32,
                                                        int32)
      IL_0071:  callvirt   instance void class FixedStack<valuetype ValType>::cpobjTest(!0)
      IL_0076:  ldloc.0
      IL_0077:  callvirt   instance void class FixedStack<valuetype ValType>::constrainedTest()
      IL_007c:  ldloc.0
      IL_007d:  callvirt   instance void class FixedStack<valuetype ValType>::basicBlockTest()
      IL_0082:  ldc.i4.s   100
      IL_0084:  newobj     instance void class FixedStack<class RefType>::.ctor(int32)
      IL_0089:  stloc.2
      IL_008a:  ldc.i4.0
      IL_008b:  stloc.3
      IL_008c:  br.s       IL_00a9

      IL_008e:  ldloc.2
      IL_008f:  ldloc.3
      IL_0090:  ldc.i4.s   100
      IL_0092:  add
      IL_0093:  ldloc.3
      IL_0094:  ldc.i4.s   101
      IL_0096:  add
      IL_0097:  ldloc.3
      IL_0098:  ldc.i4.s   102
      IL_009a:  add
      IL_009b:  newobj     instance void RefType::.ctor(int32,
                                                        int32,
                                                        int32)
      IL_00a0:  callvirt   instance void class FixedStack<class RefType>::Push(!0)
      IL_00a5:  ldloc.3
      IL_00a6:  ldc.i4.1
      IL_00a7:  add
      IL_00a8:  stloc.3
      IL_00a9:  ldloc.3
      IL_00aa:  ldc.i4.s   100
      IL_00ac:  blt.s      IL_008e

      IL_00ae:  ldstr      "****RefType Tests****"
      IL_00b3:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_00b8:  ldloc.2
      IL_00b9:  ldc.i4     0x159
      IL_00be:  ldc.i4     0x15a
      IL_00c3:  ldc.i4     0x15b
      IL_00c8:  newobj     instance void RefType::.ctor(int32,
                                                        int32,
                                                        int32)
      IL_00cd:  callvirt   instance void class FixedStack<class RefType>::stobjTest(!0)
      IL_00d2:  ldloc.2
      IL_00d3:  callvirt   instance void class FixedStack<class RefType>::mkrefanyTest()
      IL_00d8:  ldloc.2
      IL_00d9:  callvirt   instance void class FixedStack<class RefType>::constrainedTest()
      IL_00de:  call       void ReadonlyTests::callTest()
      IL_00e3:  call       void ReadonlyTests::callvirtTest()
      IL_00e8:  call       void ReadonlyTests::stind_i1Test()
      IL_00ed:  call       void ReadonlyTests::stind_i2Test()
      IL_00f2:  call       void ReadonlyTests::stind_i4Test()
      IL_00f7:  call       void ReadonlyTests::stind_i8Test()
      IL_00fc:  call       void ReadonlyTests::stind_r4Test()
      IL_0101:  call       void ReadonlyTests::stind_r8Test()
      IL_0106:  call       void ReadonlyTests::stind_iTest()
      IL_010b:  call       void ReadonlyTests::stind_refTest()
      IL_0110:  leave.s    IL_012f

    }  // end .try
    catch [mscorlib]System.Exception 
    {
      IL_0112:  stloc.s    V_4
      IL_0114:  ldstr      "Caught Unexpected Exception"
      IL_0119:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_011e:  ldloc.s    V_4
      IL_0120:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_0125:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_012a:  ldc.i4.1
      IL_012b:  stloc.s    V_5
      IL_012d:  leave.s    IL_0138

    }  // end handler
    IL_012f:  ldsfld     int32 ReadonlyTests::s_failed
    IL_0134:  stloc.s    V_5
    IL_0136:  br.s       IL_0138

    IL_0138:  ldloc.s    V_5
    IL_013a:  ret
  } // end of method ReadonlyTests::Main

  .method public hidebysig static void  callTest() cil managed
  {
    // Code size       34 (0x22)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    .try
    {
      .try
      {
        IL_0002:  call       void ReadonlyTests::callVerifyPass()
        IL_0007:  leave.s    IL_0010

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0009:  stloc.1
        IL_000a:  ldnull
        IL_000b:  stloc.1
        IL_000c:  ldc.i4.1
        IL_000d:  stloc.0
        IL_000e:  leave.s    IL_0010

      }  // end handler
      IL_0010:  leave.s    IL_0021

    }  // end .try
    finally
    {
      IL_0012:  ldloc.0
      IL_0013:  ldc.i4.0
      IL_0014:  ceq
      IL_0016:  ldstr      "callTest"
      IL_001b:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0020:  endfinally
    }  // end handler
    IL_0021:  ret
  } // end of method ReadonlyTests::callTest

  .method private hidebysig static void  callVerifyPass() cil managed
  {
    // Code size       59 (0x3b)
    .maxstack  8
    .locals init (valuetype ValType[] V_0,
             int32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     ValType
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldc.i4.0
    IL_0009:  ldelema    ValType
    IL_000e:  ldc.i4.1
    IL_000f:  ldc.i4.2
    IL_0010:  ldc.i4.3
    IL_0011:  newobj     instance void ValType::.ctor(int32,
                                                      int32,
                                                      int32)
    IL_0016:  stobj      ValType
    IL_001b:  ldc.i4.0
    IL_001c:  stloc.1
    IL_001d:  ldloc.0
    IL_001e:  ldc.i4.0
    IL_001f:  readonly.
    IL_0021:  ldelema    ValType
    IL_0026:  call       instance int32 ValType::getx()
    IL_002b:  stloc.1
    IL_002c:  ldloc.1
    IL_002d:  ldc.i4.1
    IL_002e:  ceq
    IL_0030:  ldstr      "callVerifyPass"
    IL_0035:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_003a:  ret
  } // end of method ReadonlyTests::callVerifyPass

  .method public hidebysig static void  callvirtTest() cil managed
  {
    // Code size       34 (0x22)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    .try
    {
      .try
      {
        IL_0002:  call       void ReadonlyTests::callvirtVerifyPass()
        IL_0007:  leave.s    IL_0010

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_0009:  stloc.1
        IL_000a:  ldnull
        IL_000b:  stloc.1
        IL_000c:  ldc.i4.1
        IL_000d:  stloc.0
        IL_000e:  leave.s    IL_0010

      }  // end handler
      IL_0010:  leave.s    IL_0021

    }  // end .try
    finally
    {
      IL_0012:  ldloc.0
      IL_0013:  ldc.i4.0
      IL_0014:  ceq
      IL_0016:  ldstr      "callvirtTest"
      IL_001b:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0020:  endfinally
    }  // end handler
    IL_0021:  ret
  } // end of method ReadonlyTests::callvirtTest

  .method private hidebysig static void  callvirtVerifyPass() cil managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method ReadonlyTests::callvirtVerifyPass

  .method public hidebysig static void  stind_i1Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_i1Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_i1Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_i1Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_i1Test

  .method public hidebysig static void  stind_i1Pass() cil managed
  {
    // Code size       41 (0x29)
    .maxstack  3
    .locals init (uint8[] V_0,
             uint8 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Byte
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i1
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  ldelema    [mscorlib]System.Byte
    IL_0015:  ldloc.1
    IL_0016:  stind.i1
    IL_0017:  ldloc.0
    IL_0018:  ldc.i4.0
    IL_0019:  ldelem.u1
    IL_001a:  ldc.i4.s   101
    IL_001c:  ceq
    IL_001e:  ldstr      "stind_i1Pass"
    IL_0023:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0028:  ret
  } // end of method ReadonlyTests::stind_i1Pass

  .method public hidebysig static void  stind_i1Fail() cil managed
  {
    // Code size       26 (0x1a)
    .maxstack  3
    .locals init (uint8[] V_0,
             uint8 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Byte
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i1
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  readonly.
    IL_0012:  ldelema    [mscorlib]System.Byte
    IL_0017:  ldloc.1
    IL_0018:  stind.i1
    IL_0019:  ret
  } // end of method ReadonlyTests::stind_i1Fail

  .method public hidebysig static void  stind_i2Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_i2Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_i2Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_i2Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_i2Test

  .method public hidebysig static void  stind_i2Pass() cil managed
  {
    // Code size       41 (0x29)
    .maxstack  3
    .locals init (int16[] V_0,
             int16 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int16
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i2
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  ldelema    [mscorlib]System.Int16
    IL_0015:  ldloc.1
    IL_0016:  stind.i2
    IL_0017:  ldloc.0
    IL_0018:  ldc.i4.0
    IL_0019:  ldelem.i2
    IL_001a:  ldc.i4.s   101
    IL_001c:  ceq
    IL_001e:  ldstr      "stind_i2Pass"
    IL_0023:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0028:  ret
  } // end of method ReadonlyTests::stind_i2Pass

  .method public hidebysig static void  stind_i2Fail() cil managed
  {
    // Code size       26 (0x1a)
    .maxstack  3
    .locals init (int16[] V_0,
             int16 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int16
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i2
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  readonly.
    IL_0012:  ldelema    [mscorlib]System.Int16
    IL_0017:  ldloc.1
    IL_0018:  stind.i2
    IL_0019:  ret
  } // end of method ReadonlyTests::stind_i2Fail

  .method public hidebysig static void  stind_i4Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_i4Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_i4Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_i4Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_i4Test

  .method public hidebysig static void  stind_i4Pass() cil managed
  {
    // Code size       41 (0x29)
    .maxstack  3
    .locals init (int32[] V_0,
             int32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int32
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i4
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  ldelema    [mscorlib]System.Int32
    IL_0015:  ldloc.1
    IL_0016:  stind.i4
    IL_0017:  ldloc.0
    IL_0018:  ldc.i4.0
    IL_0019:  ldelem.i4
    IL_001a:  ldc.i4.s   101
    IL_001c:  ceq
    IL_001e:  ldstr      "stind_i4Pass"
    IL_0023:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0028:  ret
  } // end of method ReadonlyTests::stind_i4Pass

  .method public hidebysig static void  stind_i4Fail() cil managed
  {
    // Code size       26 (0x1a)
    .maxstack  3
    .locals init (int32[] V_0,
             int32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int32
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  stloc.1
    IL_000a:  ldloc.0
    IL_000b:  ldc.i4.0
    IL_000c:  ldc.i4.1
    IL_000d:  stelem.i4
    IL_000e:  ldloc.0
    IL_000f:  ldc.i4.0
    IL_0010:  readonly.
    IL_0012:  ldelema    [mscorlib]System.Int32
    IL_0017:  ldloc.1
    IL_0018:  stind.i4
    IL_0019:  ret
  } // end of method ReadonlyTests::stind_i4Fail

  .method public hidebysig static void  stind_i8Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_i8Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_i8Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_i8Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_i8Test

  .method public hidebysig static void  stind_i8Pass() cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  3
    .locals init (int64[] V_0,
             int64 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int64
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  conv.i8
    IL_000a:  stloc.1
    IL_000b:  ldloc.0
    IL_000c:  ldc.i4.0
    IL_000d:  ldc.i4.1
    IL_000e:  conv.i8
    IL_000f:  stelem.i8
    IL_0010:  ldloc.0
    IL_0011:  ldc.i4.0
    IL_0012:  ldelema    [mscorlib]System.Int64
    IL_0017:  ldloc.1
    IL_0018:  stind.i8
    IL_0019:  ldloc.0
    IL_001a:  ldc.i4.0
    IL_001b:  ldelem.i8
    IL_001c:  ldc.i4.s   101
    IL_001e:  conv.i8
    IL_001f:  ceq
    IL_0021:  ldstr      "stind_i8Pass"
    IL_0026:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_002b:  ret
  } // end of method ReadonlyTests::stind_i8Pass

  .method public hidebysig static void  stind_i8Fail() cil managed
  {
    // Code size       28 (0x1c)
    .maxstack  3
    .locals init (int64[] V_0,
             int64 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int64
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  conv.i8
    IL_000a:  stloc.1
    IL_000b:  ldloc.0
    IL_000c:  ldc.i4.0
    IL_000d:  ldc.i4.1
    IL_000e:  conv.i8
    IL_000f:  stelem.i8
    IL_0010:  ldloc.0
    IL_0011:  ldc.i4.0
    IL_0012:  readonly.
    IL_0014:  ldelema    [mscorlib]System.Int64
    IL_0019:  ldloc.1
    IL_001a:  stind.i8
    IL_001b:  ret
  } // end of method ReadonlyTests::stind_i8Fail

  .method public hidebysig static void  stind_r4Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_r4Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_r4Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_r4Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_r4Test

  .method public hidebysig static void  stind_r4Pass() cil managed
  {
    // Code size       47 (0x2f)
    .maxstack  3
    .locals init (float32[] V_0,
             float32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Single
    IL_0006:  stloc.0
    IL_0007:  ldc.r4     101.
    IL_000c:  stloc.1
    IL_000d:  ldloc.0
    IL_000e:  ldc.i4.0
    IL_000f:  ldc.r4     1.
    IL_0014:  stelem.r4
    IL_0015:  ldloc.0
    IL_0016:  ldc.i4.0
    IL_0017:  ldelema    [mscorlib]System.Single
    IL_001c:  ldloc.1
    IL_001d:  stind.r4
    IL_001e:  ldloc.0
    IL_001f:  ldc.i4.0
    IL_0020:  ldelem.r4
    IL_0021:  ldloc.1
    IL_0022:  ceq
    IL_0024:  ldstr      "stind_r4Pass"
    IL_0029:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_002e:  ret
  } // end of method ReadonlyTests::stind_r4Pass

  .method public hidebysig static void  stind_r4Fail() cil managed
  {
    // Code size       33 (0x21)
    .maxstack  3
    .locals init (float32[] V_0,
             float32 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Single
    IL_0006:  stloc.0
    IL_0007:  ldc.r4     101.
    IL_000c:  stloc.1
    IL_000d:  ldloc.0
    IL_000e:  ldc.i4.0
    IL_000f:  ldc.r4     1.
    IL_0014:  stelem.r4
    IL_0015:  ldloc.0
    IL_0016:  ldc.i4.0
    IL_0017:  readonly.
    IL_0019:  ldelema    [mscorlib]System.Single
    IL_001e:  ldloc.1
    IL_001f:  stind.r4
    IL_0020:  ret
  } // end of method ReadonlyTests::stind_r4Fail

  .method public hidebysig static void  stind_r8Test() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_r8Pass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_r8Fail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_r8Test"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_r8Test

  .method public hidebysig static void  stind_r8Pass() cil managed
  {
    // Code size       55 (0x37)
    .maxstack  3
    .locals init (float64[] V_0,
             float64 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Double
    IL_0006:  stloc.0
    IL_0007:  ldc.r8     101.
    IL_0010:  stloc.1
    IL_0011:  ldloc.0
    IL_0012:  ldc.i4.0
    IL_0013:  ldc.r8     1.
    IL_001c:  stelem.r8
    IL_001d:  ldloc.0
    IL_001e:  ldc.i4.0
    IL_001f:  ldelema    [mscorlib]System.Double
    IL_0024:  ldloc.1
    IL_0025:  stind.r8
    IL_0026:  ldloc.0
    IL_0027:  ldc.i4.0
    IL_0028:  ldelem.r8
    IL_0029:  ldloc.1
    IL_002a:  ceq
    IL_002c:  ldstr      "stind_r8Pass"
    IL_0031:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0036:  ret
  } // end of method ReadonlyTests::stind_r8Pass

  .method public hidebysig static void  stind_r8Fail() cil managed
  {
    // Code size       41 (0x29)
    .maxstack  3
    .locals init (float64[] V_0,
             float64 V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Double
    IL_0006:  stloc.0
    IL_0007:  ldc.r8     101.
    IL_0010:  stloc.1
    IL_0011:  ldloc.0
    IL_0012:  ldc.i4.0
    IL_0013:  ldc.r8     1.
    IL_001c:  stelem.r8
    IL_001d:  ldloc.0
    IL_001e:  ldc.i4.0
    IL_001f:  readonly.
    IL_0021:  ldelema    [mscorlib]System.Double
    IL_0026:  ldloc.1
    IL_0027:  stind.r8
    IL_0028:  ret
  } // end of method ReadonlyTests::stind_r8Fail

  .method public hidebysig static void  stind_iTest() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_iPass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_iFail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_iTest"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_iTest

  .method public hidebysig static void  stind_iPass() cil managed
  {
    // Code size       71 (0x47)
    .maxstack  2
    .locals init (native int[] V_0,
             native int V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.IntPtr
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_000e:  stloc.1
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.0
    IL_0011:  ldelema    [mscorlib]System.IntPtr
    IL_0016:  ldc.i4.1
    IL_0017:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_001c:  stobj      [mscorlib]System.IntPtr
    IL_0021:  ldloc.0
    IL_0022:  ldc.i4.0
    IL_0023:  ldelema    [mscorlib]System.IntPtr
    IL_0028:  ldloc.1
    IL_0029:  stind.i
    IL_002a:  ldloc.0
    IL_002b:  ldc.i4.0
    IL_002c:  ldelema    [mscorlib]System.IntPtr
    IL_0031:  ldobj      [mscorlib]System.IntPtr
    IL_0036:  ldloc.1
    IL_0037:  call       bool [mscorlib]System.IntPtr::op_Equality(native int,
                                                                   native int)
    IL_003c:  ldstr      "stind_iPass"
    IL_0041:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0046:  ret
  } // end of method ReadonlyTests::stind_iPass

  .method public hidebysig static void  stind_iFail() cil managed
  {
    // Code size       45 (0x2d)
    .maxstack  2
    .locals init (native int[] V_0,
             native int V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.IntPtr
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_000e:  stloc.1
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.0
    IL_0011:  ldelema    [mscorlib]System.IntPtr
    IL_0016:  ldc.i4.1
    IL_0017:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_001c:  stobj      [mscorlib]System.IntPtr
    IL_0021:  ldloc.0
    IL_0022:  ldc.i4.0
    IL_0023:  readonly.
    IL_0025:  ldelema    [mscorlib]System.IntPtr
    IL_002a:  ldloc.1
    IL_002b:  stind.i
    IL_002c:  ret
  } // end of method ReadonlyTests::stind_iFail

  .method public hidebysig static void  stind_refTest() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  2
    .locals init (bool V_0,
             class [mscorlib]System.Security.VerificationException V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  call       void ReadonlyTests::stind_refPass()
    .try
    {
      .try
      {
        IL_0007:  call       void ReadonlyTests::stind_refFail()
        IL_000c:  leave.s    IL_0015

      }  // end .try
      catch [mscorlib]System.Security.VerificationException 
      {
        IL_000e:  stloc.1
        IL_000f:  ldnull
        IL_0010:  stloc.1
        IL_0011:  ldc.i4.1
        IL_0012:  stloc.0
        IL_0013:  leave.s    IL_0015

      }  // end handler
      IL_0015:  leave.s    IL_0023

    }  // end .try
    finally
    {
      IL_0017:  ldloc.0
      IL_0018:  ldstr      "stind_refTest"
      IL_001d:  call       void ReadonlyTests::Test(bool,
                                                    string)
      IL_0022:  endfinally
    }  // end handler
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_refTest

  .method public hidebysig static void  stind_refPass() cil managed
  {
    // Code size       69 (0x45)
    .maxstack  3
    .locals init (object[] V_0,
             object V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  box        [mscorlib]System.Int32
    IL_000e:  stloc.1
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.0
    IL_0011:  ldc.i4.1
    IL_0012:  box        [mscorlib]System.Int32
    IL_0017:  stelem.ref
    IL_0018:  ldloc.0
    IL_0019:  ldc.i4.0
    IL_001a:  ldelema    [mscorlib]System.Object
    IL_001f:  ldloc.1
    IL_0020:  stind.ref
    IL_0021:  ldloc.0
    IL_0022:  ldc.i4.0
    IL_0023:  ldelem.ref
    IL_0024:  ldloc.1
    IL_0025:  ceq
    IL_0027:  ldstr      "stind_refPass"
    IL_002c:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0031:  ldloc.0
    IL_0032:  ldc.i4.0
    IL_0033:  ldelem.ref
    IL_0034:  ldloc.1
    IL_0035:  call       bool [mscorlib]System.Object::ReferenceEquals(object,
                                                                       object)
    IL_003a:  ldstr      "stind_refPass ReferenceEquals"
    IL_003f:  call       void ReadonlyTests::Test(bool,
                                                  string)
    IL_0044:  ret
  } // end of method ReadonlyTests::stind_refPass

  .method public hidebysig static void  stind_refFail() cil managed
  {
    // Code size       36 (0x24)
    .maxstack  3
    .locals init (object[] V_0,
             object V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.0
    IL_0007:  ldc.i4.s   101
    IL_0009:  box        [mscorlib]System.Int32
    IL_000e:  stloc.1
    IL_000f:  ldloc.0
    IL_0010:  ldc.i4.0
    IL_0011:  ldc.i4.1
    IL_0012:  box        [mscorlib]System.Int32
    IL_0017:  stelem.ref
    IL_0018:  ldloc.0
    IL_0019:  ldc.i4.0
    IL_001a:  readonly.
    IL_001c:  ldelema    [mscorlib]System.Object
    IL_0021:  ldloc.1
    IL_0022:  stind.ref
    IL_0023:  ret
  } // end of method ReadonlyTests::stind_refFail

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
    IL_0026:  stsfld     int32 ReadonlyTests::s_failed
    IL_002b:  ret
  } // end of method ReadonlyTests::Test

  .method private hidebysig specialname rtspecialname static 
          void  .cctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     int32 ReadonlyTests::s_failed
    IL_0006:  ret
  } // end of method ReadonlyTests::.cctor

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method ReadonlyTests::.ctor

} // end of class ReadonlyTests


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
