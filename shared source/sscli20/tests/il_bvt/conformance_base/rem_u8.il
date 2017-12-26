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
.class public rem_un {
.method public static	int64 _rem(int64,int64) {
.locals init		(class [mscorlib]System.Exception, unsigned int64)
.maxstack		3
try_start:
	ldarg	0
	ldarg	1
	rem.un
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	br				END
divbyzero:
	isinst			[mscorlib]System.DivideByZeroException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse			FAIL
	ldc.i8			0xDB0E
	br				END
FAIL:
 	ldc.i8			0x0000000000000000
	br				END
END:
	ret
.try try_start to try_end catch [mscorlib]System.DivideByZeroException handler divbyzero to HEnd
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000000
	call		int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000000
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x0000000000000000
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555554
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555554
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000000
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000000
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000000
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000001
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xFFFFFFFFFFFFFFFE
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x5555555555555555
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 rem_un::_rem(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly rem_u8exe{}
