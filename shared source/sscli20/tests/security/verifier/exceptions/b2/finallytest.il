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
.assembly finallytest{}
.class public auto ansi finally_class extends ['mscorlib']System.Object {
.method public static void test_1_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldloc locInt1	
  brtrue  lbl_1_1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_1_startTry:
  leave lbl_1_exitSEH
lbl_1_endTry:
lbl_1_startFinally:
  lbl_1_1: nop
  endfinally
lbl_1_endFinally:
.try  lbl_1_startTry to lbl_1_endTry finally  handler lbl_1_startFinally to lbl_1_endFinally
lbl_1_exitSEH:
  ret
}
.method public static void test_2_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_2_startTry:
  leave lbl_2_exitSEH
lbl_2_endTry:
lbl_2_startFinally:
  lbl_2_1: nop
  leave lbl_2_exitSEH
  endfinally
lbl_2_endFinally:
.try  lbl_2_startTry to lbl_2_endTry finally  handler lbl_2_startFinally to lbl_2_endFinally
lbl_2_exitSEH:
  ret
}
.method public static void test_3_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_3_startTry:
  leave lbl_3_exitSEH
lbl_3_endTry:
lbl_3_startFinally:
  lbl_3_1: nop
  br lbl_3_exitSEH
  endfinally
lbl_3_endFinally:
.try  lbl_3_startTry to lbl_3_endTry finally  handler lbl_3_startFinally to lbl_3_endFinally
lbl_3_exitSEH:
  ret
}
.method public static void test_4_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_4_startTry:
  leave lbl_4_exitSEH
lbl_4_endTry:
lbl_4_startFinally:
   ldc.i4.1
   pop
lbl_4_endFinally:
.try  lbl_4_startTry to lbl_4_endTry finally  handler lbl_4_startFinally to lbl_4_endFinally
lbl_4_exitSEH:
  ret
}
.method public static void test_5_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_5_startTry:
  leave lbl_5_exitSEH
lbl_5_endTry:
lbl_5_startFinally:
   ldc.i4.1
   pop
   endfilter
lbl_5_endFinally:
.try  lbl_5_startTry to lbl_5_endTry finally  handler lbl_5_startFinally to lbl_5_endFinally
lbl_5_exitSEH:
ret
}
.method public static void test_6_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_6_startTry:
  leave lbl_6_exitSEH
lbl_6_endTry:
lbl_6_startFinally:
   ldc.i4.1
   pop
   leave lbl_6_exitSEH
lbl_6_endFinally:
.try  lbl_6_startTry to lbl_6_endTry finally  handler lbl_6_startFinally to lbl_6_endFinally
lbl_6_exitSEH:
  ret
}
.method public static void test_7_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_7_startTry:
  lbl_7_1: nop
  leave lbl_7_exitSEH
lbl_7_endTry:
lbl_7_startFinally:
   ldloc locInt1
   brtrue lbl_7_1
   endfinally
lbl_7_endFinally:
.try  lbl_7_startTry to lbl_7_endTry finally  handler lbl_7_startFinally to lbl_7_endFinally
lbl_7_exitSEH:
  ret
}
.method public static void test_8_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_8_startTry:
  lbl_8_1: nop
  leave lbl_8_exitSEH
lbl_8_endTry:
lbl_8_startFinally:
   newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
   endfinally
lbl_8_endFinally:
.try  lbl_8_startTry to lbl_8_endTry finally  handler lbl_8_startFinally to lbl_8_endFinally
lbl_8_exitSEH:
  ret
}
.method public static void test_9_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_9_startTry:
  lbl_9_1: nop
  leave lbl_9_exitSEH
lbl_9_endTry:
lbl_9_startFinally:
 rethrow  
   endfinally
lbl_9_endFinally:
.try  lbl_9_startTry to lbl_9_endTry finally  handler lbl_9_startFinally to lbl_9_endFinally
lbl_9_exitSEH:
  ret
}
.method public static void test_10_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_10_startTry1:
  br lbl_10_startTry
  lbl_10_startFinally:
     nop 
     endfinally
  lbl_10_endFinally:
  lbl_10_startTry:
    lbl_10_1: nop
    leave lbl_10_exitSEH
  lbl_10_endTry:
lbl_10_endTry1:
lbl_10_startCatch1:
  leave lbl_10_exitSEH
lbl_10_endCatch1:
.try  lbl_10_startTry to lbl_10_endTry finally  handler lbl_10_startFinally to lbl_10_endFinally
.try  lbl_10_startTry1 to lbl_10_endTry1 catch ['mscorlib']System.Exception  handler lbl_10_startCatch1 to lbl_10_endCatch1
lbl_10_exitSEH:
  ret
}
.method public static void test_11_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_11_startTry:
  lbl_11_1: nop
lbl_11_startTry1:
  lbl_11_2: nop
  leave lbl_11_exitSEH
lbl_11_endTry:
  leave lbl_11_exitSEH
lbl_11_endTry1:
lbl_11_startFinally:
   endfinally
lbl_11_endFinally:
lbl_11_startFinally1:
   endfinally
lbl_11_endFinally1:
.try  lbl_11_startTry to lbl_11_endTry finally  handler lbl_11_startFinally to lbl_11_endFinally
.try  lbl_11_startTry1 to lbl_11_endTry1 finally  handler lbl_11_startFinally1 to lbl_11_endFinally1
lbl_11_exitSEH:
  ret
}
.method public static void test_12_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_12_startTry:
  lbl_12_1: nop
  leave lbl_12_exitSEH
lbl_12_startFinally:
   endfinally
lbl_12_endFinally:
lbl_12_endTry:
lbl_12_exitSEH:
.try  lbl_12_startTry to lbl_12_endTry finally  handler lbl_12_startFinally to lbl_12_endFinally
  ret
}
.method public static void test_13_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_13_startTry:
  lbl_13_1: nop
  leave lbl_13_exitSEH
lbl_13_endTry:
lbl_13_startFinally:
   ret
   endfinally
lbl_13_endFinally:
.try  lbl_13_startTry to lbl_13_endTry finally  handler lbl_13_startFinally to lbl_13_endFinally
lbl_13_exitSEH:
  ret
}
.method public static void test_14_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_14_startTry:
  lbl_14_1: nop
  leave lbl_14_exitSEH
lbl_14_endTry:
lbl_14_startFinally:
   endfinally
lbl_14_endFinally:
lbl_14_startFinally1:
   endfinally
lbl_14_endFinally1:
.try  lbl_14_startTry to lbl_14_endTry finally  handler lbl_14_startFinally to lbl_14_endFinally
.try  lbl_14_startTry to lbl_14_endTry finally  handler lbl_14_startFinally1 to lbl_14_endFinally1
lbl_14_exitSEH:
  ret
}
.method public static void test_15_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_15_startTry:
  lbl_15_1: nop
  leave lbl_15_exitSEH
lbl_15_endTry:
lbl_15_startFinally:
   endfinally
lbl_15_endFinally:
lbl_15_startFinally1:
   endfinally
lbl_15_endFinally1:
.try  lbl_15_startTry to lbl_15_endTry finally  handler lbl_15_startFinally to lbl_15_endFinally
.try  lbl_15_startTry to lbl_15_endTry finally  handler lbl_15_startFinally1 to lbl_15_endFinally1
lbl_15_exitSEH:
  ret
}
.method public static void test_16_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_16_startTry
lbl_16_startFilter1:
  ldc.i4.1
  endfilter
lbl_16_endFilter1: 
lbl_16_startTry:
  lbl_16_1: nop
  leave lbl_16_exitSEH
lbl_16_endTry:
lbl_16_startFinally:
   endfinally
lbl_16_endFinally:
lbl_16_startFilterHandler1:
   leave lbl_16_exitSEH
lbl_16_endFilterHandler1:
.try  lbl_16_startTry to lbl_16_endTry finally  handler lbl_16_startFinally to lbl_16_endFinally
.try  lbl_16_startTry to lbl_16_endTry filter lbl_16_startFilter1  handler lbl_16_startFilterHandler1 to lbl_16_endFilterHandler1
lbl_16_exitSEH:
  ret
}
.method public static void test_17_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_17_startTry:
  lbl_17_1: nop
  leave lbl_17_exitSEH
lbl_17_endTry:
lbl_17_startFinally:
   endfinally
lbl_17_endFinally:
lbl_17_startCatch1:
   leave lbl_17_exitSEH
lbl_17_endCatch1:
.try  lbl_17_startTry to lbl_17_endTry finally  handler lbl_17_startFinally to lbl_17_endFinally
.try  lbl_17_startTry to lbl_17_endTry catch ['mscorlib']System.Exception  handler lbl_17_startCatch1 to lbl_17_endCatch1
lbl_17_exitSEH:
  ret
}
.method public static void test_18_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_18_startTry:
  lbl_18_1: nop
  leave lbl_18_exitSEH
lbl_18_endTry:
lbl_18_startFinally:
   jmp void finally_class::foo_18_finally()
   endfinally
lbl_18_endFinally:
.try  lbl_18_startTry to lbl_18_endTry finally  handler lbl_18_startFinally to lbl_18_endFinally
lbl_18_exitSEH:
  ret
}
.method public static void test_19_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_19_startTry:
  lbl_19_1: nop
  leave lbl_19_exitSEH
lbl_19_endTry:
lbl_19_startFinally:
   tail. 
   call void finally_class::foo_19_finally()
   endfinally
lbl_19_endFinally:
.try  lbl_19_startTry to lbl_19_endTry finally  handler lbl_19_startFinally to lbl_19_endFinally
lbl_19_exitSEH:
  ret
}
.method public static void foo_19_finally(){
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
}
.method public static void main()
{
    .entrypoint
    ret
}
} 
