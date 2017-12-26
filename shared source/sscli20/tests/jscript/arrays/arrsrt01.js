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


var iTestID = 71694;

// Global variables
//

var arr1;
var iUndef; // holder for array index
var iCount; // counting variable

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

///////////////////////////////
//
//  FUNCTION ArrSort
//
//  Used in array sort tests that take a sorting function
//  Sorts in REVERSE alphabetical order!

//  this is needed because of posponed bug 314108
@if(@aspx) expando @end function ArrSort(argX, argY) {
	if (argX < argY) {
		return (1);
	}
	if (argX > argY) {
		return (-1);
	}
	return (0);
}



function arrsrt01() {

    apInitTest("arrsrt01: Array.prototype.sort, testing with sparse arrays"); 



	apInitScenario("1 Sort Array of 10 elements, all undefined, no sorting function supplied");
	arr1 = new Array(10);
	arr1.sort();
	verify (arr1.join(),",,,,,,,,,", "1 Sort function failed");


	apInitScenario("2 Sort Array of 10 elements, all undefined except first element, no sorting function supplied");
	arr1 = new Array(10);
	arr1[6] = -3;
	arr1.sort();
	verify (arr1.join(),"-3,,,,,,,,,", "2 Sort function failed");


	apInitScenario("3 Sort Array of 10 elements, all undefined except first element and last element, no sorting function supplied");
	arr1 = new Array(10);
	arr1[0] = -3;
	arr1[9] = 25;
	arr1.sort();
	verify (arr1.join(),"-3,25,,,,,,,,", "3 Sort function failed");


	apInitScenario("4 Sort Array of 10 elements, all undefined except first element and some middle element, no sorting function supplied");
	arr1 = new Array(10);
	arr1[0] = -3;
	arr1[6] = 25;
	arr1.sort();
	verify (arr1.join(),"-3,25,,,,,,,,", "4 Sort function failed");


	apInitScenario("5 Sort Array of 10 elements, all undefined except last element, no sorting function supplied");
	arr1 = new Array(10);
	arr1[9] = 25;
	arr1.sort();
	verify (arr1.join(),"25,,,,,,,,,", "5 Sort function failed");


	apInitScenario("6 Sort Array of 10 elements, all undefined except last element and some middle element, no sorting function supplied");
	arr1 = new Array(10);
	arr1[6] = "Three";
	arr1[9] = 25;
	arr1.sort();
	verify (arr1.join(),"25,Three,,,,,,,,", "6 Sort function failed");


	apInitScenario("7 Sort Array of 10 elements, all defined except for a single undefined element, no sorting function supplied");
	arr1 = new Array(10);
	iUndef = 6;
	for (iCount = 0; iCount < 10; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort();
	verify (arr1.join(),"0,1,2,3,4,5,7,8,9,", "7 Sort function failed");


	apInitScenario("8 Sort Array of 10 elements, odd numbered elements defined and even numbered elements undefined, no sorting function supplied");
	arr1 = new Array(10);
	for (iCount = 0; iCount < 10; iCount++) {
		if (iCount & 1) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort();
	verify (arr1.join(),"1,3,5,7,9,,,,,", "8 Sort function failed");


	apInitScenario("9 Sort Array of 10 elements, odd numbered elements undefined and even numbered elements defined, no sorting function supplied");
	arr1 = new Array(10);
	for (iCount = 0; iCount < 10; iCount++) {
		if (!(iCount & 1)) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort();
	verify (arr1.join(),"0,2,4,6,8,,,,,", "9 Sort function failed");


	apInitScenario("10 Sort Array of 10 elements, all undefined, with sorting function");
	arr1 = new Array(10);
	arr1.sort(ArrSort);
	verify (arr1.join(),",,,,,,,,,", "10 Sort function failed");


	apInitScenario("11 Sort Array of 10 elements, all undefined except first element, with sorting function");
	arr1 = new Array(10);
	arr1[0] = -3;
	arr1.sort(ArrSort);
	verify (arr1.join(),"-3,,,,,,,,,", "11 Sort function failed");


	apInitScenario("12 Sort Array of 10 elements, all undefined except first element and last element, with sorting function");
	arr1 = new Array(10);
	arr1[0] = -3;
	arr1[9] = 25;
	arr1.sort(ArrSort);
	verify (arr1.join(),"25,-3,,,,,,,,", "12 Sort function failed");


	apInitScenario("13 Sort Array of 10 elements, all undefined except first element and some middle element, with sorting function");
	arr1 = new Array(10);
	arr1[0] = -3;
	arr1[6] = 25;
	arr1.sort(ArrSort);
	verify (arr1.join(),"25,-3,,,,,,,,", "13 Sort function failed");


	apInitScenario("14 Sort Array of 10 elements, all undefined except last element, with sorting function");
	arr1 = new Array(10);
	arr1[9] = 25;
	arr1.sort(ArrSort);
	verify (arr1.join(),"25,,,,,,,,,", "14 Sort function failed");


	apInitScenario("15 Sort Array of 10 elements, all undefined except last element and some middle element, with sorting function");
	arr1 = new Array(10);
	arr1[4] = -3;
	arr1[9] = 25;
	arr1.sort(ArrSort);
	verify (arr1.join(),"25,-3,,,,,,,,", "15 Sort function failed");


	apInitScenario("16 Sort Array of 10 elements, all defined except for a single undefined element, no sorting function supplied");
	arr1 = new Array(10);
	iUndef = 6;
	for (iCount = 0; iCount < 10; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort(ArrSort);
	verify (arr1.join(),"9,8,7,5,4,3,2,1,0,", "16 Sort function failed");


	apInitScenario("17 Sort Array of 10 elements, odd numbered elements defined and even numbered elements undefined, no sorting function supplied");
	arr1 = new Array(10);
	for (iCount = 0; iCount < 10; iCount++) {
		if (iCount & 1) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort(ArrSort);
	verify (arr1.join(),"9,7,5,3,1,,,,,", "17 Sort function failed");


	apInitScenario("18 Sort Array of 10 elements, odd numbered elements undefined and even numbered elements defined, no sorting function supplied");
	arr1 = new Array(10);
	for (iCount = 0; iCount < 10; iCount++) {
		if (!(iCount & 1)) {
			arr1[iCount] = iCount;
		}
	}
	arr1.sort(ArrSort);
	verify (arr1.join(),"8,6,4,2,0,,,,,", "18 Sort function failed");


	apInitScenario("19 Sort large array, only first and last 10 elements defined");
	arr1 = new Array(1000);
	for (iCount = 0; iCount < 10; iCount++) {
		arr1[iCount] = iCount;
	}
	for (iCount = 990; iCount < 1000; iCount++) {
		arr1[iCount] = iCount;
	}
	arr1.sort(ArrSort);
	verify (arr1.join(),"999,998,997,996,995,994,993,992,991,990,9,8,7,6,5,4,3,2,1,0,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "19 Sort function failed");



	apEndTest();
}

arrsrt01();


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
