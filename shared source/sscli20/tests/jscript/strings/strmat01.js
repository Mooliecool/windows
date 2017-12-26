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


var iTestID = 72441;

//////////
//
//  Test Case:
//
//     strmat01: String.prototype.match - can take a string or a RegExp as a parameter
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
var reg1;



function strmat01() {


    apInitTest("strmat01: String.prototype.match - can take a string or a RegExp as a parameter"); 


	str1 = new String();
	str1 = "My dog has fleas.";
	arr1 = new Array();
	reg1 = new RegExp();


	apInitScenario("1 Match String to null");
	arr1 = str1.match(null);
	verify (arr1, null, "1 Wrong result returned");	


	apInitScenario("2 Match String to undefined");
	arr1 = str1.match(undefined);
	verify (arr1, "", "2 Wrong result returned", "VS7 #111001");	


	apInitScenario("3 Match String to empty string");
	arr1 = str1.match("");
	verify (arr1.length, 1, "3.1 Wrong result returned");
	verify (arr1[0], "", "3.2 Wrong result returned");	


	apInitScenario("4 Match String to equal string");
	arr1 = str1.match("My dog has fleas.");
	verify (arr1.length, 1, "4.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "4.2 Wrong result returned");	


	apInitScenario("5 Match String to unequal string");
	arr1 = str1.match("My cat has fleas.");
	verify (arr1, null, "5.1 Wrong result returned");


	apInitScenario("6 Match String to longer String which starts with calling string");
	arr1 = str1.match("My dog has fleas.  My cat has fleas.");
	verify (arr1, null, "6.1 Wrong result returned");


	apInitScenario("7 Match String to empty regexp");
	arr1 = str1.match(reg1);
	verify (arr1.length, 1, "7.1 Wrong result returned");
	verify (arr1[0], "", "7.2 Wrong result returned");	


	apInitScenario("8 Match String to RegExp exactly equal to String");
	reg1 = /My dog has fleas./;
	arr1 = str1.match(reg1);
	verify (arr1.length, 1, "8.1 Wrong result returned");
	verify (arr1[0], "My dog has fleas.", "8.2 Wrong result returned");	


	apInitScenario("9 Match String to RegExp which will match once in String");
	reg1 = /dog/gi;
	arr1 = str1.match(reg1);
	verify (arr1.length, 1, "9.1 Wrong result returned");
	verify (arr1[0], "dog", "9.2 Wrong result returned");	


	apInitScenario("10 Match String to RegExp which will match multiple times in String");
	reg1 = / /gi;
	arr1 = str1.match(reg1);
	verify (arr1.length, 3, "10.1 Wrong result returned");
	verify (arr1[0], " ", "10.2 Wrong result returned");
	verify (arr1[1], " ", "10.2 Wrong result returned");
	verify (arr1[2], " ", "10.2 Wrong result returned");


	apInitScenario("11 Match String to RegExp which won't match in String");
	reg1 = /cat/gi;
	arr1 = str1.match(reg1);
	verify (arr1, null, "11.1 Wrong result returned");


	apEndTest();
}

strmat01();


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
