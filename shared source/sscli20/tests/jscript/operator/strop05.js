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


var iTestID = 53683;

function FuncRetVal() { return "655.35"; }
function FuncRetNulVal() { return ""; }
function myObject() { this.retVal = myObjVal(); }
function myObjVal() { return 123321; }

function strop05() {
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

    apInitTest("strop05");

    apInitScenario("string expr += expr");
    res = "micro" + "soft"; res += "alpha" + "beta";
    expected = "microsoftalphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += funcretval");
    res = "micro" + "soft"; res += FuncRetVal();
    expected = "microsoft655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += literal");
    res = "micro" + "soft"; res += "JavaScript";
    expected = "microsoftJavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += variable");
    res = "micro" + "soft"; res += strvar;
    expected = "microsoft57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += null expr");
    res = "micro" + "soft"; res += "" + "";
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += null funcretval");
    res = "micro" + "soft"; res += FuncRetNulVal();
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += null literal");
    res = "micro" + "soft"; res += "";
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += null variable");
    res = "micro" + "soft"; res += strvarnul;
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += boolean");
    res = "micro" + "soft"; res += boolvar;
    expected = "microsofttrue";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += numeric literal");
    res = "micro" + "soft"; res += 55.2;
    expected = "microsoft55.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += numeric var");
    res = "micro" + "soft"; res += numvar;
    expected = "microsoft4321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr += object");
    res = "micro" + "soft"; res += myObj;
    expected = "microsoft[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("string expr += date");
    res = "micro" + "soft"; res += myDate;
    expected = "microsoftWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apInitScenario("string expr + expr");
    res = "micro" + "soft" + "alpha" + "beta";
    expected = "microsoftalphabeta";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + funcretval");
    res = "micro" + "soft" + FuncRetVal();
    expected = "microsoft655.35";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + literal");
    res = "micro" + "soft" + "JavaScript";
    expected = "microsoftJavaScript";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + variable");
    res = "micro" + "soft" + strvar;
    expected = "microsoft57";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + null expr");
    res = "micro" + "soft" + "" + "";
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + null funcretval");
    res = "micro" + "soft" + FuncRetNulVal();
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + null literal");
    res = "micro" + "soft" + "";
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + null variable");
    res = "micro" + "soft" + strvarnul;
    expected = "microsoft";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + boolean");
    res = "micro" + "soft" + boolvar;
    expected = "microsofttrue";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + numeric literal");
    res = "micro" + "soft" + 55.2;
    expected = "microsoft55.2";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + numeric var");
    res = "micro" + "soft" + numvar;
    expected = "microsoft4321";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    apInitScenario("string expr + object");
    res = "micro" + "soft" + myObj;
    expected = "microsoft[object Object]";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("string expr + date");
    res = "micro" + "soft" + myDate;
    expected = "microsoftWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != typeof(expected)) {
	apLogFailInfo("wrong result",typeof(expected),typeof(res),"");
    }
    }

    apEndTest();
}


strop05();


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
