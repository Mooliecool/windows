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
.class public		mul_ovf_u8 {
.method public static int64 u8(int64,int64) {
.locals init		(class [mscorlib]System.OverflowException,unsigned int64)
.maxstack	2
try_start:
	ldarg	0
	ldarg	1
	mul.ovf.un
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	br		END
aHandler:
	isinst	[mscorlib]System.OverflowException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse	FAIL
	ldc.i8	0xAEAEAEAEAEAEAEAE
	br		END
FAIL:
	ldc.i8	0xEEEEEEEEEEEEEEEE
END:
	ret
.try try_start to try_end catch [mscorlib]System.OverflowException handler aHandler to HEnd
}
.method public		void mul_ovf_u8() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xAAAAAAAAAAAAAAAA
  	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000000
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000001
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x5555555555555555
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 mul_ovf_u8::u8(int64,int64)
	ldc.i8		0xAEAEAEAEAEAEAEAE
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly mul_ovf_u8exe{}
