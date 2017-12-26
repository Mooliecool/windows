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
.assembly cpobj_need_val {}
.class TestClass{
.method public static void cpobj_need_val(){
   .locals init (int32, int32)
   .zeroinit
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  ldloca.s 0
  ldloca.s 1
  cpobj string
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    call void TestClass::cpobj_need_val()
    ret
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
