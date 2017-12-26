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


var iTestID = 53681;

function FuncRetVal() { return "655.35"; }
function FuncRetNulVal() { return ""; }
function myObject() { this.retVal = myObjVal(); }
function myObjVal() { return 123321; }

function strop03() {
    var tmp,res,expected,emptyVar;

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 PST 1969

    var numvar = 4321;
    var strvar = "57";
    var strvarnul = "";
    var boolvar = new Boolean(true);

    var ary = new Array();
    ary[2] = 12.14;
    ary[3] = 13.14;
    ary[4] = 15.14;

    var myObj = new myObject();
    var myStr = new String("112");

    apInitTest("strop03");

    apInitScenario("null funcretval += expr");
    res = FuncRetNulVal(); res += "alpha" + "beta";
    expected = "alphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += funcretval");
    res = FuncRetNulVal(); res += FuncRetVal();
    expected = "655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += literal");
    res = FuncRetNulVal(); res += "JavaScript";
    expected = "JavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += variable");
    res = FuncRetNulVal(); res += strvar;
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += null expr");
    res = FuncRetNulVal(); res += "" + "";
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += null funcretval");
    res = FuncRetNulVal(); res += FuncRetNulVal();
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += null literal");
    res = FuncRetNulVal(); res += "";
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += null variable");
    res = FuncRetNulVal(); res += strvarnul;
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += boolean");
    res = FuncRetNulVal(); res += boolvar;
    expected = "true";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += numeric literal");
    res = FuncRetNulVal(); res += 55.2;
    expected = "55.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += numeric var");
    res = FuncRetNulVal(); res += numvar;
    expected = "4321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval += object");
    res = FuncRetNulVal(); res += myObj;
    expected = "[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("null funcretval += date");
    res = FuncRetNulVal(); res += myDate;
    expected = "Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apInitScenario("null funcretval + expr");
    res = FuncRetNulVal() + "alpha" + "beta";
    expected = "alphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + funcretval");
    res = FuncRetNulVal() + FuncRetVal();
    expected = "655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + literal");
    res = FuncRetNulVal() + "JavaScript";
    expected = "JavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + variable");
    res = FuncRetNulVal() + strvar;
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + null expr");
    res = FuncRetNulVal() + "" + "";
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + null funcretval");
    res = FuncRetNulVal() + FuncRetNulVal();
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + null literal");
    res = FuncRetNulVal() + "";
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + null variable");
    res = FuncRetNulVal() + strvarnul;
    expected = "";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + boolean");
    res = FuncRetNulVal() + boolvar;
    expected = "true";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + numeric literal");
    res = FuncRetNulVal() + 55.2;
    expected = "55.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + numeric var");
    res = FuncRetNulVal() + numvar;
    expected = "4321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("null funcretval + object");
    res = FuncRetNulVal() + myObj;
    expected = "[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("null funcretval + date");
    res = FuncRetNulVal() + myDate;
    expected = "Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apEndTest();
}


strop03();


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
