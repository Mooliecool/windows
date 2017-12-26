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


var iTestID = 71693;

// Global variables
//

var arr1;
var arr2;
var iUndef; // holder for array index
var iCount; // counting variable

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrspl01() {

    apInitTest("arrspl01: Array.prototype.splice"); 



	apInitScenario("1 Splice empty array with no substitution elements");
	arr1 = new Array();
	arr1.splice();
	verify (arr1.join(),"", "1.1 Splice function failed");
	verify (arr1.length, 0, "1.2 Splice function returned wrong length");


	apInitScenario("2 Splice using # of replace arguments smaller than deleteCount");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr1.splice(2,4,"XXX","YYY");
	verify (arr1.join(),"One,Two,XXX,YYY,Seven", "2.1 Splice function failed");
	verify (arr1.length, 5, "2.2 Splice function returned wrong length");


	apInitScenario("3 Splice empty array with string substitution element");
	arr1 = new Array();
	arr1.splice(0,0,"Four");
	verify (arr1.join(),"Four", "3.1 Splice function failed");
	verify (arr1.length, 1, "3.2 Splice function returned wrong length");


	apInitScenario("4 Splice empty array with numeric substitution element");
	arr1 = new Array();
	arr1.splice(0,0,4);
	verify (arr1.join(),4, "4.1 Splice function failed");
	verify (arr1.length, 1, "4.2 Splice function returned wrong length");


	apInitScenario("5 Splice empty array with array substitution element");
	arr1 = new Array();
	arr2 = new Array("Four","Five");
	arr1.splice(0,0,arr2);
	verify (arr1.join(),"Four,Five", "5.1 Splice function failed");
	verify (arr1.length, 1, "5.2 Splice function returned wrong length");


	apInitScenario("6 Splice empty array with object substitution element");
	arr1 = new Array();
	arr2 = new Object();
	arr2.mem = 25;
	arr1.splice(0,0,arr2);
	verify (arr1.join(),"[object Object]", "6.1 Splice function failed");
	verify (arr1.length, 1, "6.2 Splice function returned wrong length");


	apInitScenario("7 Splice empty array with sparse array substitution element");
	arr1 = new Array();
	arr2 = new Array(5);
	arr2[2] = 2555;
	arr1.splice(0,0,arr2);
	verify (arr1.join(),",,2555,,", "7.1 Splice function failed");
	verify (arr1.length, 1, "7.2 Splice function returned wrong length");


	apInitScenario("8 Splice single element array with DeleteCount > 1");
	arr1 = new Array("One");
	arr1.splice(0,3);
	verify (arr1.join(),"", "8.1 Splice function failed");
	verify (arr1.length, 0, "8.2 Splice function returned wrong length");


	apInitScenario("9 Splice single element array with no substitution elements");
	arr1 = new Array("One");
	arr1.splice(1,0);
	verify (arr1.join(),"One", "9.1 Splice function failed");
	verify (arr1.length, 1, "9.2 Splice function returned wrong length");


	apInitScenario("10 Splice single element array with string substitution element");
	arr1 = new Array("One");
	arr1.splice(1,0,"Four");
	verify (arr1.join(),"One,Four", "10.1 Splice function failed");
	verify (arr1.length, 2, "10.2 Splice function returned wrong length");


	apInitScenario("11 Splice single element array with numeric substitution element");
	arr1 = new Array("One");
	arr1.splice(1,0,6);
	verify (arr1.join(),"One,6", "11.1 Splice function failed");
	verify (arr1.length, 2, "11.2 Splice function returned wrong length");


	apInitScenario("12 Splice single element array with array substitution element");
	arr1 = new Array("One");
	arr2 = new Array(5,5,5);
	arr1.splice(1,0,arr2);
	verify (arr1.join(),"One,5,5,5", "12.1 Splice function failed");
	verify (arr1.length, 2, "12.2 Splice function returned wrong length");


	apInitScenario("13 Splice single element array with object substitution element");
	arr1 = new Array("One");
	arr2 = new Object();
	arr2.mem = -333;
	arr1.splice(1,0,arr2);
	verify (arr1.join(),"One,[object Object]", "13.1 Splice function failed");
	verify (arr1.length, 2, "13.2 Splice function returned wrong length");


	apInitScenario("14 Splice single element array with sparse array substitution element");
	arr1 = new Array("One");
	arr2 = new Array(5);
	arr2[2] = -333;
	arr1.splice(1,0,arr2);
	verify (arr1.join(),"One,,,-333,,", "14.1 Splice function failed");
	verify (arr1.length, 2, "14.2 Splice function returned wrong length");


	apInitScenario("15 Splice multiple element array with no substitution elements");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr1.splice(3,2);
	verify (arr1.join(),"One,Two,Three,Six,Seven", "15.1 Splice function failed");
	verify (arr1.length, 5, "15.2 Splice function returned wrong length");


	apInitScenario("16 Splice multiple element array with string substitution element");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr1.splice(3,2, "Eleven");
	verify (arr1.join(),"One,Two,Three,Eleven,Six,Seven", "16.1 Splice function failed");
	verify (arr1.length, 6, "16.2 Splice function returned wrong length");


	apInitScenario("17 Splice multiple element array with numeric substitution element");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr1.splice(3,2, 11);
	verify (arr1.join(),"One,Two,Three,11,Six,Seven", "17.1 Splice function failed");
	verify (arr1.length, 6, "17.2 Splice function returned wrong length");


	apInitScenario("18 Splice multiple element array with array substitution element");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr2 = new Array(5,5,5);
	arr1.splice(3,2, arr2);
	verify (arr1.join(),"One,Two,Three,5,5,5,Six,Seven", "18.1 Splice function failed");
	verify (arr1.length, 6, "18.2 Splice function returned wrong length");


	apInitScenario("19 Splice multiple element array with object substitution element");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr2 = new Object();
	arr2.mem = -333;
	arr1.splice(3,2, arr2);
	verify (arr1.join(),"One,Two,Three,[object Object],Six,Seven", "19.1 Splice function failed");
	verify (arr1.length, 6, "19.2 Splice function returned wrong length");


	apInitScenario("20 Splice multiple element array with sparse array substitution element");
	arr1 = new Array("One","Two","Three","Four","Five","Six","Seven");
	arr2 = new Array(5);
	arr2[2] = -333;
	arr1.splice(3,2, arr2);
	verify (arr1.join(),"One,Two,Three,,,-333,,,Six,Seven", "20.1 Splice function failed");
	verify (arr1.length, 6, "20.2 Splice function returned wrong length");


	apInitScenario("21 Splice into Array of 100 elements, all undefined");
	arr1 = new Array(100);
	arr1.splice(3,2, "Four");
	verify (arr1.join(),",,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "21.1 Splice function failed");
	verify (arr1.length, 99, "21.2 Splice function returned wrong length");


	apInitScenario("22 Splice into Array of 100 elements, all undefined except first element");
	arr1 = new Array(100);
	arr1[0] = 23;
	arr1.splice(3,2, "Four");
	verify (arr1.join(),"23,,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "22.1 Splice function failed");
	verify (arr1.length, 99, "22.2 Splice function returned wrong length");


	apInitScenario("23 Splice into Array of 100 elements, all undefined except first element and last element");
	arr1 = new Array(100);
	arr1[0] = 23;
	arr1[99] = -3;
	arr1.splice(3,2, "Four");
	verify (arr1.join(),"23,,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "23.1 Splice function failed");
	verify (arr1.length, 99, "23.2 Splice function returned wrong length");


	apInitScenario("24 Splice into Array of 100 elements, all undefined except first element and some middle element");
	arr1 = new Array(100);
	arr1[0] = 23;
	arr1[30] = -3;
	arr1.splice(3,2, "Four");
	verify (arr1.join(),"23,,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", "24.1 Splice function failed");
	verify (arr1.length, 99, "24.2 Splice function returned wrong length");


	apInitScenario("25 Splice into Array of 100 elements, all undefined except last element");
	arr1 = new Array(100);
	arr1[99] = -3;
	arr1.splice(3,2, "Four");
	verify (arr1.join(),",,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3", "25.1 Splice function failed");
	verify (arr1.length, 99, "25.2 Splice function returned wrong length");


	apInitScenario("26 Splice into Array of 100 elements, all undefined except last element and some middle element");
	arr1 = new Array(100);
	arr1[99] = 23;
	arr1[30] = -3;
	arr1.splice(3,2, "Four");
	verify (arr1.join(),",,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,-3,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,23", "26.1 Splice function failed");
	verify (arr1.length, 99, "26.2 Splice function returned wrong length");


	apInitScenario("27 Splice into Array of 100 elements, all defined except for a single undefined element");
	arr1 = new Array(100);
	iUndef = 26;
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	arr1.splice(3,2, "Four");
	verify (arr1.join(),"0,1,2,Four,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99", "27 Splice function failed");


	apInitScenario("28 Splice into Array of 100 elements, odd numbered elements defined and even numbered elements undefined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount & 1) {
			arr1[iCount] = iCount;
		}
	}
	arr1.splice(3,2,"Four");
	verify (arr1.join(),",1,,Four,5,,7,,9,,11,,13,,15,,17,,19,,21,,23,,25,,27,,29,,31,,33,,35,,37,,39,,41,,43,,45,,47,,49,,51,,53,,55,,57,,59,,61,,63,,65,,67,,69,,71,,73,,75,,77,,79,,81,,83,,85,,87,,89,,91,,93,,95,,97,,99", "28 Splice function failed");


	apInitScenario("29 Splice into Array of 100 elements, odd numbered elements undefined and even numbered elements defined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (!(iCount & 1)) {
			arr1[iCount] = iCount;
		}
	}
	arr1.splice(3,2,"Four");
	verify (arr1.join(),"0,,2,Four,,6,,8,,10,,12,,14,,16,,18,,20,,22,,24,,26,,28,,30,,32,,34,,36,,38,,40,,42,,44,,46,,48,,50,,52,,54,,56,,58,,60,,62,,64,,66,,68,,70,,72,,74,,76,,78,,80,,82,,84,,86,,88,,90,,92,,94,,96,,98,", "29 Splice function failed");


	apInitScenario("30 Splice into large array, only first and last 10 elements defined");
	arr1 = new Array(1000);
	for (iCount = 0; iCount < 10; iCount++) {
		arr1[iCount] = iCount;
	}
	for (iCount = 990; iCount < 1000; iCount++) {
		arr1[iCount] = iCount;
	}
	arr1.splice(30,20,"Four");
	verify (arr1.join(),"0,1,2,3,4,5,6,7,8,9,,,,,,,,,,,,,,,,,,,,,Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,990,991,992,993,994,995,996,997,998,999", "30 Splice function failed");



 
	apInitScenario("31 splice an array with deletecount > items remaining");
 	var arr1 = new Array(1000);
 	arr1.splice(1,999,"a");
 	if (arr1.toString() != ",a")  apLogFailInfo("31 array split incorrectly", arr1.toString(), ",a", "")

 	arr1 = new Array(1000);
 	arr1.splice(1,1000,"a");
 	if (arr1.toString() != ",a")  apLogFailInfo("31 array split incorrectly", arr1.toString(), ",a", "")

 	arr1 = new Array(1000);
 	arr1.splice(1,66000,"a");
 	if (arr1.toString() != ",a")  apLogFailInfo("31 array split incorrectly", arr1.toString(), ",a", "")

 	arr1 = new Array(1000);
 	arr1.splice(0,66000,"a");
	if (arr1.toString() != "a")  apLogFailInfo("31 array split incorrectly", arr1.toString(), "a", "")

 	apInitScenario("32 splice an array with deletecount <0");
 	var arr1 = new Array(3);
 	arr1.splice(1,-10,"a");
 	if (arr1.toString() != ",a,,")  apLogFailInfo("32 array split incorrectly", arr1.toString(), ",a,,", "")
 

 	apInitScenario("33 splice an array with startindex > elements");
 	var arr1 = new Array(5);
 	arr1.splice(5,5,"a");
 	if (arr1.toString() != ",,,,,a")  apLogFailInfo("33 array split incorrectly", arr1.toString(), ",,,,,a", "")
 
	var arr1 = new Array(5);
 	arr1.splice(66000,5,"a");
 	if (arr1.toString() != ",,,,,a")  apLogFailInfo("33 array split incorrectly", arr1.toString(), ",,,,,a", "")

 	apInitScenario("34 splice an array with startindex < 0");
 	var arr1 = new Array(5);
 	arr1.splice(-3,2,"a");
 	if (arr1.toString() != ",,a,")  apLogFailInfo("34a array split incorrectly", arr1.toString(), ",,a", "")


	apEndTest();
}

arrspl01();


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
