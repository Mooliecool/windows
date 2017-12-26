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
  .hash = (D3 27 78 BC E2 E2 FB F9 BF C1 5C DE BD 4D 18 4E   // .'x.......\..M.N
           13 9D 74 FF )                                     // ..t.
  .ver 1:2:3400:0
}
.assembly legacy library tailunit
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module tailunit.exe
// MVID: {5c24228b-3c1d-0a0b-4bc8-393428940ca8}
.imagebase 0x00400000
.file alignment 0x00000200
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000001    //  ILONLY
// Image base: 0xFECE0000


// =============== CLASS MEMBERS DECLARATION ===================

.class public sequential ansi sealed beforefieldinit Point
       extends [mscorlib]System.ValueType
{
  .field public int32 x
  .field public int32 y
} // end of class Point

.class public auto ansi beforefieldinit TailTests
       extends [mscorlib]System.Object
{
  .field private int32 m_failed
  .field private static int32 s_testInt
  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    // Code size       942 (0x3ae)
    .maxstack  13
    .locals init (class TailTests V_0,
             int32 V_1,
             valuetype Point V_2,
             float64 V_3,
             class [mscorlib]System.Exception V_4,
             int32 V_5,
             int32 V_6)
    IL_0000:  newobj     instance void TailTests::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldc.i4.0
    IL_0007:  stloc.1
    IL_0008:  ldloca.s   V_2
    IL_000a:  initobj    Point
    IL_0010:  ldc.r8     0.0
    IL_0019:  stloc.3
    .try
    {
      IL_001a:  ldloc.0
      IL_001b:  ldc.i4.s   100
      IL_001d:  ldc.i4.0
      IL_001e:  ldc.i4.1
      IL_001f:  ldc.i4.2
      IL_0020:  ldc.i4.3
      IL_0021:  ldc.i4.4
      IL_0022:  ldc.i4.5
      IL_0023:  ldc.i4.6
      IL_0024:  ldc.i4.7
      IL_0025:  ldc.i4.8
      IL_0026:  ldc.i4.s   9
      IL_0028:  ldc.i4.0
      IL_0029:  callvirt   instance int32 TailTests::ManyArgs(int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32,
                                                              int32)
      IL_002e:  stloc.1
      IL_002f:  ldloc.0
      IL_0030:  ldloc.1
      IL_0031:  ldc.i4.s   100
      IL_0033:  ceq
      IL_0035:  ldstr      "ManyArgs"
      IL_003a:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_003f:  ldc.i4.0
      IL_0040:  stloc.1
      IL_0041:  ldc.i4.s   100
      IL_0043:  ldc.i4.0
      IL_0044:  ldc.i4.1
      IL_0045:  ldc.i4.2
      IL_0046:  ldc.i4.3
      IL_0047:  ldc.i4.4
      IL_0048:  ldc.i4.5
      IL_0049:  ldc.i4.6
      IL_004a:  ldc.i4.7
      IL_004b:  ldc.i4.8
      IL_004c:  ldc.i4.s   9
      IL_004e:  ldc.i4.0
      IL_004f:  call       int32 TailTests::ManyArgsStatic(int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32,
                                                           int32)
      IL_0054:  stloc.1
      IL_0055:  ldloc.0
      IL_0056:  ldloc.1
      IL_0057:  ldc.i4.s   100
      IL_0059:  ceq
      IL_005b:  ldstr      "ManyArgsStatic"
      IL_0060:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0065:  ldc.i4.0
      IL_0066:  stloc.1
      IL_0067:  ldloc.0
      IL_0068:  ldc.i4.0
      IL_0069:  ldc.i4     0x989680
      IL_006e:  callvirt   instance int32 TailTests::Sum(int32,
                                                         int32)
      IL_0073:  stloc.1
      IL_0074:  ldloc.0
      IL_0075:  ldloc.1
      IL_0076:  ldc.i4     0x989680
      IL_007b:  ceq
      IL_007d:  ldstr      "SumBlowStack"
      IL_0082:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0087:  ldc.i4.0
      IL_0088:  stloc.1
      IL_0089:  ldc.i4.0
      IL_008a:  ldc.i4     0x989680
      IL_008f:  call       int32 TailTests::SumStatic(int32,
                                                      int32)
      IL_0094:  stloc.1
      IL_0095:  ldloc.0
      IL_0096:  ldloc.1
      IL_0097:  ldc.i4     0x989680
      IL_009c:  ceq
      IL_009e:  ldstr      "SumStaticBlowStack"
      IL_00a3:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_00a8:  ldloca.s   V_2
      IL_00aa:  ldc.i4.1
      IL_00ab:  stfld      int32 Point::x
      IL_00b0:  ldloca.s   V_2
      IL_00b2:  ldc.i4.2
      IL_00b3:  stfld      int32 Point::y
      IL_00b8:  ldloc.0
      IL_00b9:  ldc.i4.4
      IL_00ba:  ldloc.2
      IL_00bb:  callvirt   instance int32 TailTests::StructFacSum(int32,
                                                                  valuetype Point)
      IL_00c0:  stloc.1
      IL_00c1:  ldloc.0
      IL_00c2:  ldloc.1
      IL_00c3:  ldc.i4.s   40
      IL_00c5:  bne.un.s   IL_00dd

      IL_00c7:  ldloca.s   V_2
      IL_00c9:  ldfld      int32 Point::x
      IL_00ce:  ldc.i4.1
      IL_00cf:  bne.un.s   IL_00dd

      IL_00d1:  ldloca.s   V_2
      IL_00d3:  ldfld      int32 Point::y
      IL_00d8:  ldc.i4.2
      IL_00d9:  ceq
      IL_00db:  br.s       IL_00de

      IL_00dd:  ldc.i4.0
      IL_00de:  ldstr      "StructShallowReg"
      IL_00e3:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_00e8:  ldloc.0
      IL_00e9:  ldc.i4.5
      IL_00ea:  ldc.r8     1
      IL_00f3:  callvirt   instance float64 TailTests::DoubleFacRec(int32,
                                                                    float64)
      IL_00f8:  stloc.3
      IL_00f9:  ldloc.0
      IL_00fa:  ldloc.3
      IL_00fb:  ldc.r8     120
      IL_0104:  ceq
      IL_0106:  ldstr      "DoubleFacRec"
      IL_010b:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0110:  ldloc.0
      IL_0111:  ldc.i4.1
      IL_0112:  ldc.i4.2
      IL_0113:  ldc.i4.3
      IL_0114:  callvirt   instance int32 TailTests::fUneven(int32,
                                                             int32,
                                                             int32)
      IL_0119:  stloc.1
      IL_011a:  ldloc.0
      IL_011b:  ldloc.1
      IL_011c:  ldc.i4.s   16
      IL_011e:  ceq
      IL_0120:  ldstr      "Uneven"
      IL_0125:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_012a:  ldloca.s   V_2
      IL_012c:  ldc.i4.1
      IL_012d:  stfld      int32 Point::x
      IL_0132:  ldloca.s   V_2
      IL_0134:  ldc.i4.2
      IL_0135:  stfld      int32 Point::y
      IL_013a:  ldloc.0
      IL_013b:  ldc.i4     0x186a0
      IL_0140:  ldloc.2
      IL_0141:  callvirt   instance int32 TailTests::StructFacSumDeep(int32,
                                                                      valuetype Point)
      IL_0146:  stloc.1
      IL_0147:  ldloc.0
      IL_0148:  ldloc.1
      IL_0149:  ldc.i4     0x30d43
      IL_014e:  bne.un.s   IL_0166

      IL_0150:  ldloca.s   V_2
      IL_0152:  ldfld      int32 Point::x
      IL_0157:  ldc.i4.1
      IL_0158:  bne.un.s   IL_0166

      IL_015a:  ldloca.s   V_2
      IL_015c:  ldfld      int32 Point::y
      IL_0161:  ldc.i4.2
      IL_0162:  ceq
      IL_0164:  br.s       IL_0167

      IL_0166:  ldc.i4.0
      IL_0167:  ldstr      "StructRegDeep"
      IL_016c:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0171:  ldloca.s   V_2
      IL_0173:  ldc.i4.1
      IL_0174:  stfld      int32 Point::x
      IL_0179:  ldloca.s   V_2
      IL_017b:  ldc.i4.2
      IL_017c:  stfld      int32 Point::y
      IL_0181:  ldc.i4.0
      IL_0182:  stloc.1
      IL_0183:  ldc.i4     0x186a0
      IL_0188:  ldloc.2
      IL_0189:  call       int32 TailTests::StructFacSumDeepStatic(int32,
                                                                   valuetype Point)
      IL_018e:  stloc.1
      IL_018f:  ldloc.0
      IL_0190:  ldloc.1
      IL_0191:  ldc.i4     0x30d43
      IL_0196:  bne.un.s   IL_01ae

      IL_0198:  ldloca.s   V_2
      IL_019a:  ldfld      int32 Point::x
      IL_019f:  ldc.i4.1
      IL_01a0:  bne.un.s   IL_01ae

      IL_01a2:  ldloca.s   V_2
      IL_01a4:  ldfld      int32 Point::y
      IL_01a9:  ldc.i4.2
      IL_01aa:  ceq
      IL_01ac:  br.s       IL_01af

      IL_01ae:  ldc.i4.0
      IL_01af:  ldstr      "StructRegDeepStatic"
      IL_01b4:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_01b9:  ldloca.s   V_2
      IL_01bb:  ldc.i4.1
      IL_01bc:  stfld      int32 Point::x
      IL_01c1:  ldloca.s   V_2
      IL_01c3:  ldc.i4.2
      IL_01c4:  stfld      int32 Point::y
      IL_01c9:  ldc.i4.0
      IL_01ca:  stloc.1
      IL_01cb:  ldloc.0
      IL_01cc:  ldc.i4     0x186a0
      IL_01d1:  ldc.i4.0
      IL_01d2:  ldc.r8     0.0
      IL_01db:  ldc.r8     0.90000000000000002
      IL_01e4:  ldloc.2
      IL_01e5:  ldc.i4.1
      IL_01e6:  callvirt   instance int32 TailTests::StructFacSumDeepStack(int32,
                                                                           int32,
                                                                           float64,
                                                                           float64,
                                                                           valuetype Point,
                                                                           int32)
      IL_01eb:  stloc.1
      IL_01ec:  ldloc.0
      IL_01ed:  ldloc.1
      IL_01ee:  ldc.i4     0x30d43
      IL_01f3:  bne.un.s   IL_020b

      IL_01f5:  ldloca.s   V_2
      IL_01f7:  ldfld      int32 Point::x
      IL_01fc:  ldc.i4.1
      IL_01fd:  bne.un.s   IL_020b

      IL_01ff:  ldloca.s   V_2
      IL_0201:  ldfld      int32 Point::y
      IL_0206:  ldc.i4.2
      IL_0207:  ceq
      IL_0209:  br.s       IL_020c

      IL_020b:  ldc.i4.0
      IL_020c:  ldstr      "StructStackDeep"
      IL_0211:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0216:  ldloca.s   V_2
      IL_0218:  ldc.i4.1
      IL_0219:  stfld      int32 Point::x
      IL_021e:  ldloca.s   V_2
      IL_0220:  ldc.i4.2
      IL_0221:  stfld      int32 Point::y
      IL_0226:  ldc.i4.0
      IL_0227:  stloc.1
      IL_0228:  ldc.i4     0x186a0
      IL_022d:  ldc.i4.0
      IL_022e:  ldc.r8     0.0
      IL_0237:  ldc.r8     0.90000000000000002
      IL_0240:  ldloc.2
      IL_0241:  ldc.i4.1
      IL_0242:  call       int32 TailTests::StructFacSumDeepStackStatic(int32,
                                                                        int32,
                                                                        float64,
                                                                        float64,
                                                                        valuetype Point,
                                                                        int32)
      IL_0247:  stloc.1
      IL_0248:  ldloc.0
      IL_0249:  ldloc.1
      IL_024a:  ldc.i4     0x30d43
      IL_024f:  bne.un.s   IL_0267

      IL_0251:  ldloca.s   V_2
      IL_0253:  ldfld      int32 Point::x
      IL_0258:  ldc.i4.1
      IL_0259:  bne.un.s   IL_0267

      IL_025b:  ldloca.s   V_2
      IL_025d:  ldfld      int32 Point::y
      IL_0262:  ldc.i4.2
      IL_0263:  ceq
      IL_0265:  br.s       IL_0268

      IL_0267:  ldc.i4.0
      IL_0268:  ldstr      "StructStackDeepStatic"
      IL_026d:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0272:  ldloca.s   V_2
      IL_0274:  ldloca.s   V_2
      IL_0276:  ldc.i4.0
      IL_0277:  dup
      IL_0278:  stloc.s    V_6
      IL_027a:  stfld      int32 Point::y
      IL_027f:  ldloc.s    V_6
      IL_0281:  stfld      int32 Point::x
      IL_0286:  ldloc.0
      IL_0287:  ldc.i4     0x186a0
      IL_028c:  ldc.i4.0
      IL_028d:  callvirt   instance valuetype Point TailTests::StructReturnVal(int32,
                                                                               int32)
      IL_0292:  stloc.2
      IL_0293:  ldloc.0
      IL_0294:  ldloca.s   V_2
      IL_0296:  ldfld      int32 Point::x
      IL_029b:  ldc.i4     0x186a0
      IL_02a0:  bne.un.s   IL_02b2

      IL_02a2:  ldloca.s   V_2
      IL_02a4:  ldfld      int32 Point::y
      IL_02a9:  ldc.i4     0x30d40
      IL_02ae:  ceq
      IL_02b0:  br.s       IL_02b3

      IL_02b2:  ldc.i4.0
      IL_02b3:  ldstr      "StructReturnDeep"
      IL_02b8:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_02bd:  ldloca.s   V_2
      IL_02bf:  ldloca.s   V_2
      IL_02c1:  ldc.i4.0
      IL_02c2:  dup
      IL_02c3:  stloc.s    V_6
      IL_02c5:  stfld      int32 Point::y
      IL_02ca:  ldloc.s    V_6
      IL_02cc:  stfld      int32 Point::x
      IL_02d1:  ldc.i4     0x186a0
      IL_02d6:  ldc.i4.0
      IL_02d7:  call       valuetype Point TailTests::StructReturnValStatic(int32,
                                                                            int32)
      IL_02dc:  stloc.2
      IL_02dd:  ldloc.0
      IL_02de:  ldloca.s   V_2
      IL_02e0:  ldfld      int32 Point::x
      IL_02e5:  ldc.i4     0x186a0
      IL_02ea:  bne.un.s   IL_02fc

      IL_02ec:  ldloca.s   V_2
      IL_02ee:  ldfld      int32 Point::y
      IL_02f3:  ldc.i4     0x30d40
      IL_02f8:  ceq
      IL_02fa:  br.s       IL_02fd

      IL_02fc:  ldc.i4.0
      IL_02fd:  ldstr      "StructReturnDeepStatic"
      IL_0302:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0307:  ldc.i4.0
      IL_0308:  stsfld     int32 TailTests::s_testInt
      IL_030d:  ldloc.0
      IL_030e:  callvirt   instance void TailTests::NoArgs()
      IL_0313:  ldloc.0
      IL_0314:  ldsfld     int32 TailTests::s_testInt
      IL_0319:  ldc.i4     0x61a80
      IL_031e:  ceq
      IL_0320:  ldstr      "NoArgs"
      IL_0325:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_032a:  ldc.i4.0
      IL_032b:  stsfld     int32 TailTests::s_testInt
      IL_0330:  call       void TailTests::NoArgsStatic()
      IL_0335:  ldloc.0
      IL_0336:  ldsfld     int32 TailTests::s_testInt
      IL_033b:  ldc.i4     0x61a80
      IL_0340:  ceq
      IL_0342:  ldstr      "NoArgsStatic"
      IL_0347:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_034c:  ldloca.s   V_2
      IL_034e:  ldc.i4.1
      IL_034f:  stfld      int32 Point::x
      IL_0354:  ldloca.s   V_2
      IL_0356:  ldc.i4.2
      IL_0357:  stfld      int32 Point::y
      IL_035c:  ldc.i4     0x186a0
      IL_0361:  stsfld     int32 TailTests::s_testInt
      IL_0366:  ldloc.0
      IL_0367:  ldstr      "foo"
      IL_036c:  ldloc.2
      IL_036d:  callvirt   instance void TailTests::SameStacksString(string,
                                                                     valuetype Point)
      IL_0372:  ldloc.0
      IL_0373:  ldsfld     int32 TailTests::s_testInt
      IL_0378:  ldc.i4.0
      IL_0379:  ceq
      IL_037b:  ldstr      "SameStacks"
      IL_0380:  callvirt   instance void TailTests::Test(bool,
                                                         string)
      IL_0385:  leave.s    IL_03a4

    }  // end .try
    catch [mscorlib]System.Exception 
    {
      IL_0387:  stloc.s    V_4
      IL_0389:  ldstr      "Caught Unexpected Exception"
      IL_038e:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0393:  ldloc.s    V_4
      IL_0395:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_039a:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_039f:  ldc.i4.1
      IL_03a0:  stloc.s    V_5
      IL_03a2:  leave.s    IL_03ab

    }  // end handler
    IL_03a4:  ldloc.0
    IL_03a5:  ldfld      int32 TailTests::m_failed
    IL_03aa:  ret

    IL_03ab:  ldloc.s    V_5
    IL_03ad:  ret
  } // end of method TailTests::Main

  .method public hidebysig instance void 
          SameStacksString(string s,
                           valuetype Point p) cil managed
  {
    // Code size       25 (0x19)
    .maxstack  8
    IL_0000:  ldsfld     int32 TailTests::s_testInt
    IL_0005:  ldc.i4.1
    IL_0006:  sub
    IL_0007:  dup
    IL_0008:  stsfld     int32 TailTests::s_testInt
    IL_000d:  brtrue.s   IL_0010

    IL_000f:  ret

    IL_0010:  ldarg.0
    IL_0011:  ldarg.1
    IL_0012:  ldarg.2
    tail.
    IL_0013:  call       instance void TailTests::SameStacksObject(object,
                                                                   valuetype Point)
    IL_0018:  ret
  } // end of method TailTests::SameStacksString

  .method public hidebysig instance void 
          SameStacksObject(object o,
                           valuetype Point p) cil managed
  {
    // Code size       30 (0x1e)
    .maxstack  8
    IL_0000:  ldsfld     int32 TailTests::s_testInt
    IL_0005:  ldc.i4.1
    IL_0006:  sub
    IL_0007:  dup
    IL_0008:  stsfld     int32 TailTests::s_testInt
    IL_000d:  brtrue.s   IL_0010

    IL_000f:  ret

    IL_0010:  ldarg.0
    IL_0011:  ldarg.1
    IL_0012:  castclass  [mscorlib]System.String
    IL_0017:  ldarg.2
    tail.
    IL_0018:  call       instance void TailTests::SameStacksString(string,
                                                                   valuetype Point)
    IL_001d:  ret
  } // end of method TailTests::SameStacksObject

  .method public hidebysig instance void 
          NoArgs() cil managed
  {
    // Code size       32 (0x20)
    .maxstack  8
    IL_0000:  ldsfld     int32 TailTests::s_testInt
    IL_0005:  ldc.i4     0x61a80
    IL_000a:  bne.un.s   IL_000d

    IL_000c:  ret

    IL_000d:  ldsfld     int32 TailTests::s_testInt
    IL_0012:  ldc.i4.1
    IL_0013:  add
    IL_0014:  stsfld     int32 TailTests::s_testInt
    IL_0019:  ldarg.0
    tail.
    IL_001a:  call       instance void TailTests::NoArgs()
    IL_001f:  ret
  } // end of method TailTests::NoArgs

  .method public hidebysig static void  NoArgsStatic() cil managed
  {
    // Code size       31 (0x1f)
    .maxstack  8
    IL_0000:  ldsfld     int32 TailTests::s_testInt
    IL_0005:  ldc.i4     0x61a80
    IL_000a:  bne.un.s   IL_000d

    IL_000c:  ret

    IL_000d:  ldsfld     int32 TailTests::s_testInt
    IL_0012:  ldc.i4.1
    IL_0013:  add
    IL_0014:  stsfld     int32 TailTests::s_testInt
    tail.
    IL_0019:  call       void TailTests::NoArgsStatic()
    IL_001e:  ret
  } // end of method TailTests::NoArgsStatic

  .method public hidebysig instance void 
          Test(bool test,
               string msg) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  8
    IL_0000:  ldarg.1
    IL_0001:  brfalse.s  IL_0014

    IL_0003:  ldstr      "PASSED "
    IL_0008:  ldarg.2
    IL_0009:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_000e:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0013:  ret

    IL_0014:  ldstr      "FAILED "
    IL_0019:  ldarg.2
    IL_001a:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_001f:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0024:  ldarg.0
    IL_0025:  ldc.i4.1
    IL_0026:  stfld      int32 TailTests::m_failed
    IL_002b:  ret
  } // end of method TailTests::Test

  .method public hidebysig instance valuetype Point 
          StructReturnVal(int32 x,
                          int32 sum) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  4
    .locals init (valuetype Point V_0)
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_001f

    IL_0003:  ldloca.s   V_0
    IL_0005:  initobj    Point
    IL_000b:  ldloca.s   V_0
    IL_000d:  ldarg.2
    IL_000e:  stfld      int32 Point::x
    IL_0013:  ldloca.s   V_0
    IL_0015:  ldc.i4.2
    IL_0016:  ldarg.2
    IL_0017:  mul
    IL_0018:  stfld      int32 Point::y
    IL_001d:  ldloc.0
    IL_001e:  ret

    IL_001f:  ldarg.0
    IL_0020:  ldarg.1
    IL_0021:  ldc.i4.1
    IL_0022:  sub
    IL_0023:  ldarg.2
    IL_0024:  ldc.i4.1
    IL_0025:  add
    tail.
    IL_0026:  call       instance valuetype Point TailTests::StructReturnVal(int32,
                                                                             int32)
    IL_002b:  ret
  } // end of method TailTests::StructReturnVal

  .method public hidebysig static valuetype Point 
          StructReturnValStatic(int32 x,
                                int32 sum) cil managed
  {
    // Code size       43 (0x2b)
    .maxstack  3
    .locals init (valuetype Point V_0)
    IL_0000:  ldarg.0
    IL_0001:  brtrue.s   IL_001f

    IL_0003:  ldloca.s   V_0
    IL_0005:  initobj    Point
    IL_000b:  ldloca.s   V_0
    IL_000d:  ldarg.1
    IL_000e:  stfld      int32 Point::x
    IL_0013:  ldloca.s   V_0
    IL_0015:  ldc.i4.2
    IL_0016:  ldarg.1
    IL_0017:  mul
    IL_0018:  stfld      int32 Point::y
    IL_001d:  ldloc.0
    IL_001e:  ret

    IL_001f:  ldarg.0
    IL_0020:  ldc.i4.1
    IL_0021:  sub
    IL_0022:  ldarg.1
    IL_0023:  ldc.i4.1
    IL_0024:  add
    tail.
    IL_0025:  call       valuetype Point TailTests::StructReturnValStatic(int32,
                                                                          int32)
    IL_002a:  ret
  } // end of method TailTests::StructReturnValStatic

  .method public hidebysig instance int32 
          StructFacSumDeepStack(int32 x,
                                int32 y,
                                float64 a,
                                float64 b,
                                valuetype Point p,
                                int32 z) cil managed
  {
    // Code size       66 (0x42)
    .maxstack  7
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0013

    IL_0003:  ldarga.s   p
    IL_0005:  ldfld      int32 Point::x
    IL_000a:  ldarga.s   p
    IL_000c:  ldfld      int32 Point::y
    IL_0011:  add
    IL_0012:  ret

    IL_0013:  ldarga.s   p
    IL_0015:  dup
    IL_0016:  ldfld      int32 Point::x
    IL_001b:  ldc.i4.1
    IL_001c:  add
    IL_001d:  stfld      int32 Point::x
    IL_0022:  ldarga.s   p
    IL_0024:  dup
    IL_0025:  ldfld      int32 Point::y
    IL_002a:  ldc.i4.1
    IL_002b:  add
    IL_002c:  stfld      int32 Point::y
    IL_0031:  ldarg.0
    IL_0032:  ldarg.1
    IL_0033:  ldc.i4.1
    IL_0034:  sub
    IL_0035:  ldarg.2
    IL_0036:  ldarg.3
    IL_0037:  ldarg.s    b
    IL_0039:  ldarg.s    p
    IL_003b:  ldc.i4.1
    tail.
    IL_003c:  call       instance int32 TailTests::StructFacSumDeepStack(int32,
                                                                         int32,
                                                                         float64,
                                                                         float64,
                                                                         valuetype Point,
                                                                         int32)
    IL_0041:  ret
  } // end of method TailTests::StructFacSumDeepStack

  .method public hidebysig static int32  StructFacSumDeepStackStatic(int32 x,
                                                                     int32 y,
                                                                     float64 a,
                                                                     float64 b,
                                                                     valuetype Point p,
                                                                     int32 z) cil managed
  {
    // Code size       64 (0x40)
    .maxstack  6
    IL_0000:  ldarg.0
    IL_0001:  brtrue.s   IL_0013

    IL_0003:  ldarga.s   p
    IL_0005:  ldfld      int32 Point::x
    IL_000a:  ldarga.s   p
    IL_000c:  ldfld      int32 Point::y
    IL_0011:  add
    IL_0012:  ret

    IL_0013:  ldarga.s   p
    IL_0015:  dup
    IL_0016:  ldfld      int32 Point::x
    IL_001b:  ldc.i4.1
    IL_001c:  add
    IL_001d:  stfld      int32 Point::x
    IL_0022:  ldarga.s   p
    IL_0024:  dup
    IL_0025:  ldfld      int32 Point::y
    IL_002a:  ldc.i4.1
    IL_002b:  add
    IL_002c:  stfld      int32 Point::y
    IL_0031:  ldarg.0
    IL_0032:  ldc.i4.1
    IL_0033:  sub
    IL_0034:  ldarg.1
    IL_0035:  ldarg.2
    IL_0036:  ldarg.3
    IL_0037:  ldarg.s    p
    IL_0039:  ldc.i4.1
    tail.
    IL_003a:  call       int32 TailTests::StructFacSumDeepStackStatic(int32,
                                                                      int32,
                                                                      float64,
                                                                      float64,
                                                                      valuetype Point,
                                                                      int32)
    IL_003f:  ret
  } // end of method TailTests::StructFacSumDeepStackStatic

  .method public hidebysig instance int32 
          StructFacSumDeep(int32 x,
                           valuetype Point p) cil managed
  {
    // Code size       60 (0x3c)
    .maxstack  8
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0013

    IL_0003:  ldarga.s   p
    IL_0005:  ldfld      int32 Point::x
    IL_000a:  ldarga.s   p
    IL_000c:  ldfld      int32 Point::y
    IL_0011:  add
    IL_0012:  ret

    IL_0013:  ldarga.s   p
    IL_0015:  dup
    IL_0016:  ldfld      int32 Point::x
    IL_001b:  ldc.i4.1
    IL_001c:  add
    IL_001d:  stfld      int32 Point::x
    IL_0022:  ldarga.s   p
    IL_0024:  dup
    IL_0025:  ldfld      int32 Point::y
    IL_002a:  ldc.i4.1
    IL_002b:  add
    IL_002c:  stfld      int32 Point::y
    IL_0031:  ldarg.0
    IL_0032:  ldarg.1
    IL_0033:  ldc.i4.1
    IL_0034:  sub
    IL_0035:  ldarg.2
    tail.
    IL_0036:  call       instance int32 TailTests::StructFacSumDeep(int32,
                                                                    valuetype Point)
    IL_003b:  ret
  } // end of method TailTests::StructFacSumDeep

  .method public hidebysig static int32  StructFacSumDeepStatic(int32 x,
                                                                valuetype Point p) cil managed
  {
    // Code size       59 (0x3b)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  brtrue.s   IL_0013

    IL_0003:  ldarga.s   p
    IL_0005:  ldfld      int32 Point::x
    IL_000a:  ldarga.s   p
    IL_000c:  ldfld      int32 Point::y
    IL_0011:  add
    IL_0012:  ret

    IL_0013:  ldarga.s   p
    IL_0015:  dup
    IL_0016:  ldfld      int32 Point::x
    IL_001b:  ldc.i4.1
    IL_001c:  add
    IL_001d:  stfld      int32 Point::x
    IL_0022:  ldarga.s   p
    IL_0024:  dup
    IL_0025:  ldfld      int32 Point::y
    IL_002a:  ldc.i4.1
    IL_002b:  add
    IL_002c:  stfld      int32 Point::y
    IL_0031:  ldarg.0
    IL_0032:  ldc.i4.1
    IL_0033:  sub
    IL_0034:  ldarg.1
    tail.
    IL_0035:  call       int32 TailTests::StructFacSumDeepStatic(int32,
                                                                 valuetype Point)
    IL_003a:  ret
  } // end of method TailTests::StructFacSumDeepStatic

  .method public hidebysig instance int32 
          fUneven(int32 a,
                  int32 b,
                  int32 c) cil managed
  {
    // Code size       13 (0xd)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  ldarg.2
    IL_0003:  add
    IL_0004:  ldarg.2
    IL_0005:  ldarg.3
    IL_0006:  add
    tail.
    IL_0007:  call       instance int32 TailTests::gUneven(int32,
                                                           int32)
    IL_000c:  ret
  } // end of method TailTests::fUneven

  .method public hidebysig instance int32 
          gUneven(int32 a,
                  int32 b) cil managed
  {
    // Code size       10 (0xa)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  ldarg.2
    IL_0003:  add
    tail.
    IL_0004:  call       instance int32 TailTests::hUneven(int32)
    IL_0009:  ret
  } // end of method TailTests::gUneven

  .method public hidebysig instance int32 
          hUneven(int32 a) cil managed
  {
    // Code size       4 (0x4)
    .maxstack  8
    IL_0000:  ldarg.1
    IL_0001:  ldc.i4.2
    IL_0002:  mul
    IL_0003:  ret
  } // end of method TailTests::hUneven

  .method public hidebysig instance float64 
          DoubleFacRec(int32 x,
                       float64 prod) cil managed
  {
    // Code size       19 (0x13)
    .maxstack  8
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0005

    IL_0003:  ldarg.2
    IL_0004:  ret

    IL_0005:  ldarg.0
    IL_0006:  ldarg.1
    IL_0007:  ldc.i4.1
    IL_0008:  sub
    IL_0009:  ldarg.2
    IL_000a:  ldarg.1
    IL_000b:  conv.r8
    IL_000c:  mul
    tail.
    IL_000d:  call       instance float64 TailTests::DoubleFacRec(int32,
                                                                  float64)
    IL_0012:  ret
  } // end of method TailTests::DoubleFacRec

  .method public hidebysig instance int32 
          StructFacSum(int32 x,
                       valuetype Point p) cil managed
  {
    // Code size       68 (0x44)
    .maxstack  3
    .locals init (int32 V_0)
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0013

    IL_0003:  ldarga.s   p
    IL_0005:  ldfld      int32 Point::x
    IL_000a:  ldarga.s   p
    IL_000c:  ldfld      int32 Point::y
    IL_0011:  add
    IL_0012:  ret

    IL_0013:  ldarga.s   p
    IL_0015:  ldfld      int32 Point::x
    IL_001a:  ldarga.s   p
    IL_001c:  ldfld      int32 Point::y
    IL_0021:  mul
    IL_0022:  stloc.0
    IL_0023:  ldarga.s   p
    IL_0025:  ldarga.s   p
    IL_0027:  ldfld      int32 Point::y
    IL_002c:  stfld      int32 Point::x
    IL_0031:  ldarga.s   p
    IL_0033:  ldloc.0
    IL_0034:  stfld      int32 Point::y
    IL_0039:  ldarg.0
    IL_003a:  ldarg.1
    IL_003b:  ldc.i4.1
    IL_003c:  sub
    IL_003d:  ldarg.2
    tail.
    IL_003e:  call       instance int32 TailTests::StructFacSum(int32,
                                                                valuetype Point)
    IL_0043:  ret
  } // end of method TailTests::StructFacSum

  .method public hidebysig instance int32 
          Sum(int32 total,
              int32 x) cil managed
  {
    // Code size       18 (0x12)
    .maxstack  8
    IL_0000:  ldarg.2
    IL_0001:  brtrue.s   IL_0005

    IL_0003:  ldarg.1
    IL_0004:  ret

    IL_0005:  ldarg.0
    IL_0006:  ldarg.1
    IL_0007:  ldc.i4.1
    IL_0008:  add
    IL_0009:  ldarg.2
    IL_000a:  ldc.i4.1
    IL_000b:  sub
    tail.
    IL_000c:  call       instance int32 TailTests::Sum(int32,
                                                       int32)
    IL_0011:  ret
  } // end of method TailTests::Sum

  .method public hidebysig static int32  SumStatic(int32 total,
                                                   int32 x) cil managed
  {
    // Code size       17 (0x11)
    .maxstack  8
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0005

    IL_0003:  ldarg.0
    IL_0004:  ret

    IL_0005:  ldarg.0
    IL_0006:  ldc.i4.1
    IL_0007:  add
    IL_0008:  ldarg.1
    IL_0009:  ldc.i4.1
    IL_000a:  sub
    tail.
    IL_000b:  call       int32 TailTests::SumStatic(int32,
                                                    int32)
    IL_0010:  ret
  } // end of method TailTests::SumStatic

  .method public hidebysig instance int32 
          ManyArgs(int32 x,
                   int32 c0,
                   int32 c1,
                   int32 c2,
                   int32 c3,
                   int32 c4,
                   int32 c5,
                   int32 c6,
                   int32 c7,
                   int32 c8,
                   int32 c9,
                   int32 sum) cil managed
  {
    // Code size       38 (0x26)
    .maxstack  14
    IL_0000:  ldarg.1
    IL_0001:  brtrue.s   IL_0006

    IL_0003:  ldarg.s    sum
    IL_0005:  ret

    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  ldc.i4.1
    IL_0009:  sub
    IL_000a:  ldarg.3
    IL_000b:  ldarg.s    c2
    IL_000d:  ldarg.s    c3
    IL_000f:  ldarg.s    c4
    IL_0011:  ldarg.s    c5
    IL_0013:  ldarg.s    c6
    IL_0015:  ldarg.s    c7
    IL_0017:  ldarg.s    c8
    IL_0019:  ldarg.s    c9
    IL_001b:  ldarg.2
    IL_001c:  ldarg.s    sum
    IL_001e:  ldc.i4.1
    IL_001f:  add
    tail.
    IL_0020:  call       instance int32 TailTests::ManyArgs(int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32,
                                                            int32)
    IL_0025:  ret
  } // end of method TailTests::ManyArgs

  .method public hidebysig static int32  ManyArgsStatic(int32 x,
                                                        int32 c0,
                                                        int32 c1,
                                                        int32 c2,
                                                        int32 c3,
                                                        int32 c4,
                                                        int32 c5,
                                                        int32 c6,
                                                        int32 c7,
                                                        int32 c8,
                                                        int32 c9,
                                                        int32 sum) cil managed
  {
    // Code size       36 (0x24)
    .maxstack  13
    IL_0000:  ldarg.0
    IL_0001:  brtrue.s   IL_0006

    IL_0003:  ldarg.s    sum
    IL_0005:  ret

    IL_0006:  ldarg.0
    IL_0007:  ldc.i4.1
    IL_0008:  sub
    IL_0009:  ldarg.2
    IL_000a:  ldarg.3
    IL_000b:  ldarg.s    c3
    IL_000d:  ldarg.s    c4
    IL_000f:  ldarg.s    c5
    IL_0011:  ldarg.s    c6
    IL_0013:  ldarg.s    c7
    IL_0015:  ldarg.s    c8
    IL_0017:  ldarg.s    c9
    IL_0019:  ldarg.1
    IL_001a:  ldarg.s    sum
    IL_001c:  ldc.i4.1
    IL_001d:  add
    tail.
    IL_001e:  call       int32 TailTests::ManyArgsStatic(int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32,
                                                         int32)
    IL_0023:  ret
  } // end of method TailTests::ManyArgsStatic

  .method private hidebysig specialname rtspecialname static 
          void  .cctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     int32 TailTests::s_testInt
    IL_0006:  ret
  } // end of method TailTests::.cctor

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method TailTests::.ctor

} // end of class TailTests


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
