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


var iTestID = 160219;

/* -------------------------------------------------------------------------
  Test: 	TYPANN19
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var strS : System.String;

		2.  var strS : System.String = 'My dog has fleas.';

		3.  var strS : System.String = 3 + ' strikes, yer out.';

		4.  var strS : System.String = 3;

		5.  var strS : System.String = true;

		6.  var strS : System.String = ReturnSysStringVal();


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


function ReturnSysStringVal() {
	var n : System.String = "42";

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

	class Student extends Person {
		private var iGPA : float;
	
		public function GetGPA() {
			return iGPA;
		}
		
		public function SetGPA(iNewGPA) {
			iGPA = iNewGPA;
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

function typann19() {

    apInitTest("typann19: Type annotation -- Object types"); 

	apInitScenario("1.  var strS : System.String;");
	try {
		var s1 : System.String;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (String(s1), "undefined", "1.2 Wrong value","");


	apInitScenario("2.  var strS : System.String = 'My dog has fleas.';");
	try {
		var s2 : System.String = 'My dog has fleas.';
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", e.description, "error", "");
		}
	}
	verify (s2, "My dog has fleas.", "2.2 Wrong value","");
	verify (s2.GetType(), "System.String", "2.3 Wrong data type","");


	apInitScenario("3.  var strS : System.String = 3 + ' strikes, yer out.';");
	try {
		var s3 : System.String = 3 + ' strikes, yer out.';
	}
	catch (e) {
		verify (e.number, ICError, "6.1 Wrong value","");
	}
	verify (String(s3), "3 strikes, yer out.", "3.2 Wrong value","");
	verify (s3.GetType(), "System.String", "3.3 Wrong data type","");


	apInitScenario("4.  var strS : System.String = 3;");
	try {
		var s4 : System.String = 3;
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (String(s4), "3", "4.2 Wrong value","");
	verify (s4.GetType(), "System.String", "4.3 Wrong data type","");


	apInitScenario("5.  var strS : System.String = true;");
	try {
		var s5 : System.String = eval("true");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", e.description, "error", "");
		}
	}
	verify (String(s5), "true", "5.2 Wrong value","");
	verify (s5.GetType(), "System.String", "5.3 Wrong data type","");


	apInitScenario("6.  var strS : System.String = ReturnSysStringVal();");
	try {
		var s6 : System.String = ReturnSysStringVal();
	}
	catch (e) {
		verify (e.number, ICError, "6.1 Wrong value","");
	}
	verify (String(s6), "42", "6.2 Wrong value","");
	verify (s6.GetType(), "System.String", "6.3 Wrong data type","");


	apEndTest();
}


typann19();


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
