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


var iTestID = 160208;

/* -------------------------------------------------------------------------
  Test: 	TYPANN07
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var b : Boolean;

		2.  var b : Boolean = (1 > 0);

		3.  var b : Boolean = 3;

		4.  var b : Boolean = "hello";

		5.  var b : Boolean = true;

		6.  var b : Boolean = false;

		7.  var b : Boolean = ReturnBooleanVal(...);


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


function ReturnBooleanVal() {
	return new Boolean(true);
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

function typann07() {

    apInitTest("typann07: Type annotation -- Object types"); 

	apInitScenario("1.  var b : Boolean;");

	try {
		var b1 : Boolean;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (b1, false, "1.2 Wrong value","");
	verify (b1.GetType(), "System.Boolean", "1.3 Wrong data type","");


	apInitScenario("2.  var b : Boolean = (1 > 0);");

	try {
		var b2 : Boolean = (1 > 0);
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (b2, true, "2.2 Wrong value","");
	verify (b2.GetType(), "System.Boolean", "2.3 Wrong data type","");


	apInitScenario("3.  var b : Boolean = 3;");

	try {
		var b3 : Boolean = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (b3, true, "3.2 Wrong value","");
	verify (b3.GetType(), "System.Boolean", "3.3 Wrong data type","");


	apInitScenario("4.  var b : Boolean = 'hello';");

	try {
		var b4 : Boolean = "hello";
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (b4, true, "4.2 Wrong value","");
	verify (b4.GetType(), "System.Boolean", "4.3 Wrong data type","");


	apInitScenario("5.  var b : Boolean = true;");

	try {
		var b5 : Boolean = true;
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", e.description, "error", "");
		}
	}
	verify (b5, true, "5.2 Wrong value","");
	verify (b5.GetType(), "System.Boolean", "5.3 Wrong data type","");


	apInitScenario("6.  var b : Boolean = false;");

	try {
		var b6 : Boolean = false;
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", e.description, "error", "");
		}
	}
	verify (b6, false, "6.2 Wrong value","");
	verify (b6.GetType(), "System.Boolean", "6.3 Wrong data type","");


	apInitScenario("7.  var b : Boolean = ReturnBooleanVal(...);");

	try {
		var b7 : Boolean = ReturnBooleanVal();
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Error returned", e.description, "error", "");
		}
	}
	verify (b7, true, "7.2 Wrong value","");
	verify (b7.GetType(), "System.Boolean", "7.3 Wrong data type","");


	apEndTest();
}

typann07();


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
