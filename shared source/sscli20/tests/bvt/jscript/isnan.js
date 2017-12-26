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
import System

System.Environment.ExitCode = 0;

if(isNaN(parseFloat("1.23"))){
	System.Environment.ExitCode = 1;
}
if(isNaN(parseInt("1.23"))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseFloat("hello"))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt("hello"))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseFloat("hello") + 1)){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt("hello") + 1)){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,1))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,-1))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,-2))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,-10))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,37))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,40))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,100))){
	System.Environment.ExitCode = 1;
}
if(!isNaN(parseInt(123,100000))){
	System.Environment.ExitCode = 1;
}
if(isNaN(parseInt(123,10))){
	System.Environment.ExitCode = 1;
}
if(isNaN(parseInt(123,36))){
	System.Environment.ExitCode = 1;
}
if(isNaN(parseInt(123,0))){
	System.Environment.ExitCode = 1;
}

var chartest = "hello";

if(!isNaN(chartest.charCodeAt(-1))){
	System.Environment.ExitCode = 1;
}
