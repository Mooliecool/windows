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
.assembly merge1 {}
.class TestClass
{
    .field static class System.Object obj
    .field static class ['mscorlib']System.IComparable obj2
    .method public specialname rtspecialname instance void .ctor() 
    { 
        ldarg 0
        call instance void ['mscorlib']System.Object::.ctor()
        ret
    }
    .method public static void Main(class ['mscorlib']System.String[] args) 
    {
        .entrypoint
        call void TestClass::foo()
        ret
    }
    .method static void foo()
    {
      ldsflda class ['mscorlib']System.IComparable TestClass::obj2
      br merge70071
instr70071:
      ldsflda class System.Object TestClass::obj
      br merge70071
merge70071:
      call void TestClass::A(class System.Object&)
      ret
    }
    .method static void A(class System.Object&)
    {
        ret
    }
    .method static void B(class ['mscorlib']System.IComparable&)
    {
        ret
    }
}
