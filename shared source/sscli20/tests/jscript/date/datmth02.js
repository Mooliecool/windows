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


var iTestID = 51830;

function datmth02() {
    var expected;
    var res;

    var myD;

    apInitTest("datmth02");

    myD = new Date();
    // myD.setTime(0);

    apInitScenario("basic date method functionality - setDate");
    expected = 12;
    myD.setDate(expected);
    res = myD.getDate();
    if (expected != res) {
	apLogFailInfo("setDate returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setHours");
    expected = 6;
    myD.setHours(expected);
    res = myD.getHours();
    if (expected != res) {
	apLogFailInfo("setHours returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setMinutes");
    expected = 59;
    myD.setMinutes(expected);
    res = myD.getMinutes();
    if (expected != res) {
	apLogFailInfo("setMinutes returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setMonth");
    expected = 7;
    myD.setMonth(expected);
    res = myD.getMonth();
    if (expected != res) {
	apLogFailInfo("setMonth returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setSeconds");
    expected = 13;
    myD.setSeconds(expected);
    res = myD.getSeconds();
    if (expected != res) {
	apLogFailInfo("setSeconds returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setYear");
    expected = 89;
    myD.setYear(expected);
    res = myD.getYear();
    if (expected != res) {
	apLogFailInfo("setYear returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - setTime");
    expected = 2000;
    myD.setTime(expected + 0.5555);
    res = myD.getTime();
    if (expected != res) {
	apLogFailInfo("setTime returned wrong result",expected,res,"");
    }

    apEndTest();
}


datmth02();


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
