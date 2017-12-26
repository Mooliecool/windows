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


var iTestID = 51750;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }


@if(!@aspx)
	function myObject() { this.toString = myObjVal; }
@else
	expando function myObject() { this.toString = myObjVal; }
@end

function band02() {
    var tmp,res,expected,emptyVar;

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 1969
    myDate.setTime(Date.UTC(70,0,1,0) + myDate.getTimezoneOffset() * 60000 - 28800000);
    var numvar = 4321;
    var strvar = "57";
    var boolvar = true; // new Boolean(true);

    var ary = new Array();
    ary[2] = 12.14;
    ary[3] = 13.14;
    ary[4] = 15.14;

    var myObj = new myObject();
    var myStr = "112"; // new String("112");

    apInitTest("band02");

    apInitScenario("ary[3] & ary[3]");
    res = ary[3] & ary[3];
    expected = 13;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() & ary[3]");
    res = FuncRetVal() & ary[3];
    expected = 13;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() & ary[3]");
    res = myDate.getDate() & ary[3];
    expected = 13;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar & ary[3]");
    res = boolvar & ary[3];
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar & FuncRetVal()");
    res = numvar & FuncRetVal();
    expected = 129;
    if (expected != res) {
    	apLogFailInfo("wrong return value", expected, res, "175195");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type", "string", res, "175195");
    }

    apInitScenario("strvar & FuncRetVal()");
    res = strvar & FuncRetVal();
    expected = 9;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] & FuncRetVal()");
    res = ary[3] & FuncRetVal();
    expected = 13;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() & FuncRetVal()");
    res = FuncRetVal() & FuncRetVal();
    expected = 655;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() & FuncRetVal()");
    res = myDate.getDate() & FuncRetVal();
    expected = 15;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar & FuncRetVal()");
    res = boolvar & FuncRetVal();
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar & myDate.getDate()");
    res = numvar & myDate.getDate();
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar & myDate.getDate()");
    res = strvar & myDate.getDate();
    expected = 25;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] & myDate.getDate()");
    res = ary[3] & myDate.getDate();
    expected = 13;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() & myDate.getDate()");
    res = FuncRetVal() & myDate.getDate();
    expected = 15;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() & myDate.getDate()");
    res = myDate.getDate() & myDate.getDate();
    expected = 31;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar & myDate.getDate()");
    res = boolvar & myDate.getDate();
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar & boolvar");
    res = numvar & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar & boolvar");
    res = strvar & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] & boolvar");
    res = ary[3] & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() & boolvar");
    res = FuncRetVal() & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() & boolvar");
    res = myDate.getDate() & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar & boolvar");
    res = boolvar & boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apEndTest();
}


band02();


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
