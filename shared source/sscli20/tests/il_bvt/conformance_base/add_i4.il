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
.class public _add {
.method public static	int32 _add(int32,int32,int32) {
.maxstack		20
	ldarg	0
	ldarg	1
	add
	ldarg	2
	ceq
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
	ldc.i4		0x80000000
	ldc.i4		0x80000000
	ldc.i4		0x00000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x7FFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0x00000000
	ldc.i4		0x80000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0x00000001
	ldc.i4		0x80000001
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0x7FFFFFFF
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0x55555555
	ldc.i4		0xD5555555
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x80000000
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x2AAAAAAA
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x80000000
	ldc.i4		0x7FFFFFFF		
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFE
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x7FFFFFFE
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x55555555
	ldc.i4		0x55555554
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xAAAAAAA9
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x80000000
	ldc.i4		0x80000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x7FFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x55555555
	ldc.i4		0x55555555
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xAAAAAAAA
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x80000000
	ldc.i4		0x80000001
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	ldc.i4		0x00000002
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x80000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x55555555
	ldc.i4		0x55555556
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xAAAAAAAB
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x80000000
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x7FFFFFFE
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x00000000
	ldc.i4		0x7FFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x00000001
	ldc.i4		0x80000000
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x7FFFFFFF
	ldc.i4		0xFFFFFFFE
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x55555555
	ldc.i4		0xD5555554
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x7FFFFFFF
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x2AAAAAA9
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x80000000
	ldc.i4		0xD5555555
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x55555554
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x00000000
	ldc.i4		0x55555555
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x00000001
	ldc.i4		0x55555556
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x7FFFFFFF
	ldc.i4		0xD5555554
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0x55555555
	ldc.i4		0xAAAAAAAA
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x55555555
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x80000000
	ldc.i4		0x2AAAAAAA
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xAAAAAAA9
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x00000000
	ldc.i4		0xAAAAAAAA
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x00000001
	ldc.i4		0xAAAAAAAB
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x7FFFFFFF
	ldc.i4		0x2AAAAAA9
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x55555555
	ldc.i4		0xFFFFFFFF
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0xAAAAAAAA
	ldc.i4		0xAAAAAAAA
	ldc.i4		0x55555554
	call	int32 _add::_add(int32,int32,int32)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly add_I4exe{}
