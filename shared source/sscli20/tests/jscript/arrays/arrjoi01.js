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


var iTestID = 71687;

// Global variables
//

var arr1;
var arr2;
//var iLargestNumber = 0xFFFFFFFF; // (2^32)-1
var iLargestNumber = 10000000; // 10 million
var iUndef; // holder for array index
var iCount; // counting variable
var strTemp;

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrjoi01() {

    apInitTest("arrjoi01: Array.prototype.join method should comma-delimit if no separator is specified"); 



	arr1 = new Array("One","Two","Three");


	apInitScenario("1 Join with no arguments");
	verify (arr1.join(),"One,Two,Three", "1 Join method didn't provide the correct separator");


	apInitScenario("2 Join with one string argument");
	verify (arr1.join("-"), "One-Two-Three", "2 Join method didn't provide the correct separator");


	apInitScenario("3 Join with multi-character string argument");
	verify (arr1.join("+-+"), "One+-+Two+-+Three", "3 Join method didn't provide the correct separator");


	apInitScenario("4 Join with multi-character string argument with leading space");
	verify (arr1.join(" -+"), "One -+Two -+Three", "4 Join method didn't provide the correct separator");


	apInitScenario("5 Join with multi-character string argument with leading space");
	verify (arr1.join("   "), "One   Two   Three", "5 Join method didn't provide the correct separator");


	apInitScenario("6 Join with one empty string argument");
	verify (arr1.join(""), "OneTwoThree", "6 Join method didn't provide the correct separator");


	apInitScenario("7 Join with multiple arguments");
	verify (arr1.join("-", "+"), "One-Two-Three", "7 Join method didn't provide the correct separator");


	apInitScenario("8 Join with one number argument");
	verify (arr1.join(2), "One2Two2Three", "8 Join method didn't provide the correct separator");


	apInitScenario("9 Join with one Object argument");
	var obj = new Object();
    obj.mem = 2;
	verify (arr1.join(obj), "One[object Object]Two[object Object]Three", "9 Join method didn't provide the correct separator");
	obj = null;


	apInitScenario("10 Join with one Array argument");
	arr2 = new Array ("-","+");
	verify (arr1.join(arr2),"One-,+Two-,+Three","10 Join method didn't provide the correct separator");


	apInitScenario("11 Join undefined Array");
	arr2 = new Array();
	verify (arr2.join(),"","11 Join method didn't provide the correct separator");


	apInitScenario("12 Join with undefined Array argument");
	arr2 = new Array();
	verify (arr1.join(arr2), "OneTwoThree", "12 Join method didn't provide the correct separator");


	apInitScenario("13 Join with null argument");
	verify (arr1.join(null), "OnenullTwonullThree", "13 Join method didn't provide the correct separator");


	apInitScenario("14 Join with Array of numbers argument");
	arr2 = new Array(2,5,7);
	verify (arr1.join(arr2), "One2,5,7Two2,5,7Three", "14 Join method didn't provide the correct separator");


	apInitScenario("15 Join with Array of strings argument");
	arr2 = new Array("2","5","7");
	verify (arr1.join(arr2), "One2,5,7Two2,5,7Three", "15 Join method didn't provide the correct separator");


	apInitScenario("16 Join with Array of objects argument");
	arr2 = new Array(3);
	arr2[0] = new Object;
	arr2[1] = new Object;
	arr2[2] = new Object;
	verify (arr1.join(arr2), "One[object Object],[object Object],[object Object]Two[object Object],[object Object],[object Object]Three", "16 Join method didn't provide the correct separator");


	apInitScenario("17 Join with Array of arrays argument");
	arr2 = new Array(3);
	arr2[0] = new Array(1,2);
	arr2[1] = new Array(3,4);
	arr2[2] = new Array(5,6);
	verify (arr1.join(arr2), "One1,2,3,4,5,6Two1,2,3,4,5,6Three", "17 Join method didn't provide the correct separator");


	apInitScenario("18 Join with Array of multiple element types argument");
	arr2 = new Array(3);
	arr2[0] = new Array(1,2);
	arr2[1] = new Array("3","4");
	arr2[2] = new Array(5.01,6.01);
	verify (arr1.join(arr2), "One1,2,3,4,5.01,6.01Two1,2,3,4,5.01,6.01Three", "18 Join method didn't provide the correct separator");

	

	apEndTest();
}

arrjoi01();


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
