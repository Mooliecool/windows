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


var iTestID = 77545;

/* -------------------------------------------------------------------------
  Test: 	UNICOD07
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode escape sequences in comments
 
  Keywords:	unicode literal escape eval comment
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Single escape sequence

		2.  Multiple escape sequences

		3.  Multiple escape sequences separated by characters

		4.  Newline escape sequences (\u000A, \u000D, \u2028, \u2029)



  Abstract:	 Testing that Unicode escape sequences in comments are not evaluated.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp+" ", sAct, "");
}



//////////
//
// Global variables

var str1;


function unicod07() {

    apInitTest("unicod07: Unicode compliance -- Unicode escape sequences in comments"); 

	var rExp = new RegExp();

	apInitScenario("1.  Single escape sequence");
	function abc() { /* \u2B0 */ }
	verify (escape(abc), "function%20abc%28%29%20%7B%20/*%20%5Cu2B0%20*/%20%7D", "1.1 Wrong string length");
	str1 = abc.toString();
	verify (str1.length, 30, "1.2 Wrong result returned");
	verify (str1.charCodeAt(20), 92, "1.3 Wrong result returned");
	verify (str1.charCodeAt(21), 117, "1.4 Wrong result returned");
	verify (str1.charCodeAt(22), 50, "1.5 Wrong result returned");
	verify (str1.charCodeAt(23), 66, "1.6 Wrong result returned");
	verify (str1.charCodeAt(24), 48, "1.7 Wrong result returned");


	apInitScenario("2.  Multiple escape sequences");
	function abcd() { /* \u2B0\u3000 */ }
	verify (escape(abcd), "function%20abcd%28%29%20%7B%20/*%20%5Cu2B0%5Cu3000%20*/%20%7D", "2.1 Wrong string length");
	str1 = abcd.toString();
	verify (str1.length, 37, "2.2 Wrong result returned");
	verify (str1.charCodeAt(21), 92, "2.3 Wrong result returned");
	verify (str1.charCodeAt(22), 117, "2.4 Wrong result returned");
	verify (str1.charCodeAt(23), 50, "2.5 Wrong result returned");
	verify (str1.charCodeAt(24), 66, "2.6 Wrong result returned");
	verify (str1.charCodeAt(25), 48, "2.7 Wrong result returned");
	verify (str1.charCodeAt(26), 92, "2.8 Wrong result returned");
	verify (str1.charCodeAt(27), 117, "2.9 Wrong result returned");
	verify (str1.charCodeAt(28), 51, "2.10 Wrong result returned");
	verify (str1.charCodeAt(29), 48, "2.11 Wrong result returned");
	verify (str1.charCodeAt(30), 48, "2.12 Wrong result returned");
	verify (str1.charCodeAt(31), 48, "2.13 Wrong result returned");
	verify (str1.charCodeAt(32), 32, "2.14 Wrong result returned");


	apInitScenario("3.  Multiple escape sequences separated by characters");
	function abcde() { /* \u2B0test\u3000 */ }
	verify (escape(abcde), "function%20abcde%28%29%20%7B%20/*%20%5Cu2B0test%5Cu3000%20*/%20%7D", "3.1 Wrong string length");
	str1 = abcde.toString();
	verify (str1.length, 42, "3.2 Wrong result returned");
	verify (str1.charCodeAt(22), 92, "3.3 Wrong result returned");
	verify (str1.charCodeAt(23), 117, "3.4 Wrong result returned");
	verify (str1.charCodeAt(24), 50, "3.5 Wrong result returned");
	verify (str1.charCodeAt(25), 66, "3.6 Wrong result returned");
	verify (str1.charCodeAt(26), 48, "3.7 Wrong result returned");
	verify (str1.charCodeAt(27), 116, "3.8 Wrong result returned");
	verify (str1.charCodeAt(28), 101, "3.9 Wrong result returned");
	verify (str1.charCodeAt(29), 115, "3.10 Wrong result returned");
	verify (str1.charCodeAt(30), 116, "3.11 Wrong result returned");
	verify (str1.charCodeAt(31), 92, "3.12 Wrong result returned");
	verify (str1.charCodeAt(32), 117, "3.13 Wrong result returned");
	verify (str1.charCodeAt(33), 51, "3.14 Wrong result returned");
	verify (str1.charCodeAt(34), 48, "3.15 Wrong result returned");
	verify (str1.charCodeAt(35), 48, "3.16 Wrong result returned");
	verify (str1.charCodeAt(36), 48, "3.17 Wrong result returned");
	verify (str1.charCodeAt(37), 32, "3.18 Wrong result returned");


	apInitScenario("4.  Newline escape sequence 000A");
	function abcde1() { /* \u000A */ }
	verify (escape(abcde1), "function%20abcde1%28%29%20%7B%20/*%20%5Cu000A%20*/%20%7D", "4.1 Wrong string length");
	str1 = abcde1.toString();
	verify (str1.length, 34, "4.2 Wrong result returned");
	verify (str1.charCodeAt(23), 92, "3.3 Wrong result returned");
	verify (str1.charCodeAt(24), 117, "3.4 Wrong result returned");
	verify (str1.charCodeAt(25), 48, "3.5 Wrong result returned");
	verify (str1.charCodeAt(26), 48, "3.6 Wrong result returned");
	verify (str1.charCodeAt(27), 48, "3.7 Wrong result returned");
	verify (str1.charCodeAt(28), 65, "3.8 Wrong result returned");
	verify (str1.charCodeAt(29), 32, "3.9 Wrong result returned");


	apInitScenario("5.  Newline escape sequence 000D");
	function abcde2() { /* \u000D */ }
	verify (escape(abcde2), "function%20abcde2%28%29%20%7B%20/*%20%5Cu000D%20*/%20%7D", "5.1 Wrong string length");
	str1 = abcde2.toString();
	verify (str1.length, 34, "5.2 Wrong result returned");
	verify (str1.charCodeAt(23), 92, "5.3 Wrong result returned");
	verify (str1.charCodeAt(24), 117, "5.4 Wrong result returned");
	verify (str1.charCodeAt(25), 48, "5.5 Wrong result returned");
	verify (str1.charCodeAt(26), 48, "5.6 Wrong result returned");
	verify (str1.charCodeAt(27), 48, "5.7 Wrong result returned");
	verify (str1.charCodeAt(28), 68, "5.8 Wrong result returned");
	verify (str1.charCodeAt(29), 32, "5.9 Wrong result returned");


	apInitScenario("6.  Newline escape sequence 2028");
	function abcde3() { /* \u2028 */ }
	verify (escape(abcde3), "function%20abcde3%28%29%20%7B%20/*%20%5Cu2028%20*/%20%7D", "6.1 Wrong string length");
	str1 = abcde3.toString();
	verify (str1.length, 34, "6.2 Wrong result returned");
	verify (str1.charCodeAt(23), 92, "6.3 Wrong result returned");
	verify (str1.charCodeAt(24), 117, "6.4 Wrong result returned");
	verify (str1.charCodeAt(25), 50, "6.5 Wrong result returned");
	verify (str1.charCodeAt(26), 48, "6.6 Wrong result returned");
	verify (str1.charCodeAt(27), 50, "6.7 Wrong result returned");
	verify (str1.charCodeAt(28), 56, "6.8 Wrong result returned");
	verify (str1.charCodeAt(29), 32, "6.9 Wrong result returned");


	apInitScenario("7.  Newline escape sequence 2029");
	function abcde4() { /* \u2029 */ }
	verify (escape(abcde4), "function%20abcde4%28%29%20%7B%20/*%20%5Cu2029%20*/%20%7D", "7.1 Wrong string length");
	str1 = abcde4.toString();
	verify (str1.length, 34, "7.2 Wrong result returned");
	verify (str1.charCodeAt(23), 92, "7.3 Wrong result returned");
	verify (str1.charCodeAt(24), 117, "7.4 Wrong result returned");
	verify (str1.charCodeAt(25), 50, "7.5 Wrong result returned");
	verify (str1.charCodeAt(26), 48, "7.6 Wrong result returned");
	verify (str1.charCodeAt(27), 50, "7.7 Wrong result returned");
	verify (str1.charCodeAt(28), 57, "7.8 Wrong result returned");
	verify (str1.charCodeAt(29), 32, "7.9 Wrong result returned");


	apEndTest();
}

unicod07();


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
