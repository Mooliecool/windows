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
.class Ldvirtftn {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack       20
.locals ()
        newobj instance void [mscorlib]System.Object::.ctor()
        ldvirtftn instance class [mscorlib]System.String [mscorlib]System.Object::ToString()
        newobj instance void [mscorlib]System.Object::.ctor()
        ldvirtftn instance class [mscorlib]System.String [mscorlib]System.Object::ToString()
        ceq
    brfalse     FAIL
PASS:
	ldc.i4	0x0000
    ret
FAIL:
	ldc.i4	0x0001
    ret
}
}
.assembly c_ldvirtftn.exe {}
