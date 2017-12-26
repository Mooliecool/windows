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


var iTestID = 77543;

/* -------------------------------------------------------------------------
  Test: 	UNICOD05
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	String literals containing Unicode escape sequences
 
  Keywords:	unicode literal escape eval string literals
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  String containing single escape sequence

		2.  String containing multiple escape sequences

		3.  Single esc sequence at beginning of multi-character string

		4.  Single esc sequence with following space at beginning of multi-character string

		5.  Single esc sequence at end of multi-character string

		6.  Single esc sequence with preceding space at end of multi-character string

		7.  Single esc sequence in the middle of a multi-character string

		8.  Single esc sequence with separating spaces in the middle of a multi-char string

		9.  Multiple adjacent esc sequences at the beginning of a multi-character string

		10. Multiple adjacent esc sequences at the end of a multi-character string

		11. Multiple adjacent esc sequences in the middle of a multi-character string

		12. Multiple non-adjacent esc sequences


  Abstract:	 Testing that Unicode escape sequences in string literals are evaluated
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




function unicod05() {

    apInitTest("unicod05: Unicode compliance -- String literals containing Unicode escape sequences"); 

	var str1;

	apInitScenario("1.  String containing single escape sequence");
	str1 = "\u02B0";
	verify (typeof(str1), "string", "1.1 Wrong data type");
	verify (str1.length, 1, "1.2 Wrong string length");
	verify (str1.charCodeAt(0), 0x02B0, "1.3 Wrong character code returned");


	apInitScenario("2.  String containing multiple escape sequences");
	str1 = "\u02B0\u3000\u2000";
	verify (typeof(str1), "string", "2.1 Wrong data type");
	verify (str1.length, 3, "2.2 Wrong string length");
	verify (str1.charCodeAt(0), 0x02B0, "2.3 Wrong character code returned");
	verify (str1.charCodeAt(1), 0x3000, "2.4 Wrong character code returned");
	verify (str1.charCodeAt(2), 0x2000, "2.5 Wrong character code returned");


	apInitScenario("3.  Single esc sequence at beginning of multi-character string");
	str1 = "\u02B0My dog has fleas.";
	verify (typeof(str1), "string", "3.1 Wrong data type");
	verify (str1.length, 18, "3.2 Wrong string length");
	verify (str1.charCodeAt(0), 0x02B0, "3.3 Wrong character code returned");
	verify (str1.charCodeAt(1), 77, "3.4 Wrong character code returned");
	verify (str1.charCodeAt(2), 121, "3.5 Wrong character code returned");


	apInitScenario("4.  Single esc sequence with following space at beginning of multi-character string");
	str1 = "\u02B0 My dog has fleas.";
	verify (typeof(str1), "string", "4.1 Wrong data type");
	verify (str1.length, 19, "4.2 Wrong string length");
	verify (str1.charCodeAt(0), 0x02B0, "4.3 Wrong character code returned");
	verify (str1.charCodeAt(1), 32, "4.4 Wrong character code returned");
	verify (str1.charCodeAt(2), 77, "4.5 Wrong character code returned");


	apInitScenario("5.  Single esc sequence at end of multi-character string");
	str1 = "My dog has fleas.\u02B0";
	verify (typeof(str1), "string", "5.1 Wrong data type");
	verify (str1.length, 18, "5.2 Wrong string length");
	verify (str1.charCodeAt(17), 0x02B0, "5.3 Wrong character code returned");
	verify (str1.charCodeAt(16), 46, "5.4 Wrong character code returned");
	verify (str1.charCodeAt(0), 77, "5.5 Wrong character code returned");


	apInitScenario("6.  Single esc sequence with preceding space at end of multi-character string");
	str1 = "My dog has fleas. \u02B0";
	verify (typeof(str1), "string", "6.1 Wrong data type");
	verify (str1.length, 19, "6.2 Wrong string length");
	verify (str1.charCodeAt(18), 0x02B0, "6.3 Wrong character code returned");
	verify (str1.charCodeAt(17), 32, "6.4 Wrong character code returned");
	verify (str1.charCodeAt(16), 46, "6.5 Wrong character code returned");
	verify (str1.charCodeAt(0), 77, "6.6 Wrong character code returned");


	apInitScenario("7.  Single esc sequence in the middle of a multi-character string");
	str1 = "My dog h\u02B0as fleas.";
	verify (typeof(str1), "string", "7.1 Wrong data type");
	verify (str1.length, 18, "7.2 Wrong string length");
	verify (str1.charCodeAt(7), 104, "7.3 Wrong character code returned");
	verify (str1.charCodeAt(8), 0x02B0, "7.4 Wrong character code returned");
	verify (str1.charCodeAt(9), 97, "7.5 Wrong character code returned");


	apInitScenario("8.  Single esc sequence with separating spaces in the middle of a multi-char string");
	str1 = "My dog h \u02B0 as fleas.";
	verify (typeof(str1), "string", "8.1 Wrong data type");
	verify (str1.length, 20, "8.2 Wrong string length");
	verify (str1.charCodeAt(7), 104, "8.3 Wrong character code returned");
	verify (str1.charCodeAt(8), 32, "8.4 Wrong character code returned");
	verify (str1.charCodeAt(9), 0x02B0, "8.5 Wrong character code returned");
	verify (str1.charCodeAt(10), 32, "8.6 Wrong character code returned");
	verify (str1.charCodeAt(11), 97, "8.7 Wrong character code returned");


	apInitScenario("9.  Multiple adjacent esc sequences at the beginning of a multi-character string");
	str1 = "\u02B0\u3000\u2000My dog has fleas.";
	verify (typeof(str1), "string", "9.1 Wrong data type");
	verify (str1.length, 20, "9.2 Wrong string length");
	verify (str1.charCodeAt(0), 0x02B0, "9.3 Wrong character code returned");
	verify (str1.charCodeAt(1), 0x3000, "9.4 Wrong character code returned");
	verify (str1.charCodeAt(2), 0x2000, "9.5 Wrong character code returned");
	verify (str1.charCodeAt(3), 77, "9.6 Wrong character code returned");
	verify (str1.charCodeAt(4), 121, "9.7 Wrong character code returned");


	apInitScenario("10. Multiple adjacent esc sequences at the end of a multi-character string");
	str1 = "My dog has fleas.\u02B0\u3000\u2000";
	verify (typeof(str1), "string", "10.1 Wrong data type");
	verify (str1.length, 20, "10.2 Wrong string length");
	verify (str1.charCodeAt(15), 115, "10.3 Wrong character code returned");
	verify (str1.charCodeAt(16), 46, "10.4 Wrong character code returned");
	verify (str1.charCodeAt(17), 0x02B0, "10.5 Wrong character code returned");
	verify (str1.charCodeAt(18), 0x3000, "10.6 Wrong character code returned");
	verify (str1.charCodeAt(19), 0x2000, "10.7 Wrong character code returned");


	apInitScenario("11. Multiple adjacent esc sequences in the middle of a multi-character string");
	str1 = "My dog h\u02B0\u3000\u2000as fleas.";
	verify (typeof(str1), "string", "11.1 Wrong data type");
	verify (str1.length, 20, "11.2 Wrong string length");
	verify (str1.charCodeAt(7), 104, "11.3 Wrong character code returned");
	verify (str1.charCodeAt(8), 0x02B0, "11.4 Wrong character code returned");
	verify (str1.charCodeAt(9), 0x3000, "11.5 Wrong character code returned");
	verify (str1.charCodeAt(10), 0x2000, "11.6 Wrong character code returned");
	verify (str1.charCodeAt(11), 97, "11.7 Wrong character code returned");


	apInitScenario("12. Multiple non-adjacent esc sequences");
	str1 = "My dog \u02B0h\u3000a\u2000s fleas.";
	verify (typeof(str1), "string", "12.1 Wrong data type");
	verify (str1.length, 20, "12.2 Wrong string length");
	verify (str1.charCodeAt(7), 0x02B0, "12.3 Wrong character code returned");
	verify (str1.charCodeAt(8), 104, "12.4 Wrong character code returned");
	verify (str1.charCodeAt(9), 0x3000, "12.5 Wrong character code returned");
	verify (str1.charCodeAt(10), 97, "12.6 Wrong character code returned");
	verify (str1.charCodeAt(11), 0x2000, "12.7 Wrong character code returned");


	apEndTest();
}

unicod05();


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
