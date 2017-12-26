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


var iTestID = 53667;

var g_OnDBCS = false;
var g_lang = "";

function GetLang()
{
	g_lang = apGlobalObj.apGetLangExt(apGlobalObj.LangHost());
	if(g_lang == "JP" || g_lang == "KO" || g_lang == "CHS" || g_lang == "CHT")
		g_OnDBCS = true;
}

function FuncRetStr() {
  return "FuNcTiOn rEtUrNiNg StRiNg";
}

function myObject() { this.toString = myObjStr; }
function myObjStr() { return "MY object STRing"; }

function ucase05() {
    var tmp,res,expected,emptyVar;

    apInitTest("ucase05");
    GetLang();

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 1969

    var numvar = 4321;
    var strvar = "Hippopotamus";

    var ary = new Array();
    ary[2] = "zERo";
    ary[3] = "alBATrOSs";
    ary[4] = "what FLAVOR is it?";

    var myObj = new myObject();

    var crlf = unescape("%0d%0a");
    var lf = unescape("%0a"); // LF
    @if (@_win16)
	var FRScode =lf+"FUNCTION FUNCRETSTR() {"+lf+"    [NATIVE CODE]"+lf+"}"+lf;
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
    var Mathcode = "[OBJECT MATH]";

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("call toUpperCase() with date (coerced to string)");
    res = ("" + myDate).toUpperCase();
    expected = "WED DEC 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with date.toString()");
    res = myDate.toString().toUpperCase();
    expected = "WED DEC 31 16:00:00 PST 1969";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    }

    apInitScenario("call toUpperCase() with function returning string");
    res = FuncRetStr().toUpperCase();
    expected = "FUNCTION RETURNING STRING";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    
    function FuncRetDBCSStr() { return "???|?¢F?¡L??"; }


	    apInitScenario("call toUpperCase() with function returning DBCS string");
    if(g_OnDBCS)
    {
	    res = FuncRetDBCSStr().toUpperCase();
	    expected = "???|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
	        apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }    

    apInitScenario("call toUpperCase() with null string");
    res = "".toUpperCase();
    expected = "";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with coerced numeric constant");
    res = ("" + 1334).toUpperCase();
    expected = "1334";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with string constant");
    res = "Bernoulli".toUpperCase();
    expected = "BERNOULLI";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
  

	    apInitScenario("call toUpperCase() with DBCS string constant");
    if(g_OnDBCS)
    {
	    res = "???|?¢F?¡L??".toUpperCase();
	    expected = "???|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
        	apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }

    apInitScenario("call toUpperCase() with string expression");
    res = ("AbC" + "dEf").toUpperCase();
    expected = "ABCDEF";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
	

	    apInitScenario("call toUpperCase() with DBCS string expression");
    if(g_OnDBCS)
    {
	    res = ("???|?¢F?¡L??" + "???|?¢F?¡L??").toUpperCase();
	    expected = "???|?¢F?¡L?????|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
        	apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }

    apInitScenario("call toUpperCase() with string variable");
    res = strvar.toUpperCase();
    expected = "HIPPOPOTAMUS";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }



	    apInitScenario("call toUpperCase() with DBCS string variable");
    if(g_OnDBCS)
    {
	    strvar = "???|?¢F?¡L??";
	    res = strvar.toUpperCase();
	    expected = "???|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
        	apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }

    apInitScenario("call toUpperCase() with coerced numeric variable");
    res = ("" + numvar).toUpperCase();
    expected = "4321";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with string array element");
    res = ary[3].toUpperCase();
    expected = "ALBATROSS";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }
    


	    apInitScenario("call toUpperCase() with DBCS string array element");
    if(g_OnDBCS)
    {
	    ary[3] = "???|?¢F?¡L?????|?¢F?¡L??";
	    res = ary[3].toUpperCase();
	    expected = "???|?¢F?¡L?????|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
        	apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }

    apInitScenario("call toUpperCase() with user-defined object member");
    res = myObj.toString().toUpperCase();
    expected = "MY OBJECT STRING";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with string.toString");
    res = "Horse".toString().toUpperCase();
    expected = "HORSE";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }


	    apInitScenario("call toUpperCase() with (DBCS) string.toString");
    if(g_OnDBCS)
    {
	    res = "???|?¢F?¡L??".toString().toUpperCase();
	    expected = "???|?¢F?¡L??";
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "string") {
        	apLogFailInfo("result had wrong type","string",typeof(res),"");
	    }
    }

    apInitScenario("call toUpperCase() with Math.toString");
    res = Math.toString().toUpperCase();
    expected = Mathcode.toUpperCase();
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apInitScenario("call toUpperCase() with eval string");
    res = eval("\"bOINGo\"").toUpperCase();
    expected = "BOINGO";
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "string") {
        apLogFailInfo("result had wrong type","string",typeof(res),"");
    }

    apEndTest();
}


ucase05();


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
