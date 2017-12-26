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
.class public bne_un {
.field public static	int64 all
.field public static	int64 none
.field public static	int64 odd
.field public static	int64 even
.method public static void initialize() {
.maxstack		10
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld		int64 bne_un::all
	ldc.i8		0x0000000000000000
	stsfld		int64 bne_un::none
	ldc.i8		0x5555555555555555
	stsfld		int64 bne_un::odd
	ldc.i8		0xAAAAAAAAAAAAAAAA
	stsfld		int64 bne_un::even
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void bne_un::initialize()
	ldsfld	int64 bne_un::all
	ldsfld	int64 bne_un::all
	bne.un FAIL
	ldsfld	int64 bne_un::all
	ldsfld	int64 bne_un::none
	bne.un A
	br FAIL
A:
	ldsfld	int64 bne_un::all
	ldsfld	int64 bne_un::odd
	bne.un B
	br FAIL
B:
	ldsfld	int64 bne_un::all
	ldsfld	int64 bne_un::even
	bne.un C
	br FAIL
C:
	ldsfld	int64 bne_un::none
	ldsfld	int64 bne_un::all
	bne.un D
	br FAIL
D:
	ldsfld	int64 bne_un::none
	ldsfld	int64 bne_un::none
	bne.un FAIL
	ldsfld	int64 bne_un::none
	ldsfld	int64 bne_un::odd
	bne.un E
	br FAIL
E:
	ldsfld	int64 bne_un::none
	ldsfld	int64 bne_un::even
	bne.un F
	br FAIL
F:
	ldsfld	int64 bne_un::odd
	ldsfld	int64 bne_un::all
	bne.un G
	br FAIL
G:
	ldsfld	int64 bne_un::odd
	ldsfld	int64 bne_un::none
	bne.un H
	br FAIL
H:
	ldsfld	int64 bne_un::odd
	ldsfld	int64 bne_un::odd
	bne.un FAIL
	ldsfld	int64 bne_un::odd
	ldsfld	int64 bne_un::even
	bne.un I
	br FAIL
I:
	ldsfld	int64 bne_un::even
	ldsfld	int64 bne_un::all
	bne.un J
	br FAIL
J:
	ldsfld	int64 bne_un::even
	ldsfld	int64 bne_un::none
	bne.un K
	br FAIL
K:
	ldsfld	int64 bne_un::even
	ldsfld	int64 bne_un::odd
	bne.un L
L:
	ldsfld	int64 bne_un::even
	ldsfld	int64 bne_un::even
	bne.un FAIL
	br BACKCHECK
TOPASS:
	br PASS
BACKCHECK:
	ldc.i8 0x0
	ldc.i8 0x1
	bne.un TOPASS
	br FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly bne_u8exe{}
