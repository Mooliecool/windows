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


var iTestID = 51783;

function coerce01() {
    var expected;
    var res;

    apInitTest("coerce01");

    apInitScenario("number to string");
    var x = 3.000;
    expected = "3";
    res = "" + x;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong type",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("date to string");
    var y = new Date();
    y.setTime(0);
    y.setTime(Date.UTC(70,0,1,0) + y.getTimezoneOffset() * 60000 - 28800000);
    expected = "Wed Dec 31 16:00:00 PST 1969";
    res = "" + y;
    if (expected != res) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong type",typeof(expected),typeof(res),"");
    }
    }

    apEndTest();
}


coerce01();


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
