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


var iTestID = 160214;

/* -------------------------------------------------------------------------
  Test: 	TYPANN14
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var re : RegExp;

		2.  var re : RegExp = /abc/gi;

		3.  var re : RegExp = new RegExp('', 'gi');

		4.  var re : RegExp = new RegExp('def');

		5.  var re : RegExp = 3;

		6.  var re : RegExp = 'hello';

		7.  var re : RegExp = true;

		8.  var re : RegExp = ReturnRegExpVal(...);


  Abstract:	 Testing variable assignment through type annotation..
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


-------------------------------------------------------------------------*/


/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


function ReturnRegExpVal() {
	var n = new RegExp("[0..9]");

	return n;
}


/*----------
/
/  Class definitions
/
----------*/

	class Person {
		private var iAge : int;
	
		public function GetAge() {
			return iAge;
		}
		
		public function SetAge(iNewAge) {
			iAge = iNewAge;
		}
	}


import System

var ICErrorString;
if (ScriptEngineBuildVersion() >= 9254) {
	ICErrorString = "Exception of type System.InvalidCastException was thrown.";
}
else {
	ICErrorString = "An exception of type System.InvalidCastException was thrown.";
}
var ICError = -2146823266;

function typann14() {

    apInitTest("typann14: Type annotation -- Object types"); 

	apInitScenario("1.  var re : RegExp;");
	try {
		var r1 : RegExp;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (r1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var re : RegExp = /abc/gi;");
	try {
		var r2 : RegExp = /abc/gi;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (r2.toString(), /abc/ig, "2.2 Wrong value","");
	verify (r2.GetType(), "Microsoft.JScript.RegExpObject", "2.3 Wrong data type","");


	apInitScenario("3.  var re : RegExp = new RegExp('', 'gi');");
	try {
		var r3 : RegExp = new RegExp('', 'gi');
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", e.description, "error", "");
		}
	}
	verify (r3.toString(), "
	verify (r3.GetType(), "Microsoft.JScript.RegExpObject", "3.3 Wrong data type","");


	apInitScenario("4.  var re : RegExp = new RegExp('def');");
	try {
		var r4 : RegExp = new RegExp('def');
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (r4.toString(), "/def/", "4.2 Wrong value","");
	verify (r4.GetType(), "Microsoft.JScript.RegExpObject", "4.3 Wrong data type","");


	apInitScenario("5.  var re : RegExp = 3;");
	try {
		var r5 : RegExp = eval("3");
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong value","");
	}
	verify (r5, undefined, "5.2 Wrong value","");


	apInitScenario("6.  var re : RegExp = 'hello';");
	try {
		var r6 : RegExp = eval("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "6.1 Wrong value","");
	}
	verify (r6, undefined, "6.2 Wrong value","");


	apInitScenario("7.  var re : RegExp = true;");
	try {
		var r7 : RegExp = eval("true");
	}
	catch (e) {
		verify (e.number, ICError, "7.1 Wrong value","");
	}
	verify (r7, undefined, "7.2 Wrong value","");


	apInitScenario("8.  var re : RegExp = ReturnRegExpVal(...);");
	try {
		var r8 : RegExp = ReturnRegExpVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.description, "error", "");
		}
	}
	verify (r8.toString(), "/[0..9]/", "8.2 Wrong value","");
	verify (r8.GetType(), "Microsoft.JScript.RegExpObject", "8.3 Wrong data type","");


	apEndTest();
}

typann14();


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
