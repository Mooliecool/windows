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


var iTestID = 65397;

function FuncRetStr() {
  return "function returning string";
}

function myObject() { this.toString = myObjStr; }
function myObjStr() { return "my object string"; }

function string10() {
    var tmp,res,expected,emptyVar;

    apInitTest("string10");

    var myStr = new String("Apple Core");
    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 PST 1969
    myDate.setTime(Date.UTC(70,0,1,0) + myDate.getTimezoneOffset() * 60000 - 28800000);
    var numvar = 4321;
    var strvar = "Hippopotamus";

    var ary = new Array();
    ary[2] = "zero";
    ary[3] = "albatross";
    ary[4] = "what flavor is it?";

    var myObj = new myObject();

    var crlf = unescape("%0d%0a");
    var lf = unescape("%0a"); // LF
    @if (@_win16)
	var FRScode =lf+"function FuncRetStr() {"+lf+"    [native code]"+lf+"}"+lf;
	var FRScode2="error";
	var FRScode3="error";
@else
    //crlf = unescape("%0d%0a"); 
    crlf = Environment.NewLine;
    var cr = unescape("%0d");

    var FRScode = "function FuncRetStr() {" + crlf + '  return "function returning string";' + crlf + "}";
    var FRScode2 = "function FuncRetStr() {" + cr + '  return "function returning string";' + cr + "}";
    var FRScode3 = "function FuncRetStr() {" + crlf + 'return "function returning string";' + crlf + "}";
@end
    var Mathcode = "[object Math]";

    apInitScenario("add exp numeric constant to string object");
    res = 3e300 + myStr;
    expected = "3e+300Apple Core";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("add exp numeric constant to date (coerced to string)");
    res = 3e300 + ("" + myDate);
    expected = "3e+300Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to date.toString()");
    res = 3e300 + (myDate.toString());
    expected = "3e+300Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    }

    apInitScenario("add exp numeric constant to function returning string");
    res = 3e300 + FuncRetStr();
    expected = "3e+300function returning string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to null string");
    res = 3e300 + "";
    expected = "3e+300";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to string constant");
    res = 3e300 + "Bernoulli";
    expected = "3e+300Bernoulli";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to string expression");
    res = 3e300 + ("abc" + "def");
    expected = "3e+300abcdef";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to string variable");
    res = 3e300 + strvar;
    expected = "3e+300Hippopotamus";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to string array element");
    res = 3e300 + ary[3];
    expected = "3e+300albatross";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to user-defined object member");
    res = 3e300 + myObj.toString();
    expected = "3e+300my object string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to string.toString");
    res = 3e300 + "Horse".toString();
    expected = "3e+300Horse";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to Math.toString");
    res = 3e300 + Math.toString();
    expected = "3e+300" + Mathcode;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add exp numeric constant to eval string");
    res = 3e300 + eval("\"boingo\"");
    expected = "3e+300boingo";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apEndTest();
}

string10();


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
