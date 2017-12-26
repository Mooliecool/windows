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


var iTestID = 160210;

/* -------------------------------------------------------------------------
  Test: 	TYPANN10
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var e : Error;

		2.  var e : Error = new Error(32767);

		3.  var e : Error = 3;

		4.  var e : Error = "hello";

		5.  var e : Error = true;

		6.  var e : Error = ReturnErrorVal(...);

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


function ReturnErrorVal() {
	return new Error(1025);
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

function typann10() {

    apInitTest("typann10: Type annotation -- Object types"); 

	apInitScenario("1.  var e : Error;");
	try {
		var e1 : Error;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (e1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var e : Error = new Error(32767);");
	try {
		var e2 : Error = new Error(32767);
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (e2.number, 32767, "2.2 Wrong value","");
	verify (e2.GetType(), "Microsoft.JScript.ErrorObject", "2.3 Wrong data type","");


	apInitScenario("3.  var e : Error = 3;");
	try {
		var e3 : Error = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "3.1 Wrong error","");
	}
	verify (e3, undefined, "3.2 Wrong value","");


	apInitScenario("4.  var e : Error = 'hello';");
	try {
		var e4 : Error = eval ("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "4.1 Wrong error","");
	}
	verify (e4, undefined, "4.2 Wrong value","");


	apInitScenario("5.  var e : Error = true;");
	try {
		var e5 : Error = eval(true);
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error","");
	}
	verify (e5, undefined, "5.2 Wrong value","");


	apInitScenario("6.  var e : Error = ReturnErrorVal(...);");
	try {
		var e6 : Error = ReturnErrorVal();
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", e.description, "error", "");
		}
	}
	verify (e6.number, 1025, "6.2 Wrong value","");
	verify (e6.GetType(), "Microsoft.JScript.ErrorObject", "6.3 Wrong data type","");


	apEndTest();
}


typann10();


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
