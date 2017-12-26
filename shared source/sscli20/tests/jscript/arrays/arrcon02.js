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


var iTestID = 124920;

// Global variables
//

var arr1;
var arr2;
var arr3;

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrcon02() {
	
	var SV = ScriptEngineMajorVersion() + (ScriptEngineMinorVersion() / 10);
	if (SV >= 7.0){
		var iLargestNumber = 0xFFFFFFEF; // (2^32)-16
	}else{
		var iLargestNumber = 0xFFFFFFFF; // (2^32)-1
	}

	apInitTest("arrcon02: Array.prototype.concat method arguments -- VERY LARGE ARRAYS"); 

	arr1 = new Array("One","Two","Three");

	apInitScenario("1 Concat 2^32 elements into arbitrary array");
	arr2 = new Array(iLargestNumber);
	try {
		arr3 = arr1.concat(arr2);
	}
	catch (e) {
		if ( apGetLocale () == 1033 ) {
			verify(e.description,"Out of memory", "1 Wrong error returned");
		}
	}


	apInitScenario("2 Concat into Array of 2^32 elements");
	arr2 = new Array(iLargestNumber);
	try {
		arr3 = arr1.concat(arr2);
	}
	catch (e) {
		if ( apGetLocale () == 1033 ) {
			verify(e.description,"Out of memory","2 Wrong error returned");
		}
	}


	apEndTest();
}

arrcon02();


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
