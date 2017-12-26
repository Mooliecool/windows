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
.assembly fptr{}
.class A{
.method public static int32 l(){
.locals init(int32 ll)
ldftn void A::k()
ret
}
.method private static void k(){
.locals init(int32 ll)
 ldstr "inside private"
 call       void [mscorlib]System.Console::WriteLine(class System.String)
ret
}
}
.class TestClass{
.method public static void t(){
.locals init(int32 ll)
 ldstr "pointer to private function at"
 call       void [mscorlib]System.Console::WriteLine(class System.String)
call int32 A::l()
call       void [mscorlib]System.Console::WriteLine(int32)
 ldstr "returning"
 call       void [mscorlib]System.Console::WriteLine(class System.String) 
ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::t()
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
