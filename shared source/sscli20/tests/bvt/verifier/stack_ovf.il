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
.assembly stack_ovf{}
.class TestClass{
.method public hidebysig static void  stack_ovf() cil managed
{
  .maxstack  1
  .locals init (int32 V_0)
   ldc.i4 1
   brtrue l1
l1:ldc.i4 1
   ldc.i4 1 
   leave l2 
l2:		 		
    ret
} 
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::stack_ovf()
    .entrypoint
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