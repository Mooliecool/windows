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


var iTestID = 160209;

/* -------------------------------------------------------------------------
  Test: 	TYPANN08
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var d : Date;

		2.  var d : Date = new Date();

		3.  var d : Date = 3;

		4.  var d : Date = "hello";

		5.  var d : Date = true;

		6.  var d : Date = ReturnDateVal(...);

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


function ReturnDateVal() {
	return new Date(2001,1,14);
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


function typann08() {

    apInitTest("typann08: Type annotation -- Object types"); 

	apInitScenario("1.  var d : Date;");
	try {
		var d1 : Date;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (d1, undefined, "1.2 Wrong value","");


	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
		apInitScenario("2.  var d : Date = new Date();");
	try {
	var d2 : Date = new Date(2001,0,13);
	}
	catch (e) {
	if (e.number != 0) {
	apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
	}	
	}
	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	verify (d2.valueOf(), 979372800000, "2.2 Wrong value","");
	}
	verify (d2.GetType(), "Microsoft.JScript.DateObject", "2.3 Wrong data type","");
	}

	apInitScenario("3.  var d : Date = 3;");
	try {
		var d3 : Date = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "3.2 Wrong error","");
	}
	verify (d3, undefined, "3.2 Wrong value","");


	apInitScenario("4.  var d : Date = 'hello';");
	try {
		var d4 : Date = eval("hello");
	}
	catch (e) {
		if ((e.number != -2146827153) && (e.number != -2146823279)) {
			apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.number, -2146827153, "");
		}
	}
	verify (d4, undefined, "4.2 Wrong value","");


	apInitScenario("5.  var d : Date = true;");
	try {
		var d5 : Date = eval(true);
	}
	catch (e) {
		verify (e.number, ICError, "5.2 Wrong error","");
	}
	verify (d5, undefined, "5.2 Wrong value","");


	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario("6.  var d : Date = ReturnDateVal(...);");
	try {
		var d6 : Date = ReturnDateVal();
	}
	catch (e) {
		if (e.description != ICErrorString) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", e.description, "error", "");
		}
	}
	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	verify (d6.valueOf(), 982137600000, "6.2 Wrong value","");
	}
	verify (d6.GetType(), "Microsoft.JScript.DateObject", "6.3 Wrong data type","");
	}

	apEndTest();
}

typann08();


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
