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
.assembly illegalop {}
.class TestClass{
.method public static void illegalop(){
ldstr "this is a string"
pop
.emitbyte 0xe4
ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    .locals init (int32 V_1)
   .try {
      .try {
            ldc.i4.0
            stloc V_1
            call void TestClass::illegalop()
            leave.s EndIL
           }
      catch ['mscorlib']System.InvalidProgramException
           {
            pop
            ldstr      "TEST PASSED"
            call       void ['mscorlib']System.Console::WriteLine(string)
            ldc.i4.0
            stloc      V_1
            leave.s    EndTry
           }  
      EndTry: leave.s EndIL
    }  
    catch ['mscorlib']System.Exception 
    {
        pop
        ldstr      "TEST FAILED"
        call       void ['mscorlib']System.Console::WriteLine(string)
        ldc.i4.1
        stloc      V_1
        leave.s    EndIL
    }  
    EndIL:    ldloc V_1
              call       void [mscorlib]System.Environment::set_ExitCode(int32)
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
