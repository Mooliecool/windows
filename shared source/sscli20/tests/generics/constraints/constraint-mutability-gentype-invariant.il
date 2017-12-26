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

// Metadata version: v1.2.21230
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .hash = (77 44 54 E4 D3 61 56 02 14 E9 88 CD D8 31 EB C9   // wDT..aV......1..
           2B DF E4 40 )                                     // +..@
  .ver 1:2:3300:0
}
.assembly test
{

  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 

  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module test.exe
// MVID: {B1E5359B-2F69-470A-9D5A-3CFCAC471629}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x04190000

// =============== CLASS MEMBERS DECLARATION ===================

.class private auto ansi beforefieldinit Finder<([mscorlib]System.IComparable) T>
       extends [mscorlib]System.Object
{
  .method public hidebysig instance int32 
          Find(object x,
               !0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.2
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !0
    IL_0010:  ldarg.1
    IL_000b:  constrained.        !0
    IL_0011:  callvirt   instance int32 [mscorlib]System.IComparable::CompareTo(object)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.2
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method Finder::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Finder::.ctor

} // end of class Finder

.class private auto ansi beforefieldinit StaticFind
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Find<([mscorlib]System.IComparable) T>(object x,
                                                                                !!0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.1
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !!0
    IL_0010:  ldarg.0
    IL_000b:  constrained.        !!0
    IL_0011:  callvirt   instance int32 [mscorlib]System.IComparable::CompareTo(object)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.1
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method StaticFind::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method StaticFind::.ctor

} // end of class StaticFind

.class interface private abstract auto ansi GIComparable<([mscorlib]System.Object) T>
{
  .method public hidebysig newslot abstract virtual 
          instance int32  CompareTo(!0 x) cil managed
  {
  } // end of method GIComparable::CompareTo

} // end of class GIComparable

.class private auto ansi beforefieldinit Finder2<(class GIComparable<!0>) T>
       extends [mscorlib]System.Object
{
  .method public hidebysig instance int32 
          Find(!0 x,
               !0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.2
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !0
    IL_0010:  ldarg.1
    IL_000b:  constrained.        !0
    IL_0011:  callvirt   instance int32 class GIComparable<!0>::CompareTo(!0)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.2
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method Finder2::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Finder2::.ctor

} // end of class Finder2

.class private auto ansi beforefieldinit StaticFind2
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Find<(class GIComparable<!!0>) T>(!!0 x,
                                                                           !!0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.1
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !!0
    IL_0010:  ldarg.0
    IL_000b:  constrained.        !!0
    IL_0011:  callvirt   instance int32 class GIComparable<!!0>::CompareTo(!0)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.1
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method StaticFind2::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method StaticFind2::.ctor

} // end of class StaticFind2

.class private sequential ansi sealed beforefieldinit S1
       extends [mscorlib]System.ValueType
       implements [mscorlib]System.IComparable
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       15 (0xf)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 S1::x
    IL_0007:  ldarg.0
    IL_0008:  ldarg.2
    IL_0009:  stfld      int32 S1::y
    IL_000e:  ret
  } // end of method S1::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(object o) cil managed
  {
    // Code size       94 (0x5e)
    .maxstack  2
    .locals init (valuetype S1 V_0,
             int32 V_1)
    IL_0000:  ldarg.1
    IL_0001:  unbox      S1
    IL_0006:  ldobj      S1
    IL_000b:  stloc.0
    IL_000c:  ldarg.0
    IL_000d:  ldfld      int32 S1::x
    IL_0012:  ldloca.s   V_0
    IL_0014:  ldfld      int32 S1::x
    IL_0019:  bge.s      IL_001f

    IL_001b:  ldc.i4.m1
    IL_001c:  stloc.1
    IL_001d:  br.s       IL_005c

    IL_001f:  ldarg.0
    IL_0020:  ldfld      int32 S1::x
    IL_0025:  ldloca.s   V_0
    IL_0027:  ldfld      int32 S1::x
    IL_002c:  ble.s      IL_0032

    IL_002e:  ldc.i4.1
    IL_002f:  stloc.1
    IL_0030:  br.s       IL_005c

    IL_0032:  ldarg.0
    IL_0033:  ldfld      int32 S1::y
    IL_0038:  ldloca.s   V_0
    IL_003a:  ldfld      int32 S1::y
    IL_003f:  bge.s      IL_0045

    IL_0041:  ldc.i4.m1
    IL_0042:  stloc.1
    IL_0043:  br.s       IL_005c

    IL_0045:  ldarg.0
    IL_0046:  ldfld      int32 S1::y
    IL_004b:  ldloca.s   V_0
    IL_004d:  ldfld      int32 S1::y
    IL_0052:  ble.s      IL_0058

    IL_0054:  ldc.i4.1
    IL_0055:  stloc.1
    IL_0056:  br.s       IL_005c

    IL_0058:  ldc.i4.0
    IL_0059:  stloc.1
    IL_005a:  br.s       IL_005c

    IL_005c:  ldloc.1
    IL_005d:  ret
  } // end of method S1::CompareTo

} // end of class S1

.class private sequential ansi sealed beforefieldinit S2
       extends [mscorlib]System.ValueType
       implements class GIComparable<valuetype S2>
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       15 (0xf)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 S2::x
    IL_0007:  ldarg.0
    IL_0008:  ldarg.2
    IL_0009:  stfld      int32 S2::y
    IL_000e:  ret
  } // end of method S2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(valuetype S2 that) cil managed
  {
    // Code size       82 (0x52)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 S2::x
    IL_0006:  ldarga.s   that
    IL_0008:  ldfld      int32 S2::x
    IL_000d:  bge.s      IL_0013

    IL_000f:  ldc.i4.m1
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_0050

    IL_0013:  ldarg.0
    IL_0014:  ldfld      int32 S2::x
    IL_0019:  ldarga.s   that
    IL_001b:  ldfld      int32 S2::x
    IL_0020:  ble.s      IL_0026

    IL_0022:  ldc.i4.1
    IL_0023:  stloc.0
    IL_0024:  br.s       IL_0050

    IL_0026:  ldarg.0
    IL_0027:  ldfld      int32 S2::y
    IL_002c:  ldarga.s   that
    IL_002e:  ldfld      int32 S2::y
    IL_0033:  bge.s      IL_0039

    IL_0035:  ldc.i4.m1
    IL_0036:  stloc.0
    IL_0037:  br.s       IL_0050

    IL_0039:  ldarg.0
    IL_003a:  ldfld      int32 S2::y
    IL_003f:  ldarga.s   that
    IL_0041:  ldfld      int32 S2::y
    IL_0046:  ble.s      IL_004c

    IL_0048:  ldc.i4.1
    IL_0049:  stloc.0
    IL_004a:  br.s       IL_0050

    IL_004c:  ldc.i4.0
    IL_004d:  stloc.0
    IL_004e:  br.s       IL_0050

    IL_0050:  ldloc.0
    IL_0051:  ret
  } // end of method S2::CompareTo

} // end of class S2

.class private auto ansi beforefieldinit R1
       extends [mscorlib]System.Object
       implements [mscorlib]System.IComparable
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       21 (0x15)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 R1::x
    IL_000d:  ldarg.0
    IL_000e:  ldarg.2
    IL_000f:  stfld      int32 R1::y
    IL_0014:  ret
  } // end of method R1::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(object o) cil managed
  {
    // Code size       85 (0x55)
    .maxstack  2
    .locals init (class R1 V_0,
             int32 V_1)
    IL_0000:  ldarg.1
    IL_0001:  castclass  R1
    IL_0006:  stloc.0
    IL_0007:  ldarg.0
    IL_0008:  ldfld      int32 R1::x
    IL_000d:  ldloc.0
    IL_000e:  ldfld      int32 R1::x
    IL_0013:  bge.s      IL_0019

    IL_0015:  ldc.i4.m1
    IL_0016:  stloc.1
    IL_0017:  br.s       IL_0053

    IL_0019:  ldarg.0
    IL_001a:  ldfld      int32 R1::x
    IL_001f:  ldloc.0
    IL_0020:  ldfld      int32 R1::x
    IL_0025:  ble.s      IL_002b

    IL_0027:  ldc.i4.1
    IL_0028:  stloc.1
    IL_0029:  br.s       IL_0053

    IL_002b:  ldarg.0
    IL_002c:  ldfld      int32 R1::y
    IL_0031:  ldloc.0
    IL_0032:  ldfld      int32 R1::y
    IL_0037:  bge.s      IL_003d

    IL_0039:  ldc.i4.m1
    IL_003a:  stloc.1
    IL_003b:  br.s       IL_0053

    IL_003d:  ldarg.0
    IL_003e:  ldfld      int32 R1::y
    IL_0043:  ldloc.0
    IL_0044:  ldfld      int32 R1::y
    IL_0049:  ble.s      IL_004f

    IL_004b:  ldc.i4.1
    IL_004c:  stloc.1
    IL_004d:  br.s       IL_0053

    IL_004f:  ldc.i4.0
    IL_0050:  stloc.1
    IL_0051:  br.s       IL_0053

    IL_0053:  ldloc.1
    IL_0054:  ret
  } // end of method R1::CompareTo

} // end of class R1

.class private auto ansi beforefieldinit R2
       extends [mscorlib]System.Object
       implements class GIComparable<class R2>
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       21 (0x15)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 R2::x
    IL_000d:  ldarg.0
    IL_000e:  ldarg.2
    IL_000f:  stfld      int32 R2::y
    IL_0014:  ret
  } // end of method R2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(class R2 that) cil managed
  {
    // Code size       78 (0x4e)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 R2::x
    IL_0006:  ldarg.1
    IL_0007:  ldfld      int32 R2::x
    IL_000c:  bge.s      IL_0012

    IL_000e:  ldc.i4.m1
    IL_000f:  stloc.0
    IL_0010:  br.s       IL_004c

    IL_0012:  ldarg.0
    IL_0013:  ldfld      int32 R2::x
    IL_0018:  ldarg.1
    IL_0019:  ldfld      int32 R2::x
    IL_001e:  ble.s      IL_0024

    IL_0020:  ldc.i4.1
    IL_0021:  stloc.0
    IL_0022:  br.s       IL_004c

    IL_0024:  ldarg.0
    IL_0025:  ldfld      int32 R2::y
    IL_002a:  ldarg.1
    IL_002b:  ldfld      int32 R2::y
    IL_0030:  bge.s      IL_0036

    IL_0032:  ldc.i4.m1
    IL_0033:  stloc.0
    IL_0034:  br.s       IL_004c

    IL_0036:  ldarg.0
    IL_0037:  ldfld      int32 R2::y
    IL_003c:  ldarg.1
    IL_003d:  ldfld      int32 R2::y
    IL_0042:  ble.s      IL_0048

    IL_0044:  ldc.i4.1
    IL_0045:  stloc.0
    IL_0046:  br.s       IL_004c

    IL_0048:  ldc.i4.0
    IL_0049:  stloc.0
    IL_004a:  br.s       IL_004c

    IL_004c:  ldloc.0
    IL_004d:  ret
  } // end of method R2::CompareTo

} // end of class R2

.class private sequential ansi sealed beforefieldinit OS1
       extends [mscorlib]System.ValueType
       implements [mscorlib]System.IComparable
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 OS1::x
    IL_0007:  ret
  } // end of method OS1::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(object o) cil managed
  {
    // Code size       56 (0x38)
    .maxstack  2
    .locals init (valuetype OS1 V_0,
             int32 V_1)
    IL_0000:  ldarg.1
    IL_0001:  unbox      OS1
    IL_0006:  ldobj      OS1
    IL_000b:  stloc.0
    IL_000c:  ldarg.0
    IL_000d:  ldfld      int32 OS1::x
    IL_0012:  ldloca.s   V_0
    IL_0014:  ldfld      int32 OS1::x
    IL_0019:  bge.s      IL_001f

    IL_001b:  ldc.i4.m1
    IL_001c:  stloc.1
    IL_001d:  br.s       IL_0036

    IL_001f:  ldarg.0
    IL_0020:  ldfld      int32 OS1::x
    IL_0025:  ldloca.s   V_0
    IL_0027:  ldfld      int32 OS1::x
    IL_002c:  ble.s      IL_0032

    IL_002e:  ldc.i4.1
    IL_002f:  stloc.1
    IL_0030:  br.s       IL_0036

    IL_0032:  ldc.i4.0
    IL_0033:  stloc.1
    IL_0034:  br.s       IL_0036

    IL_0036:  ldloc.1
    IL_0037:  ret
  } // end of method OS1::CompareTo

} // end of class OS1

.class private sequential ansi sealed beforefieldinit OS2
       extends [mscorlib]System.ValueType
       implements class GIComparable<valuetype OS2>
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 OS2::x
    IL_0007:  ret
  } // end of method OS2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(valuetype OS2 that) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 OS2::x
    IL_0006:  ldarga.s   that
    IL_0008:  ldfld      int32 OS2::x
    IL_000d:  bge.s      IL_0013

    IL_000f:  ldc.i4.m1
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_002a

    IL_0013:  ldarg.0
    IL_0014:  ldfld      int32 OS2::x
    IL_0019:  ldarga.s   that
    IL_001b:  ldfld      int32 OS2::x
    IL_0020:  ble.s      IL_0026

    IL_0022:  ldc.i4.1
    IL_0023:  stloc.0
    IL_0024:  br.s       IL_002a

    IL_0026:  ldc.i4.0
    IL_0027:  stloc.0
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.0
    IL_002b:  ret
  } // end of method OS2::CompareTo

} // end of class OS2

.class private auto ansi beforefieldinit OR1
       extends [mscorlib]System.Object
       implements [mscorlib]System.IComparable
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 OR1::x
    IL_000d:  ret
  } // end of method OR1::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(object o) cil managed
  {
    // Code size       49 (0x31)
    .maxstack  2
    .locals init (class OR1 V_0,
             int32 V_1)
    IL_0000:  ldarg.1
    IL_0001:  castclass  OR1
    IL_0006:  stloc.0
    IL_0007:  ldarg.0
    IL_0008:  ldfld      int32 OR1::x
    IL_000d:  ldloc.0
    IL_000e:  ldfld      int32 OR1::x
    IL_0013:  bge.s      IL_0019

    IL_0015:  ldc.i4.m1
    IL_0016:  stloc.1
    IL_0017:  br.s       IL_002f

    IL_0019:  ldarg.0
    IL_001a:  ldfld      int32 OR1::x
    IL_001f:  ldloc.0
    IL_0020:  ldfld      int32 OR1::x
    IL_0025:  ble.s      IL_002b

    IL_0027:  ldc.i4.1
    IL_0028:  stloc.1
    IL_0029:  br.s       IL_002f

    IL_002b:  ldc.i4.0
    IL_002c:  stloc.1
    IL_002d:  br.s       IL_002f

    IL_002f:  ldloc.1
    IL_0030:  ret
  } // end of method OR1::CompareTo

} // end of class OR1

.class private auto ansi beforefieldinit OR2
       extends [mscorlib]System.Object
       implements class GIComparable<class OR2>
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 OR2::x
    IL_000d:  ret
  } // end of method OR2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo(class OR2 that) cil managed
  {
    // Code size       42 (0x2a)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 OR2::x
    IL_0006:  ldarg.1
    IL_0007:  ldfld      int32 OR2::x
    IL_000c:  bge.s      IL_0012

    IL_000e:  ldc.i4.m1
    IL_000f:  stloc.0
    IL_0010:  br.s       IL_0028

    IL_0012:  ldarg.0
    IL_0013:  ldfld      int32 OR2::x
    IL_0018:  ldarg.1
    IL_0019:  ldfld      int32 OR2::x
    IL_001e:  ble.s      IL_0024

    IL_0020:  ldc.i4.1
    IL_0021:  stloc.0
    IL_0022:  br.s       IL_0028

    IL_0024:  ldc.i4.0
    IL_0025:  stloc.0
    IL_0026:  br.s       IL_0028

    IL_0028:  ldloc.0
    IL_0029:  ret
  } // end of method OR2::CompareTo

} // end of class OR2

.class private auto ansi beforefieldinit Maine
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Main(string[] args) cil managed
  {
    .entrypoint
    // Code size       2147 (0x863)
    .maxstack  5
    .locals init (int32 V_0,
             int32[] V_1,
             int32 V_2,
             class Finder<int32> V_3,
             int32 V_4,
             int32 V_5,
             valuetype S1[] V_6,
             int32 V_7,
             class Finder<valuetype S1> V_8,
             int32 V_9,
             int32 V_10,
             valuetype S2[] V_11,
             int32 V_12,
             class Finder2<valuetype S2> V_13,
             int32 V_14,
             int32 V_15,
             class R1[] V_16,
             int32 V_17,
             class Finder<class R1> V_18,
             int32 V_19,
             int32 V_20,
             class R2[] V_21,
             int32 V_22,
             class Finder2<class R2> V_23,
             int32 V_24,
             int32 V_25,
             valuetype OS1[] V_26,
             int32 V_27,
             class Finder<valuetype OS1> V_28,
             int32 V_29,
             int32 V_30,
             valuetype OS2[] V_31,
             int32 V_32,
             class Finder2<valuetype OS2> V_33,
             int32 V_34,
             int32 V_35,
             class OR1[] V_36,
             int32 V_37,
             class Finder<class OR1> V_38,
             int32 V_39,
             int32 V_40,
             class OR2[] V_41,
             int32 V_42,
             class Finder2<class OR2> V_43,
             int32 V_44,
             int32 V_45,
             class OR1[] V_46,
             int32 V_47,
             class Finder<class [mscorlib]System.IComparable> V_48,
             int32 V_49,
             int32 V_50,
             class OR2[] V_51,
             int32 V_52,
             class Finder2<class OR2> V_53,
             int32 V_54,
             int32 V_55,
             int32 V_56)
    IL_0000:  ldarg.0
    IL_0001:  ldlen
    IL_0002:  conv.i4
    IL_0003:  ldc.i4.1
    IL_0004:  bge.s      IL_001d

    IL_0006:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_000b:  ldstr      "Usage: perf <N>"
    IL_0010:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0015:  ldc.i4.1
    IL_0016:  stloc.s    V_56
    IL_0018:  br         IL_0864

    IL_001d:  ldarg.0
    IL_001e:  ldc.i4.0
    IL_001f:  ldelem.ref
    IL_0020:  call       int32 [mscorlib]System.Int32::Parse(string)
    IL_0025:  stloc.0
    IL_0026:  ldloc.0
    IL_0027:  newarr     [mscorlib]System.Int32
    IL_002c:  stloc.1
    IL_002d:  ldc.i4.0
    IL_002e:  stloc.2
    IL_002f:  br.s       IL_0039

    IL_0031:  ldloc.1
    IL_0032:  ldloc.2
    IL_0033:  ldloc.2
    IL_0034:  stelem.i4
    IL_0035:  ldloc.2
    IL_0036:  ldc.i4.1
    IL_0037:  add
    IL_0038:  stloc.2
    IL_0039:  ldloc.2
    IL_003a:  ldloc.0
    IL_003b:  blt.s      IL_0031

    IL_003d:  newobj     instance void class Finder<int32>::.ctor()
    IL_0042:  stloc.3
    IL_0043:  ldc.i4.0
    IL_0044:  stloc.s    V_4
    IL_0046:  br.s       IL_0076

    IL_0048:  ldloc.3
    IL_0049:  ldloc.s    V_4
    IL_004b:  box        [mscorlib]System.Int32
    IL_0050:  ldloc.1
    IL_0051:  callvirt   instance int32 class Finder<int32>::Find(object,
                                                                  !0[])
    IL_0056:  ldloc.s    V_4
    IL_0058:  beq.s      IL_0070

    IL_005a:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_005f:  ldstr      "Error finding {0} using Finder<int>.Find"
    IL_0064:  ldloc.s    V_4
    IL_0066:  box        [mscorlib]System.Int32
    IL_006b:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0070:  ldloc.s    V_4
    IL_0072:  ldc.i4.1
    IL_0073:  add
    IL_0074:  stloc.s    V_4
    IL_0076:  ldloc.s    V_4
    IL_0078:  ldloc.0
    IL_0079:  blt.s      IL_0048

    IL_007b:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0080:  ldstr      "Test PaSSeD (int,1)"
    IL_0085:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_008a:  ldc.i4.0
    IL_008b:  stloc.s    V_5
    IL_008d:  br.s       IL_00bc

    IL_008f:  ldloc.s    V_5
    IL_0091:  box        [mscorlib]System.Int32
    IL_0096:  ldloc.1
    IL_0097:  call       int32 StaticFind::Find<int32>(object,
                                                       !!0[])
    IL_009c:  ldloc.s    V_5
    IL_009e:  beq.s      IL_00b6

    IL_00a0:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_00a5:  ldstr      "Error finding {0} using StaticFind.Find<int>"
    IL_00aa:  ldloc.s    V_5
    IL_00ac:  box        [mscorlib]System.Int32
    IL_00b1:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_00b6:  ldloc.s    V_5
    IL_00b8:  ldc.i4.1
    IL_00b9:  add
    IL_00ba:  stloc.s    V_5
    IL_00bc:  ldloc.s    V_5
    IL_00be:  ldloc.0
    IL_00bf:  blt.s      IL_008f

    IL_00c1:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_00c6:  ldstr      "Test PaSSeD (int,2)"
    IL_00cb:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_00d0:  ldloc.0
    IL_00d1:  newarr     S1
    IL_00d6:  stloc.s    V_6
    IL_00d8:  ldc.i4.0
    IL_00d9:  stloc.s    V_7
    IL_00db:  br.s       IL_00f5

    IL_00dd:  ldloc.s    V_6
    IL_00df:  ldloc.s    V_7
    IL_00e1:  ldelema    S1
    IL_00e6:  ldloc.s    V_7
    IL_00e8:  ldloc.s    V_7
    IL_00ea:  call       instance void S1::.ctor(int32,
                                                 int32)
    IL_00ef:  ldloc.s    V_7
    IL_00f1:  ldc.i4.1
    IL_00f2:  add
    IL_00f3:  stloc.s    V_7
    IL_00f5:  ldloc.s    V_7
    IL_00f7:  ldloc.0
    IL_00f8:  blt.s      IL_00dd

    IL_00fa:  newobj     instance void class Finder<valuetype S1>::.ctor()
    IL_00ff:  stloc.s    V_8
    IL_0101:  ldc.i4.0
    IL_0102:  stloc.s    V_9
    IL_0104:  br.s       IL_013d

    IL_0106:  ldloc.s    V_8
    IL_0108:  ldloc.s    V_9
    IL_010a:  ldloc.s    V_9
    IL_010c:  newobj     instance void S1::.ctor(int32,
                                                 int32)
    IL_0111:  box        S1
    IL_0116:  ldloc.s    V_6
    IL_0118:  callvirt   instance int32 class Finder<valuetype S1>::Find(object,
                                                                         !0[])
    IL_011d:  ldloc.s    V_9
    IL_011f:  beq.s      IL_0137

    IL_0121:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0126:  ldstr      "Error finding {0} using Finder<S1>.Find"
    IL_012b:  ldloc.s    V_9
    IL_012d:  box        [mscorlib]System.Int32
    IL_0132:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0137:  ldloc.s    V_9
    IL_0139:  ldc.i4.1
    IL_013a:  add
    IL_013b:  stloc.s    V_9
    IL_013d:  ldloc.s    V_9
    IL_013f:  ldloc.0
    IL_0140:  blt.s      IL_0106

    IL_0142:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0147:  ldstr      "Test PaSSeD (S1,1)"
    IL_014c:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0151:  ldc.i4.0
    IL_0152:  stloc.s    V_10
    IL_0154:  br.s       IL_018b

    IL_0156:  ldloc.s    V_10
    IL_0158:  ldloc.s    V_10
    IL_015a:  newobj     instance void S1::.ctor(int32,
                                                 int32)
    IL_015f:  box        S1
    IL_0164:  ldloc.s    V_6
    IL_0166:  call       int32 StaticFind::Find<valuetype S1>(object,
                                                              !!0[])
    IL_016b:  ldloc.s    V_10
    IL_016d:  beq.s      IL_0185

    IL_016f:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0174:  ldstr      "Error finding {0} using StaticFind.Find<S1>"
    IL_0179:  ldloc.s    V_10
    IL_017b:  box        [mscorlib]System.Int32
    IL_0180:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0185:  ldloc.s    V_10
    IL_0187:  ldc.i4.1
    IL_0188:  add
    IL_0189:  stloc.s    V_10
    IL_018b:  ldloc.s    V_10
    IL_018d:  ldloc.0
    IL_018e:  blt.s      IL_0156

    IL_0190:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0195:  ldstr      "Test PaSSeD (S1,2)"
    IL_019a:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_019f:  ldloc.0
    IL_01a0:  newarr     S2
    IL_01a5:  stloc.s    V_11
    IL_01a7:  ldc.i4.0
    IL_01a8:  stloc.s    V_12
    IL_01aa:  br.s       IL_01c4

    IL_01ac:  ldloc.s    V_11
    IL_01ae:  ldloc.s    V_12
    IL_01b0:  ldelema    S2
    IL_01b5:  ldloc.s    V_12
    IL_01b7:  ldloc.s    V_12
    IL_01b9:  call       instance void S2::.ctor(int32,
                                                 int32)
    IL_01be:  ldloc.s    V_12
    IL_01c0:  ldc.i4.1
    IL_01c1:  add
    IL_01c2:  stloc.s    V_12
    IL_01c4:  ldloc.s    V_12
    IL_01c6:  ldloc.0
    IL_01c7:  blt.s      IL_01ac

    IL_01c9:  newobj     instance void class Finder2<valuetype S2>::.ctor()
    IL_01ce:  stloc.s    V_13
    IL_01d0:  ldc.i4.0
    IL_01d1:  stloc.s    V_14
    IL_01d3:  br.s       IL_0207

    IL_01d5:  ldloc.s    V_13
    IL_01d7:  ldloc.s    V_14
    IL_01d9:  ldloc.s    V_14
    IL_01db:  newobj     instance void S2::.ctor(int32,
                                                 int32)
    IL_01e0:  ldloc.s    V_11
    IL_01e2:  callvirt   instance int32 class Finder2<valuetype S2>::Find(!0,
                                                                          !0[])
    IL_01e7:  ldloc.s    V_14
    IL_01e9:  beq.s      IL_0201

    IL_01eb:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_01f0:  ldstr      "Error finding {0} using Finder<S2>.Find"
    IL_01f5:  ldloc.s    V_14
    IL_01f7:  box        [mscorlib]System.Int32
    IL_01fc:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0201:  ldloc.s    V_14
    IL_0203:  ldc.i4.1
    IL_0204:  add
    IL_0205:  stloc.s    V_14
    IL_0207:  ldloc.s    V_14
    IL_0209:  ldloc.0
    IL_020a:  blt.s      IL_01d5

    IL_020c:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0211:  ldstr      "Test PaSSeD (S2,1)"
    IL_0216:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_021b:  ldc.i4.0
    IL_021c:  stloc.s    V_15
    IL_021e:  br.s       IL_0250

    IL_0220:  ldloc.s    V_15
    IL_0222:  ldloc.s    V_15
    IL_0224:  newobj     instance void S2::.ctor(int32,
                                                 int32)
    IL_0229:  ldloc.s    V_11
    IL_022b:  call       int32 StaticFind2::Find<valuetype S2>(!!0,
                                                               !!0[])
    IL_0230:  ldloc.s    V_15
    IL_0232:  beq.s      IL_024a

    IL_0234:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0239:  ldstr      "Error finding {0} using StaticFind2.Find<S2>"
    IL_023e:  ldloc.s    V_15
    IL_0240:  box        [mscorlib]System.Int32
    IL_0245:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_024a:  ldloc.s    V_15
    IL_024c:  ldc.i4.1
    IL_024d:  add
    IL_024e:  stloc.s    V_15
    IL_0250:  ldloc.s    V_15
    IL_0252:  ldloc.0
    IL_0253:  blt.s      IL_0220

    IL_0255:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_025a:  ldstr      "Test PaSSeD (S2,2)"
    IL_025f:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0264:  ldloc.0
    IL_0265:  newarr     R1
    IL_026a:  stloc.s    V_16
    IL_026c:  ldc.i4.0
    IL_026d:  stloc.s    V_17
    IL_026f:  br.s       IL_0285

    IL_0271:  ldloc.s    V_16
    IL_0273:  ldloc.s    V_17
    IL_0275:  ldloc.s    V_17
    IL_0277:  ldloc.s    V_17
    IL_0279:  newobj     instance void R1::.ctor(int32,
                                                 int32)
    IL_027e:  stelem.ref
    IL_027f:  ldloc.s    V_17
    IL_0281:  ldc.i4.1
    IL_0282:  add
    IL_0283:  stloc.s    V_17
    IL_0285:  ldloc.s    V_17
    IL_0287:  ldloc.0
    IL_0288:  blt.s      IL_0271

    IL_028a:  newobj     instance void class Finder<class R1>::.ctor()
    IL_028f:  stloc.s    V_18
    IL_0291:  ldc.i4.0
    IL_0292:  stloc.s    V_19
    IL_0294:  br.s       IL_02c8

    IL_0296:  ldloc.s    V_18
    IL_0298:  ldloc.s    V_19
    IL_029a:  ldloc.s    V_19
    IL_029c:  newobj     instance void R1::.ctor(int32,
                                                 int32)
    IL_02a1:  ldloc.s    V_16
    IL_02a3:  callvirt   instance int32 class Finder<class R1>::Find(object,
                                                                     !0[])
    IL_02a8:  ldloc.s    V_19
    IL_02aa:  beq.s      IL_02c2

    IL_02ac:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_02b1:  ldstr      "Error finding {0} using Finder<R1>.Find"
    IL_02b6:  ldloc.s    V_19
    IL_02b8:  box        [mscorlib]System.Int32
    IL_02bd:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_02c2:  ldloc.s    V_19
    IL_02c4:  ldc.i4.1
    IL_02c5:  add
    IL_02c6:  stloc.s    V_19
    IL_02c8:  ldloc.s    V_19
    IL_02ca:  ldloc.0
    IL_02cb:  blt.s      IL_0296

    IL_02cd:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_02d2:  ldstr      "Test PaSSeD (R1,1)"
    IL_02d7:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_02dc:  ldc.i4.0
    IL_02dd:  stloc.s    V_20
    IL_02df:  br.s       IL_0311

    IL_02e1:  ldloc.s    V_20
    IL_02e3:  ldloc.s    V_20
    IL_02e5:  newobj     instance void R1::.ctor(int32,
                                                 int32)
    IL_02ea:  ldloc.s    V_16
    IL_02ec:  call       int32 StaticFind::Find<class R1>(object,
                                                          !!0[])
    IL_02f1:  ldloc.s    V_20
    IL_02f3:  beq.s      IL_030b

    IL_02f5:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_02fa:  ldstr      "Error finding {0} using StaticFind.Find<R1>"
    IL_02ff:  ldloc.s    V_20
    IL_0301:  box        [mscorlib]System.Int32
    IL_0306:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_030b:  ldloc.s    V_20
    IL_030d:  ldc.i4.1
    IL_030e:  add
    IL_030f:  stloc.s    V_20
    IL_0311:  ldloc.s    V_20
    IL_0313:  ldloc.0
    IL_0314:  blt.s      IL_02e1

    IL_0316:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_031b:  ldstr      "Test PaSSeD (R1,2)"
    IL_0320:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0325:  ldloc.0
    IL_0326:  newarr     R2
    IL_032b:  stloc.s    V_21
    IL_032d:  ldc.i4.0
    IL_032e:  stloc.s    V_22
    IL_0330:  br.s       IL_0346

    IL_0332:  ldloc.s    V_21
    IL_0334:  ldloc.s    V_22
    IL_0336:  ldloc.s    V_22
    IL_0338:  ldloc.s    V_22
    IL_033a:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_033f:  stelem.ref
    IL_0340:  ldloc.s    V_22
    IL_0342:  ldc.i4.1
    IL_0343:  add
    IL_0344:  stloc.s    V_22
    IL_0346:  ldloc.s    V_22
    IL_0348:  ldloc.0
    IL_0349:  blt.s      IL_0332

    IL_034b:  newobj     instance void class Finder2<class R2>::.ctor()
    IL_0350:  stloc.s    V_23
    IL_0352:  ldc.i4.0
    IL_0353:  stloc.s    V_24
    IL_0355:  br.s       IL_0389

    IL_0357:  ldloc.s    V_23
    IL_0359:  ldloc.s    V_24
    IL_035b:  ldloc.s    V_24
    IL_035d:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_0362:  ldloc.s    V_21
    IL_0364:  callvirt   instance int32 class Finder2<class R2>::Find(!0,
                                                                      !0[])
    IL_0369:  ldloc.s    V_24
    IL_036b:  beq.s      IL_0383

    IL_036d:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0372:  ldstr      "Error finding {0} using Finder<R2>.Find"
    IL_0377:  ldloc.s    V_24
    IL_0379:  box        [mscorlib]System.Int32
    IL_037e:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0383:  ldloc.s    V_24
    IL_0385:  ldc.i4.1
    IL_0386:  add
    IL_0387:  stloc.s    V_24
    IL_0389:  ldloc.s    V_24
    IL_038b:  ldloc.0
    IL_038c:  blt.s      IL_0357

    IL_038e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0393:  ldstr      "Test PaSSeD (R2,1)"
    IL_0398:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_039d:  ldc.i4.0
    IL_039e:  stloc.s    V_25
    IL_03a0:  br.s       IL_03d2

    IL_03a2:  ldloc.s    V_25
    IL_03a4:  ldloc.s    V_25
    IL_03a6:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_03ab:  ldloc.s    V_21
    IL_03ad:  call       int32 StaticFind2::Find<class R2>(!!0,
                                                           !!0[])
    IL_03b2:  ldloc.s    V_25
    IL_03b4:  beq.s      IL_03cc

    IL_03b6:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_03bb:  ldstr      "Error finding {0} using StaticFind2.Find<R2>"
    IL_03c0:  ldloc.s    V_25
    IL_03c2:  box        [mscorlib]System.Int32
    IL_03c7:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_03cc:  ldloc.s    V_25
    IL_03ce:  ldc.i4.1
    IL_03cf:  add
    IL_03d0:  stloc.s    V_25
    IL_03d2:  ldloc.s    V_25
    IL_03d4:  ldloc.0
    IL_03d5:  blt.s      IL_03a2

    IL_03d7:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_03dc:  ldstr      "Test PaSSeD (R2,2)"
    IL_03e1:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_03e6:  ldloc.0
    IL_03e7:  newarr     OS1
    IL_03ec:  stloc.s    V_26
    IL_03ee:  ldc.i4.0
    IL_03ef:  stloc.s    V_27
    IL_03f1:  br.s       IL_0409

    IL_03f3:  ldloc.s    V_26
    IL_03f5:  ldloc.s    V_27
    IL_03f7:  ldelema    OS1
    IL_03fc:  ldloc.s    V_27
    IL_03fe:  call       instance void OS1::.ctor(int32)
    IL_0403:  ldloc.s    V_27
    IL_0405:  ldc.i4.1
    IL_0406:  add
    IL_0407:  stloc.s    V_27
    IL_0409:  ldloc.s    V_27
    IL_040b:  ldloc.0
    IL_040c:  blt.s      IL_03f3

    IL_040e:  newobj     instance void class Finder<valuetype OS1>::.ctor()
    IL_0413:  stloc.s    V_28
    IL_0415:  ldc.i4.0
    IL_0416:  stloc.s    V_29
    IL_0418:  br.s       IL_044f

    IL_041a:  ldloc.s    V_28
    IL_041c:  ldloc.s    V_29
    IL_041e:  newobj     instance void OS1::.ctor(int32)
    IL_0423:  box        OS1
    IL_0428:  ldloc.s    V_26
    IL_042a:  callvirt   instance int32 class Finder<valuetype OS1>::Find(object,
                                                                          !0[])
    IL_042f:  ldloc.s    V_29
    IL_0431:  beq.s      IL_0449

    IL_0433:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0438:  ldstr      "Error finding {0} using Finder<OS1>.Find"
    IL_043d:  ldloc.s    V_29
    IL_043f:  box        [mscorlib]System.Int32
    IL_0444:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0449:  ldloc.s    V_29
    IL_044b:  ldc.i4.1
    IL_044c:  add
    IL_044d:  stloc.s    V_29
    IL_044f:  ldloc.s    V_29
    IL_0451:  ldloc.0
    IL_0452:  blt.s      IL_041a

    IL_0454:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0459:  ldstr      "Test PaSSeD (OS1,1)"
    IL_045e:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0463:  ldc.i4.0
    IL_0464:  stloc.s    V_30
    IL_0466:  br.s       IL_049b

    IL_0468:  ldloc.s    V_30
    IL_046a:  newobj     instance void OS1::.ctor(int32)
    IL_046f:  box        OS1
    IL_0474:  ldloc.s    V_26
    IL_0476:  call       int32 StaticFind::Find<valuetype OS1>(object,
                                                               !!0[])
    IL_047b:  ldloc.s    V_30
    IL_047d:  beq.s      IL_0495

    IL_047f:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0484:  ldstr      "Error finding {0} using StaticFind.Find<OS1>"
    IL_0489:  ldloc.s    V_30
    IL_048b:  box        [mscorlib]System.Int32
    IL_0490:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0495:  ldloc.s    V_30
    IL_0497:  ldc.i4.1
    IL_0498:  add
    IL_0499:  stloc.s    V_30
    IL_049b:  ldloc.s    V_30
    IL_049d:  ldloc.0
    IL_049e:  blt.s      IL_0468

    IL_04a0:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_04a5:  ldstr      "Test PaSSeD (OS1,2)"
    IL_04aa:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_04af:  ldloc.0
    IL_04b0:  newarr     OS2
    IL_04b5:  stloc.s    V_31
    IL_04b7:  ldc.i4.0
    IL_04b8:  stloc.s    V_32
    IL_04ba:  br.s       IL_04d2

    IL_04bc:  ldloc.s    V_31
    IL_04be:  ldloc.s    V_32
    IL_04c0:  ldelema    OS2
    IL_04c5:  ldloc.s    V_32
    IL_04c7:  call       instance void OS2::.ctor(int32)
    IL_04cc:  ldloc.s    V_32
    IL_04ce:  ldc.i4.1
    IL_04cf:  add
    IL_04d0:  stloc.s    V_32
    IL_04d2:  ldloc.s    V_32
    IL_04d4:  ldloc.0
    IL_04d5:  blt.s      IL_04bc

    IL_04d7:  newobj     instance void class Finder2<valuetype OS2>::.ctor()
    IL_04dc:  stloc.s    V_33
    IL_04de:  ldc.i4.0
    IL_04df:  stloc.s    V_34
    IL_04e1:  br.s       IL_0513

    IL_04e3:  ldloc.s    V_33
    IL_04e5:  ldloc.s    V_34
    IL_04e7:  newobj     instance void OS2::.ctor(int32)
    IL_04ec:  ldloc.s    V_31
    IL_04ee:  callvirt   instance int32 class Finder2<valuetype OS2>::Find(!0,
                                                                           !0[])
    IL_04f3:  ldloc.s    V_34
    IL_04f5:  beq.s      IL_050d

    IL_04f7:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_04fc:  ldstr      "Error finding {0} using Finder<OS2>.Find"
    IL_0501:  ldloc.s    V_34
    IL_0503:  box        [mscorlib]System.Int32
    IL_0508:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_050d:  ldloc.s    V_34
    IL_050f:  ldc.i4.1
    IL_0510:  add
    IL_0511:  stloc.s    V_34
    IL_0513:  ldloc.s    V_34
    IL_0515:  ldloc.0
    IL_0516:  blt.s      IL_04e3

    IL_0518:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_051d:  ldstr      "Test PaSSeD (OS2,1)"
    IL_0522:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0527:  ldc.i4.0
    IL_0528:  stloc.s    V_35
    IL_052a:  br.s       IL_055a

    IL_052c:  ldloc.s    V_35
    IL_052e:  newobj     instance void OS2::.ctor(int32)
    IL_0533:  ldloc.s    V_31
    IL_0535:  call       int32 StaticFind2::Find<valuetype OS2>(!!0,
                                                                !!0[])
    IL_053a:  ldloc.s    V_35
    IL_053c:  beq.s      IL_0554

    IL_053e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0543:  ldstr      "Error finding {0} using StaticFind2.Find<OS2>"
    IL_0548:  ldloc.s    V_35
    IL_054a:  box        [mscorlib]System.Int32
    IL_054f:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0554:  ldloc.s    V_35
    IL_0556:  ldc.i4.1
    IL_0557:  add
    IL_0558:  stloc.s    V_35
    IL_055a:  ldloc.s    V_35
    IL_055c:  ldloc.0
    IL_055d:  blt.s      IL_052c

    IL_055f:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0564:  ldstr      "Test PaSSeD (OS2,2)"
    IL_0569:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_056e:  ldloc.0
    IL_056f:  newarr     OR1
    IL_0574:  stloc.s    V_36
    IL_0576:  ldc.i4.0
    IL_0577:  stloc.s    V_37
    IL_0579:  br.s       IL_058d

    IL_057b:  ldloc.s    V_36
    IL_057d:  ldloc.s    V_37
    IL_057f:  ldloc.s    V_37
    IL_0581:  newobj     instance void OR1::.ctor(int32)
    IL_0586:  stelem.ref
    IL_0587:  ldloc.s    V_37
    IL_0589:  ldc.i4.1
    IL_058a:  add
    IL_058b:  stloc.s    V_37
    IL_058d:  ldloc.s    V_37
    IL_058f:  ldloc.0
    IL_0590:  blt.s      IL_057b

    IL_0592:  newobj     instance void class Finder<class OR1>::.ctor()
    IL_0597:  stloc.s    V_38
    IL_0599:  ldc.i4.0
    IL_059a:  stloc.s    V_39
    IL_059c:  br.s       IL_05ce

    IL_059e:  ldloc.s    V_38
    IL_05a0:  ldloc.s    V_39
    IL_05a2:  newobj     instance void OR1::.ctor(int32)
    IL_05a7:  ldloc.s    V_36
    IL_05a9:  callvirt   instance int32 class Finder<class OR1>::Find(object,
                                                                      !0[])
    IL_05ae:  ldloc.s    V_39
    IL_05b0:  beq.s      IL_05c8

    IL_05b2:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_05b7:  ldstr      "Error finding {0} using Finder<OR1>.Find"
    IL_05bc:  ldloc.s    V_39
    IL_05be:  box        [mscorlib]System.Int32
    IL_05c3:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_05c8:  ldloc.s    V_39
    IL_05ca:  ldc.i4.1
    IL_05cb:  add
    IL_05cc:  stloc.s    V_39
    IL_05ce:  ldloc.s    V_39
    IL_05d0:  ldloc.0
    IL_05d1:  blt.s      IL_059e

    IL_05d3:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_05d8:  ldstr      "Test PaSSeD (OR1,1)"
    IL_05dd:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_05e2:  ldc.i4.0
    IL_05e3:  stloc.s    V_40
    IL_05e5:  br.s       IL_0615

    IL_05e7:  ldloc.s    V_40
    IL_05e9:  newobj     instance void OR1::.ctor(int32)
    IL_05ee:  ldloc.s    V_36
    IL_05f0:  call       int32 StaticFind::Find<class OR1>(object,
                                                           !!0[])
    IL_05f5:  ldloc.s    V_40
    IL_05f7:  beq.s      IL_060f

    IL_05f9:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_05fe:  ldstr      "Error finding {0} using StaticFind.Find<OR1>"
    IL_0603:  ldloc.s    V_40
    IL_0605:  box        [mscorlib]System.Int32
    IL_060a:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_060f:  ldloc.s    V_40
    IL_0611:  ldc.i4.1
    IL_0612:  add
    IL_0613:  stloc.s    V_40
    IL_0615:  ldloc.s    V_40
    IL_0617:  ldloc.0
    IL_0618:  blt.s      IL_05e7

    IL_061a:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_061f:  ldstr      "Test PaSSeD (OR1,2)"
    IL_0624:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0629:  ldloc.0
    IL_062a:  newarr     OR2
    IL_062f:  stloc.s    V_41
    IL_0631:  ldc.i4.0
    IL_0632:  stloc.s    V_42
    IL_0634:  br.s       IL_0648

    IL_0636:  ldloc.s    V_41
    IL_0638:  ldloc.s    V_42
    IL_063a:  ldloc.s    V_42
    IL_063c:  newobj     instance void OR2::.ctor(int32)
    IL_0641:  stelem.ref
    IL_0642:  ldloc.s    V_42
    IL_0644:  ldc.i4.1
    IL_0645:  add
    IL_0646:  stloc.s    V_42
    IL_0648:  ldloc.s    V_42
    IL_064a:  ldloc.0
    IL_064b:  blt.s      IL_0636

    IL_064d:  newobj     instance void class Finder2<class OR2>::.ctor()
    IL_0652:  stloc.s    V_43
    IL_0654:  ldc.i4.0
    IL_0655:  stloc.s    V_44
    IL_0657:  br.s       IL_0689

    IL_0659:  ldloc.s    V_43
    IL_065b:  ldloc.s    V_44
    IL_065d:  newobj     instance void OR2::.ctor(int32)
    IL_0662:  ldloc.s    V_41
    IL_0664:  callvirt   instance int32 class Finder2<class OR2>::Find(!0,
                                                                       !0[])
    IL_0669:  ldloc.s    V_44
    IL_066b:  beq.s      IL_0683

    IL_066d:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0672:  ldstr      "Error finding {0} using Finder<OR2>.Find"
    IL_0677:  ldloc.s    V_44
    IL_0679:  box        [mscorlib]System.Int32
    IL_067e:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0683:  ldloc.s    V_44
    IL_0685:  ldc.i4.1
    IL_0686:  add
    IL_0687:  stloc.s    V_44
    IL_0689:  ldloc.s    V_44
    IL_068b:  ldloc.0
    IL_068c:  blt.s      IL_0659

    IL_068e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0693:  ldstr      "Test PaSSeD (OR2,1)"
    IL_0698:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_069d:  ldc.i4.0
    IL_069e:  stloc.s    V_45
    IL_06a0:  br.s       IL_06d0

    IL_06a2:  ldloc.s    V_45
    IL_06a4:  newobj     instance void OR2::.ctor(int32)
    IL_06a9:  ldloc.s    V_41
    IL_06ab:  call       int32 StaticFind2::Find<class OR2>(!!0,
                                                            !!0[])
    IL_06b0:  ldloc.s    V_45
    IL_06b2:  beq.s      IL_06ca

    IL_06b4:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_06b9:  ldstr      "Error finding {0} using StaticFind2.Find<OR2>"
    IL_06be:  ldloc.s    V_45
    IL_06c0:  box        [mscorlib]System.Int32
    IL_06c5:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_06ca:  ldloc.s    V_45
    IL_06cc:  ldc.i4.1
    IL_06cd:  add
    IL_06ce:  stloc.s    V_45
    IL_06d0:  ldloc.s    V_45
    IL_06d2:  ldloc.0
    IL_06d3:  blt.s      IL_06a2

    IL_06d5:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_06da:  ldstr      "Test PaSSeD (OR2,2)"
    IL_06df:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_085a:  ldc.i4.s   0          // Pass
    IL_085c:  stloc.s    V_56
    IL_085e:  br.s       IL_0864

    IL_0860:  ldc.i4.s   1          // Fail
    IL_0862:  stloc.s    V_56

    IL_0864:  ldloc.s    V_56
    IL_0866:  ret
  } // end of method Maine::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Maine::.ctor

} // end of class Maine


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
// WARNING: Created Win32 resource file constrained-il.res
