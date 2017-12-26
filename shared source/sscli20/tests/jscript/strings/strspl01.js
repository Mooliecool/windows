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


var iTestID = 72446;


//////////
//
//  Test Case:
//
//     strspl01: String.prototype.split -  can take a string or a RegExp, and has an optional second parameter [limit]
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}



//////////
//
//  Global variables
//


var str1;
var arr1;
var arr2;
var reg1;
var iLargestNumber = 0xFFFFFFFF; // (2^32)-1
var e;


reg1 = new RegExp();



function strspl01() {


    apInitTest("strspl01: String.prototype.split -  can take a string or a RegExp, and has an optional second parameter [limit]"); 


	str1 = new String();
	str1 = "My dog has fleas.";
	e = new Error();

	apInitScenario("1 Zero arguments");
	arr1 = str1.split();
	verify (arr1, "My dog has fleas.", "1 Wrong result returned");


	apInitScenario("2 Limit == null");
	arr1 = str1.split(" ", null);
	verify (arr1.length, 4, "2.1 Wrong result returned");
	verify (arr1[0], "My", "2.2 Wrong result returned");
	verify (arr1[1], "dog", "2.3 Wrong result returned");
	verify (arr1[2], "has", "2.4 Wrong result returned");
	verify (arr1[3], "fleas.", "2.5 Wrong result returned");
	verify (arr1[4], undefined, "2.6 Wrong result returned");


       //    cut/paste error during tc development
	apInitScenario("3 Limit == undefined");
	arr1 = str1.split(" ", undefined);
	verify (arr1.length, 4, "3.1 Wrong result returned");
	verify (arr1[0], "My", "3.2 Wrong result returned");
	verify (arr1[1], "dog", "3.3 Wrong result returned");
	verify (arr1[2], "has", "3.4 Wrong result returned");
	verify (arr1[3], "fleas.", "3.5 Wrong result returned");
	verify (arr1[4], undefined, "3.6 Wrong result returned");

	apInitScenario("4 Limit == 0");
	arr1 = str1.split(" ", 0);
	verify (arr1.length, 0, "4.1 Wrong result returned");


	apInitScenario("5 Limit == 1");
	arr1 = str1.split(" ", 1);
	verify (arr1.length, 1, "5.1 Wrong result returned");
	verify (arr1[0], "My", "5.2 Wrong result returned");
	verify (arr1[1], undefined, "5.3 Wrong result returned");


	apInitScenario("6 Limit == -1");
	arr1 = str1.split(" ", -1);
	verify (arr1.length, 4, "6.1 Wrong result returned");
	verify (arr1[0], "My", "6.2 Wrong result returned");
	verify (arr1[1], "dog", "6.3 Wrong result returned");
	verify (arr1[2], "has", "6.4 Wrong result returned");
	verify (arr1[3], "fleas.", "6.5 Wrong result returned");


	apInitScenario("7 Limit == large positive number");
	arr1 = str1.split(" ", Number.MAX_VALUE);
	verify (arr1.length, 4, "7.1 Wrong result returned - VS7");
	verify (arr1[0], "My", "7.2 Wrong result returned");
	verify (arr1[1], "dog", "7.3 Wrong result returned");
	verify (arr1[2], "has", "7.4 Wrong result returned");
	verify (arr1[3], "fleas.", "7.5 Wrong result returned");


	apInitScenario("8 Limit == large negative number");
	arr1 = str1.split(" ", Number.MIN_VALUE);
	verify (arr1.length, 0, "8.1 Wrong result returned");
	verify (arr1[0], undefined, "8.2 Wrong result returned");
	verify (arr1[1], undefined, "8.3 Wrong result returned");
	verify (arr1[2], undefined, "8.4 Wrong result returned");
	verify (arr1[3], undefined, "8.5 Wrong result returned");


	apInitScenario("9 Limit is float value");
	arr1 = str1.split(" ", 3.14159);
	verify (arr1.length, 3, "9.1 Wrong result returned");
	verify (arr1[0], "My", "9.2 Wrong result returned");
	verify (arr1[1], "dog", "9.3 Wrong result returned");
	verify (arr1[2], "has", "9.4 Wrong result returned");
	verify (arr1[3], undefined, "9.5 Wrong result returned");


	apInitScenario("10 Limit is empty string");
	try {
		arr1 = str1.split(" ", "");
	}
	catch(e) {
		verify (e.number, -2146828283, "10.1 Wrong error number returned");
	}


	apInitScenario("11 Limit is non-empty string of letters");
	e.number = 0;
	try {
		arr1 = str1.split(" ", "abc");
	}
	catch(e) {
		verify (e.number, -2146828283, "11.1 Wrong error number returned");
	}


	apInitScenario("12 Limit is non-empty string of numbers");
	e.number = 0;
	try {
		arr1 = str1.split(" ", "123");
	}
	catch(e) {
		verify (e.number, -2146828283, "12.1 Wrong error number returned");
	}


	apInitScenario("13 Limit is array");
	e.number = 0;
	arr2 = new Array(3,4,-5);
	try {
		arr1 = str1.split(" ", arr2);
	}
	catch(e) {
		verify (e.number, -2146828283, "13.1 Wrong error number returned");
	}


	apInitScenario("14 Limit is object");
	e.number = 0;
	arr2 = new Object();
	arr2.mem = -5;
	try {
		arr1 = str1.split(" ", arr2);
	}
	catch(e) {
		verify (e.number, -2146828283, "14.1 Wrong error number returned");
	}


	apInitScenario("15 No separator argument, limit argument");
	arr1 = str1.split(undefined, 3);
	verify (arr1.length, 1, "15.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "15.2 Wrong result returned");
	verify (arr1[1], undefined, "15.3 Wrong result returned");


	apInitScenario("16 Separator is null");
	arr1 = str1.split(null, 3);
	verify (arr1.length, 1, "16.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "16.2 Wrong result returned");
	verify (arr1[1], undefined, "16.3 Wrong result returned");


	apInitScenario("17 Separator is empty string");
	arr1 = str1.split("", 6);
	verify (arr1.length, 6, "17.1 Wrong result returned");
	verify (arr1[0], "M", "17.2 Wrong result returned");
	verify (arr1[1], "y", "17.3 Wrong result returned");
	verify (arr1[2], " ", "17.4 Wrong result returned");
	verify (arr1[3], "d", "17.5 Wrong result returned");
	verify (arr1[4], "o", "17.6 Wrong result returned");
	verify (arr1[5], "g", "17.7 Wrong result returned");
	verify (arr1[6], undefined, "17.8 Wrong result returned");


	apInitScenario("18 Separator is exactly equal to String");
	arr1 = str1.split("My dog has fleas.", 3);
	verify (arr1.length, 2, "18.1 Wrong result returned");
	verify (arr1[0], "", "18.2 Wrong result returned");
	verify (arr1[1], "", "18.3 Wrong result returned");
	verify (arr1[2], undefined, "18.4 Wrong result returned");


	apInitScenario("19 Separator is not found in String");
	arr1 = str1.split(",", 3);
	verify (arr1.length, 1, "19.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "19.2 Wrong result returned");
	verify (arr1[1], undefined, "19.3 Wrong result returned");


	apInitScenario("20 Separator matches beginning of String");
	arr1 = str1.split("My", 3);
	verify (arr1.length, 2, "20.1 Wrong result returned");
	verify (arr1[0], "", "20.2 Wrong result returned");
	verify (arr1[1], " dog has fleas.", "20.3 Wrong result returned");
	verify (arr1[2], undefined, "20.4 Wrong result returned");


	apInitScenario("21 Separator matches end of String");
	arr1 = str1.split(".", 3);
	verify (arr1.length, 2, "21.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas", "21.2 Wrong result returned");
	verify (arr1[1], "", "21.3 Wrong result returned");
	verify (arr1[2], undefined, "21.4 Wrong result returned");


	apInitScenario("22 Separator is longer String which starts with calling string");
	arr1 = str1.split("My dog has fleas.  Dogs hate cats.", 3);
	verify (arr1.length, 1, "22.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "22.2 Wrong result returned");
	verify (arr1[1], undefined, "22.3 Wrong result returned");


	apInitScenario("23 Separator is empty RegExp");
	reg1 = new RegExp("");
	arr1 = str1.split(reg1, 3);
	verify (arr1.length, 3, "23.1 Wrong result returned");
	// the following return value, (empty string in first array element) is correct,
	// since an empty regular expression will match the beginning of the string as
	// a valid match point
	verify (arr1[0], "M", "23.2 Wrong result returned");
	verify (arr1[1], "y", "23.3 Wrong result returned");
	verify (arr1[2], " ", "23.4 Wrong result returned");
	verify (arr1[3], undefined, "23.5 Wrong result returned");


	apInitScenario("24 Separator is RegExp exactly equal to String");
	str1 = "My dog has fleas.";
	reg1 = /My dog has fleas./;
	arr1 = str1.split(reg1, 3);
	verify (arr1.length, 0, "24.1 Wrong result returned");
	verify (arr1[0], undefined, "24.2 Wrong result returned");
	verify (arr1[1], undefined, "24.3 Wrong result returned");
	verify (arr1[2], undefined, "24.4 Wrong result returned");


	apInitScenario("25 Separator is RegExp which will match one String char");
	reg1 = / /;
	arr1 = str1.split(reg1, 3);
	verify (arr1.length, 3, "25.1 Wrong result returned");
	verify (arr1[0], "My", "25.2 Wrong result returned");
	verify (arr1[1], "dog", "25.3 Wrong result returned");
	verify (arr1[2], "has", "25.4 Wrong result returned");
	verify (arr1[3], undefined, "25.5 Wrong result returned");


	apInitScenario("26 Separator is RegExp which will match multiple String chars");
	reg1 = /\w+as/;  // will match 'has' and 'fleas'
	arr1 = str1.split(reg1, 3);
	verify (arr1.length, 3, "26.1 Wrong result returned");
	verify (arr1[0], "My dog ", "26.2 Wrong result returned");
	verify (arr1[1], " ", "26.3 Wrong result returned");
	verify (arr1[2], ".", "26.4 Wrong result returned");
	verify (arr1[3], undefined, "26.5 Wrong result returned");


	apInitScenario("27 Separator is RegExp which won't match String");
	reg1 = /[0-9]/;  // trying to match a digit
	arr1 = str1.split(reg1, 3);
	verify (arr1.length, 1, "27.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "27.2 Wrong result returned");
	verify (arr1[1], undefined, "27.3 Wrong result returned");


	apEndTest();

}

strspl01();


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
