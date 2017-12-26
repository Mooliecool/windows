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


var iTestID = 52914;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }

@if(!@aspx)
	function myObject() { this.retVal = myObjVal(); }
@else
	expando function myObject() { this.retVal = myObjVal(); }
@end

function minu01() {
    var tmp,res,expected,emptyVar;

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 1969

    var numvar = 4321;
    var strvar = "57";
    var boolvar = true; // new Boolean(true);
	var emptystrvar = "";

    var ary = new Array();
    ary[2] = 12.14;
    ary[3] = 13.14;
    ary[4] = 15.14;

    var myObj = new myObject();
    var myStr = new String("112");

    apInitTest("minu01");

    apInitScenario("numvar - numvar");
    res = numvar - numvar;
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - numvar");
    res = strvar - numvar;
    expected = -4264;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - numvar");
    res = ary[3] - numvar;
    expected = -4307.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - numvar");
    res = FuncRetVal() - numvar;
    expected = -3665.65;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - numvar");
    res = myDate.getDate() - numvar;
    expected = -4290;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - numvar");
    res = boolvar - numvar;
    expected = -4320;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - strvar");
    res = numvar - strvar;
    expected = 4264;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - strvar");
    res = strvar - strvar;
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - strvar");
    res = ary[3] - strvar;
    expected = -43.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - strvar");
    res = FuncRetVal() - strvar;
    expected = 598.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - strvar");
    res = myDate.getDate() - strvar;
    expected = -26;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - strvar");
    res = boolvar - strvar;
    expected = -56;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - ary[3]");
    res = numvar - ary[3];
    expected = 4307.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - ary[3]");
    res = strvar - ary[3];
    expected = 43.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - ary[3]");
    res = ary[3] - ary[3];
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - ary[3]");
    res = FuncRetVal() - ary[3];
    expected = 642.21;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - ary[3]");
    res = myDate.getDate() - ary[3];
    expected = 17.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - ary[3]");
    res = boolvar - ary[3];
    expected = -12.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - FuncRetVal()");
    res = numvar - FuncRetVal();
    expected = 3665.65;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - FuncRetVal()");
    res = strvar - FuncRetVal();
    expected = -598.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - FuncRetVal()");
    res = ary[3] - FuncRetVal();
    expected = -642.21;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - FuncRetVal()");
    res = FuncRetVal() - FuncRetVal();
    expected = 3e300 - 3e300;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - FuncRetVal()");
    res = myDate.getDate() - FuncRetVal();
    expected = -624.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - FuncRetVal()");
    res = boolvar - FuncRetVal();
    expected = -654.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - myDate.getDate()");
    res = numvar - myDate.getDate();
    expected = 4290;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - myDate.getDate()");
    res = strvar - myDate.getDate();
    expected = 26;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - myDate.getDate()");
    res = ary[3] - myDate.getDate();
    expected = -17.86;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - myDate.getDate()");
    res = FuncRetVal() - myDate.getDate();
    expected = 624.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - myDate.getDate()");
    res = myDate.getDate() - myDate.getDate();
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - myDate.getDate()");
    res = boolvar - myDate.getDate();
    expected = -30;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - boolvar");
    res = numvar - boolvar;
    expected = 4320;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - boolvar");
    res = strvar - boolvar;
    expected = 56;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - boolvar");
    res = ary[3] - boolvar;
    expected = 12.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - boolvar");
    res = FuncRetVal() - boolvar;
    expected = 654.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - boolvar");
    res = myDate.getDate() - boolvar;
    expected = 30;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - boolvar");
    res = boolvar - boolvar;
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar - myObj.retVal");
    res = numvar - myObj.retVal;
    expected = -119000;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar - myObj.retVal");
    res = strvar - myObj.retVal;
    expected = -123264;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] - myObj.retVal");
    res = ary[3] - myObj.retVal;
    expected = -123307.86
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() - myObj.retVal");
    res = FuncRetVal() - myObj.retVal;
    expected = -122665.65
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() - myObj.retVal");
    res = myDate.getDate() - myObj.retVal;
    expected = -123290
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar - myObj.retVal");
    res = boolvar - myObj.retVal;
    expected = -123320;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("emptystrvar - numvar");
    res = emptystrvar - numvar;
    expected = -4321;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apEndTest();
}


minu01();


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
