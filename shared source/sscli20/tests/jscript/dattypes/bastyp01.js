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


var iTestID = 51739;

function bastyp01() {
    var expected;
    var res;

    apInitTest("bastyp01");

    apInitScenario("basic data type - string");
    var v1 = "something";
    expected = "something";
    if (expected != v1) {
	apLogFailInfo("string comparison failed",expected,v1,"");
    }
    expected = "string";
    if (typeof(v1) != expected) {
	apLogFailInfo("string had wrong type",expected,typeof(v1),"");
    }

    apInitScenario("basic data type - static string");
    expected = "something";
    if (expected != "something") {
	apLogFailInfo("static string comparison failed",expected,"something","");
    }
    expected = "string";
    if (typeof("something") != expected) {
	apLogFailInfo("static string had wrong type",expected,"","");
    }

    apInitScenario("basic data type - number");
    var v2 = 300;
    expected = 300;
    if (expected != v2) {
	apLogFailInfo("number comparison failed",expected,v2,"");
    }
    expected = "number";
    if (typeof(v2) != expected) {
	apLogFailInfo("number had wrong type",expected,typeof(v2),"");
    }

    apInitScenario("basic data type - date");
    var v3 = new Date();
    expected = "object";
    if (typeof(v3) != expected) {
	apLogFailInfo("date had wrong type",expected,typeof(v3),"");
    }

    apInitScenario("basic data type - number");
    var v4 = 3.1;
    expected = 3.1;
    if (expected != v4) {
	apLogFailInfo("number comparison failed",expected,v4,"");
    }
    expected = "number";
    if (typeof(v4) != expected) {
	apLogFailInfo("number had wrong type",expected,typeof(v4),"");
    }

    apInitScenario("basic data type - number");
    var v5 = 13.56789567e19;
    expected = 13.56789567e19;
    if (expected != v5) {
	apLogFailInfo("number comparison failed",expected,v5,"");
    }
    expected = "number";
    if (typeof(v5) != expected) {
	apLogFailInfo("number had wrong type",expected,typeof(v5),"");
    }

    apInitScenario("basic data type - array");
    var v6 = new Array();
    expected = "object";
    if (typeof(v6) != expected) {
	apLogFailInfo("array had wrong type",expected,typeof(v6),"");
    }

    apEndTest();
}


bastyp01();


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
