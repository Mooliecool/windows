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


var iTestID = 72430;



//////////
//
//  Test case:
//
//     strcha01: String.prototype.charAt should return empty string if index is out-of-bounds
//
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes, sBUG){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, sBUG);
}



//////////
//
// Global variables


var str1;
var undefined;



function strcha01() {

    apInitTest("strcha01: String.prototype.charAt should return empty string if index is out-of-bounds"); 


	str1 = "My dog has fleas.";

	apInitScenario("1 No index argument");
	verify (str1.charAt(), "M", "1 Wrong character returned", "VS");	


	apInitScenario("2 Null index argument");
	verify (str1.charAt(null), "M", "2 Wrong character returned", "");	


	apInitScenario("3 Undefined index argument");
	verify (str1.charAt(undefined), "M", "3 Wrong character returned", "");	


	apInitScenario("4 Index == 0");
	verify (str1.charAt(0), "M", "4 Wrong character returned", "");	


	apInitScenario("5 Index == 1");
	verify (str1.charAt(1), "y", "5 Wrong character returned", "");	


	apInitScenario("6 Index == -1");
	verify (str1.charAt(-1), "", "6 Wrong character returned", "");	


	apInitScenario("7 Index == String.length - 1");
	verify (str1.charAt(str1.length-1), ".", "7 Wrong character returned", "");	


	apInitScenario("8 Index == String.length");
	verify (str1.charAt(str1.length), "", "8 Wrong character returned", "");	


	apInitScenario("9 Index == String.length + 1");
	verify (str1.charAt(str1.length + 1), "", "9 Wrong character returned", "");	


	apInitScenario("10 Index == large positive number");
	verify (str1.charAt(Int32.MaxValue), "", "10 Wrong character returned", "");	


	apInitScenario("11 Index == large negative number");
	verify (str1.charAt(Int32.MinValue), "", "11 Wrong character returned", "");	


	apInitScenario("12 Index is float value");
	verify (str1.charAt(3.14159), "d", "12 Wrong character returned", "");	


	apInitScenario("13 Index is empty string");
	verify (str1.charAt(""), "M", "13 Wrong character returned", "");	


	apInitScenario("14 Index is non-empty string of letters");
	verify (str1.charAt("dog"), "M", "14 Wrong character returned", "");	


	apInitScenario("15 Index is non-empty string of numbers");
	verify (str1.charAt("3"), "d", "15 Wrong character returned", "");	


	apInitScenario("16 Index is Array");
	var arr1 = new Array(3);
	arr1[0] = 3;
	arr1[1] = 4;
	arr1[2] = 5;
	verify (str1.charAt(arr1), "M", "16 Wrong character returned", "");	


	apInitScenario("17 Index is Object");
	var obj1 = new Object();
	obj1.mem = 3;
	verify (str1.charAt(arr1), "M", "17 Wrong character returned", "");	


	apEndTest();
}

strcha01();


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
