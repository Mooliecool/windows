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


var iTestID = 65400;

function FuncRetStr() {
  return "function returning string";
}

function myObject() { this.toString = myObjStr; }
function myObjStr() { return "my object string"; }

function string13() {
    var tmp,res,expected,emptyVar;

    apInitTest("string13");

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

    apInitScenario("add string constant to string object");
    res = "dead man's party" + myStr;
    expected = "dead man's partyApple Core";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("add string constant to date (coerced to string)");
    res = "dead man's party" + ("" + myDate);
    expected = "dead man's partyWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to date.toString()");
    res = "dead man's party" + (myDate.toString());
    expected = "dead man's partyWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    }

    apInitScenario("add string constant to function returning string");
    res = "dead man's party" + FuncRetStr();
    expected = "dead man's partyfunction returning string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to null");
    res = "dead man's party" + null;
    expected = "dead man's partynull";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to null string");
    res = "dead man's party" + "";
    expected = "dead man's party";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to coerced numeric constant");
    res = "dead man's party" + ("" + 1234);
    expected = "dead man's party1234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to numeric constant");
    res = "dead man's party" + 1234;
    expected = "dead man's party1234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to exp numeric constant");
    res = "dead man's party" + 3e300;
    expected = "dead man's party3e+300";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to fp numeric constant");
    res = "dead man's party" + 1234.56;
    expected = "dead man's party1234.56";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to numeric variable");
    res = "dead man's party" + numvar;
    expected = "dead man's party4321";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to string constant");
    res = "dead man's party" + "Bernoulli";
    expected = "dead man's partyBernoulli";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to string expression");
    res = "dead man's party" + ("abc" + "def");
    expected = "dead man's partyabcdef";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to string variable");
    res = "dead man's party" + strvar;
    expected = "dead man's partyHippopotamus";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to string array element");
    res = "dead man's party" + ary[3];
    expected = "dead man's partyalbatross";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to empty variable");
    res = "dead man's party" + emptyVar;
    expected = "dead man's partyundefined";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to user-defined object member");
    res = "dead man's party" + myObj.toString();
    expected = "dead man's partymy object string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to string.toString");
    res = "dead man's party" + "Horse".toString();
    expected = "dead man's partyHorse";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to Math.toString");
    res = "dead man's party" + Math.toString();
    expected = "dead man's party" + Mathcode;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string constant to eval string");
    res = "dead man's party" + eval("\"boingo\"");
    expected = "dead man's partyboingo";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apEndTest();
}

string13();


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
