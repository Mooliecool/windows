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
/////////////////////////////////////////////////////////////////////////
import System;

function fib(i : int) : int
{
	return i <= 0 ? 0
	     : i == 1 ? 1
	     : fib(i-1) + fib(i-2);
}

if(fib(20) == 6765) {
	System.Environment.ExitCode = 0;
} else {
	System.Environment.ExitCode = 1;
}

