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


var iTestID = 71692;

// Global variables
//

var arr1;
var arr2;
var arr3;
var iLargestNumber = 0xFFFFFFFF; // (2^32)-1

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrslc01() {

    apInitTest("arrslc01: Array.prototype.slice (Both <start >and <end> args can be negative)"); 


	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");


	apInitScenario("1 Neg start, no end arg");
	arr2 = arr1.slice(-5);
	verify (arr2.join(), "Three,Four,Five,Six,Seven", "1 Slice method failed");


	apInitScenario("2 Neg start, pos end, (start + length < end)");
	arr2 = arr1.slice(-5,5);
	verify (arr2.join(), "Three,Four,Five", "2 Slice method failed");


	apInitScenario("3 Neg start, pos end, (start + length > end)");
	arr2 = arr1.slice(-5,1);
	verify (arr2.length, 0, "3 Slice method failed");


	apInitScenario("4 Neg start, pos end, (start + length = end)");
	arr2 = arr1.slice(-5,2);
	verify (arr2.length, 0, "4 Slice method failed");

	apInitScenario("5 Neg start, pos end, (end > length)");
	arr2 = arr1.slice(-5,9);
	verify (arr2.length, 5, "5 Slice method failed");

	apInitScenario("6 Neg start, neg end, (start + length < end + length)");
	arr2 = arr1.slice(-5,-2);
	verify (arr2.join(), "Three,Four,Five", "6 Slice method failed");


	apInitScenario("7 Neg start, neg end, (start + length > end + length)");
	arr2 = arr1.slice(-5,-6);
	verify (arr2.length, 0, "7 Slice method failed");


	apInitScenario("8 Neg start, neg end, (start + length = end + length)");
	arr2 = arr1.slice(-5,-5);
	verify (arr2.length, 0, "8 Slice method failed");


	apInitScenario("9 Pos start, neg end, (start < end + length)");
	arr2 = arr1.slice(5,-1);
	verify (arr2.join(), "Six", "9 Slice method failed");


	apInitScenario("10 Pos start, neg end, (start > end + length)");
	arr2 = arr1.slice(5,-5);
	verify (arr2.length, 0, "10 Slice method failed");


	apInitScenario("11 Pos start, neg end, (start = end + length)");
	arr2 = arr1.slice(5,-2);
	verify (arr2.length, 0, "11 Slice method failed");


	apInitScenario("12 start = 0, no end arg");
	arr2 = arr1.slice(0);
	verify (arr2.join(), "One,Two,Three,Four,Five,Six,Seven", "12 Slice method failed");


	apInitScenario("13 start = 0, neg end");
	arr2 = arr1.slice(0,-5);
	verify (arr2.join(), "One,Two", "13 Slice method failed");


	apInitScenario("14 Neg start, end = 0");
	arr2 = arr1.slice(-5,0);
	verify (arr2.join(), "", "14 Slice method failed");


	apInitScenario("15 Neg start with large negative value, pos end");
	arr2 = arr1.slice(-10,5);
	verify (arr2.join(), "One,Two,Three,Four,Five", "15 Slice method failed");


	apInitScenario("16 Pos start, neg end with large negative value");
	arr2 = arr1.slice(5,-10);
	verify (arr2.join(), "", "16 Slice method failed");


	apInitScenario("17 Pos start, pos end > length");
	arr2 = arr1.slice(5,10);
	verify (arr2.join(), "Six,Seven", "17 Slice method failed");


	apInitScenario("18 Pos start > length, pos end");
	arr2 = arr1.slice(10,5);
	verify (arr2.join(), "", "18 Slice method failed");


	apInitScenario("19 Pos start > length, pos end > length");
	arr2 = arr1.slice(10,15);
	verify (arr2.join(), "", "19 Slice method failed");


	apInitScenario("20 Slice using float values");
	arr2 = arr1.slice(3.1,5.7);
	verify (arr2.join(), "Four,Five", "20 Slice method failed");


	apInitScenario("21 Slice Array of 2^32 elements");
	arr3 = new Array(iLargestNumber);
	arr2 = arr3.slice(3000,3005)
	verify (arr2.join(), ",,,,", "21 Slice method failed");


	apInitScenario("22 Slice empty array");
	arr3 = new Array();
	arr2 = arr3.slice(3,5);
	verify (arr2.join(), "", "22 Slice method failed");


	apInitScenario("23 Generic object method:  make new Object, assign it a length property and use Slice");
@if (@_fast)
@else
	arr3 = new Object();
	arr3[0] = "Ten";
	arr3[1] = "Twenty";
	arr3[2] = "Thirty";
	arr3[3] = "Forty";
	arr3[4] = "Fifty";
	arr3[5] = "Sixty";
	arr3[6] = "Seventy";
	Object.prototype.length = Array.prototype.length;
	Object.prototype.slice = Array.prototype.slice;
	arr3.length = 7;
	arr2 = arr3.slice(3,5);
	verify (arr2.join(), "Forty,Fifty", "23 Slice method failed");
	verify (arr3.length, 7, "23 Slice method failed");
@end

	apEndTest();
}

arrslc01();


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
