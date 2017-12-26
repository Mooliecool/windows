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


var iTestID = 53752;

//two main scenarios
//generate NaN's to verify that atan2 conforms with the spec
//generate numerical answers to make sure that atan2 conformas w/ spec

var p_infinity = Number.POSITIVE_INFINITY;
var n_infinity = Number.NEGATIVE_INFINITY;
var nan = Number.NaN;
	
function printOutput(a)
{	//this function evaluates the string that is passed in

	var b_noErr, bExp = true;
	
	with (Math)
		var specialEval = eval(a);
	
	if ((typeof specialEval) == 'boolean')
		b_noErr = specialEval;
	else
		b_noErr = false;

	if (!b_noErr)
		apLogFailInfo(a+" failed: "+" "+specialEval, bExp, b_noErr, "");
}

	
function equal(a, b)
{	//this function verifies the equality of two numbers sent to it
	//including the numbers' sign i.e. for this function +0 != -0

	var bTest = (a == b) && (1 / a == 1 / b);
	if (bTest)
		return true;
	return 'false (' + a + ', ' + b + ', ' + (1/a) + ', ' + (1/b) + ')';
}

function atan2001() {

    apInitTest("atan2_001 ");	

	//atan2(y,x)


	apInitScenario("1. Atan2 tests that produce NaN answers");

	// atan2 (nan, *)
	printOutput("isNaN(atan2(nan, nan))");
	printOutput("isNaN(atan2(nan, p_infinity))");
	printOutput("isNaN(atan2(nan, n_infinity))");
	printOutput("isNaN(atan2(nan, +1))");
	printOutput("isNaN(atan2(nan, -1))");
	printOutput("isNaN(atan2(nan, +0))");
	printOutput("isNaN(atan2(nan, -0))");
	
	// atan2 (*, nan)
	printOutput("isNaN(atan2(p_infinity, nan))");
	printOutput("isNaN(atan2(+1, nan))");
	printOutput("isNaN(atan2(+0, nan))");
	printOutput("isNaN(atan2(n_infinity, nan))");
	printOutput("isNaN(atan2(-1, nan))");
	printOutput("isNaN(atan2(-0, nan))");
	

	apInitScenario("2. Atan2 tests that produce Numeric answers");

	// atan2 (+1, *)
	printOutput("equal(atan2(+1, p_infinity), +0)");
	printOutput("equal(atan2(+1, n_infinity), +PI)");
	printOutput("equal(atan2(+1, +1), +PI / 4)");
	printOutput("equal(atan2(+1, -1), +3 * PI / 4)");
	printOutput("equal(atan2(+1, +0), +PI / 2)");
	printOutput("equal(atan2(+1, -0), +PI / 2)");

	// atan2 (-1, *)
	printOutput("equal(atan2(-1, p_infinity), -0)");
	printOutput("equal(atan2(-1, n_infinity), -PI)");
	printOutput("equal(atan2(-1, +1), -PI / 4)");
	printOutput("equal(atan2(-1, -1), -3 * PI / 4)");
	printOutput("equal(atan2(-1, +0), -PI / 2)");
	printOutput("equal(atan2(-1, -0), -PI / 2)");

	// atan2 (+0, *)
	printOutput("equal(atan2(+0, p_infinity), +0)");
	printOutput("equal(atan2(+0, n_infinity), +PI)");
	printOutput("equal(atan2(+0, +1), +0)");
	printOutput("equal(atan2(+0, -1), +PI)");
	printOutput("equal(atan2(+0, +0), +0)");
	printOutput("equal(atan2(+0, -0), +PI)");
	
	// atan2 (-0, *)
	printOutput("equal(atan2(-0, p_infinity), -0)");
	printOutput("equal(atan2(-0, n_infinity), -PI)");
	printOutput("equal(atan2(-0, +1), -0)");
	printOutput("equal(atan2(-0, -1), -PI)");
	printOutput("equal(atan2(-0, +0), -0)");
	printOutput("equal(atan2(-0, -0), -PI)");
	
	// atan2 (p_infinity, *)  see bug233663
	@if(@_jscript_version<7)
	printOutput("equal(atan2(p_infinity, p_infinity), +PI / 4)");
	printOutput("equal(atan2(p_infinity, n_infinity), +3 * PI / 4)");
	@else
	printOutput("isNaN(atan2(p_infinity, p_infinity), nan)");
	printOutput("isNaN(atan2(p_infinity, n_infinity), nan)");
	@end
	printOutput("equal(atan2(p_infinity, +1), +PI / 2)");
	printOutput("equal(atan2(p_infinity, -1), +PI / 2)");
	printOutput("equal(atan2(p_infinity, +0), +PI / 2)");
	printOutput("equal(atan2(p_infinity, -0), +PI / 2)");

	// atan2 (n_infinity, *)
	@if(@_jscript_version<7)
	printOutput("equal(atan2(n_infinity, p_infinity), -PI / 4)");
	printOutput("equal(atan2(n_infinity, n_infinity), -3 * PI / 4)");
	@else
	printOutput("isNaN(atan2(n_infinity, p_infinity), nan)");
	printOutput("isNaN(atan2(n_infinity, n_infinity), nan)");
	@end
	printOutput("equal(atan2(n_infinity, +1), -PI / 2)");
	printOutput("equal(atan2(n_infinity, -1), -PI / 2)");
	printOutput("equal(atan2(n_infinity, +0), -PI / 2)");
	printOutput("equal(atan2(n_infinity, -0), -PI / 2)");

/*****************************************************************************/


    apEndTest();

}

	


atan2001();


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
