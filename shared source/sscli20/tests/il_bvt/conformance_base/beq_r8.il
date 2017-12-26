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
.assembly extern convdll{}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack 2
.locals init	(class [convdll]ConvDLL)
newobj instance void [convdll]ConvDLL::.ctor()
stloc 0
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq R8INFMAX
	br FAIL
R8INFMAX:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq R8MAXONE
	br FAIL
R8MAXONE:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq R8ONEZERO
	br FAIL
R8ONEZERO:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq R8ZERONEGZERO
	br FAIL
R8ZERONEGZERO:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldc.r8		float64(0x8000000000000000)
	beq R8ZERONEGONE
	br FAIL
R8ZERONEGONE:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq R8NEGZERONEGZERO
	br FAIL
R8NEGZERONEGZERO:
	ldc.r8		float64(0x8000000000000000)
	ldc.r8		float64(0x8000000000000000)
	beq R8NEGZEROONE
	br FAIL
R8NEGZEROONE:
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldc.r8		float64(0x8000000000000000)
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq R8NEGONEMIN
	br FAIL
R8NEGONEMIN:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq R8MINNEGINF
	br FAIL
R8MINNEGINF:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq R8NEGINFNAN
	br FAIL
R8NEGINFNAN:
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::max_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::zero_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldc.r8		float64(0x8000000000000000)
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_one_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::min_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::_inf_r8
	beq FAIL
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	ldloc 0
	ldfld float64 [convdll]ConvDLL::NaN_r8
	beq FAIL
	br BACKCHECK
TOPASS:
	br PASS
BACKCHECK:
	ldc.r8 0x0
	dup
	beq TOPASS
	br FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
.assembly beq_r8exe{}
