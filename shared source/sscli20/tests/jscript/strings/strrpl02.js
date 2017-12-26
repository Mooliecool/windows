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


var iTestID = 72443;

//////////
//
//  Test Case:
//
//     strrpl02: String.prototype.replace -  can take a function as a replacement value
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


function ReplaceFunc(strRepl) {
	strRepl = strRepl.toString();
	strRepl = strRepl.toUpperCase();
	return strRepl;
}


function ReplaceNoArgs() {
	return "XXX";
}


function ReplaceTwoArgs(str1, str2) {
	return str2+str1;
}


function ReplaceSideEffect(str1) {
	reg1 = /cat/gi;
	return str1;
}



//////////
//
//  Global variables
//

var str1;
var str2;
var arr1;
var reg1;



reg1 = new RegExp();



function strrpl02() {


    apInitTest("strrpl02: String.prototype.replace -  can take a function as a replacement value"); 


	str1 = new String();
	arr1 = new Array();


	apInitScenario("1 Matching argument is string, replaceValue supplied");
	str1 = "My dog has fleas.";
	str2 = str1.replace("dog", "xxx");
	verify (str2, "My xxx has fleas.", "1.1 Wrong result returned");
	str2 = str1.replace("dog", ReplaceFunc("xxx"));
	verify (str2, "My XXX has fleas.", "1.2 Wrong result returned");	


	apInitScenario("2 Matching argument is regexp, replaceValue supplied");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, "xxx");
	verify (str2, "My xxx has fleas.", "2.1 Wrong result returned");
	str2 = str1.replace(reg1, ReplaceFunc("xxx"));
	verify (str2, "My XXX has fleas.", "2.2 Wrong result returned");	


	apInitScenario("3 Matching argument is array");
	str1 = "My dog has fleas.";
	arr1 = new Array(3,-5,"Four");
	str2 = str1.replace(arr1, "xxx");
	verify (str2, "My dog has fleas.", "3.1 Wrong result returned");
	str2 = str1.replace(arr1, ReplaceFunc("xxx"));
	verify (str2, "My dog has fleas.", "3.2 Wrong result returned");	


	apInitScenario("4 Matching argument is object");
	str1 = "My dog has fleas.";
	arr1 = new Object();
	arr1.mem = "Four";
	str2 = str1.replace(arr1, "xxx");
	verify (str2, "My dog has fleas.", "4.1 Wrong result returned");
	str2 = str1.replace(arr1, ReplaceFunc("xxx"));
	verify (str2, "My dog has fleas.", "4.2 Wrong result returned");	


	apInitScenario("5 Matching argument is number");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, 4);
	verify (str2, "My 4 has fleas.", "5 Wrong result returned");


	apInitScenario("6 replaceValue is function with no arguments");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, ReplaceNoArgs());
	verify (str2, "My XXX has fleas.", "6 Wrong result returned");


	apInitScenario("7 replaceValue is function with two arguments");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, ReplaceTwoArgs("XXX", "YYY"));
	verify (str2, "My YYYXXX has fleas.", "7 Wrong result returned");


	apInitScenario("8 replaceValue has side effect that changes regExp value");
	str1 = "My dog has fleas.";
	reg1 = /dog/gi;
	str2 = str1.replace(reg1, ReplaceSideEffect("XXX"));
	verify (str2, "My XXX has fleas.", "8.1 Wrong result returned");
	str2 = str1.replace(reg1, ReplaceSideEffect("XXX"));
	verify (str2, "My dog has fleas.", "8.2 Wrong result returned");


	apEndTest();
}

strrpl02();


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
