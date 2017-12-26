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


var iTestID = 53125;

function prcdnc01() {
    var expected;
    var res;

    apInitTest("prcdnc01");

    apInitScenario("Math operator precedence:  + 2 * ");
    res = 3 + 2 * 2;
    expected = 7;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }

    apInitScenario("Math operator precedence:  * 2 + ");
    res = 3 * 2 + 2;
    expected = 8;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }

    apInitScenario("Math operator precedence:  + 2 / ");
    res = 3 + 2 / 2;
    expected = 4;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }

    apInitScenario("Math operator precedence:  / 2 + ");
    res = 3 / 2 + 2;
    expected = 3.5;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }

    apInitScenario("Math operator precedence:  - 2 * ");
    res = 3 - 2 * 2;
    expected = -1;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }

    apEndTest();
}


prcdnc01();


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
