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

// test for unaligned prefix

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly unaligned
{
  .permissionset reqmin = (3C 00 50 00 65 00 72 00 6D 00 69 00 73 00 73 00   // <.P.e.r.m.i.s.s.
                           69 00 6F 00 6E 00 53 00 65 00 74 00 20 00 63 00   // i.o.n.S.e.t. .c.
                           6C 00 61 00 73 00 73 00 3D 00 22 00 53 00 79 00   // l.a.s.s.=.".S.y.
                           73 00 74 00 65 00 6D 00 2E 00 53 00 65 00 63 00   // s.t.e.m...S.e.c.
                           75 00 72 00 69 00 74 00 79 00 2E 00 50 00 65 00   // u.r.i.t.y...P.e.
                           72 00 6D 00 69 00 73 00 73 00 69 00 6F 00 6E 00   // r.m.i.s.s.i.o.n.
                           53 00 65 00 74 00 22 00 0D 00 0A 00 20 00 20 00   // S.e.t."..... . .
                           20 00 20 00 20 00 20 00 20 00 20 00 20 00 20 00   //  . . . . . . . .
                           20 00 20 00 20 00 20 00 20 00 76 00 65 00 72 00   //  . . . . .v.e.r.
                           73 00 69 00 6F 00 6E 00 3D 00 22 00 31 00 22 00   // s.i.o.n.=.".1.".
                           3E 00 0D 00 0A 00 20 00 20 00 20 00 3C 00 49 00   // >..... . . .<.I.
                           50 00 65 00 72 00 6D 00 69 00 73 00 73 00 69 00   // P.e.r.m.i.s.s.i.
                           6F 00 6E 00 20 00 63 00 6C 00 61 00 73 00 73 00   // o.n. .c.l.a.s.s.
                           3D 00 22 00 53 00 79 00 73 00 74 00 65 00 6D 00   // =.".S.y.s.t.e.m.
                           2E 00 53 00 65 00 63 00 75 00 72 00 69 00 74 00   // ..S.e.c.u.r.i.t.
                           79 00 2E 00 50 00 65 00 72 00 6D 00 69 00 73 00   // y...P.e.r.m.i.s.
                           73 00 69 00 6F 00 6E 00 73 00 2E 00 53 00 65 00   // s.i.o.n.s...S.e.
                           63 00 75 00 72 00 69 00 74 00 79 00 50 00 65 00   // c.u.r.i.t.y.P.e.
                           72 00 6D 00 69 00 73 00 73 00 69 00 6F 00 6E 00   // r.m.i.s.s.i.o.n.
                           2C 00 20 00 6D 00 73 00 63 00 6F 00 72 00 6C 00   // ,. .m.s.c.o.r.l.
                           69 00 62 00 2C 00 20 00 56 00 65 00 72 00 73 00   // i.b.,. .V.e.r.s.
                           69 00 6F 00 6E 00 3D 00 31 00 2E 00 30 00 2E 00   // i.o.n.=.1...0...
                           33 00 33 00 30 00 30 00 2E 00 30 00 2C 00 20 00   // 3.3.0.0...0.,. .
                           43 00 75 00 6C 00 74 00 75 00 72 00 65 00 3D 00   // C.u.l.t.u.r.e.=.
                           6E 00 65 00 75 00 74 00 72 00 61 00 6C 00 2C 00   // n.e.u.t.r.a.l.,.
                           20 00 50 00 75 00 62 00 6C 00 69 00 63 00 4B 00   //  .P.u.b.l.i.c.K.
                           65 00 79 00 54 00 6F 00 6B 00 65 00 6E 00 3D 00   // e.y.T.o.k.e.n.=.
                           62 00 37 00 37 00 61 00 35 00 63 00 35 00 36 00   // b.7.7.a.5.c.5.6.
                           31 00 39 00 33 00 34 00 65 00 30 00 38 00 39 00   // 1.9.3.4.e.0.8.9.
                           22 00 0D 00 0A 00 20 00 20 00 20 00 20 00 20 00   // "..... . . . . .
                           20 00 20 00 20 00 20 00 20 00 20 00 20 00 20 00   //  . . . . . . . .
                           20 00 20 00 20 00 76 00 65 00 72 00 73 00 69 00   //  . . .v.e.r.s.i.
                           6F 00 6E 00 3D 00 22 00 31 00 22 00 0D 00 0A 00   // o.n.=.".1.".....
                           20 00 20 00 20 00 20 00 20 00 20 00 20 00 20 00   //  . . . . . . . .
                           20 00 20 00 20 00 20 00 20 00 20 00 20 00 20 00   //  . . . . . . . .
                           46 00 6C 00 61 00 67 00 73 00 3D 00 22 00 53 00   // F.l.a.g.s.=.".S.
                           6B 00 69 00 70 00 56 00 65 00 72 00 69 00 66 00   // k.i.p.V.e.r.i.f.
                           69 00 63 00 61 00 74 00 69 00 6F 00 6E 00 22 00   // i.c.a.t.i.o.n.".
                           2F 00 3E 00 0D 00 0A 00 3C 00 2F 00 50 00 65 00   // /.>.....<./.P.e.
                           72 00 6D 00 69 00 73 00 73 00 69 00 6F 00 6E 00   // r.m.i.s.s.i.o.n.
                           53 00 65 00 74 00 3E 00 0D 00 0A 00 )             // S.e.t.>.....
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module unaligned.exe
.custom instance void [mscorlib]System.Security.UnverifiableCodeAttribute::.ctor() = ( 01 00 00 00 ) 
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class private sequential ansi sealed beforefieldinit SmallValue
       extends [mscorlib]System.ValueType
{
} // end of class SmallValue

.class private explicit ansi sealed beforefieldinit UnalignedValue
       extends [mscorlib]System.ValueType
{
} // end of class UnalignedValue

.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
} // end of class MainApp


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class private sequential ansi sealed beforefieldinit SmallValue
       extends [mscorlib]System.ValueType
{
  .field public float64 x
} // end of class SmallValue

.class private explicit ansi sealed beforefieldinit UnalignedValue
       extends [mscorlib]System.ValueType
{
  .field [1] public int8 a
  .field [1] public int16 b
  .field [1] public int32 c
  .field [1] public int64 d
  .field [1] public valuetype SmallValue x
} // end of class UnalignedValue

.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    // Code size       344 (0x158)
    .maxstack  3
    .locals (valuetype UnalignedValue V_0,
             int64 V_1,
             valuetype SmallValue V_2,
             unsigned int8* V_3,
             unsigned int16* V_4,
             unsigned int32* V_5,
             unsigned int64* V_6,
             unsigned int64 V_7,
             int32 V_8)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    UnalignedValue
    IL_0008:  ldloca.s   V_0
    IL_000a:  ldc.i8     0xffffffffffffff0
              unaligned. 0x1
    IL_0013:  stfld      int64 UnalignedValue::d
    IL_0018:  ldloca.s   V_0
    IL_001a:  ldc.i4     0x77777777
              unaligned. 0x1
    IL_001f:  stfld      int32 UnalignedValue::c
    IL_0024:  ldloca.s   V_0
    IL_0026:  ldc.i4     0x3333
              unaligned. 0x1
    IL_002b:  stfld      int16 UnalignedValue::b
    IL_0030:  ldloca.s   V_0
    IL_0032:  ldc.i4.s   17
              unaligned. 0x1
    IL_0034:  stfld      int8 UnalignedValue::a
    IL_0039:  ldloca.s   V_0
              unaligned. 0x1
    IL_003b:  ldfld      int8 UnalignedValue::a
    IL_0040:  conv.i4
    IL_0041:  ldloca.s   V_0
              unaligned. 0x1
    IL_0043:  ldfld      int16 UnalignedValue::b
    IL_0048:  add
    IL_0049:  ldloca.s   V_0
              unaligned. 0x1
    IL_004b:  ldfld      int32 UnalignedValue::c
    IL_0050:  add
    IL_0051:  conv.i8
    IL_0052:  ldloca.s   V_0
              unaligned. 0x1
    IL_0054:  ldfld      int64 UnalignedValue::d
    IL_0059:  add
    IL_005a:  stloc.1
    IL_005b:  ldloc.1              
    IL_005c:  ldsfld     bool [mscorlib]System.BitConverter::IsLittleEndian
    IL_0061:  brtrue.s   IL_006b

    IL_0063:  ldc.i8     0x11337778113388AB
    IL_0069:  br.s       IL_0074

    IL_006b:  ldc.i8     0xfffffffeeee9944
    IL_0074:  beq.s      IL_0092

    IL_0076:  ldstr      "Unexpected long value:"
    IL_007b:  ldloca.s   V_1
    IL_007d:  call       instance string [mscorlib]System.Int64::ToString()
    IL_0082:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_0087:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_008c:  ldc.i4.1
    IL_008d:  call       void [mscorlib]System.Environment::Exit(int32)
    IL_0092:  ldloca.s   V_0
              unaligned. 0x1
    IL_0094:  ldfld      valuetype SmallValue UnalignedValue::x
    IL_0099:  stloc.2
    IL_009a:  ldloca.s   V_2
    IL_009c:  dup
    IL_009d:  ldfld      float64 SmallValue::x
    IL_00a2:  ldc.r8     1.
    IL_00ab:  add
    IL_00ac:  stfld      float64 SmallValue::x
    IL_00b1:  ldloca.s   V_0
    IL_00b3:  ldloc.2
              unaligned. 0x1
    IL_00b4:  stfld      valuetype SmallValue UnalignedValue::x
    IL_00b9:  ldloca.s   V_0
    IL_00bb:  ldflda     int8 UnalignedValue::a
    IL_00c0:  stloc.3
    IL_00c1:  ldloca.s   V_0
    IL_00c3:  ldflda     int16 UnalignedValue::b
    IL_00c8:  stloc.s    V_4
    IL_00ca:  ldloca.s   V_0
    IL_00cc:  ldflda     int32 UnalignedValue::c
    IL_00d1:  stloc.s    V_5
    IL_00d3:  ldloca.s   V_0
    IL_00d5:  ldflda     int64 UnalignedValue::d
    IL_00da:  stloc.s    V_6
    IL_00dc:  ldloc.s    V_6
    IL_00de:  ldc.i8     0xeeeeeeeeeeeeee0
              unaligned. 0x1
    IL_00e7:  stind.i8
    IL_00e8:  ldloc.s    V_5
    IL_00ea:  ldc.i4     0x66666666
              unaligned. 0x1
    IL_00ef:  stind.i4
    IL_00f0:  ldloc.s    V_4
    IL_00f2:  ldc.i4     0x2222
              unaligned. 0x1
    IL_00f7:  stind.i2
    IL_00f8:  ldloc.3
    IL_00f9:  ldc.i4.0
              unaligned. 0x1
    IL_00fa:  stind.i1
    IL_00fb:  ldloc.3
              unaligned. 0x1
    IL_00fc:  ldind.u1
    IL_00fd:  conv.u8
    IL_00fe:  ldloc.s    V_4
              unaligned. 0x1
    IL_0100:  ldind.u2
    IL_0101:  conv.u8
    IL_0102:  add
    IL_0103:  ldloc.s    V_5
              unaligned. 0x1
    IL_0105:  ldind.u4
    IL_0106:  conv.u8
    IL_0107:  add
    IL_0108:  ldloc.s    V_6
              unaligned. 0x1
    IL_010a:  ldind.i8
    IL_010b:  add
    IL_010c:  stloc.s    V_7
    IL_010e:  ldloc.s    V_7
    IL_0110:  ldsfld     bool [mscorlib]System.BitConverter::IsLittleEndian
    IL_0115:  brtrue.s   IL_011f

    IL_0117:  ldc.i8     0x226666EF115568
    IL_011d:  br.s       IL_0128

    IL_011f:  ldc.i8     0xeeeeeeecccc6600
    IL_0128:  beq.s      IL_0146

    IL_012a:  ldstr      "Unexpected ulong value:"
    IL_012f:  ldloca.s   V_7
    IL_0131:  call       instance string [mscorlib]System.UInt64::ToString()
    IL_0136:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_013b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0140:  ldc.i4.2
    IL_0141:  call       void [mscorlib]System.Environment::Exit(int32)
    IL_0146:  ldstr      "PASSED"
    IL_014b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0150:  ldc.i4.0
    IL_0151:  stloc.s    V_8
    IL_0153:  br.s       IL_0155

    IL_0155:  ldloc.s    V_8
    IL_0157:  ret
  } // end of method MainApp::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

} // end of class MainApp
