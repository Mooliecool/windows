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


var iTestID = 72431;

//////////
//
//  Test Spec:
//
//     strcon01: String.prototype.concat can take zero or more arguments
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


function x(){ iCount = 3; }


function y() { iCount = -5; }	



//////////
//
//  Global variables
//


var str1;
var str2;
var str3;
var iCount; // counting variable



function strcon01() {

    apInitTest("strcon01: String.prototype.concat can take zero or more arguments"); 


	str1 = "My dog has fleas.";
	str2 = "My cat has ticks.";
	str3 = "I should take my pets to the vet.";


	apInitScenario("1 Zero arguments");
	verify (str1.concat(), "My dog has fleas.", "1 Wrong string returned");	


	apInitScenario("2 One argument");
	verify (str1.concat(str2), "My dog has fleas.My cat has ticks.", "2 Wrong string returned");	


	apInitScenario("3 Multiple arguments");
	verify (str1.concat(str2,str3), "My dog has fleas.My cat has ticks.I should take my pets to the vet.", "3 Wrong string returned");	


	apInitScenario("4 Multiple arguments of different data types");
	verify (str1.concat(str2,3,"Four"), "My dog has fleas.My cat has ticks.3Four", "4 Wrong string returned");	


	apInitScenario("5 Multiple arguments of functions");
	// x and y are functions defined above
	verify (str1.concat(str2,x,y), "My dog has fleas.My cat has ticks.function x(){ iCount = 3; }function y() { iCount = -5; }", "4 Wrong string returned");	


	apEndTest();
}

strcon01();


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
