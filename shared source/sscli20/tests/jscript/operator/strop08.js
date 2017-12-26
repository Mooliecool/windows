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


var iTestID = 53686;

function FuncRetVal() { return "655.35"; }
function FuncRetNulVal() { return ""; }
function myObject() { this.retVal = myObjVal(); }
function myObjVal() { return 123321; }

function strop08() {
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

    apInitTest("strop08");

    apInitScenario("string var += expr");
    res = strvar; res += "alpha" + "beta";
    expected = "57alphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += funcretval");
    res = strvar; res += FuncRetVal();
    expected = "57655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += literal");
    res = strvar; res += "JavaScript";
    expected = "57JavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += variable");
    res = strvar; res += strvar;
    expected = "5757";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += null expr");
    res = strvar; res += "" + "";
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += null funcretval");
    res = strvar; res += FuncRetNulVal();
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += null literal");
    res = strvar; res += "";
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += null variable");
    res = strvar; res += strvarnul;
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += boolean");
    res = strvar; res += boolvar;
    expected = "57true";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += numeric literal");
    res = strvar; res += 55.2;
    expected = "5755.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += numeric var");
    res = strvar; res += numvar;
    expected = "574321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var += object");
    res = strvar; res += myObj;
    expected = "57[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("string var += date");
    res = strvar; res += myDate;
    expected = "57Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apInitScenario("string var + expr");
    res = strvar + "alpha" + "beta";
    expected = "57alphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + funcretval");
    res = strvar + FuncRetVal();
    expected = "57655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + literal");
    res = strvar + "JavaScript";
    expected = "57JavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + variable");
    res = strvar + strvar;
    expected = "5757";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + null expr");
    res = strvar + "" + "";
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + null funcretval");
    res = strvar + FuncRetNulVal();
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + null literal");
    res = strvar + "";
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + null variable");
    res = strvar + strvarnul;
    expected = "57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + boolean");
    res = strvar + boolvar;
    expected = "57true";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + numeric literal");
    res = strvar + 55.2;
    expected = "5755.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + numeric var");
    res = strvar + numvar;
    expected = "574321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string var + object");
    res = strvar + myObj;
    expected = "57[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("string var + date");
    res = strvar + myDate;
    expected = "57Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apEndTest();
}

strop08();


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
