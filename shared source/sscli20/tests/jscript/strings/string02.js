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


var iTestID = 65389;

function FuncRetStr() {
  return "function returning string";
}

function myObjStr() { return "my object string"; }


@if(!@aspx)
	function myObject() { this.toString = myObjStr; }
@else
	expando function myObject() { this.toString = myObjStr; }
@end


function string02() {
    var tmp,res,expected,emptyVar;

    apInitTest("string02");

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
    var myStr = new String("Apple Core");

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
    
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {

    apInitScenario("add date.toString to string object");
    res = myDate.toString() + myStr;
    expected = "Wed Dec 31 16:00:00 PST 1969Apple Core";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to date (coerced to string)");
    res = myDate.toString() + ("" + myDate);
    expected = "Wed Dec 31 16:00:00 PST 1969Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to date.toString");
    res = myDate.toString() + (myDate.toString());
    expected = "Wed Dec 31 16:00:00 PST 1969Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to function returning string");
    res = myDate.toString() + FuncRetStr();
    expected = "Wed Dec 31 16:00:00 PST 1969function returning string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to null");
    res = myDate.toString() + null;
    expected = "Wed Dec 31 16:00:00 PST 1969null";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to null string");
    res = myDate.toString() + "";
    expected = "Wed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to coerced numeric constant");
    res = myDate.toString() + ("" + 1234);
    expected = "Wed Dec 31 16:00:00 PST 19691234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to numeric constant");
    res = myDate.toString() + 1234;
    expected = "Wed Dec 31 16:00:00 PST 19691234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to exp numeric constant");
    res = myDate.toString() + 3e300;
    expected = "Wed Dec 31 16:00:00 PST 19693e+300";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to fp numeric constant");
    res = myDate.toString() + 1234.56;
    expected = "Wed Dec 31 16:00:00 PST 19691234.56";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to numeric variable");
    res = myDate.toString() + numvar;
    expected = "Wed Dec 31 16:00:00 PST 19694321";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to string constant");
    res = myDate.toString() + "Bernoulli";
    expected = "Wed Dec 31 16:00:00 PST 1969Bernoulli";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to string expression");
    res = myDate.toString() + ("abc" + "def");
    expected = "Wed Dec 31 16:00:00 PST 1969abcdef";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to string variable");
    res = myDate.toString() + strvar;
    expected = "Wed Dec 31 16:00:00 PST 1969Hippopotamus";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to string array element");
    res = myDate.toString() + ary[3];
    expected = "Wed Dec 31 16:00:00 PST 1969albatross";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to eval string");
    res = myDate.toString() + eval("\"boingo\"");
    expected = "Wed Dec 31 16:00:00 PST 1969boingo";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to empty variable");
    res = myDate.toString() + emptyVar;
    expected = "Wed Dec 31 16:00:00 PST 1969undefined";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to user-defined object member");
    res = myDate.toString() + myObj.toString();
    expected = "Wed Dec 31 16:00:00 PST 1969my object string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to string.toString");
    res = myDate.toString() + "Horse".toString();
    expected = "Wed Dec 31 16:00:00 PST 1969Horse";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add date.toString to Math.toString");
    res = myDate.toString() + Math.toString();
    expected = "Wed Dec 31 16:00:00 PST 1969" + Mathcode;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    }
    apEndTest();
}


string02();


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
