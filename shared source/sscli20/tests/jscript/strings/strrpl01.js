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


var iTestID = 72442;

//////////
//
//  Test Case:
//
//     strrpl01: String.prototype.replace - can take a string or a RegExp as a parameter
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
var str2;
var arr1;
var reg1;



function strrpl01() {


    apInitTest("strrpl01: String.prototype.replace - can take a string or a RegExp as a parameter"); 


	str1 = new String();
	arr1 = new Array();
	reg1 = new RegExp();


	apInitScenario("1 Matching argument is null");
	str1 = "My dog has fleas.";
	str2 = str1.replace(null, "XXX");
	verify (str2, "My dog has fleas.", "1 Wrong result returned");	


	apInitScenario("2 Matching argument is undefined");
	str1 = "My dog has fleas.";
	str2 = str1.replace(undefined, "XXX");
	verify (str2, "My dog has fleas.", "2 Wrong result returned");	


	apInitScenario("3 Matching argument is empty string");
	str1 = "My dog has fleas.";
	str2 = str1.replace("", "XXX");
	verify (str2, "XXXMy dog has fleas.", "3 Wrong result returned");	


	apInitScenario("4 Matching argument is exactly equal to String");
	str1 = "My dog has fleas.";
	str2 = str1.replace("My dog has fleas.", "XXX");
	verify (str2, "XXX", "4 Wrong result returned");	


	apInitScenario("5 Matching argument is not found in String");
	str1 = "My dog has fleas.";
	str2 = str1.replace("cat", "XXX");
	verify (str2, "My dog has fleas.", "5 Wrong result returned");	


	apInitScenario("6 Matching argument matches beginning of String");
	str1 = "My dog has fleas.";
	str2 = str1.replace("My ", "XXX");
	verify (str2, "XXXdog has fleas.", "6 Wrong result returned");	


	apInitScenario("7 Matching argument matches end of String");
	str1 = "My dog has fleas.";
	str2 = str1.replace("s.", "XXX");
	verify (str2, "My dog has fleaXXX", "7 Wrong result returned");	


	apInitScenario("8 Matching argument is longer String which starts with calling string");
	str1 = "My dog has fleas.";
	str2 = str1.replace("My dog has fleas.  But I don't.", "XXX");
	verify (str2, "My dog has fleas.", "8 Wrong result returned");	


	apInitScenario("9 Matching argument is empty RegExp");
	str1 = "My dog has fleas.";
	str2 = str1.replace(reg1, "XXX");
	verify (str2, "XXXMy dog has fleas.", "9 Wrong result returned");	


	apInitScenario("10 Matching argument is RegExp exactly equal to String");
	str1 = "My dog has fleas.";
	reg1 = /My dog has fleas./gi;
	str2 = str1.replace(reg1, "XXX");
	verify (str2, "XXX", "10 Wrong result returned");	


	apInitScenario("11 Matching argument is RegExp which will match once in String");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, "XXX");
	verify (str2, "My XXX has fleas.", "11 Wrong result returned");	


	apInitScenario("12 Matching argument is RegExp which will match multiple times in String");
	str1 = "My dog has fleas.";
	reg1 = / /gi;
	str2 = str1.replace(reg1, "XXX");
	verify (str2, "MyXXXdogXXXhasXXXfleas.", "12 Wrong result returned");	


	apInitScenario("13 Matching argument is RegExp which won't match String");
	str1 = "My dog has fleas.";
	reg1 = /cat/gi;
	str2 = str1.replace(reg1, "XXX");
	verify (str2, "My dog has fleas.", "13 Wrong result returned");	


	apInitScenario("14 ReplaceValue is null");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, null);
	verify (str2, "My null has fleas.", "14 Wrong result returned");	


	apInitScenario("15 ReplaceValue is undefined");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, undefined);
	verify (str2, "My undefined has fleas.", "15 Wrong result returned");	


	apInitScenario("16 ReplaceValue is empty string");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, "");
	verify (str2, "My  has fleas.", "16 Wrong result returned");	


	apInitScenario("17 ReplaceValue is number");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, 3);
	verify (str2, "My 3 has fleas.", "17 Wrong result returned");	


	apInitScenario("18 ReplaceValue is RegExp");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, reg1);
	verify (str2, "My /dog/ig has fleas.", "18 Wrong result returned");	


	apInitScenario("19 ReplaceValue is Array");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	arr1 = new Array(3);
	arr1[0] = 3;
	arr1[1] = -5;
	arr1[2] = "Four";
	str2 = str1.replace(reg1, arr1);
	verify (str2, "My 3,-5,Four has fleas.", "19 Wrong result returned");	


	apInitScenario("20 ReplaceValue is Object");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	arr1 = new Object();
	arr1.mem = 3;
	str2 = str1.replace(reg1, arr1);
	verify (str2, "My [object Object] has fleas.", "20 Wrong result returned");	


	apEndTest();
}

strrpl01();


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
