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
.assembly test_102_catch {}
.class TestClass{
.method public static void test_102_catch(){
   .try
    {
      nop
      newobj instance void [mscorlib]System.ArgumentException::.ctor()
      throw 	
_leave:
      ldstr "back in try"
      call void [mscorlib]System.Console::WriteLine(string)	
      leave.s    _ret
    }
    catch [mscorlib]System.OverflowException 
    {
      leave.s    _ret
    }
    catch [mscorlib]System.ArgumentException 
    {
      pop
      leave.s    _leave
    }
_ret:
    ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_102_catch()
    .entrypoint
    ret
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_000101:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
