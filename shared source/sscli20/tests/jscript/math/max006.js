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


var iTestID = 78311;


//Max006.js - ECMA3 compliance Math.Max testing (multiple argument list)

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) {
        apLogFailInfo (sMsg, sExp, sRes, sBug);
    }
}

function max006() {

	apInitTest ("Math.max ECMA3 changes testing - support for any number of args");

	//----------------------------------------------------------------
	apInitScenario("1. no arguments");

	verify (Math.max(), -Infinity, "no arguments", "");
	verify (Math.max.length, 2, "length property", );

	//----------------------------------------------------------------
	apInitScenario("2. just 1 argument");

	verify (Math.max(1), 1, "1 argument 1", "");
	verify (Math.max(0), 0, "1 argument 2", "");
	verify (Math.max(Number("00.008")), 0.008, "1 argument 3", "");
	verify (Math.max(Number.MAX_VALUE), 1.7976931348623157e308, "1 argument 4", "");
	verify (Math.max(Number.MIN_VALUE), 5e-324, "1 argument 5", "");

	//----------------------------------------------------------------
	apInitScenario("3. multiple arguments");
	
	var x = 5.4839e52;
	verify (Math.max(1, 2, 3, 4, 5, 6, 7, 8), 8, "multiple arguments 1", "");
	verify (Math.max(-1, 0, 1), 1, "multiple arguments 2", "");
	verify (Math.max(5.6, 10,2, 584749.12395, 660000), 660000, "multiple arguments 3", "");
	verify (Math.max(0.0, 0.48593e-10, x, "5.5e52"), 5.5e52, "multiple arguments 4", "");
	verify (Math.max(Number(Infinity), Infinity), Infinity, "multiple arguments 5", "");
	verify (Math.max(12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12), 12, "multiple arguments 6", "");
	verify (Math.max("5.6", -13.6, 0.53), 5.6, "multiple arguments 7", "");
       verify (Math.max(Number(-Infinity), -Infinity), -Infinity, "multiple arguments 8", "");
       verify (Math.max(-Infinity, Infinity), Infinity, "multiple arguments 9", "");


	apEndTest();
}


max006();


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
