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


var iTestID = 77547;

/* -------------------------------------------------------------------------
  Test: 	UNICOD09
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode escape sequences in string comparison
 
  Keywords:	unicode literal escape eval string comparison compare
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Single character strings

		2.  Multiple character strings of same Unicode characters

		3.  Multiple character strings of many different Unicode characters



  Abstract:	 Testing that Unicode escape sequences are compared correctly.
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
var str2;
var str3;
var str4;

function unicod09() {

    apInitTest("unicod09: Unicode compliance -- String comparison"); 

	apInitScenario("1.  Single character strings");
	str1 = "\u02B0";
	str2 = "\u02B0";
	str3 = "\u02AF";
	str4 = "\u02B1";
	verify ((str1 < str2), false, "1.1 Wrong result returned");
	verify ((str1 == str2), true, "1.2 Wrong result returned");
	verify ((str1 > str2), false, "1.3 Wrong result returned");
	verify ((str1 > str3), true, "1.4 Wrong result returned");
	verify ((str1 < str4), true, "1.5 Wrong result returned");


	apInitScenario("2.  Multiple character strings of same Unicode characters");
	str1 = "\u02B0\u02B0\u02B0\u02B0";
	str2 = "\u02B0\u02B0\u02B0\u02B0";
	str3 = "\u02B0\u02B0\u02B0\u02AF";
	str4 = "\u02B0\u02B0\u02B0\u02B1";
	verify ((str1 < str2), false, "2.1 Wrong result returned");
	verify ((str1 == str2), true, "2.2 Wrong result returned");
	verify ((str1 > str2), false, "2.3 Wrong result returned");
	verify ((str1 > str3), true, "2.4 Wrong result returned");
	verify ((str1 < str4), true, "2.5 Wrong result returned");


	apInitScenario("3.  Multiple character strings of many different Unicode characters");
	str1 = "\u02B0\u3000\u3000\u02B0\u2000";
	str2 = "\u02B0\u3000\u3000\u02B0\u2000";
	str3 = "\u02B0\u3000\u3000\u02AF\u2000";
	str4 = "\u02B0\u3000\u3000\u02B1\u2000";
	verify ((str1 < str2), false, "3.1 Wrong result returned");
	verify ((str1 == str2), true, "3.2 Wrong result returned");
	verify ((str1 > str2), false, "3.3 Wrong result returned");
	verify ((str1 > str3), true, "3.4 Wrong result returned");
	verify ((str1 < str4), true, "3.5 Wrong result returned");


	apEndTest();
}

unicod09();


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
