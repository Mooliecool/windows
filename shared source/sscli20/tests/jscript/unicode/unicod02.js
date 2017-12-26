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


var iTestID = 77540;

/* -------------------------------------------------------------------------
  Test: 	UNICOD02
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode values used in JScript variable identifiers
 
  Keywords:	unicode literal escape variable identifier eval
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Upper case letters (Lu)

		2.  Lower case letters (Ll)

		3.  Title case letters (Lt)

		4.  Modifier letters (Lm)

		5.  Other letters (Lo)

		6.  Number letters (Nl)

		7.  Space separators (Zs)

		8.  Non-spacing marks (Mn)

		9.  Combining spacing marks (Mc)

		10.  Unicode digits (Nd)

		11.  Connector punctuation (Pc)

		12.  Format control (Cf)


  Abstract:	 Unicode characters are legal for use in variable identifiers,
			 with the exception of spacing marks and format characters.
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

function DateCompare (d1, d2) {
	if (Math.abs(d2 - d1) > 1000) {
		return false;
	}
	else {
		return true;
	}
}

//////////
//
// Global variables




function unicod02() {

    apInitTest("unicod02: Unicode compliance -- Unicode values used in JScript variable identifiers."); 

	var bError = false;

	apInitScenario("1  Uppercase letters");
	var u\u004Dn01 = 3;
	verify (u\u004Dn01, 3, "1. Wrong identifier value");


	apInitScenario("2  Lowercase letters");
	var u\u006Dn01 = 4;
	verify (u\u006Dn01, 4, "2. Wrong identifier value");


	apInitScenario("3  Titlecase letters");
	var u\u01C5n01 = 5;
	verify (u\u01C5n01, 5, "3. Wrong identifier value");


	apInitScenario("4  Modifier letters");
	bError = false;
	var u\u02B0n01 = 6;
	verify (u\u02B0n01, 6, "4. Wrong identifier value");


	apInitScenario("5  Other letters");
	var u\u05D0n01 = 7;
	verify (u\u05D0n01, 7, "5. Wrong identifier value");


	apInitScenario("6  Number letters");
	var u\u2160n01 = 8;
	verify (u\u2160n01, 8, "6. Wrong identifier value");


	apInitScenario("7  Space separators");
	bError = false;
	try {
		eval("var u\u3000n01 = 9;");
	}
	catch (e) {
		verify (e.number, -2146827284, "Wrong error returned");
		bError = true;
	}
	if (bError == false) {
	}


	apInitScenario("8  Non-spacing marks");
	var u\u302An01 = 10;
	verify (u\u302An01, 10, "8. Wrong identifier value");


	apInitScenario("9  Combining spacing marks");
	var u\u0982n01 = 11;
	verify (u\u0982n01, 11, "9. Wrong identifier value");


	apInitScenario("10  Unicode digits");
	var u\u09E6n01 = 12;
	verify (u\u09E6n01, 12, "10. Wrong identifier value");


	apInitScenario("11  Connector punctuation");
	var u\u203Fn01 = 13;
	verify (u\u203Fn01, 13, "11. Wrong identifier value");


	apInitScenario("12  Format control");
	bError = false;
	try {
		eval("var u\u200Cn01 = 14;");
	}
	catch (e) {
		verify (e.number, -2146827274, "Wrong error returned");
		bError = true;
	}


	apEndTest();
}


unicod02();


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
