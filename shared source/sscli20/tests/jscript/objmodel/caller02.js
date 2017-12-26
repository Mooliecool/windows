//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 53110;


function verify(msg, arg1, arg2){
   if(arg1!=arg2)
	apLogFailInfo(" Scenario failed: " + msg,arg2,arg1,"");
}

function caller02(){
    apInitTest("caller02");

    //--------------------------------------------------------------------
    apInitScenario("Top level. Arg.callee == function name - not in fast mode");
@if(!@_fast)
    verify("Top level. Arg.callee == function name",caller02.arguments.callee, caller02);

    A(caller02.arguments);
@else
	A("damn, i hate fast mode");
@end

    // Functionality of the arguments object has changed for version 7.0
    
    apInitScenario("test arguments object embedded in multiple functions w/eval - not in fast mode");
@if(!@_fast)
    eval("AE(null)");
@end
    AR(0);

    apEndTest();
}


function A(x)
{
	//--------------------------------------------------------------------
    apInitScenario("Second level. Arg.callee == second level function - not in fast mode");
@if(!@_fast)
	verify("Second level. Arg.callee == second level function",A.arguments.callee , A);
@end
	//--------------------------------------------------------------------
    apInitScenario("Second level. Arg.callee == second level function - not in fast mode");
@if(!@_fast)
	verify("Second level. Arg.callee == second level function",A.caller , caller02);
@end
	//--------------------------------------------------------------------
    apInitScenario("Second level. Arg.caller == current args - not in fast mode");
@if(!@_fast)
	verify("Second level. Arg.caller == current args",A.arguments.caller , x);

	B(A.arguments);
@else
	B("this is quite a pain in the rear");
@end
}


function B(x)
{
	//--------------------------------------------------------------------
    apInitScenario("Third level. Arg.callee == third level function - not in fast mode");
@if(!@_fast)
	verify("Third level. Arg.callee == third level function",B.arguments.callee, B);
@end
	//--------------------------------------------------------------------
    apInitScenario("Third level. Function.caller == second level function - not in fast mode");
@if(!@_fast)
	verify("Third level. Function.caller == second level function",B.caller , A);
@end
	//--------------------------------------------------------------------
    apInitScenario("Third level. Function.caller.caller == Top level function - not in fast mode");
@if(!@_fast)
	verify("Third level. Function.caller.caller == Top level function",B.caller.caller , caller02);
@end
	//--------------------------------------------------------------------
    apInitScenario("Third level. Args.caller == current args - not in fast mode");
@if(!@_fast)
	verify("Third level. Args.caller == current args",B.arguments.caller , x);

	C(B.arguments);
@else
	C("The new Maddog apInitScen counter doesn't help");
@end
}


function C(x)
{
	//--------------------------------------------------------------------
    apInitScenario("Fourth level. Args.callee == fourth level function - not in fast mode");
@if(!@_fast)
	verify("Fourth level. Args.callee == fourth level function",C.arguments.callee , C);
@end
	//--------------------------------------------------------------------
    apInitScenario("Fourth level. Function.caller == third level function - not in fast mode");
@if(!@_fast)
	verify("Fourth level. Function.caller == third level function",C.caller , B);
@end
	//--------------------------------------------------------------------
    apInitScenario("Fourth level. Function.caller.caller == second level function - not in fast mode");
@if(!@_fast)
	verify("Fourth level. Function.caller.caller == second level function",C.caller.caller , A);
@end
	//--------------------------------------------------------------------
    apInitScenario("Fourth level. Func.caller.caller.caller == Top level function - not in fast mode");
@if(!@_fast)
	verify("Fourth level. Func.caller.caller.caller == Top level function",C.caller.caller.caller , caller02);
@end
	//--------------------------------------------------------------------
    apInitScenario("Fourth level. Args.caller == current args - not in fast mode");
@if(!@_fast)
	verify("Fourth level. Args.caller == current args",C.arguments.caller , x);
@end
}

@if (@_jscript_version < 7) 
//the arguments chain is different depending on whether you are in JScript 5.x or 7.0
//this is slightly messier since the maddog fix to count scenarios, replace apInitScenario's when debugging
function AE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). args.callee == Top level function");
	verify("jscript_version < 7: Top level eval(). args.callee == Top level function",AE.arguments.callee , AE);
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). Function.caller == null");
	verify("jscript_version < 7: Top level eval(). Function.caller == null",AE.caller , null);
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). args.caller == null");
	verify("jscript_version < 7: Top level eval(). args.caller == null",AE.arguments.caller , null);
	eval("BE(AE.arguments)");
}


function BE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). args.callee == Second level function");
	verify("jscript_version < 7: Second level eval(). args.callee == Second level function",BE.arguments.callee , BE);
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). Function.caller == null");
	verify("jscript_version < 7: Second level eval(). Function.caller == null",BE.caller , null);
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). args.caller == null");
	verify("jscript_version < 7: Second level eval(). args.caller == null",BE.arguments.caller , null);

	eval("CE(BE.arguments)");
}


function CE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). args.callee == Third level function");
	verify("jscript_version < 7: Third level eval(). args.callee == Third level function",CE.arguments.callee , CE);
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). Function.caller == null");
	verify("jscript_version < 7: Third level eval(). Function.caller == null",CE.caller , null);
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). args.caller == null");
	verify("jscript_version < 7: Third level eval(). args.caller == null",CE.arguments.caller , null);
}


@else //the arguments chain is different depending on whether you are in JScript 5.x or 7.0
@if(!@_fast)
function AE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). args.callee == Top level function");
	verify("jscript_version >= 7: Top level eval(). args.callee == Top level function",AE.arguments.callee , AE);
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). Function.caller == null");
	verify("jscript_version >= 7: Top level eval(). Function.caller == null",AE.caller , caller02);
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). typeof(args.caller) == object");
	verify("jscript_version >= 7: Top level eval(). typeof(args.caller) == object",typeof(arguments.caller), "object");
	//--------------------------------------------------------------------
    //apInitScenario("Top level eval(). args.caller.callee == caller02");
	verify("jscript_version >= 7: Top level eval(). args.caller.callee == caller02",arguments.caller.callee , caller02);

	eval("BE(arguments)");
}

function BE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). args.callee == Second level function");
	verify("jscript_version >= 7: Second level eval(). args.callee == Second level function",BE.arguments.callee , BE);
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). typeof(Function.caller) == function");
	verify("jscript_version >= 7: Second level eval(). typeof(Function.caller) == function",typeof(BE.caller) , "function");
	//--------------------------------------------------------------------
    //apInitScenario("Second level eval(). args.caller == null");
	verify("jscript_version >= 7: Second level eval(). args.caller == null",BE.arguments.caller.callee , AE);

	eval("CE(BE.arguments)");
}


function CE(x)
{
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). args.callee == Third level function");
	verify("jscript_version >= 7: Third level eval(). args.callee == Third level function",CE.arguments.callee , CE);
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). typeof(Function.caller) == function");
	verify("jscript_version >= 7: Third level eval(). typeof(Function.caller) == function",typeof(CE.caller) , "function");
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). typeof(args.caller) == object");
	verify("jscript_version >= 7: Third level eval(). typeof(args.caller) == object",typeof(CE.arguments.caller), "object");
	//--------------------------------------------------------------------
    //apInitScenario("Third level eval(). args.caller == null");
	verify("jscript_version >= 7: Third level eval(). args.caller == null",CE.arguments.caller.callee , BE);
}

@end 	//not in fast mode
@end	//version 7.0 conditional


function AR(c)
{
	if (c < 1)
		{
		//--------------------------------------------------------------------
    	apInitScenario("Top level recursion. Function.caller == main() - not in fast mode");
@if(!@_fast)
		verify("Top level recursion. Function.caller == main()",AR.caller , caller02);
@end
		AR(c + 1);
		}
	else
	{
		//--------------------------------------------------------------------
    	apInitScenario("Second level recursion. Function.caller == Function name - not in fast mode");
@if(!@_fast)
		verify("Second level recursion. Function.caller == Function name",AR.caller , AR);
@end
	}
}


caller02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}
