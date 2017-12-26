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
.class public ble_un {
.field public static	int64 all
.field public static	int64 none
.field public static	int64 odd
.field public static	int64 even
.method public static	void initialize() {
.maxstack	10
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld	int64 ble_un::all
	ldc.i8		0x0000000000000000
	stsfld	int64 ble_un::none
	ldc.i8		0x5555555555555555
	stsfld	int64 ble_un::odd
	ldc.i8		0xAAAAAAAAAAAAAAAA
	stsfld	int64 ble_un::even
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void ble_un::initialize()
	ldsfld	int64 ble_un::all
	ldsfld	int64 ble_un::all
	ble.un A
	br FAIL
A:
	ldsfld	int64 ble_un::all
	ldsfld	int64 ble_un::none
	ble.un FAIL
	ldsfld	int64 ble_un::all
	ldsfld	int64 ble_un::odd
	ble.un FAIL
	ldsfld	int64 ble_un::all
	ldsfld	int64 ble_un::even
	ble.un FAIL
	ldsfld	int64 ble_un::none
	ldsfld	int64 ble_un::all
	ble.un B
	br FAIL
B:
	ldsfld	int64 ble_un::none
	ldsfld	int64 ble_un::none
	ble.un C
	br FAIL
C:
	ldsfld	int64 ble_un::none
	ldsfld	int64 ble_un::odd
	ble.un D
	br FAIL
D:
	ldsfld	int64 ble_un::none
	ldsfld	int64 ble_un::even
	ble.un E
	br FAIL
E:
	ldsfld	int64 ble_un::odd
	ldsfld	int64 ble_un::all
	ble.un F
	br FAIL
F:
	ldsfld	int64 ble_un::odd
	ldsfld	int64 ble_un::none
	ble.un FAIL
	ldsfld	int64 ble_un::odd
	ldsfld	int64 ble_un::odd
	ble.un G
	br FAIL
G:
	ldsfld	int64 ble_un::odd
	ldsfld	int64 ble_un::even
	ble.un H
	br FAIL
H:
	ldsfld	int64 ble_un::even
	ldsfld	int64 ble_un::all
	ble.un I
	br FAIL
I:
	ldsfld	int64 ble_un::even
	ldsfld	int64 ble_un::none
	ble.un FAIL
	ldsfld	int64 ble_un::even
	ldsfld	int64 ble_un::odd
	ble.un FAIL
	ldsfld	int64 ble_un::even
	ldsfld	int64 ble_un::even
	ble.un BACKCHECK
	br FAIL
TOPASS:
	br PASS
BACKCHECK:
	ldc.i8 0x0
	ldc.i8 0x1
	ble.un TOPASS
	br FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly ble_u8exe{}
