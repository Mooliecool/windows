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


var iTestID = 160207;

/* -------------------------------------------------------------------------
  Test: 	TYPANN06
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var arr : Array;

		2.  var arr : Array = new Array(...);

		3.  var arr : [2,4,5,6];

		4.  var arr : Array = String.split("My dog has fleas.", " ");

		5.  var arr : Array = 3;

		6.  var arr : Array = "hello";

		7.  var arr : Array = false;

		8.  var arr : Array = ReturnArrayVal(...);


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


function ReturnArrayVal() {
	return new Array(6,7,8,9);
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

function typann06() {

    apInitTest("typann06: Type annotation -- Object types"); 

	apInitScenario("1.  var arr : Array;");

	try {
		var a1 : Array;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (a1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var arr : Array = new Array(...);");

	try {
		var a2 : Array = new Array(3,4,5,6);
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (a2, "3,4,5,6", "2.2 Wrong value","");
	verify (a2.GetType(), "Microsoft.JScript.ArrayObject", "2.3 Wrong data type","");


	apInitScenario("3.  var arr : [3,4,5,6];");

	try {
		var a3 : Array = [3,4,5,6];
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (a3, "3,4,5,6", "3.2 Wrong value","");
	verify (a3.GetType(), "Microsoft.JScript.ArrayObject", "3.3 Wrong data type","");


	apInitScenario("4.  var arr : Array = 'My dog has fleas.'.split(' ');");

	try {
		var a4 : Array = "My dog has fleas.".split(" ");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (a4, "My,dog,has,fleas.", "4.2 Wrong value","");
	verify (a4.GetType(), "Microsoft.JScript.ArrayObject", "4.3 Wrong data type","");


	apInitScenario("5.  var arr : Array = 3;");

	try {
		var a5 : Array = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "5.1 Wrong error returned","");
	}
	verify (a5, undefined, "5.2 Wrong value","");


	apInitScenario("6.  var arr : Array = 'hello';");

	try {
		var a6 : Array = eval("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "6.1 Wrong error returned","");
	}
	verify (a6, undefined, "6.2 Wrong value","");


	apInitScenario("7.  var arr : Array = false;");

	try {
		var a7 : Array = eval("false");
	}
	catch (e) {
		verify (e.number, ICError, "7.1 Wrong error returned","");

	}
	verify (a7, undefined, "7.2 Wrong value","");


	apInitScenario("8.  var arr : Array = ReturnArrayVal(...);");

	try {
		var a8 : Array = ReturnArrayVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.description, "error", "");
		}
	}
	verify (a8, "6,7,8,9", "8.2 Wrong value","");
	verify (a8.GetType(), "Microsoft.JScript.ArrayObject", "8.3 Wrong data type","");


	apEndTest();
}


typann06();


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
