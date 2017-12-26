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


var iTestID = 77544;

/* -------------------------------------------------------------------------
  Test: 	UNICOD06
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Regular expressions containing Unicode escape sequences
 
  Keywords:	unicode literal escape eval regular expression regexp
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Regexp containing single escape sequence

		2.  Regexp containing multiple escape sequences

		3.  Single esc sequence at beginning of multi-character regexp

		4.  Single esc sequence with following space at beginning of multi-character regexp

		5.  Single esc sequence at end of multi-character regexp

		6.  Single esc sequence with preceding space at end of multi-character regexp

		7.  Single esc sequence in the middle of a multi-character regexp

		8.  Single esc sequence with separating spaces in the middle of a multi-char regexp

		9.  Multiple adjacent esc sequences at the beginning of a multi-character regexp

		10. Multiple adjacent esc sequences at the end of a multi-character regexp

		11. Multiple adjacent esc sequences in the middle of a multi-character regexp

		12. Multiple non-adjacent esc sequences


  Abstract:	 Testing that Unicode escape sequences in regular expressions are evaluated
			 correctly.
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




function unicod06() {

    apInitTest("unicod06: Unicode compliance -- Regular expressions containing Unicode escape sequences"); 

	var rExp = new RegExp();

	apInitScenario("1.  Regexp containing single escape sequence");
	rExp = /\u02B0/;
	verify (escape(rExp.source), "%5Cu02B0", "1.1 Wrong string length");
	verify (rExp.test("\u02B0"), true, "1.2 Wrong result returned");


	apInitScenario("2.  Regexp containing multiple escape sequences");
	rExp = /\u02B0\u3000/;
	verify (escape(rExp.source), "%5Cu02B0%5Cu3000", "2.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "2.2 Wrong result returned");
	verify (rExp.test("\u3000"), false, "2.3 Wrong result returned");
	verify (rExp.test("\u02B0\u3000"), true, "2.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "2.5 Wrong result returned");


	apInitScenario("3.  Single esc sequence at beginning of multi-character regexp");
	rExp = /\u02B0abc/;
	verify (escape(rExp.source), "%5Cu02B0abc", "3.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "3.2 Wrong result returned");
	verify (rExp.test("\u02B0abc"), true, "3.3 Wrong result returned");
	verify (rExp.test("abc\u02B0abcdef"), true, "3.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "3.5 Wrong result returned");


	apInitScenario("4.  Single esc sequence with following space at beginning of multi-character regexp");
	rExp = /\u02B0 abc/;
	verify (escape(rExp.source), "%5Cu02B0%20abc", "4.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "4.2 Wrong result returned");
	verify (rExp.test("\u02B0 abc"), true, "4.3 Wrong result returned");
	verify (rExp.test("abc\u02B0 abcdef"), true, "4.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0 "), false, "4.5 Wrong result returned");


	apInitScenario("5.  Single esc sequence at end of multi-character regexp");
	rExp = /abc\u02B0/;
	verify (escape(rExp.source), "abc%5Cu02B0", "5.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "5.2 Wrong result returned");
	verify (rExp.test("abc\u02B0 abc"), true, "5.3 Wrong result returned");
	verify (rExp.test("abc\u02B0 abcdef"), true, "5.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0 "), false, "5.5 Wrong result returned");


	apInitScenario("6.  Single esc sequence with preceding space at end of multi-character regexp");
	rExp = /abc \u02B0/;
	verify (escape(rExp.source), "abc%20%5Cu02B0", "6.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "6.2 Wrong result returned");
	verify (rExp.test("abc \u02B0 abc"), true, "6.3 Wrong result returned");
	verify (rExp.test("abc \u02B0 abcdef"), true, "6.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0 "), false, "6.5 Wrong result returned");


	apInitScenario("7.  Single esc sequence in the middle of a multi-character regexp");
	rExp = /abc\u02B0wxyz/;
	verify (escape(rExp.source), "abc%5Cu02B0wxyz", "7.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "7.2 Wrong result returned");
	verify (rExp.test("abc\u02B0wxyz"), true, "7.3 Wrong result returned");
	verify (rExp.test("xyzabc\u02B0wxyzabcdef"), true, "7.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "7.5 Wrong result returned");


	apInitScenario("8.  Single esc sequence with separating spaces in the middle of a multi-char regexp");
	rExp = /abc \u02B0 wxyz/;
	verify (escape(rExp.source), "abc%20%5Cu02B0%20wxyz", "8.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "8.2 Wrong result returned");
	verify (rExp.test("abc \u02B0 wxyz"), true, "8.3 Wrong result returned");
	verify (rExp.test("xyzabc \u02B0 wxyzabcdef"), true, "8.4 Wrong result returned");
	verify (rExp.test("\u3000 \u02B0 "), false, "8.5 Wrong result returned");


	apInitScenario("9.  Multiple adjacent esc sequences at the beginning of a multi-character regexp");
	rExp = /\u02B0\u3000\u2000abc/;
	verify (escape(rExp.source), "%5Cu02B0%5Cu3000%5Cu2000abc", "9.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "9.2 Wrong result returned");
	verify (rExp.test("\u02B0\u3000\u2000abc"), true, "9.3 Wrong result returned");
	verify (rExp.test("xyzabc\u02B0\u3000\u2000abcwxyzabcdef"), true, "9.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "9.5 Wrong result returned");


	apInitScenario("10. Multiple adjacent esc sequences at the end of a multi-character regexp");
	rExp = /abc\u02B0\u3000\u2000/;
	verify (escape(rExp.source), "abc%5Cu02B0%5Cu3000%5Cu2000", "10.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "10.2 Wrong result returned");
	verify (rExp.test("abc\u02B0\u3000\u2000"), true, "10.3 Wrong result returned");
	verify (rExp.test("xyzabc\u02B0\u3000\u2000abcwxyzabcdef"), true, "10.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "10.5 Wrong result returned");


	apInitScenario("11. Multiple adjacent esc sequences in the middle of a multi-character regexp");
	rExp = /abc\u02B0\u3000\u2000wxyz/;
	verify (escape(rExp.source), "abc%5Cu02B0%5Cu3000%5Cu2000wxyz", "11.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "11.2 Wrong result returned");
	verify (rExp.test("abc\u02B0\u3000\u2000wxyz"), true, "11.3 Wrong result returned");
	verify (rExp.test("xyzabc\u02B0\u3000\u2000wxyzabcdef"), true, "11.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "11.5 Wrong result returned");


	apInitScenario("12. Multiple non-adjacent esc sequences");
	rExp = /ab\u02B0cd\u3000ef\u2000wxyz/;
	verify (escape(rExp.source), "ab%5Cu02B0cd%5Cu3000ef%5Cu2000wxyz", "12.1 Wrong string length");
	verify (rExp.test("\u02B0"), false, "12.2 Wrong result returned");
	verify (rExp.test("ab\u02B0cd\u3000ef\u2000wxyz"), true, "12.3 Wrong result returned");
	verify (rExp.test("xyzab\u02B0cd\u3000ef\u2000wxyzabcdef"), true, "12.4 Wrong result returned");
	verify (rExp.test("\u3000\u02B0"), false, "12.5 Wrong result returned");


	apEndTest();
}

unicod06();


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
