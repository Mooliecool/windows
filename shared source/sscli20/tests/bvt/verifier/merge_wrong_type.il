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
.subsystem 0x00000003
.corflags 0x00000001
.assembly extern mscorlib
{
  .publickeytoken = (03 68 91 16 D3 A4 AE 33 )
  .hash = (62 3B 27 03 8B 17 03 EC B2 BD 94 CF 1A 36 2C 96 49 ED A3 FF )
  .ver 2000:14:1928:0
}
.assembly 'merge_wrong_type'
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module merge_wrong_type.exe
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
.method public hidebysig vararg native int VarArg(int32, refany) il managed
{
	.maxstack  8
	.locals init (value class [mscorlib]System.ArgIterator args)
	ldarg.1
	brfalse BRANCH1
	ldarg.1
	br MERGE1
BRANCH1:
	ldloca args
	arglist
	call instance void [mscorlib]System.ArgIterator::.ctor(value class [mscorlib]System.RuntimeArgumentHandle)
	ldloca args
	call instance refany [mscorlib]System.ArgIterator::GetNextArg()
MERGE1:
	refanyval native int
	ldind.i
	pop
	ldarg.1
	brfalse BRANCH2
	ldloca args
	arglist
	call instance void [mscorlib]System.ArgIterator::.ctor(value class [mscorlib]System.RuntimeArgumentHandle)
	ldloca args
	call instance refany [mscorlib]System.ArgIterator::GetNextArg()
	br MERGE2
BRANCH2:
	ldarg.2
MERGE2:
	refanyval native int
	ldind.i
	ret
} 
.method public hidebysig static void Main(class [mscorlib]System.String[]) il managed
{
	.entrypoint
	.maxstack  8
	.locals init (native int)
	ldc.i4 11
	stloc.0
	newobj instance void TestClass::.ctor()
	ldloc.0
	ldloca 0
	mkrefany native int
	dup
	call instance vararg native int TestClass::VarArg(int32, refany, ..., refany)
	call void [mscorlib]System.Console::WriteLine(int32)
	ret
} 
.method public hidebysig specialname rtspecialname instance void .ctor() il managed
{
	.maxstack  8
	ldarg.0
	call       instance void ['mscorlib']System.Object::.ctor()
	ret
} 
} 
