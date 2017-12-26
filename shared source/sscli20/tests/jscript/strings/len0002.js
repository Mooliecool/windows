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


var iTestID = 52895;

var g_OnDBCS = false;
var g_lang;
 
function FuncRetStr() {
  return "FuNcTiOn rEtUrNiNg StRiNg";
}
function FuncRetStrDBCS()
{
	return "‚ ‚¦‚¢‚¨‚¤";
}

function myObject() { this.toString = myObjStr; }
function myObjStr() { return "MY object STRing"; }

function GetLang()
{
	g_lang = apGlobalObj.apGetLangExt(apGlobalObj.LangHost());
	if(g_lang == "JP" || g_lang == "KO" || g_lang == "CHS" || g_lang == "CHT")
		g_OnDBCS = true;
}

function len0002() {
    var tmp,res,expected,expected2,emptyVar;

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 1969

    var numvar = 4321;
    var strvar = "Hippopotamus";

    var ary = new Array();
    ary[2] = "zERo";
    ary[3] = "alBATrOSs";
    ary[4] = "what FLAVOR is it?";

    var myObj = new myObject();
    var myStr = new String("Apple Core");

    var crlf = unescape("%0d%0a"); 
    var FRScode = "function FuncRetStr() {" + crlf + '  return "FuNcTiOn rEtUrNiNg StRiNg";' + crlf +"}";
    apInitTest("len0002");
    GetLang();

    apInitScenario("evaluate .length with string object");
    res = myStr.length;
    expected = 10;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }


        apInitScenario("evaluate .length with string object using DBCS String");
    if(g_OnDBCS)
    {
	var myDBCSStr = new String("‚ ‚¦‚¢‚¨‚¤");
        res = myDBCSStr.length;
 	expected = 5;
 	if(res != expected)
		apLogFailInfo("wrong result", expected, res, "");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("evaluate .length with date (coerced to string)");
    res = ("" + myDate).length;
    expected = 28;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
    
    apInitScenario("evaluate .length with date.toString()");
    res = myDate.toString().length;
    expected = 28;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
    }

    apInitScenario("evaluate .length with function returning string");
    res = FuncRetStr().length;
    expected = 25;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
 
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {

	    apInitScenario("evaluate .length with function returning a DBCS String");
    if(g_OnDBCS)
    {
	    res = FuncRetStrDBCS().length;
	    expected = 5;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
    	}
   } 
   }
    apInitScenario("evaluate .length with null string");
    res = "".length;
    expected = 0;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }

    apInitScenario("evaluate .length with coerced numeric constant");
    res = ("" + 1334).length;
    expected = 4;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
    

	    apInitScenario("evaluate .length with coerced numeric constant using a DBCS String");
    if(g_OnDBCS)
    {
	    res = ("‚ ‚¦‚¢‚¨‚¤" + 1334).length;
	    expected = 9;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
	    }
    }

    apInitScenario("evaluate .length with string constant");
    res = "Bernoulli".length;
    expected = 9;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
	

	apInitScenario("evaluate .length with DBCS String constant");
   if(g_OnDBCS)
    {
	    res = "‚ ‚¦‚¢‚¨‚¤".length;
	    expected = 5;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
	    }
    }

    apInitScenario("evaluate .length with string expression");
    res = ("AbC" + "dEf").length;
    expected = 6;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrng type","number",typeof(res),"");
    }
	

	    apInitScenario("evaluate .length with DBCS string expression");
    if(g_OnDBCS)
    {
	    res = ("‚ ‚¦‚¢‚¨‚¤" + "‚ ‚¦‚¢‚¨‚¤").length;
	    expected = 10;
	    if (res != expected) {
	        apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrng type","number",typeof(res),"");
	    }
    }

    apInitScenario("evaluate .length with string variable");
    res = strvar.length;
    expected = 12;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
	

	    apInitScenario("evaluate .length with DBCS string variable");
    if(g_OnDBCS)
    {
 	    var dbcsvar = "‚ ‚¦‚¢‚¨‚¤";
	    res = dbcsvar.length;
	    expected = 5;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
	    }
    }

    apInitScenario("evaluate .length with coerced numeric variable");
    res = ("" + numvar).length;
    expected = 4;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }


	    apInitScenario("evaluate .length with coerced numeric variable");
    if(g_OnDBCS)
    {
	    res = ("‚ ‚¦‚¢‚¨‚¤" + numvar).length;
	    expected = 9;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
	    }  
    }

    apInitScenario("evaluate .length with string array element");
    res = ary[3].length;
    expected = 9;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
	


	    apInitScenario("evaluate .length with DBCS string array element");
    if(g_OnDBCS)
    {
 	    var dbcsarr = new Array();
 	    dbcsarr[0] = "‚ ‚¦‚¢‚¨‚¤";   
	    res = dbcsarr[0].length;
	    expected = 5;
	    if (res != expected) {
        	apLogFailInfo("wrong result",expected,res,"");
	    }
	    if (typeof(res) != "number") {
		apLogFailInfo("result had wrong type","number",typeof(res),"");
	    }
    }

    apInitScenario("evaluate .length with user-defined object member");
    res = myObj.toString().length;
    expected = 16;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }

    apInitScenario("evaluate .length with string.toString");
    res = "Horse".toString().length;
    expected = 5;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }
    
    apInitScenario("evaluate .length with udf.toString");
    res = FuncRetStr.toString().length;
    @if ( @_win16 )
    expected = 45;
    expected2 = 45;
    @else
    expected = FRScode.length;
    expected2 = FRScode.length-2;
    @end
    if (res != expected) {
		if (res != expected2)
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }

    apInitScenario("evaluate .length with Math.toString");
    res = Math.toString().length;
    expected = 13;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }

    apInitScenario("evaluate .length with eval string");
    res = eval("\"bOINGo\"").length;
    expected = 6;
    if (res != expected) {
        apLogFailInfo("wrong result",expected,res,"");
    }
    if (typeof(res) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(res),"");
    }

    apEndTest();
}


len0002();


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
