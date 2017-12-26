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
//-desc: dlg ctor is native
.assembly dlg23
{
}
.class public auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native int 'method') runtime managed
  {
  } 
  .method public hidebysig virtual instance int32 
          Invoke(string arg,
                 int32& retVal) runtime managed
  {
  } 
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(string arg,
                      int32& retVal,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime native
  {
  } 
  .method public hidebysig newslot virtual 
          instance int32  EndInvoke(int32& retVal,
                                    class [mscorlib]System.IAsyncResult result) runtime managed
  {
  } 
} 
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
.method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    .maxstack  2
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_000d:  ret
  } 
  .field private int32 i
  .method private hidebysig specialname rtspecialname 
          instance void  .ctor(int32 j) cil managed
  {
    .maxstack  2
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 TestClass::i
    IL_000d:  ret
  } 
  .method private hidebysig instance int32 
          Foo(string arg,
              int32& retVal) cil managed
  {
    .maxstack  3
    .locals init (int32 V_0,
             object[] V_1)
    IL_0000:  ldc.i4.4
    IL_0001:  newarr     [mscorlib]System.Object
    IL_0006:  stloc.1
    IL_0007:  ldloc.1
    IL_0008:  ldc.i4.0
    IL_0009:  ldstr      "Inside Delagated function i = "
    IL_000e:  stelem.ref
    IL_000f:  ldloc.1
    IL_0010:  ldc.i4.1
    IL_0011:  ldarg.0
    IL_0012:  ldfld      int32 TestClass::i
    IL_0017:  box        [mscorlib]System.Int32
    IL_001c:  stelem.ref
    IL_001d:  ldloc.1
    IL_001e:  ldc.i4.2
    IL_001f:  ldstr      " arg = "
    IL_0024:  stelem.ref
    IL_0025:  ldloc.1
    IL_0026:  ldc.i4.3
    IL_0027:  ldarg.1
    IL_0028:  stelem.ref
    IL_0029:  ldloc.1
    IL_002a:  call       string [mscorlib]System.String::Concat(object[])
    IL_002f:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0034:  ldarg.2
    IL_0035:  dup
    IL_0036:  ldind.i4
    IL_0037:  ldc.i4.1
    IL_0038:  add
    IL_0039:  stind.i4
    IL_003a:  ldarg.1
    IL_003b:  callvirt   instance int32 [mscorlib]System.String::get_Length()
    IL_0040:  stloc.0
    IL_0041:  br.s       IL_0043
    IL_0043:  ldloc.0
    IL_0044:  ret
  } 
  .method public hidebysig static int32  Main(string[] args) cil managed
  {
    .entrypoint
    .maxstack  5
    .locals init (class TestClass V_0,
             class MyDelegate V_1,
             int32 V_2,
             class [mscorlib]System.IAsyncResult V_3,
             int32 V_4,
             int32 V_5)
    IL_0000:  ldc.i4.8
    IL_0001:  newobj     instance void TestClass::.ctor(int32)
    IL_0006:  stloc.0
    IL_0007:  ldloc.0
    IL_0008:  ldftn      instance int32 TestClass::Foo(string,
                                                     int32&)
    IL_000e:  newobj     instance void MyDelegate::.ctor(object,
                                                         native int)
    IL_0013:  stloc.1
    IL_0014:  ldc.i4.3
    IL_0015:  stloc.2
    IL_0016:  ldstr      "\nCalling BeginInvoke"
    IL_001b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0020:  ldloc.1
    IL_0021:  ldstr      "Hi"
    IL_0026:  ldloca.s   V_2
    IL_0028:  ldnull
    IL_0029:  ldnull
    IL_002a:  callvirt   instance class [mscorlib]System.IAsyncResult MyDelegate::BeginInvoke(string,
                                                                                              int32&,
                                                                                              class [mscorlib]System.AsyncCallback,
                                                                                              object)
    IL_002f:  stloc.3
    IL_0030:  ldc.i4.0
    IL_0031:  stloc.s    V_4
    IL_0033:  ldstr      "Calling EndInvoke "
    IL_0038:  ldloc.1
    IL_0039:  ldloca.s   V_4
    IL_003b:  ldloc.3
    IL_003c:  callvirt   instance int32 MyDelegate::EndInvoke(int32&,
                                                              class [mscorlib]System.IAsyncResult)
    IL_0041:  box        [mscorlib]System.Int32
    IL_0046:  call       string [mscorlib]System.String::Concat(object,
                                                                object)
    IL_004b:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0050:  ldstr      "byref return = "
    IL_0055:  ldloc.s    V_4
    IL_0057:  box        [mscorlib]System.Int32
    IL_005c:  call       string [mscorlib]System.String::Concat(object,
                                                                object)
    IL_0061:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0066:  ldstr      "byref = "
    IL_006b:  ldloc.2
    IL_006c:  box        [mscorlib]System.Int32
    IL_0071:  call       string [mscorlib]System.String::Concat(object,
                                                                object)
    IL_0076:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_007b:  ldc.i4.0
    IL_007c:  stloc.s    V_5
    IL_007e:  br.s       IL_0080
    IL_0080:  ldloc.s    V_5
    IL_0082:  ret
  } 
} 
