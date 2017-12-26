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
.assembly legacy library 'sizeof'
{

  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 

  .permissionset reqmin
            "<PermissionSet class=\"System.Security.PermissionSe"
  + "t\"\r\nversion=\"1\">\r\n<IPermission class=\"System.Security.Permis"
  + "sions.SecurityPermission, mscorlib, Version=2.0.3500.0, Cult"
  + "ure=neutral, PublicKeyToken=b77a5c561934e089\"\r\nversion=\"1\"\r\n"
  + "Flags=\"SkipVerification\"/>\r\n</PermissionSet>\r\n"
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module sizeof.exe
// MVID: {2485e3be-b769-6047-39a8-f2da533f30c9}
.custom instance void [mscorlib]System.Security.UnverifiableCodeAttribute::.ctor() = ( 01 00 00 00 ) 
.imagebase 0x00400000
.file alignment 0x00000200
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000001    //  ILONLY
// Image base: 0x07C70000


// =============== CLASS MEMBERS DECLARATION ===================

.class public sequential ansi sealed beforefieldinit ValType
       extends [mscorlib]System.ValueType
{
  .field private int32 a
  .field private int32 b
  .field private int32 c
} // end of class ValType

.class public auto ansi beforefieldinit RefType
       extends [mscorlib]System.Object
{
  .field private int32 a
  .field private int32 b
  .field private int32 c
  .field private int32 d
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       35 (0x23)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.0
    IL_0002:  stfld      int32 RefType::a
    IL_0007:  ldarg.0
    IL_0008:  ldc.i4.0
    IL_0009:  stfld      int32 RefType::b
    IL_000e:  ldarg.0
    IL_000f:  ldc.i4.0
    IL_0010:  stfld      int32 RefType::c
    IL_0015:  ldarg.0
    IL_0016:  ldc.i4.0
    IL_0017:  stfld      int32 RefType::d
    IL_001c:  ldarg.0
    IL_001d:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0022:  ret
  } // end of method RefType::.ctor

} // end of class RefType

.class public auto ansi beforefieldinit SizeOfTests
       extends [mscorlib]System.Object
{
  .field private static int32 s_failed
  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    // Code size       194 (0xc2)
    .maxstack  2
    .locals (class Container<int32> V_0,
             class Container<float64> V_1,
             class Container<valuetype ValType> V_2,
             class Container<class RefType> V_3,
             class [mscorlib]System.Exception V_4,
             int32 V_5)
    .try
    {
      IL_0000:  newobj     instance void class Container<int32>::.ctor()
      IL_0005:  stloc.0
      IL_0006:  newobj     instance void class Container<float64>::.ctor()
      IL_000b:  stloc.1
      IL_000c:  newobj     instance void class Container<valuetype ValType>::.ctor()
      IL_0011:  stloc.2
      IL_0012:  newobj     instance void class Container<class RefType>::.ctor()
      IL_0017:  stloc.3
      IL_0018:  ldc.i4.1
      IL_0019:  ldstr      "sizeof(System.Int32)"
      IL_001e:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_0023:  sizeof     ValType
      IL_0029:  ldc.i4.s   12
      IL_002b:  ceq
      IL_002d:  ldstr      "sizeof(ValType)"
      IL_0032:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_0037:  sizeof     RefType
      IL_003d:  ldc.i4.4
      IL_003e:  ceq
      IL_0040:  ldstr      "sizeof(RefType)"
      IL_0045:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_004a:  ldloc.0
      IL_004b:  callvirt   instance int32 class Container<int32>::sizeofT()
      IL_0050:  ldc.i4.4
      IL_0051:  ceq
      IL_0053:  ldstr      "Cint.sizeofT()"
      IL_0058:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_005d:  ldloc.1
      IL_005e:  callvirt   instance int32 class Container<float64>::sizeofT()
      IL_0063:  ldc.i4.8
      IL_0064:  ceq
      IL_0066:  ldstr      "Cdouble.sizeofT()"
      IL_006b:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_0070:  ldloc.2
      IL_0071:  callvirt   instance int32 class Container<valuetype ValType>::sizeofT()
      IL_0076:  ldc.i4.s   12
      IL_0078:  ceq
      IL_007a:  ldstr      "CValType.sizeofT()"
      IL_007f:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_0084:  ldloc.3
      IL_0085:  callvirt   instance int32 class Container<class RefType>::sizeofT()
      IL_008a:  ldc.i4.4
      IL_008b:  ceq
      IL_008d:  ldstr      "CRefType.sizeofT()"
      IL_0092:  call       void SizeOfTests::Test(bool,
                                                  string)
      IL_0097:  leave.s    IL_00b6

    }  // end .try
    catch [mscorlib]System.Exception 
    {
      IL_0099:  stloc.s    V_4
      IL_009b:  ldstr      "Caught Unexpected Exception"
      IL_00a0:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_00a5:  ldloc.s    V_4
      IL_00a7:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_00ac:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_00b1:  ldc.i4.1
      IL_00b2:  stloc.s    V_5
      IL_00b4:  leave.s    IL_00bf

    }  // end handler
    IL_00b6:  ldsfld     int32 SizeOfTests::s_failed
    IL_00bb:  stloc.s    V_5
    IL_00bd:  br.s       IL_00bf

    IL_00bf:  ldloc.s    V_5
    IL_00c1:  ret
  } // end of method SizeOfTests::Main

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
    IL_0026:  stsfld     int32 SizeOfTests::s_failed
    IL_002b:  ret
  } // end of method SizeOfTests::Test

  .method private hidebysig specialname rtspecialname static 
          void  .cctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     int32 SizeOfTests::s_failed
    IL_0006:  ret
  } // end of method SizeOfTests::.cctor

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method SizeOfTests::.ctor

} // end of class SizeOfTests

.class public auto ansi beforefieldinit Container<([mscorlib]System.Object) T>
       extends [mscorlib]System.Object
{
  .method public hidebysig instance int32 
          sizeofT() cil managed
  {
    // Code size       6 (0x6)
    .maxstack  1
    .locals (int32 V_0)
    sizeof !0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0004

    IL_0004:  ldloc.0
    IL_0005:  ret
  } // end of method Container::sizeofT

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Container::.ctor

} // end of class Container


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
