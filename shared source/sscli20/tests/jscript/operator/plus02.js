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


var iTestID = 52913;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }

@if(!@aspx)
	function myObject() { this.retVal = myObjVal(); }
@else
	expando function myObject() { this.retVal = myObjVal(); }
@end

function plus02() {
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

    apInitTest("plus02");

    apInitScenario("numvar += numvar");
    res = numvar; res += numvar;
    expected = 8642;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += numvar");
    res = strvar; res += numvar;
    expected = "574321";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += numvar");
    res = ary[3]; res += numvar;
    expected = 4334.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += numvar");
    res = FuncRetVal(); res += numvar;
    expected = 4976.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += numvar");
    res = myDate.getDate(); res += numvar;
    expected = 4352;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += numvar");
    res = boolvar; res += numvar;
    expected = 4322;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += strvar");
    res = numvar; res += strvar;
    expected = "432157";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("strvar += strvar");
    res = strvar; res += strvar;
    expected = "5757";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += strvar");
    res = ary[3]; res += strvar;
    expected = "13.1457";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("FuncRetVal() += strvar");
    res = FuncRetVal(); res += strvar;
    expected = "655.3557";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("myDate.getDate() += strvar");
    res = myDate.getDate(); res += strvar;
    expected = "3157";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("boolvar += strvar");
    res = boolvar; res += strvar;
    expected = "true57";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("numvar += ary[3]");
    res = numvar; res += ary[3];
    expected = 4334.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += ary[3]");
    res = strvar; res += ary[3];
    expected = "5713.14";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += ary[3]");
    res = ary[3]; res += ary[3];
    expected = 26.28;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += ary[3]");
    res = FuncRetVal(); res += ary[3];
    expected = 668.49;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += ary[3]");
    res = myDate.getDate(); res += ary[3];
    expected = 44.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += ary[3]");
    res = boolvar; res += ary[3];
    expected = 14.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += FuncRetVal()");
    res = numvar; res += FuncRetVal();
    expected = 4976.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += FuncRetVal()");
    res = strvar; res += FuncRetVal();
    expected = "57655.35";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += FuncRetVal()");
    res = ary[3]; res += FuncRetVal();
    expected = 668.49;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += FuncRetVal()");
    res = FuncRetVal(); res += FuncRetVal();
    expected = 1310.7;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += FuncRetVal()");
    res = myDate.getDate(); res += FuncRetVal();
    expected = 686.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += FuncRetVal()");
    res = boolvar; res += FuncRetVal();
    expected = 656.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += myDate.getDate()");
    res = numvar; res += myDate.getDate();
    expected = 4352;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += myDate.getDate()");
    res = strvar; res += myDate.getDate();
    expected = "5731";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += myDate.getDate()");
    res = ary[3]; res += myDate.getDate();
    expected = 44.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += myDate.getDate()");
    res = FuncRetVal(); res += myDate.getDate();
    expected = 686.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += myDate.getDate()");
    res = myDate.getDate(); res += myDate.getDate();
    expected = 62;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += myDate.getDate()");
    res = boolvar; res += myDate.getDate();
    expected = 32;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += boolvar");
    res = numvar; res += boolvar;
    expected = 4322;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += boolvar");
    res = strvar; res += boolvar;
    expected = "57true";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += boolvar");
    res = ary[3]; res += boolvar;
    expected = 14.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += boolvar");
    res = FuncRetVal(); res += boolvar;
    expected = 656.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += boolvar");
    res = myDate.getDate(); res += boolvar;
    expected = 32;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += boolvar");
    res = boolvar; res += boolvar;
    expected = 2;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += myObj.retVal");
    res = numvar; res += myObj.retVal;
    expected = 127642;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar += myObj.retVal");
    res = strvar; res += myObj.retVal;
    expected = "57123321";
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apInitScenario("ary[3] += myObj.retVal");
    res = ary[3]; res += myObj.retVal;
    expected = 123334.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() += myObj.retVal");
    res = FuncRetVal(); res += myObj.retVal;
    expected = 655.35 + 123000 + 321;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() += myObj.retVal");
    res = myDate.getDate(); res += myObj.retVal;
    expected = 123352;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar += myObj.retVal");
    res = boolvar; res += myObj.retVal;
    expected = 123322;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar += emptystrvar");
    res = numvar; res += emptystrvar;
    expected = 4321;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "string") {
    	apLogFailInfo("wrong return type","string",res,"");
    }

    apEndTest();
}


plus02();


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
