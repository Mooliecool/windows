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


var iTestID = 71695;

// Global variables
//

var arr1;

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrtls01() {

    apInitTest("arrtls01: Array.prototype.toLocaleString"); 


    //----------------------------------------------------------------------------
	//   Array.prototype.toLocaleString (The list separator from Regional Settings
	//   is used to separate items in the array. If the separator does not contain
	//   a trailing space (it can be more than one character in length), one is
	//   appended to it)
	//


	apInitScenario("1 toLocaleString -- Numbers");
	arr1 = new Array(1000.1,100000.001,1000000.0001);
	verify (arr1.toLocaleString(), "1,000.10, 100,000.00, 1,000,000.00", "1 toLocaleString method error");


	apInitScenario("2 toLocaleString -- Strings");
	arr1 = new Array("1000.1","100000.001","1000000.0001");
	verify (arr1.toLocaleString(), "1000.1, 100000.001, 1000000.0001", "2 toLocaleString method error");


	apInitScenario("3 toLocaleString -- Arrays of numbers");
	arr1 = new Array(3);
	arr1[0] = new Array(3,300,3000);
	arr1[1] = new Array(-5000,2400000000,.05);
	arr1[2] = new Array(0.0,.0,0001);
	verify (arr1.toLocaleString(), "3.00, 300.00, 3,000.00, -5,000.00, 2,400,000,000.00, 0.05, 0.00, 0.00, 1.00", "3 toLocaleString method error");

	apInitScenario("4 toLocaleString -- Arrays");
	arr1 = new Array(3);
	arr1[0] = new Array();
	arr1[1] = new Array();
	arr1[2] = new Array();
	verify (arr1.toLocaleString(), ", , ", "4 toLocaleString method error");

	apInitScenario("5 toLocaleString -- Objects");
	arr1 = new Array(3);
	arr1[0] = new Object();
	arr1[0].mem = 25;
	arr1[1] = new Object();
	arr1[0].mem = 23;
	arr1[2] = new Object();
	arr1[0].mem = 22;
	verify (arr1.toLocaleString(), "[object Object], [object Object], [object Object]", "5 toLocaleString method error");


	apInitScenario("6 toLocaleString -- Dates");
	arr1 = new Array(3);
	arr1[0] = new Date(2034,12,31);
	arr1[1] = new Date(1997,11,23)
	arr1[2] = new Date(1945,2,13);
	verify (arr1.toLocaleString(), "Wednesday, January 31, 2035 12:00:00 AM, Tuesday, December 23, 1997 12:00:00 AM, Tuesday, March 13, 1945 12:00:00 AM", "6 toLocaleString method error");

	apEndTest();
}

arrtls01();


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
