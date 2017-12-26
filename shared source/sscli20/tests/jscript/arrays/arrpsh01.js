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


var iTestID = 71689;

// Global variables
//

var arr1;
var arr2;
var arr3;
var iUndef; // holder for array index
var iCount; // counting variable

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function arrpsh01() {

    apInitTest("arrpsh01: Array.prototype.push"); 



	apInitScenario("1 Push zero elements (should not add any elements)");
	arr1 = new Array("One","Two","Three");
	arr1.push();
	verify (arr1.join(), "One,Two,Three", "1.1 Push method pushed an element even though there were no arguments");
	verify (arr1.length, 3, "1.2 Push method error");
	arr1.push("Four");
	verify (arr1.join(), "One,Two,Three,Four", "1.3 Push method error");
	verify (arr1.length, 4, "1.4 Push method error");
	

	apInitScenario("2 Push empty array element");
	arr1 = new Array("One","Two","Three");
	arr2 = new Array(1);
	arr1.push(arr2[0]);
	verify (arr1.join(), "One,Two,Three,", "2.1 Push method failed");
	verify (arr1.length, 4, "2.2 Push method error");
	arr1.push("Four");
	verify (arr1.join(), "One,Two,Three,,Four", "2.3 Push method error");
	verify (arr1.length, 5, "2.4 Push method error");


	apInitScenario("3 Push one element to array of numbers");
	arr1 = new Array(23,24,25);
	arr1.push(-3);
	verify (arr1.join(), "23,24,25,-3", "3.1 Push method failed");
	verify (arr1.length, 4, "3.2 Push method error");
	arr1.push(-3);
	verify (arr1.join(), "23,24,25,-3,-3", "3.1 Push method failed");
	verify (arr1.length, 5, "3.2 Push method error");


	apInitScenario("4 Push multiple elements to array of numbers");
	arr1 = new Array(23,24,25);
	arr1.push(-3,-23,-5555);
	verify (arr1.join(), "23,24,25,-3,-23,-5555", "4.1 Push method failed");
	verify (arr1.length, 6, "4.2 Push method error");
	arr1.push(-3,-23,-5555);
	verify (arr1.join(), "23,24,25,-3,-23,-5555,-3,-23,-5555", "4.1 Push method failed");
	verify (arr1.length, 9, "4.2 Push method error");


	apInitScenario("5 Push sparse array of elements to array of numbers");
	arr1 = new Array(23,24,25);
	arr2 = new Array(5);
	arr2[2] = -5432;
	arr1.push(arr2);
	verify (arr1.join(), "23,24,25,,,-5432,,", "5.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "5.2 Push method error");
	arr1.push(arr2);
	verify (arr1.join(), "23,24,25,,,-5432,,,,,-5432,,", "5.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "5.4 Push method error");


	apInitScenario("6 Push one element to array of strings");
	arr1 = new Array("One","Two","Three");
	arr1.push("Four");
	verify (arr1.join(), "One,Two,Three,Four", "6.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "6.2 Push method error");
	arr1.push("Four");
	verify (arr1.join(), "One,Two,Three,Four,Four", "6.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "6.4 Push method error");


	apInitScenario("7 Push multiple elements to array of strings");
	arr1 = new Array("One","Two","Three");
	arr1.push("Six","Seven","Eight");
	verify (arr1.join(), "One,Two,Three,Six,Seven,Eight", "7.1 Push method failed");
	verify (arr1.length, 6, "7.2 Push method error");
	arr1.push("Six","Seven","Eight");
	verify (arr1.join(), "One,Two,Three,Six,Seven,Eight,Six,Seven,Eight", "7.3 Push method failed");
	verify (arr1.length, 9, "7.4 Push method error");


	apInitScenario("8 Push sparse array of elements to array of strings");
	arr1 = new Array("One","Two","Three");
	arr2 = new Array(5);
	arr2[2] = "Twelve";
	arr1.push(arr2);
	verify (arr1.join(), "One,Two,Three,,,Twelve,,", "8.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "8.2 Push method error");
	arr1.push(arr2);
	verify (arr1.join(), "One,Two,Three,,,Twelve,,,,,Twelve,,", "8.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "8.4 Push method error");


	apInitScenario("9 Push one element to array of arrays");
	arr1 = new Array(3);
	arr1[0] = new Array(1,2,3);
	arr1[1] = new Array(4,5,6);
	arr1[2] = new Array(7,8);
	arr2 = new Array(-3,-4,-5);
	arr1.push(arr2);
	verify (arr1.join(), "1,2,3,4,5,6,7,8,-3,-4,-5", "9.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "9.2 Push method error");
	arr1.push(arr2);
	verify (arr1.join(), "1,2,3,4,5,6,7,8,-3,-4,-5,-3,-4,-5", "9.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "9.4 Push method error");


	apInitScenario("10 Push multiple elements to array of arrays");
	arr1 = new Array(3);
	arr1[0] = new Array(1,2,3);
	arr1[1] = new Array(4,5,6);
	arr1[2] = new Array(7,8);
	arr2 = new Array(-3,-4,-5);
	arr3 = new Array(-23,-24,-25);
	arr1.push(arr2,arr3);
	verify (arr1.join(), "1,2,3,4,5,6,7,8,-3,-4,-5,-23,-24,-25", "10.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "10.2 Push method error");


	apInitScenario("11 Push sparse array of elements to array of arrays");
	arr1 = new Array(3);
	arr1[0] = new Array(1,2,3);
	arr1[1] = new Array(4,5,6);
	arr1[2] = new Array(7,8);
	arr2 = new Array(5);
	arr2[2] = new Array(-23,-24,-25);
	arr1.push(arr2);
	verify (arr1.join(), "1,2,3,4,5,6,7,8,,,-23,-24,-25,,", "11.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "11.2 Push method error");


	apInitScenario("12 Push one element to array of objects");
	arr1 = new Array(3);
	arr1[0] = new Object;
	arr1[1] = new Object;
	arr1[2] = new Object;
	arr2 = new Object;
	arr1.push(arr2);
	verify (arr1.join(), "[object Object],[object Object],[object Object],[object Object]", "12.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "12.2 Push method error");
	arr1.push(arr2);
	verify (arr1.join(), "[object Object],[object Object],[object Object],[object Object],[object Object]", "12.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "12.4 Push method error");


	apInitScenario("13 Push multiple elements to array of objects");
	arr1 = new Array(3);
	arr1[0] = new Object;
	arr1[1] = new Object;
	arr1[2] = new Object;
	arr2 = new Array(2);
	arr2[0] = new Object;
	arr2[1] = new Object;
	arr3 = new Array(3);
	arr3[0] = new Object;
	arr3[1] = new Object;
	arr3[2] = new Object;
	arr1.push(arr2,arr3);
	verify (arr1.join(), "[object Object],[object Object],[object Object],[object Object],[object Object],[object Object],[object Object],[object Object]", "13.1 Push method failed");
	// arr1.length is increased by only 2 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "13.2 Push method error");


	apInitScenario("14 Push sparse array of elements to array of objects");
	arr1 = new Array(3);
	arr1[0] = new Object;
	arr1[1] = new Object;
	arr1[2] = new Object;
	arr2 = new Array(5);
	arr2[2] = new Object;
	arr1.push(arr2);
	verify (arr1.join(), "[object Object],[object Object],[object Object],,,[object Object],,", "14.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "14.2 Push method error");


	apInitScenario("15 Push one element to array of multiple elements of different types");
	arr1 = new Array(23,"Four",new Object);
	arr1.push(-3);
	verify (arr1.join(), "23,Four,[object Object],-3", "15.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "15.2 Push method error");
	arr1.push(-3);
	verify (arr1.join(), "23,Four,[object Object],-3,-3", "15.3 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 5, "15.4 Push method error");


	apInitScenario("16 Push multiple elements to array of multiple elements of different types");
	arr1 = new Array(23,"Four",new Object);
	arr1.push(-3,-23,"Five");
	verify (arr1.join(), "23,Four,[object Object],-3,-23,Five", "16.1 Push method failed");
	verify (arr1.length, 6, "16.2 Push method error");
	arr1.push(-3,-23,"Five");
	verify (arr1.join(), "23,Four,[object Object],-3,-23,Five,-3,-23,Five", "16.3 Push method failed");
	verify (arr1.length, 9, "16.4 Push method error");


	apInitScenario("17 Push sparse array of elements to array of multiple elements of different types");
	arr1 = new Array(23,"Four",new Object);
	arr2 = new Array(5);
	arr2[2] = "Seven";
	arr2[3] = 8;
	arr1.push(arr2);
	verify (arr1.join(), "23,Four,[object Object],,,Seven,8,", "17.1 Push method failed");
	// arr1.length is increased by only 1 here because it is the array object that is counted, not its elements
	verify (arr1.length, 4, "17.2 Push method error");


	apInitScenario("18 Push elements to Array of 100 elements, all undefined");
	arr1 = new Array(100);
	arr1.push(-3,5);
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3,5", "18.1 Push method failed");
	verify (arr1.length, 102, "18.2 Push method error");


	apInitScenario("19 Push elements to Array of 100 elements, all undefined except first element");
	arr1 = new Array(100);
	arr1[0] = "Four";
	arr1.push(-3,5);
	verify (arr1.join(), "Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-3,5", "19.1 Push method failed");
	verify (arr1.length, 102, "19.2 Push method error");


	apInitScenario("20 Push elements to Array of 100 elements, all undefined except first element and last element");
	arr1 = new Array(100);
	arr1[0] = "Four";
	arr1[99] = 16;
	arr1.push(-3,5);
	verify (arr1.join(), "Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,16,-3,5", "20.1 Push method failed");
	verify (arr1.length, 102, "20.2 Push method error");


	apInitScenario("21 Push elements to Array of 100 elements, all undefined except first element and some middle element");
	arr1 = new Array(100);
	arr1[0] = "Four";
	arr1[95] = 16;
	arr1.push(-3,5);
	verify (arr1.join(), "Four,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,16,,,,,-3,5", "21.1 Push method failed");
	verify (arr1.length, 102, "21.2 Push method error");


	apInitScenario("22 Push elements to Array of 100 elements, all undefined except last element");
	arr1 = new Array(100);
	arr1[99] = "Four";
	arr1.push(-3,5);
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,Four,-3,5", "22.1 Push method failed");
	verify (arr1.length, 102, "22.2 Push method error");


	apInitScenario("23 Push elements to Array of 100 elements, all undefined except last element and some middle element");
	arr1 = new Array(100);
	arr1[99] = "Four";
	arr1[95] = 16;
	arr1.push(-3,5);
	verify (arr1.join(), ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,16,,,,Four,-3,5", "23.1 Push method failed");
	verify (arr1.length, 102, "23.2 Push method error");


	apInitScenario("24 Push elements to Array of 100 elements, all defined except for a single undefined element");
	arr1 = new Array(100);
	iUndef = 43;
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount != iUndef) {
			arr1[iCount] = iCount;
		}
	}
	arr1.push(-3,5);
	verify (arr1.join(), "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,-3,5", "24.1 Push method failed");
	verify (arr1.length, 102, "24.2 Push method error");


	apInitScenario("25 Push elements to Array of 100 elements, odd elements defined and even elements undefined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (iCount & 0x1) {
			arr1[iCount] = iCount;
		}
	}
	arr1.push(-3,5);
	verify (arr1.join(), ",1,,3,,5,,7,,9,,11,,13,,15,,17,,19,,21,,23,,25,,27,,29,,31,,33,,35,,37,,39,,41,,43,,45,,47,,49,,51,,53,,55,,57,,59,,61,,63,,65,,67,,69,,71,,73,,75,,77,,79,,81,,83,,85,,87,,89,,91,,93,,95,,97,,99,-3,5", "25.1 Push method returned wrong value");
	verify (arr1.length, 102, "25.2 Push method error");


	apInitScenario("26 Push elements to Array of 100 elements, odd elements undefined and even elements defined");
	arr1 = new Array(100);
	for (iCount = 0; iCount < 100; iCount++) {
		if (!(iCount & 0x1)) {
			arr1[iCount] = iCount;
		}
	}
	arr1.push(-3,5);
	verify (arr1.join(), "0,,2,,4,,6,,8,,10,,12,,14,,16,,18,,20,,22,,24,,26,,28,,30,,32,,34,,36,,38,,40,,42,,44,,46,,48,,50,,52,,54,,56,,58,,60,,62,,64,,66,,68,,70,,72,,74,,76,,78,,80,,82,,84,,86,,88,,90,,92,,94,,96,,98,,-3,5", "26.1 Push method returned wrong value");
	verify (arr1.length, 102, "26.2 Push method error");



	apEndTest();
}

arrpsh01();


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
