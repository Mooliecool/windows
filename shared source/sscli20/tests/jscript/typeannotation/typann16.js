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


var iTestID = 160216;

/* -------------------------------------------------------------------------
  Test: 	TYPANN16
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var vbArr : VBArray;

		2.  var vbArr : VBArray = new VBArray();

		3.  var vbArr : VBArray = 3;

		4.  var vbArr : VBArray = 'hello';

		5.  var vbArr : VBArray = true;


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


function ReturnStringVal() {
	var n = new String("[0..9]");

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

function typann16() {

    apInitTest("typann16: Type annotation -- Object types"); 

	apInitScenario("1.  var vbArr : VBArray;");
	try {
		var v1 : VBArray;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (v1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var vbArr : VBArray = new VBArray();");
	try {
		var v2 : VBArray = new VBArray();
	}
	catch (e) {
		if (e.description != "VBArray expected") {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", e.description, "error", "");
		}
	}
	verify (v2, undefined, "2.2 Wrong value","");


	apInitScenario("3.  var vbArr : VBArray = 3;");
	try {
		var v3 : VBArray = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "3.1 Wrong value","");
	}
	verify (v3, undefined, "3.2 Wrong value","");


	apInitScenario("4.  var vbArr : VBArray = 'hello';");
	try {
		var v4 : VBArray = eval("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "4.1 Wrong value","");
	}
	verify (v4, undefined, "4.2 Wrong value","");


	apInitScenario("5.  var vbArr : VBArray = true;");
	try {
		var v5 :VBArray = eval("true");
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong value","");
	}
	verify (v5, undefined, "5.2 Wrong value","");


	apEndTest();
}


typann16();


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
