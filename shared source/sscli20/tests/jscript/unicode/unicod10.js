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


var iTestID = 77548;

/* -------------------------------------------------------------------------
  Test: 	UNICOD10
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode escape sequences in string comparison
 
  Keywords:	unicode literal escape eval string comparison compare
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Arrays of single character strings

		2.  Arrays of multiple character strings of same Unicode characters

		3.  Arrays of multiple character strings of many different Unicode characters


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

var arr1;
var str1;

function unicod10() {

    apInitTest("unicod10: Unicode compliance -- Sorting"); 

	apInitScenario("1.  Arrays of single character strings");
	arr1 = new Array();
	arr1[0] = "\u02B0";
	arr1[1] = "\u02B0";
	arr1[2] = "\u02AF";
	arr1[3] = "\u02B1";
	arr1.sort();
	str1 = arr1.join();
	verify (escape(arr1.join()), "%u02AF%2C%u02B0%2C%u02B0%2C%u02B1", "1.1 Wrong result returned");
	verify (str1.length, 7, "1.2 Wrong string length returned");
	verify (str1.charCodeAt(0), 0x02AF, "1.3 Wrong string length returned");
	verify (str1.charCodeAt(2), 0x02B0, "1.4 Wrong string length returned");
	verify (str1.charCodeAt(4), 0x02B0, "1.5 Wrong string length returned");
	verify (str1.charCodeAt(6), 0x02B1, "1.6 Wrong string length returned");


	apInitScenario("2.  Arrays of multiple character strings of same Unicode characters");
	arr1 = new Array();
	arr1[0] = "\u02B0\u02B0\u02B0\u02B0";
	arr1[1] = "\u02B0\u02B0\u02B0\u02B0";
	arr1[2] = "\u02B0\u02B0\u02B0\u02AF";
	arr1[3] = "\u02B0\u02B0\u02B0\u02B1";
	arr1.sort();
	str1 = arr1.join();
	verify (escape(arr1.join()), "%u02B0%u02B0%u02B0%u02AF%2C%u02B0%u02B0%u02B0%u02B0%2C%u02B0%u02B0%u02B0%u02B0%2C%u02B0%u02B0%u02B0%u02B1", "2.1 Wrong result returned");
	verify (str1.length, 19, "2.2 Wrong string length returned");
	verify (str1.charCodeAt(3), 0x02AF, "2.3 Wrong string length returned");
	verify (str1.charCodeAt(8), 0x02B0, "2.4 Wrong string length returned");
	verify (str1.charCodeAt(13), 0x02B0, "2.5 Wrong string length returned");
	verify (str1.charCodeAt(18), 0x02B1, "2.6 Wrong string length returned");


	apInitScenario("3.  Arrays of multiple character strings of many different Unicode characters");
	arr1 = new Array();
	arr1[0] = "\u02B0\u3000\u02B0\u2000";
	arr1[1] = "\u02B0\u3000\u02B0\u2000";
	arr1[2] = "\u02B0\u3000\u02AF\u2000";
	arr1[3] = "\u02B0\u3000\u02B1\u2000";
	arr1.sort();
	str1 = arr1.join();
	verify (escape(arr1.join()), "%u02B0%u3000%u02AF%u2000%2C%u02B0%u3000%u02B0%u2000%2C%u02B0%u3000%u02B0%u2000%2C%u02B0%u3000%u02B1%u2000", "3.1 Wrong result returned");
	verify (str1.length, 19, "3.2 Wrong string length returned");
	verify (str1.charCodeAt(2), 0x02AF, "3.3 Wrong string length returned");
	verify (str1.charCodeAt(7), 0x02B0, "3.4 Wrong string length returned");
	verify (str1.charCodeAt(12), 0x02B0, "3.5 Wrong string length returned");
	verify (str1.charCodeAt(17), 0x02B1, "3.6 Wrong string length returned");


	apEndTest();
}

unicod10();


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
