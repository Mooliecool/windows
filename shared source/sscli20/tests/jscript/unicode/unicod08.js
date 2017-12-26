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


var iTestID = 77546;

/* -------------------------------------------------------------------------
  Test: 	UNICOD08
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode escape sequences in URI strings
 
  Keywords:	unicode literal escape eval comment
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Single escape sequence

		2.  Multiple escape sequences

		3.  Multiple escape sequences separated by characters

		4-7. Newline escape sequences (\u000A, \u000D, \u2028, \u2029)



  Abstract:	 Testing that Unicode escape sequences in URIs are handled correctly.
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


function unicod08() {

    apInitTest("unicod08: Unicode compliance -- Unicode escape sequences in URI strings"); 

	apInitScenario("1.  Single escape sequence");
	str1 = "http://www.microsoft.com/testing.asp?eval=\u02B0";
	verify (encodeURI(str1), "http:
	verify (str1.length, 43, "1.2 Wrong result returned");


	apInitScenario("2.  Multiple escape sequences");
	str1 = "http://www.microsoft.com/testing.asp?eval=\u3000\u02B0";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "2.2 Wrong result returned");


	apInitScenario("3.  Multiple escape sequences separated by characters");
	str1 = "http://www.microsoft.com/testing.asp?eval\u3000=\u02B0";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "3.2 Wrong result returned");


	apInitScenario("4.  Newline escape sequence u000A");
	str1 = "http://www.microsoft.com/testing.asp?eval\u000A=1";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "4.2 Wrong result returned");


	apInitScenario("5.  Newline escape sequence u000D");
	str1 = "http://www.microsoft.com/testing.asp?eval\u000D=1";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "5.2 Wrong result returned");


	apInitScenario("6.  Newline escape sequence  u2028");
	str1 = "http://www.microsoft.com/testing.asp?eval\u2028=1";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "6.2 Wrong result returned");


	apInitScenario("7.  Newline escape sequence u2029");
	str1 = "http://www.microsoft.com/testing.asp?eval\u2029=1";
	verify (encodeURI(str1), "http:
	verify (str1.length, 44, "7.2 Wrong result returned");


	apEndTest();
}

unicod08();


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
