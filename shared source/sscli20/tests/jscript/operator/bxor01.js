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


var iTestID = 51753;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }


@if(!@aspx)
	function myObject() { this.toString = myObjVal; }
@else
	expando function myObject() { this.toString = myObjVal; }
@end

function bxor01() {
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

    apInitTest("bxor01");

    apInitScenario("numvar ^ numvar");
    res = numvar ^ numvar;
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar ^ numvar");
    res = strvar ^ numvar;
    expected = 4312;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] ^ numvar");
    res = ary[3] ^ numvar;
    expected = 4332;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() ^ numvar");
    res = FuncRetVal() ^ numvar;
    expected = 4718;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() ^ numvar");
    res = myDate.getDate() ^ numvar;
    expected = 4350;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar ^ numvar");
    res = boolvar ^ numvar;
    expected = 4320;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar ^ strvar");
    res = numvar ^ strvar;
    expected = 4312;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar ^ strvar");
    res = strvar ^ strvar;
    expected = 0;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] ^ strvar");
    res = ary[3] ^ strvar;
    expected = 52;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() ^ strvar");
    res = FuncRetVal() ^ strvar;
    expected = 694;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() ^ strvar");
    res = myDate.getDate() ^ strvar;
    expected = 38;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar ^ strvar");
    res = boolvar ^ strvar;
    expected = 56;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar ^ ary[3]");
    res = numvar ^ ary[3];
    expected = 4332;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar ^ ary[3]");
    res = strvar ^ ary[3];
    expected = 52;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apEndTest();
}


bxor01();


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
