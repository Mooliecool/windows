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


var iTestID = 160217;

/* -------------------------------------------------------------------------
  Test: 	TYPANN17
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var o : Person;

		2.  var o : Person = new Person();

		3.  var o : Person = 3;

		4.  var o : Person = 'hello';

		5.  var o : Person = true;

		6.  var o : Person = ReturnPersonVal(...);


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


function ReturnPersonVal() {
	var n = new Person();
	n.SetAge(42);

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

function typann17() {

    apInitTest("typann17: Type annotation -- Object types"); 

	apInitScenario("1.  var o : Person;");
	try {
		var o1 : Person;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (o1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var o : Person = new Person();");
	try {
		var o2 : Person = new Person();
		o2.SetAge(3);
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", e.description, "error", "");
		}
	}
	verify (o2.GetAge(), 3, "2.2 Wrong value","");
	verify (o2.GetType(), "Person", "2.3 Wrong data type","");


	apInitScenario("3.  var o : Person = 3;");
	try {
		var o3 : Person = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error","");
	}
	verify (o3, undefined, "3.2 Wrong value","");


	apInitScenario("4.  var o : Person = 'hello';");
	try {
		var o4 : Person = eval("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error","");
	}
	verify (o4, undefined, "4.2 Wrong value","");


	apInitScenario("5.  var o : Person = true;");
	try {
		var o5 : Person = eval("true");
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error","");
	}
	verify (o5, undefined, "5.2 Wrong value","");


	apInitScenario("6.  var o : Person = ReturnPersonVal();");
	try {
		var o6 : Person = ReturnPersonVal();
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error","");
	}
	verify (o6.GetAge(), 42, "6.2 Wrong value","");
	verify (o6.GetType(), "Person", "6.3 Wrong data type","");


	apEndTest();
}


typann17();


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
