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


var iTestID = 241812;

// Global variables
//

var arr1;
var arr2;
//var iLargestNumber = 0xFFFFFFFF; // (2^32)-1
var iLargestNumber = 10000000; // 10 million
// 10,000,000 causes IIS to timeout on aspx, reduce number in this case
@if(@aspx)
  var iLargestNumber = 1000000; // 1 million
@else
  var iLargestNumber = 10000000; // 10 million
@end
var iUndef; // holder for array index
var iCount; // counting variable
var strTemp;

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrjoi02() {

    apInitTest("arrjoi02: Array.prototype.join method should comma-delimit if no separator is specified"); 

	var obj = new Object();
       obj.mem = 2;

	apInitScenario("19 Join Array of 100 elements, all undefined");
	arr1 = new Array(100);
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "19 Join method didn't provide the correct separator");


	apInitScenario("20 Join Array of 100 elements, all undefined except for first element");
	arr1 = new Array(100);
	arr1[0] = 30;
	verify (arr1.join(), "30,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "20 Join method didn't provide the correct separator");


	apInitScenario("21 Join Array of 100 elements, all undefined except for first element and last element");
	arr1 = new Array(100);
	arr1[0] = 30;
	arr1[99] = -23;
	verify (arr1.join(), "30,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-23", "21 Join method didn't provide the correct separator");


	apInitScenario("22 Join Array of 100 elements, all undefined except for first element and some middle element");
	arr1 = new Array(100);
	arr1[0] = 30;
	arr1[30] = -23;
	verify (arr1.join(), "30,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "22 Join method didn't provide the correct separator");


	apInitScenario("23 Join Array of 100 elements, all undefined except last element");
	arr1 = new Array(100);
	arr1[99] = -43;
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-43", "23 Join method didn't provide the correct separator");


	apInitScenario("24 Join Array of 100 elements, all undefined except last element and some middle element");
	arr1 = new Array(100);
	arr1[99] = -43;
	arr1[30] = -23;
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-43", "24 Join method didn't provide the correct separator");


	apInitScenario("25 Join Array of 100 elements, all defined except for a single undefined element");
	arr1 = new Array(100);
	iUndef = 33;
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.join(), "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99", "25 Join method didn't provide the correct separator");


	apInitScenario("26 Join Array of 100 elements, odd elements defined and even elements undefined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount & 1) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.join(), ",1,,3,,5,,7,,9,,11,,13,,15,,17,,19,,21,,23,,25,,27,,29,,31,,33,,35,,37,,39,,41,,43,,45,,47,,49,,51,,53,,55,,57,,59,,61,,63,,65,,67,,69,,71,,73,,75,,77,,79,,81,,83,,85,,87,,89,,91,,93,,95,,97,,99", "26 Join method didn't provide the correct separator");


	apInitScenario("27 Join Array of 100 elements, odd elements undefined and even elements defined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (!(iCount & 1)) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.join(), "0,,2,,4,,6,,8,,10,,12,,14,,16,,18,,20,,22,,24,,26,,28,,30,,32,,34,,36,,38,,40,,42,,44,,46,,48,,50,,52,,54,,56,,58,,60,,62,,64,,66,,68,,70,,72,,74,,76,,78,,80,,82,,84,,86,,88,,90,,92,,94,,96,,98,", "27 Join method didn't provide the correct separator");


	apInitScenario("28 Join undefined Array");
@if(!@rotor)
	arr1 = new Array(iLargestNumber);
	try {
		strTemp = arr1.join();
	}
	catch (e) {
		verify (e.number,-2146828281, "28 Wrong error returned");
	}
@end	

	apEndTest();
}

arrjoi02();


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
