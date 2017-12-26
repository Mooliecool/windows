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

.assembly 'tail_calli' { }
.assembly extern 'mscorlib'{}

.class public TestClass
{
.method public static default void main()
{
  .entrypoint
  .maxstack  2
	ldftn      void [mscorlib]System.Console::WriteLine(string)
	box	   int32
	ldstr      "PASS!"
	call	   void TestClass::run_test(object, object)
        ldc.i4.0
        call       void [mscorlib]System.Environment::Exit(int32)
	ret
}

.method public static default  void run_test(
	class [mscorlib]System.Object 'procedure', 
	class [mscorlib]System.Object 'Y') 
{
   .maxstack  2
	ldarg 'Y'
	ldarg 'procedure'
	unbox int32
	ldobj int32
	tail.
	calli	void (string)
	ret
}
}
