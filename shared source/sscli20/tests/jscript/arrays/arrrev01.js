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


var iTestID = 71690;

// Global variables
//

var arr1;
var iUndef; // holder for array index
var iCount; // counting variable

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrrev01() {

    apInitTest("arrrev01: Array.prototype.reverse, testing with sparse arrays"); 



	apInitScenario("1 Reverse Array of 100 elements, all undefined");
	arr1 = new Array(100);
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "1.1 Reverse method failed");
	verify (arr1.length, 100, "1.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "1.3 Reverse method failed");
	verify (arr1.length, 100, "1.4 Reverse method error");

	apInitScenario("2 Reverse Array of 100 elements, all undefined except first element");
	arr1 = new Array(100);
	arr1[0] = -3;
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "2.1 Reverse method failed");
	verify (arr1.length, 100, "2.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), "-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "2.3 Reverse method failed");
	verify (arr1.length, 100, "2.4 Reverse method error");


	apInitScenario("3 Reverse Array of 100 elements, all undefined except first element and last element");
	arr1 = new Array(100);
	arr1[0] = -3;
	arr1[99] = 23;
	arr1.reverse();
	verify (arr1.join(), "23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "3.1 Reverse method failed");
	verify (arr1.length, 100, "3.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), "-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23", "3.3 Reverse method failed");
	verify (arr1.length, 100, "3.4 Reverse method error");


	apInitScenario("4 Reverse Array of 100 elements, all undefined except first element and some middle element");
	arr1 = new Array(100);
	arr1[0] = -3;
	arr1[30] = 23;
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "4.1 Reverse method failed");
	verify (arr1.length, 100, "4.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), "-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "4.3 Reverse method failed");
	verify (arr1.length, 100, "4.4 Reverse method error");


	apInitScenario("5 Reverse Array of 100 elements, all undefined except last element");
	arr1 = new Array(100);
	arr1[99] = -3;
	arr1.reverse();
	verify (arr1.join(), "-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "5.1 Reverse method failed");
	verify (arr1.length, 100, "5.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "5.3 Reverse method failed");
	verify (arr1.length, 100, "5.4 Reverse method error");


	apInitScenario("6 Reverse Array of 100 elements, all undefined except last element and some middle element");
	arr1 = new Array(100);
	arr1[99] = -3;
	arr1[30] = 23;
	arr1.reverse();
	verify (arr1.join(), "-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "6.1 Reverse method failed");
	verify (arr1.length, 100, "6.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "6.3 Reverse method failed");
	verify (arr1.length, 100, "6.4 Reverse method error");


	apInitScenario("7 Reverse Array of 100 elements, all defined except for a single undefined element");
	arr1 = new Array(100);
	iUndef = 43;
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	arr1.reverse();
	verify (arr1.join(), "99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0", "7.1 Reverse method failed");
	verify (arr1.length, 100, "7.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99", "7.3 Reverse method failed");
	verify (arr1.length, 100, "7.4 Reverse method error");


	apInitScenario("8 Reverse to Array of 100 elements, odd elements defined and even elements undefined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount & 0x1) {
			arr1[iCount] = iCount;
		}
	}
	arr1.reverse();
	verify (arr1.join(), "99,,97,,95,,93,,91,,89,,87,,85,,83,,81,,79,,77,,75,,73,,71,,69,,67,,65,,63,,61,,59,,57,,55,,53,,51,,49,,47,,45,,43,,41,,39,,37,,35,,33,,31,,29,,27,,25,,23,,21,,19,,17,,15,,13,,11,,9,,7,,5,,3,,1,", "8.1 Reverse method returned wrong value");
	verify (arr1.length, 100, "8.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), ",1,,3,,5,,7,,9,,11,,13,,15,,17,,19,,21,,23,,25,,27,,29,,31,,33,,35,,37,,39,,41,,43,,45,,47,,49,,51,,53,,55,,57,,59,,61,,63,,65,,67,,69,,71,,73,,75,,77,,79,,81,,83,,85,,87,,89,,91,,93,,95,,97,,99", "8.3 Reverse method returned wrong value");
	verify (arr1.length, 100, "8.4 Reverse method error");


	apInitScenario("9 Reverse Array of 100 elements, odd elements undefined and even elements defined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (!(iCount & 0x1)) {
			arr1[iCount] = iCount;
		}
	}
	arr1.reverse();
	verify (arr1.join(), ",98,,96,,94,,92,,90,,88,,86,,84,,82,,80,,78,,76,,74,,72,,70,,68,,66,,64,,62,,60,,58,,56,,54,,52,,50,,48,,46,,44,,42,,40,,38,,36,,34,,32,,30,,28,,26,,24,,22,,20,,18,,16,,14,,12,,10,,8,,6,,4,,2,,0", "9.1 Reverse method returned wrong value");
	verify (arr1.length, 100, "9.2 Reverse method error");
	arr1.reverse();
	verify (arr1.join(), "0,,2,,4,,6,,8,,10,,12,,14,,16,,18,,20,,22,,24,,26,,28,,30,,32,,34,,36,,38,,40,,42,,44,,46,,48,,50,,52,,54,,56,,58,,60,,62,,64,,66,,68,,70,,72,,74,,76,,78,,80,,82,,84,,86,,88,,90,,92,,94,,96,,98,", "9.3 Reverse method returned wrong value");
	verify (arr1.length, 100, "9.4 Reverse method error");


	apInitScenario("10 Reverse array of 100 elements. Contents of array are of different types. All undefined except for some middle elements");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		switch (iCount) {
			case 10: 
			case 20: 
			case 30:
			case 40:
				arr1[iCount] = false;
				break;
			case 53:
			case 63:
			case 73:
			case 83:
				arr1[iCount] = true;
				break;
			case 48:
			case 49:
			case 50:
			case 51:
				arr1[iCount] = iCount;
				break;
			case 22:
			case 24:
			case 26:
			case 28:
				arr1[iCount] = "hello";
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				arr1[iCount] = iCount + 0.25;
				break;
		}
		
    }
	arr1.reverse();
	verify (arr1.join(), ",,,,,,,,,,,,,,,,true,,,,,,,,,,true,,,,,,,,,,true,,,,,,,,,,true,,51,50,49,48,,,,,,,,false,,38.25,37.25,36.25,35.25,,,,,false,,hello,,hello,,hello,,hello,,false,,,,,,,,,,false,,,,,,,,,,", "10. Reverse method returned wrong value");


	apEndTest();
}



arrrev01();


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
