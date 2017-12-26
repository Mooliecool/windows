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
.method public static	int32 _rem(int32,int32) {
.locals init		(class [mscorlib]System.Exception,unsigned int32)
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
	ldc.i4			0xDB0E
	br				END
FAIL:
	ldc.i4	0x0001
	br				END
END:
	ret
.try try_start to try_end catch [mscorlib]System.DivideByZeroException handler divbyzero to HEnd
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555554
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFE
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555554
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555555
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555555
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555555
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x55555555
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x00000000
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xDB0E
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x00000001
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xFFFFFFFE
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xFFFFFFFF
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0xAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x55555555
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xAAAAAAAA
	call	int32 rem_un::_rem(int32,int32)
	ldc.i4		0x00000000
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
.assembly rem_u4exe{}
