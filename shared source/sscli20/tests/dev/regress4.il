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
.assembly regress4
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module regress4.exe
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x06f10000
//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi beforefieldinit A
       extends [mscorlib]System.Object
{
} // end of class A

.class public auto ansi beforefieldinit B
       extends A
{
} // end of class B

.class private auto ansi sealed Dlg
       extends [mscorlib]System.MulticastDelegate
{
} // end of class Dlg

.class public auto ansi beforefieldinit C
       extends A
{
} // end of class C


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi beforefieldinit A
       extends [mscorlib]System.Object
{
  .field family int32 m_secureField
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       14 (0xe)
    .maxstack  2
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldc.i4.0
    IL_0008:  stfld      int32 A::m_secureField
    IL_000d:  ret
  } // end of method A::.ctor

  .method family hidebysig instance int32 
          GetSecureField() cil managed
  {
    // Code size       11 (0xb)
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 A::m_secureField
    IL_0006:  stloc.0
    IL_0007:  br.s       IL_0009

    IL_0009:  ldloc.0
    IL_000a:  ret
  } // end of method A::GetSecureField

} // end of class A

.class public auto ansi beforefieldinit B
       extends A
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       14 (0xe)
    .maxstack  2
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldc.i4.1
    IL_0008:  stfld      int32 A::m_secureField
    IL_000d:  ret
  } // end of method B::.ctor

} // end of class B

.class private auto ansi sealed Dlg
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native int 'method') runtime managed
  {
  } // end of method Dlg::.ctor

  .method public hidebysig virtual instance int32 
          Invoke() runtime managed
  {
  } // end of method Dlg::Invoke

  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed
  {
  } // end of method Dlg::BeginInvoke

  .method public hidebysig newslot virtual 
          instance int32  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed
  {
  } // end of method Dlg::EndInvoke

} // end of class Dlg

.class public auto ansi beforefieldinit C
       extends A
{
  .method public hidebysig static void  RunTest() cil managed
  {
      .maxstack  3
      .locals init (class Dlg V_0)
      IL_0000:  newobj     instance void B::.ctor()
      IL_0005:  ldftn      instance int32 A::GetSecureField()
      IL_000b:  newobj     instance void Dlg::.ctor(object,
                                                    native int)
      IL_0010:  stloc.0
      IL_0011:  ldloc.0
      IL_0012:  callvirt   instance int32 Dlg::Invoke()
      IL_0017:  call       void [mscorlib]System.Console::WriteLine(int32)
                ret
  }

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       82 (0x52)
    .maxstack  3
    .locals init (class Dlg V_0)
    .try
    {
      IL_0012:  call       void C::RunTest()
      IL_001c:  ldstr      "FAILED: Verification exception not thrown"
      IL_0021:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0026:  ldc.i4.1
      IL_0027:  call       void [mscorlib]System.Environment::Exit(int32)
      IL_002c:  leave.s    IL_0041

    }  // end .try
    catch [mscorlib]System.Security.VerificationException
    {
      IL_002e:  pop
      IL_002f:  ldstr      "PASSED: Verification exception thrown"
      IL_0034:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0039:  ldc.i4.0
      IL_003a:  call       void [mscorlib]System.Environment::Exit(int32)
      IL_003f:  leave.s    IL_0051

    }  // end handler
    IL_0041:  ldstr      "FAILED: unexpected"
    IL_0046:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_004b:  ldc.i4.1
    IL_004c:  call       void [mscorlib]System.Environment::Exit(int32)
    IL_0051:  ret
  } // end of method C::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } // end of method C::.ctor

} // end of class C
