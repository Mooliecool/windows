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


var iTestID = 105182;

/* -------------------------------------------------------------------------
  Test: 	TYPANN01
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for int type
 
  Keywords:	type annotation annotate value int
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : int;
		
		2.  var a : int = 3;

		3.  var a : int = (date.parse ("3/2/2000"));

		4.  var a : int = (long value);

		5.  var a : int = undefined;

		6.  var a : int; a = 3;

		7.  var a : int = "hello";

		8.  var a : int = "5";

		9.  var a : int = "5abc";

		10. var a : int = " 5";

		11. var a : int = "5 ";

		12. var a : int = int.MaxValue;

		13. var a : int = int.MinValue;

		14. var a : int = Number.POSITIVE_INFINITY;

		15. var a : int = Number.NEGATIVE_INFINITY;

		16. var a : int = NaN;

		17. var a : int = true;

		18. var a : int = ReturnIntVal(...);


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


function ReturnIntVal() {
	var intVal : int;

	intVal = 5;

	return intVal;
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

function typann01() {

    apInitTest("typann01: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : int;");

	try {
		var a1 : int;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an int", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Int32", "1.3 Wrong data type","");


	apInitScenario("2.  var a : int = 3;");

	try {
		var a2 : int = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an int", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Int32", "2.3 Wrong data type","");


	apInitScenario("3.  var a : int = (date.parse ('3/2/2000'));");

/*	try {
		var a3 : int = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an int", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Int32", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : int = (long value);");

	try {
		var a4 : int = eval(2147483647 + 10);  // int.MaxValue + 10
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, 0, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Int32", "4.3 Wrong data type","");


	apInitScenario("5.  var a : int = undefined;");

	try {
		var a5 : int = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an int", "error", "");
		}
	}
	verify (a5, 0, "5.2 Wrong value","");
	verify (a5.GetType(), "System.Int32", "5.3 Wrong data type","");


	apInitScenario("6.  var a : int; a = 3;");

	try {
		var a6 : int; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an int", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Int32", "6.3 Wrong data type","");


	apInitScenario("7.  var a : int = 'hello';");

	try {
		var a7 : int = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, 0, "7.2 Wrong value","");
	verify (a7.GetType(), "System.Int32", "7.3 Wrong data type","");


	apInitScenario("8.  var a : int = '5';");

	try {
		var a8 : int = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Int32", "8.3 Wrong data type","");


	apInitScenario("9.  var a : int = '5abc';");

	try {
		var a9 : int = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, 0, "9.2 Wrong value","");
	verify (a9.GetType(), "System.Int32", "9.3 Wrong data type","");


	apInitScenario("10. var a : int = ' 5';");

	try {
		var a10 : int = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Int32", "10.3 Wrong data type","");


	apInitScenario("11. var a : int = '5 ';");

	try {
		var a11 : int = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Int32", "11.3 Wrong data type","");


	apInitScenario("12. var a : int = int.MaxValue;");

	try {
		var a12 : int = int.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, 2147483647, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Int32", "12.3 Wrong data type","");


	apInitScenario("13. var a : int = int.MinValue;");

	try {
		var a13 : int = int.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, -2147483648, "13.2 Wrong value","");
	verify (a13.GetType(), "System.Int32", "13.3 Wrong data type","");


	apInitScenario("14.  var a : int = Number.POSITIVE_INFINITY");

	try {
		var a14 : int = eval("Number.POSITIVE_INFINITY");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, 0, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Int32", "14.3 Wrong data type","");


	apInitScenario("15.  var a : int = Number.NEGATIVE_INFINITY");

	try {
		var a15 : int = eval("Number.NEGATIVE_INFINITY");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, 0, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Int32", "15.3 Wrong data type","");


	apInitScenario("16. var a : int = NaN;");

	try {
		var a16 : int = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, 0, "16.2 Wrong value","");
	verify (a16.GetType(), "System.Int32", "16.3 Wrong data type","");


	apInitScenario("17. var a : int = true;");

	try {
		var a17 : int = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, 1, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Int32", "17.3 Wrong data type","");


	apInitScenario("18. var a : int = ReturnIntVal(...);");

	try {
		var a18 : int = ReturnIntVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, 5, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Int32", "18.3 Wrong data type","");


	apEndTest();
}

typann01();


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
