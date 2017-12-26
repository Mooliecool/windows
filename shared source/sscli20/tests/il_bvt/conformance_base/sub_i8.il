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
.class public _sub {
.method public static	int64 _sub(int64,int64) {
.maxstack		20
	ldarg	0
	ldarg	1
	sub
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldc.i8		0x8000000000000000
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x2AAAAAAAAAAAAAAB
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xD555555555555556
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x7FFFFFFFFFFFFFFF		
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAB
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555556
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000002
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000002
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAC
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555557
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x8000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x7FFFFFFFFFFFFFFE
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x2AAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xD555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xD555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555556
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555554
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xD555555555555556
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAB
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x8000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x2AAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAB
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000000
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x0000000000000001
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0xAAAAAAAAAAAAAAA9
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x2AAAAAAAAAAAAAAB
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0x5555555555555555
	call	int64 _sub::_sub(int64,int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _sub::_sub(int64,int64)
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
.assembly sub_i8exe{}
