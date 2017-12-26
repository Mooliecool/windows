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


var iTestID = 163578;

/* -------------------------------------------------------------------------
  Test: 	TYPANN26
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for short type
 
  Keywords:	type annotation annotate value short
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : short;
		
		2.  var a : short = 3;

		3.  var a : short = (date.parse ("3/2/2000"));

		4.  var a : short = (value greater than short.MaxValue);

		5.  var a : short = undefined;

		6.  var a : short; a = 3;

		7.  var a : short = "hello";

		8.  var a : short = "5";

		9.  var a : short = "5abc";

		10. var a : short = " 5";

		11. var a : short = "5 ";

		12. var a : short = short.MaxValue;

		13. var a : short = short.MinValue;

		14. var a : short = Number.POSITIVE_INFINITY;

		15. var a : short = Number.NEGATIVE_INFINITY;

		16. var a : short = NaN;

		17. var a : short = true;

		18. var a : short = ReturnshortVal(...);


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


function ReturnshortVal(str) {
	var shortVal : short;

	shortVal = short(str);

	return shortVal;
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

function typann26() {

    apInitTest("typann26: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : short;");
	try {
		var a1 : short;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an short", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Int16", "1.3 Wrong data type","");


	apInitScenario("2.  var a : short = 3;");
	try {
		var a2 : short = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an short", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Int16", "2.3 Wrong data type","");


	apInitScenario("3.  var a : short = (date.parse ('3/2/2000'));");

/*
	try {
		var a3 : short = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an short", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Int16", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : short = (value greater than short.MaxValue);");
	try {
		var a4 : short = eval(1.7976931348623157e+308 * 2);  // short.MaxValue * 2
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, 0, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Int16", "4.3 Wrong data type","");


	apInitScenario("5.  var a : short = undefined;");
	try {
		var a5 : short = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an short", "error", "");
		}
	}
	verify (a5, "", "5.2 Wrong value","");
	verify (a5.GetType(), "System.Int16", "5.3 Wrong data type","");


	apInitScenario("6.  var a : short; a = 3;");
	try {
		var a6 : short; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an short", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Int16", "6.3 Wrong data type","");


	apInitScenario("7.  var a : short = 'hello';");
	try {
		var a7 : short = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, 0, "7.2 Wrong value","");
	verify (a7.GetType(), "System.Int16", "7.3 Wrong data type","");


	apInitScenario("8.  var a : short = '5';");
	try {
		var a8 : short = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Int16", "8.3 Wrong data type","");


	apInitScenario("9.  var a : short = '5abc';");
	try {
		var a9 : short = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, 0, "9.2 Wrong value","");
	verify (a9.GetType(), "System.Int16", "9.3 Wrong data type","");


	apInitScenario("10. var a : short = ' 5';");
	try {
		var a10 : short = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Int16", "10.3 Wrong data type","");


	apInitScenario("11. var a : short = '5 ';");
	try {
		var a11 : short = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Int16", "11.3 Wrong data type","");


	apInitScenario("12. var a : short = short.MaxValue;");
	try {
		var a12 : short = short.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, 32767, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Int16", "12.3 Wrong data type","");


	apInitScenario("13. var a : short = short.MinValue;");
	try {
		var a13 : short = short.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, -32768, "13.2 Wrong value","");
	verify (a13.GetType(), "System.Int16", "13.3 Wrong data type","");


	apInitScenario("14. var a : short = Number.POSITIVE_INFINITY");
	var a14 : short;
	try {
		eval ("a14 = Number.POSITIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, 0, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Int16", "14.3 Wrong data type","");


	apInitScenario("15. var a : short = Number.NEGATIVE_INFINITY");
	var a15 : short;
	try {
		eval("a15 = Number.NEGATIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, 0, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Int16", "15.3 Wrong data type","");


	apInitScenario("16. var a : short = NaN;");
	try {
		var a16 : short = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, "", "16.2 Wrong value","");
	verify (a16.GetType(), "System.Int16", "16.3 Wrong data type","");


	apInitScenario("17. var a : short = true;");
	try {
		var a17 : short = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, true, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Int16", "17.3 Wrong data type","");


	apInitScenario("18. var a : short = ReturnshortVal(...);");
	try {
		var a18 : short = ReturnshortVal("5");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, 5, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Int16", "18.3 Wrong data type","");


	apEndTest();
}

typann26();


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
