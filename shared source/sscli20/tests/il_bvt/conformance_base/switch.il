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
.class public		_switch {
.method public		void _switch() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
.locals	 init (int32)
	ldc.i4	0x0
	stloc	0
	ldc.i4	0x0
	TAG:
	switch (TC1,TC2,TC3)
	ldloc	0
	ldc.i4		0x3
	ceq
	brfalse		FAIL
	br			PASS
		TC1:
			ldloc	0
			ldc.i4		0x1
			add
			stloc	0
			ldc.i4		0x1
			br			TAG
		TC2:
			ldloc	0
			ldc.i4		0x1
			add
			stloc	0
			ldc.i4		0x2
			br			TAG
		TC3:
			ldloc	0
			ldc.i4		0x1
			add
			stloc	0
			ldc.i4		0x3
			br			TAG
	br	FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly switchexe{}
