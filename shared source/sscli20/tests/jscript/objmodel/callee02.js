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


var iTestID = 79155;


/* callee02.js - test some recursive functions on the callee property
*/

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

function callee02() {

	apInitTest ("Recursive tests using the function callee property - not run in fast mode");
	
	apInitScenario ("1. sum of positive numbers up to n");
@if(!@_fast)
		function sumNum(n) {if (n < 1) return 0; else return (n + arguments.callee(n-1));}
		verify (sumNum(4), 10, "sumNum up to n", "");
@end
	apInitScenario ("2. factorial");
@if(!@_fast)
		function factorial(n) {if (n<=1) return 1; else return (n * factorial.arguments.callee(n-1));}
		verify (factorial(4), 24, "factorial function testing", "");
@end
	apEndTest();
}


callee02();


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
