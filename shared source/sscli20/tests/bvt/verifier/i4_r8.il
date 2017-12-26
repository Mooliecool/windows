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
.file alignment 512
.corflags 0x00000001
.assembly extern mscorlib
{
	.publickeytoken = (03 68 91 16 D3 A4 AE 33 )
	.hash = (F6 D5 FA EB 33 59 20 94 E6 13 2F B1 97 DA F4 B6 19 97 49 A8 )
	.ver 1:0:2313:0
}
.assembly I4_R8 {
.hash algorithm 0x00008004
.ver 0:0:0:0
}
.module I4_R8.exe
.class value public auto ansi serializable sealed UserEnum extends [mscorlib]System.Enum
{
	.field public specialname rtspecialname int32 value__
	.field public static literal value class UserEnum red = int32(0x00000000)
	.field public static literal value class UserEnum green = int32(0x00000001)
}
.class value public auto ansi sealed UserValueClass extends [mscorlib]System.ValueType
{
	.field public int32[] gcref
}
.class public auto ansi UserClass extends [mscorlib]System.Object
{
	.method public hidebysig specialname rtspecialname instance void .ctor() il managed
	{
		.maxstack  8
		ldarg.0
		call       instance void [mscorlib]System.Object::.ctor()
		ret
	}
}
.class public auto ansi UserSubClass extends UserClass
{
	.method public hidebysig specialname rtspecialname 
	instance void .ctor() il managed
	{
		.maxstack  8
		ldarg.0
		call       instance void UserClass::.ctor()
		ret
	}
}
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
	.method private hidebysig static vararg int32 VarArg() il managed
	{
		.maxstack 1
		ldc.i4.0
		ret
	}
	.method public hidebysig static void Main(class [mscorlib]System.String[]) il managed
	{
		.maxstack  8
		.entrypoint
		.zeroinit
		.locals init (int32 INT32,
		         class [mscorlib]System.Object OBJ,
		         value class UserValueClass VAL)
		ldc.i4.0
		call vararg int32 TestClass::VarArg(..., float64)
		pop
		ret
	}
	.method public hidebysig specialname rtspecialname instance void .ctor() il managed
	{
		.maxstack  8
		ldarg.0
		call instance void ['mscorlib']System.Object::.ctor()
		ret
	}
}
