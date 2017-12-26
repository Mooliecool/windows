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
.assembly arglistexe{}
.method public static vararg int32 CompareArgs(int32){
.locals init (valuetype [mscorlib]System.ArgIterator Args_First,
           valuetype [mscorlib]System.ArgIterator Args_Second)
.maxstack 10
	ldloca.s   Args_First
	arglist
	call       instance void [mscorlib]System.ArgIterator::.ctor(valuetype [mscorlib]System.RuntimeArgumentHandle)
	ldloca.s   Args_Second
	arglist
	call       instance void [mscorlib]System.ArgIterator::.ctor(valuetype [mscorlib]System.RuntimeArgumentHandle)
	ldc.i4.0
  MoreSetup:
  	ldloca.s   Args_Second
	call       instance typedref [mscorlib]System.ArgIterator::GetNextArg()
	pop
	ldc.i4.1
	add
	dup
	ldarg.0
	blt.s      MoreSetup
	pop
	ldc.i4.0
  Again:
  	ldloca.s   Args_Second
	call       instance typedref [mscorlib]System.ArgIterator::GetNextArg()
	refanyval  [mscorlib]System.Int32
	ldind.i4
	ldloca.s   Args_First
	call       instance typedref [mscorlib]System.ArgIterator::GetNextArg()
	refanyval  [mscorlib]System.Int32
	ldind.i4
	beq.s      GoodCompare
	pop
	ldc.i4 0
	ret
  GoodCompare:
  	ldc.i4.1
	add
  	dup
	ldarg.0
	blt.s      Again
	pop
	ldc.i4 0xAAAA
	ret
}
.method public static int32 main() {
.entrypoint
.locals	()
.maxstack	10
	ldc.i4	1
	ldc.i4	1
	ldc.i4	1
	call	vararg int32 CompareArgs(int32,...,int32,int32)
	brfalse	FAIL
	ldc.i4	4
	ldc.i4	2
	ldc.i4	3
	ldc.i4	4
	ldc.i4	5
	ldc.i4	2
	ldc.i4	3
	ldc.i4	4
	ldc.i4	5
	call	vararg int32 CompareArgs(int32,...,int32,int32,int32,int32,int32,int32,int32,int32)
	brfalse	FAIL
	ldc.i4	1
	ldc.i4	2 
	ldc.i4	1
	call	vararg int32 CompareArgs(int32,...,int32,int32)
	brtrue	FAIL
	ldc.i4	4
	ldc.i4	2
	ldc.i4	3
	ldc.i4	4
	ldc.i4	5
	ldc.i4	2
	ldc.i4	1
	ldc.i4	4
	ldc.i4	5
	call	vararg int32 CompareArgs(int32,...,int32,int32,int32,int32,int32,int32,int32,int32)
	brtrue	FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
