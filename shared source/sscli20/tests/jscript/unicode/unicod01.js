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


var iTestID = 77539;

/* -------------------------------------------------------------------------
  Test: 	UNICOD01
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode values used in JScript function identifiers
 
  Keywords:	unicode literal escape function identifier eval
 
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

  Abstract:	 Unicode characters are legal for use in function identifiers,
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




function unicod01() {

    apInitTest("unicod01: Unicode compliance -- Unicode values used in JScript function identifiers."); 

	var bError = false;

	apInitScenario("1  Uppercase letters");
	function u\u004Dn01() { return 3; }
	verify (escape(u\u004Dn01), "function%20u%5Cu004Dn01%28%29%20%7B%20return%203%3B%20%7D", "1. Wrong function value");


	apInitScenario("2  Lowercase letters");
	function u\u006Dn01() { return 3; }
	verify (escape(u\u006Dn01), "function%20u%5Cu006Dn01%28%29%20%7B%20return%203%3B%20%7D", "2. Wrong function value");


	apInitScenario("3  Titlecase letters");
	function u\u01C5n01() { return 3; }
	verify (escape(u\u01C5n01), "function%20u%5Cu01C5n01%28%29%20%7B%20return%203%3B%20%7D", "3. Wrong function value");


	apInitScenario("4  Modifier letters");
	bError = false;
	function u\u02B0n01() { return 3; }
	verify (escape(u\u02B0n01), "function%20u%5Cu02B0n01%28%29%20%7B%20return%203%3B%20%7D", "4. Wrong function value");

	apInitScenario("5  Other letters");
	function u\u05D0n01() { return 3; }
	verify (escape(u\u05D0n01), "function%20u%5Cu05D0n01%28%29%20%7B%20return%203%3B%20%7D", "5. Wrong function value");


	apInitScenario("6  Number letters");
	function u\u2160n01() { return 3; }
	verify (escape(u\u2160n01), "function%20u%5Cu2160n01%28%29%20%7B%20return%203%3B%20%7D", "6. Wrong function value");


	apInitScenario("7  Space separators");
	bError = false;
	try {
		eval("function u\u3000n01() { return 3; }");
	}
	catch (e) {
		// -2146827283 is for legacy script, -2146823266 is for 7.0+
		if (e.number == -2146827283 || e.number == -2146823266) {
		}
		else {
			verify (e.number, -2146827283, "Wrong error returned");
			bError = true;
		}
	}
	if (bError == false) {
	}


	apInitScenario("8  Non-spacing marks");
	function u\u302An01() { return 3; }
	verify (escape(u\u302An01), "function%20u%5Cu302An01%28%29%20%7B%20return%203%3B%20%7D", "8. Wrong function value");


	apInitScenario("9  Combining spacing marks");
	function u\u0982n01() { return 3; }
	verify (escape(u\u0982n01), "function%20u%5Cu0982n01%28%29%20%7B%20return%203%3B%20%7D", "9. Wrong function value");


	apInitScenario("10  Unicode digits");
	function u\u09E6n01() { return 3; }
	verify (escape(u\u09E6n01), "function%20u%5Cu09E6n01%28%29%20%7B%20return%203%3B%20%7D", "10. Wrong function value");


	apInitScenario("11  Connector punctuation");
	function u\u203Fn01() { return 3; }
	verify (escape(u\u203Fn01), "function%20u%5Cu203Fn01%28%29%20%7B%20return%203%3B%20%7D", "11. Wrong function value");


	apEndTest();
}

unicod01();


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
