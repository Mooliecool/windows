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
.assembly VarArgsexe{}
.class public VarArgs
{
	.field public int32 m_i
	.method public specialname instance void .ctor(int32 i) il managed
	{
		ldarg		0			
		call		instance void [mscorlib]System.Object::.ctor()
		ldarg		0			
		ldarg		i
		stfld		int32 VarArgs::m_i
		ret
	}
	.method public static int32 AddUp(	int32 i,
						value class [mscorlib]System.Variant v,
						class VarArgs o,
						float64 d) il managed
	{
		.locals(int32 sum)
		ldarg 		i			
		stloc		sum
		ldarga		v			
		call		instance int32 [mscorlib]System.Variant::ToInt32()
		ldloc		sum
		add
		stloc		sum
		ldarg		o			
		ldfld		int32 VarArgs::m_i
		ldloc		sum
		add
		stloc		sum
		ldarg		d			
		conv.i4
		ldloc		sum
		add
		stloc		sum
		ldloc		sum			
		ret
	}
	.method public static vararg value class [mscorlib]System.Variant
		StaticVarArg(	int32 i,
				value class [mscorlib]System.Variant v,
				class VarArgs o,
				float64 d,
				...) il managed
	{
		.locals (value class [mscorlib]System.ArgIterator iter, int32 retVal)
		ldarg		i
		ldarg		v
		ldarg		o
		ldarg		d
		call		int32 VarArgs::AddUp(int32 i, value class [mscorlib]System.Variant v,
						     class VarArgs o, float64 d)
		stloc		retVal
		arglist
		newobj 		instance void [mscorlib]System.ArgIterator::.ctor(value class [mscorlib]System.RuntimeArgumentHandle)
		stloc 		iter
		ldloca		iter
		call		instance refany [mscorlib]System.ArgIterator::GetNextArg()
		refanyval	[mscorlib]System.Int32
		ldind.i4
		ldloca		iter
		call		instance refany [mscorlib]System.ArgIterator::GetNextArg()
		refanyval	[mscorlib]System.Variant
		ldobj		[mscorlib]System.Variant
		ldloca		iter
		call		instance refany [mscorlib]System.ArgIterator::GetNextArg()
		refanyval	VarArgs
		ldind.ref
		ldloca		iter
		call		instance refany [mscorlib]System.ArgIterator::GetNextArg()
		refanyval	[mscorlib]System.Double
		ldind.r8
		call		int32 VarArgs::AddUp(int32 i, value class [mscorlib]System.Variant v,
						     class VarArgs o, float64 d)
		ldloc		retVal
		add
		stloc		retVal
		ldloca		iter
		call		instance int32 [mscorlib]System.ArgIterator::GetRemainingCount()
		ldloc		retVal
		add
		stloc		retVal		
		ldloc 		retVal
		newobj		instance void [mscorlib]System.Variant::.ctor(int32)
		ret
	}
	.method public static int32 main() il managed
	{
		.entrypoint
		.locals (int32 i, value class [mscorlib]System.Variant v, class VarArgs o, float64 d,
			 value class [mscorlib]System.Variant var)
		.maxstack	10
		ldc.i4		10
		stloc		i
		ldc.i4		20
		newobj		instance void [mscorlib]System.Variant::.ctor(int32)
		stloc		v
		ldc.i4		30
		newobj		instance void VarArgs::.ctor(int32)
		stloc		o
		ldc.i4		40
		conv.r8
		stloc		d
		ldloc		i
		ldloc		v
		ldloc		o
		ldloc		d
		ldloc		i
		ldloc		v
		ldloc		o
		ldloc		d
		ldc.i4		1000
		call		vararg value class [mscorlib]System.Variant VarArgs::StaticVarArg(
					int32 i, value class [mscorlib]System.Variant v, class VarArgs o, float64 d, ...,
					int32 i, value class [mscorlib]System.Variant v, class VarArgs o, float64 d,
					int32) 
		stloc 		var
		ldloca		var
		call		instance int32 [mscorlib]System.Variant::ToInt32()
		ldc.i4		201
		beq		PASS
FAIL:
		call		void [mscorlib]System.Console::WriteLine(class [mscorlib]System.String)
		ldc.i4		0x0001
		ret
PASS:
		ldstr		"Pass"
		call		void [mscorlib]System.Console::WriteLine(class [mscorlib]System.String)
		ldc.i4		0x0000
		ret
	}
}
