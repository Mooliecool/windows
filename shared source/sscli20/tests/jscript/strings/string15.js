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


var iTestID = 65402;

function FuncRetStr() {
  return "function returning string";
}

function myObject() { this.toString = myObjStr; }
function myObjStr() { return "my object string"; }

function string15() {
    var tmp,res,expected,emptyVar;

    apInitTest("string15");

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

    apInitScenario("add string variable to string object");
    res = strvar + myStr;
    expected = "HippopotamusApple Core";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("add string variable to date (coerced to string)");
    res = strvar + ("" + myDate);
    expected = "HippopotamusWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to date.toString()");
    res = strvar + (myDate.toString());
    expected = "HippopotamusWed Dec 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    }
    apInitScenario("add string variable to function returning string");
    res = strvar + FuncRetStr();
    expected = "Hippopotamusfunction returning string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to null");
    res = strvar + null;
    expected = "Hippopotamusnull";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to null string");
    res = strvar + "";
    expected = "Hippopotamus";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to coerced numeric constant");
    res = strvar + ("" + 1234);
    expected = "Hippopotamus1234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to numeric constant");
    res = strvar + 1234;
    expected = "Hippopotamus1234";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to exp numeric constant");
    res = strvar + 3e300;
    expected = "Hippopotamus3e+300";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to fp numeric constant");
    res = strvar + 1234.56;
    expected = "Hippopotamus1234.56";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to numeric variable");
    res = strvar + numvar;
    expected = "Hippopotamus4321";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to string constant");
    res = strvar + "Bernoulli";
    expected = "HippopotamusBernoulli";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to string expression");
    res = strvar + ("abc" + "def");
    expected = "Hippopotamusabcdef";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to string variable");
    res = strvar + strvar;
    expected = "HippopotamusHippopotamus";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to string array element");
    res = strvar + ary[3];
    expected = "Hippopotamusalbatross";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to empty variable");
    res = strvar + emptyVar;
    expected = "Hippopotamusundefined";
    if (res != expected) {
	apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to user-defined object member");
    res = strvar + myObj.toString();
    expected = "Hippopotamusmy object string";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to string.toString");
    res = strvar + "Horse".toString();
    expected = "HippopotamusHorse";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to Math.toString");
    res = strvar + Math.toString();
    expected = "Hippopotamus" + Mathcode;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("add string variable to eval string");
    res = strvar + eval("\"boingo\"");
    expected = "Hippopotamusboingo";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario();
    res = "hello".toString(1,2);
    expected = "hello";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    res = "hello".toString(1);
    expected = "hello";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
	res = "hello".toString("asd");
    expected = "hello";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
	res = "hello".toString(1,2,3,4.009);
    expected = "hello";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
	res = new Date(0).toString(1,2);
    expected = new Date(0).toString();;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }



    apEndTest();
}



string15();


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
