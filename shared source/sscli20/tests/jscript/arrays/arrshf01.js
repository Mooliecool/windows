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


var iTestID = 71691;

// Global variables
//

var arr1;
var iLargestNumber = 0xFFFFFFFF; // (2^32)-1
var iUndef; // holder for array index
var iCount; // counting variable

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrshf01() {

    apInitTest("arrshf01: Array.prototype.shift"); 


	apInitScenario("1 Shift empty array");
	arr1 = new Array();
	var iTmp = arr1.shift();
	verify (iTmp, undefined, "1.1 Shift method failed");
	verify (arr1.join(), "", "1.2 Shift method failed");
	verify (arr1.length, 0, "1.3 Shift method failed");


	apInitScenario("2 Shift from single element Array of Numbers");
	arr1 = new Array(1);	
	arr1[0] = 3;
	verify (arr1.shift(), 3, "2.1 Shift from array of numbers returned wrong value");
	verify (arr1.length, 0, "2.2 Shift method failed");


	apInitScenario("3 Shift from single element Array of Strings");
	arr1 = new Array(1);	
	arr1[0] = "Three";
	verify (arr1.shift(), "Three", "3.1 Shift from array of strings returned wrong value");
	verify (arr1.length, 0, "3.2 Shift method failed");


	apInitScenario("4 Shift from single element Array of Objects");
	arr1 = new Array(1);	
	arr1[0] = new Object;
	verify (arr1.shift(), "[object Object]", "4.1 Shift from array of objects returned wrong value");
	verify (arr1.length, 0, "4.2 Shift method failed");


	apInitScenario("5 Shift from single element Array of Arrays");
	arr1 = new Array(1);	
	arr1[0] = new Array(3);
	arr1[0][0] = 3;
	arr1[0][1] = 4;
	arr1[0][2] = 5;
	verify (arr1.shift(), "3,4,5", "5.1 Shift from array of arrays returned wrong value");
	verify (arr1.length, 0, "5.2 Shift method failed");


	apInitScenario("6 Shift from single element Array of multiple types");
	arr1 = new Array(1);	
	arr1[0] = new Array(3);
	arr1[0][0] = 3;
	arr1[0][1] = "Four";
	arr1[0][2] = 5.01;
	verify (arr1.shift(), "3,Four,5.01", "6.1 Shift from array of arrays returned wrong value");
	verify (arr1.length, 0, "6.2 Shift method failed");


	apInitScenario("7 Shift from multiple element Array of Numbers");
	arr1 = new Array(3,5,8);
	verify (arr1.shift(), 3, "7.1 Shift from array of numbers returned wrong value");
	verify (arr1.length, 2, "7.2 Shift from array of numbers didn't decrement length correctly");
	verify (arr1.shift(), 5, "7.3 Shift from array of numbers returned wrong value");
	verify (arr1.length, 1, "7.4 Shift from array of numbers didn't decrement length correctly");
	verify (arr1.shift(), 8, "7.5 Shift from array of numbers returned wrong value");
	verify (arr1.length, 0, "7.6 Shift from array of numbers didn't decrement length correctly");
	verify (arr1.shift(), undefined, "7.7 Shift from array of numbers returned wrong value");


	apInitScenario("8 Shift from multiple element Array of Strings");
	arr1 = new Array("Three", "Five", "Eight");
	verify (arr1.shift(), "Three", "8.1 Shift from array of strings returned wrong value");
	verify (arr1.length, 2, "8.2 Shift from array of strings didn't decrement length correctly");
	verify (arr1.shift(), "Five", "8.3 Shift from array of strings returned wrong value");
	verify (arr1.length, 1, "8.4 Shift from array of strings didn't decrement length correctly");
	verify (arr1.shift(), "Eight", "8.5 Shift from array of strings returned wrong value");
	verify (arr1.length, 0, "8.6 Shift from array of strings didn't decrement length correctly");
	verify (arr1.shift(), undefined, "8.7 Shift from array of strings returned wrong value");


	apInitScenario("9 Shift from multiple element Array of Objects");
	arr1 = new Array(3);	
	arr1[0] = new Object;
	arr1[0].mem = 23;
	arr1[1] = new Object;
	arr1[1].mem = 24;
	arr1[2] = new Object;
	arr1[2].mem = 25;
	verify (arr1.shift().mem, 23, "9.1 Shift from array of objects returned wrong value");
	verify (arr1.length, 2, "9.2 Shift from array of objects didn't decrement length correctly");
	verify (arr1.shift().mem, 24, "9.3 Shift from array of objects returned wrong value");
	verify (arr1.length, 1, "9.4 Shift from array of objects didn't decrement length correctly");
	verify (arr1.shift().mem, 25, "9.5 Shift from array of objects returned wrong value");
	verify (arr1.length, 0, "9.6 Shift from array of objects didn't decrement length correctly");
	verify (arr1.shift(), undefined, "9.7 Shift from array of objects returned wrong value");


	apInitScenario("10 Shift from multiple element Array of Arrays");
	arr1 = new Array(3);	
	arr1[0] = new Array(3);
	arr1[0][0] = 3;
	arr1[0][1] = 5;
	arr1[0][2] = 8;
	arr1[1] = new Array(3);
	arr1[1][0] = 30;
	arr1[1][1] = 50;
	arr1[1][2] = 80;
	arr1[2] = new Array(3);
	arr1[2][0] = 300;
	arr1[2][1] = 500;
	arr1[2][2] = 800;
	verify (arr1.shift().join(), "3,5,8", "10.1 Shift from array of arrays returned wrong value");
	verify (arr1.length, 2, "10.2 Shift from array of arrays didn't decrement length correctly");
	verify (arr1.shift().join(), "30,50,80", "10.3 Shift from array of arrays returned wrong value");
	verify (arr1.length, 1, "10.4 Shift from array of arrays didn't decrement length correctly");
	verify (arr1.shift().join(), "300,500,800", "10.5 Shift from array of arrays returned wrong value");
	verify (arr1.length, 0, "10.6 Shift from array of arrays didn't decrement length correctly");


	apInitScenario("11 Shift from multiple element Array of multiple types");
	arr1 = new Array(3);	
	arr1[0] = new Array(3);
	arr1[0][0] = 3;
	arr1[0][1] = "Four";
	arr1[0][2] = 5.01;
	arr1[1] = new Array(3);
	arr1[1][0] = 30;
	arr1[1][1] = "Forty";
	arr1[1][2] = 50.01;
	arr1[2] = new Array(3);
	arr1[2][0] = 300;
	arr1[2][1] = "Four hundred";
	arr1[2][2] = 500.01;
	verify (arr1.shift(), "3,Four,5.01", "11.1 Shift from array of multiple types returned wrong value");
	verify (arr1.length, 2, "11.2 Shift from array of multiple types didn't decrement length correctly");
	verify (arr1.shift(), "30,Forty,50.01", "11.3 Shift from array of multiple types returned wrong value");
	verify (arr1.length, 1, "11.4 Shift from array of multiple types didn't decrement length correctly");
	verify (arr1.shift(), "300,Four hundred,500.01", "11.5 Shift from array of multiple types returned wrong value");
	verify (arr1.length, 0, "11.6 Shift from array of multiple types didn't decrement length correctly");




	apInitScenario("13 Shift from Array of 100 elements, all undefined except first element");
	arr1 = new Array(100);	
	arr1[0] = "Three";
	verify (arr1.shift(), "Three", "13.1 Shift returned wrong value");
	verify (arr1.length, 99, "13.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "13.3 Shift returned wrong value");
	verify (arr1.length, 98, "13.4 Shift didn't decrement length correctly");


	apInitScenario("14 Shift from Array of 100 elements, all undefined except first element and last element");
	arr1 = new Array(100);	
	arr1[0] = "Four";
	arr1[99] = "Five";
	verify (arr1.shift(), "Four", "14.1 Shift returned wrong value");
	verify (arr1.length, 99, "14.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "14.3 Shift returned wrong value");
	verify (arr1.length, 98, "14.4 Shift didn't decrement length correctly");


	apInitScenario("15 Shift from Array of 100 elements, all undefined except first element and some middle element");
	arr1 = new Array(100);	
	arr1[0] = 3;
	arr1[49] = 8;
	verify (arr1.shift(), 3, "15.1 Shift returned wrong value");
	verify (arr1.length, 99, "15.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "15.3 Shift returned wrong value");
	verify (arr1.length, 98, "15.4 Shift didn't decrement length correctly");


	apInitScenario("16 Shift from Array of 100 elements, all undefined except last element");
	arr1 = new Array(100);	
	arr1[99] = 27;
	verify (arr1.shift(), undefined, "16.1 Shift returned wrong value");
	verify (arr1.length, 99, "16.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "16.3 Shift returned wrong value");
	verify (arr1.length, 98, "16.4 Shift didn't decrement length correctly");


	apInitScenario("17 Shift from Array of 100 elements, all undefined except last element and some middle element");
	arr1 = new Array(100);
	arr1[30] = 43;
	arr1[99] = 6734;
	verify (arr1.shift(), undefined, "17.1 Shift returned wrong value");
	verify (arr1.length, 99, "17.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "17.3 Shift returned wrong value");
	verify (arr1.length, 98, "17.4 Shift didn't decrement length correctly");


	apInitScenario("18 Shift from Array of 100 elements, all defined except for a single undefined element");
	arr1 = new Array(100);
	iUndef = 43;
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.shift(), 0, "18 Shift returned wrong value");


	apInitScenario("19 Shift from Array of 100 elements, odd numbered elements defined and even numbered elements undefined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount & 0x1) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.shift(), undefined, "19.1 Shift returned wrong value");
	verify (arr1.length, 99, "19.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), 1, "19.3 Shift returned wrong value");
	verify (arr1.length, 98, "19.4 Shift didn't decrement length correctly");


	apInitScenario("20 Shift from Array of 100 elements, odd numbered elements undefined and even numbered elements defined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (!(iCount & 0x1)) {
			arr1[iCount] = iCount;
		}
	}
	verify (arr1.shift(), 0, "20.1 Shift returned wrong value");
	verify (arr1.length, 99, "20.2 Shift didn't decrement length correctly");
	verify (arr1.shift(), undefined, "20.3 Shift returned wrong value");
	verify (arr1.length, 98, "20.4 Shift didn't decrement length correctly");


	apEndTest();
}

arrshf01();


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
