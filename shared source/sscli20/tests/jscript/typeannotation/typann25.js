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


var iTestID = 163577;

/* -------------------------------------------------------------------------
  Test: 	TYPANN25
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for boolean type
 
  Keywords:	type annotation annotate value boolean
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : boolean;
		
		2.  var a : boolean = 3;

		3.  var a : boolean = (date.parse ("3/2/2000"));

		4.  var a : boolean = (value greater than boolean.MaxValue);

		5.  var a : boolean = undefined;

		6.  var a : boolean; a = 3;

		7.  var a : boolean = "hello";

		8.  var a : boolean = "5";

		9.  var a : boolean = "5abc";

		10. var a : boolean = " 5";

		11. var a : boolean = "5 ";

		12. var a : boolean = boolean.MaxValue;

		13. var a : boolean = boolean.MinValue;

		14. var a : boolean = Number.POSITIVE_INFINITY;

		15. var a : boolean = Number.NEGATIVE_INFINITY;

		16. var a : boolean = NaN;

		17. var a : boolean = true;

		18. var a : boolean = ReturnbooleanVal(...);


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


function ReturnbooleanVal(str) {
	var booleanVal : boolean;

	booleanVal = boolean(str);

	return booleanVal;
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

function typann25() {

    apInitTest("typann25: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : boolean;");
	try {
		var a1 : boolean;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an boolean", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Boolean", "1.3 Wrong data type","");


	apInitScenario("2.  var a : boolean = 3;");
	try {
		var a2 : boolean = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an boolean", "error", "");
		}
	}
	verify (a2, true, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Boolean", "2.3 Wrong data type","");


	apInitScenario("3.  var a : boolean = (date.parse ('3/2/2000'));");

/*
	try {
		var a3 : boolean = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an boolean", "error", "");
		}
	}
	verify (a3, true, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Boolean", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : boolean = (value greater than boolean.MaxValue);");
	try {
		var a4 : boolean = eval(1.7976931348623157e+308 * 2);  // boolean.MaxValue * 2
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, true, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Boolean", "4.3 Wrong data type","");


	apInitScenario("5.  var a : boolean = undefined;");
	try {
		var a5 : boolean = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an boolean", "error", "");
		}
	}
	verify (a5, "", "5.2 Wrong value","");
	verify (a5.GetType(), "System.Boolean", "5.3 Wrong data type","");


	apInitScenario("6.  var a : boolean; a = 3;");
	try {
		var a6 : boolean; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an boolean", "error", "");
		}
	}
	verify (a6, true, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Boolean", "6.3 Wrong data type","");


	apInitScenario("7.  var a : boolean = 'hello';");
	try {
		var a7 : boolean = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, true, "7.2 Wrong value","");
	verify (a7.GetType(), "System.Boolean", "7.3 Wrong data type","");


	apInitScenario("8.  var a : boolean = '5';");
	try {
		var a8 : boolean = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, true, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Boolean", "8.3 Wrong data type","");


	apInitScenario("9.  var a : boolean = '5abc';");
	try {
		var a9 : boolean = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, true, "9.2 Wrong value","");
	verify (a9.GetType(), "System.Boolean", "9.3 Wrong data type","");


	apInitScenario("10. var a : boolean = ' 5';");
	try {
		var a10 : boolean = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, true, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Boolean", "10.3 Wrong data type","");


	apInitScenario("11. var a : boolean = '5 ';");
	try {
		var a11 : boolean = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, true, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Boolean", "11.3 Wrong data type","");


	apInitScenario("12. var a : boolean = true;");
	try {
		var a12 : boolean = true;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, true, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Boolean", "12.3 Wrong data type","");


	apInitScenario("13. var a : boolean = boolean.False;");
	try {
		var a13 : boolean = false;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, "", "13.2 Wrong value","");
	verify (a13.GetType(), "System.Boolean", "13.3 Wrong data type","");


	apInitScenario("14. var a : boolean = Number.POSITIVE_INFINITY");
	var a14 : boolean;
	try {
		eval ("a14 = Number.POSITIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, true, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Boolean", "14.3 Wrong data type","");


	apInitScenario("15. var a : boolean = Number.NEGATIVE_INFINITY");
	var a15 : boolean;
	try {
		eval("a15 = Number.NEGATIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, true, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Boolean", "15.3 Wrong data type","");


	apInitScenario("16. var a : boolean = NaN;");
	try {
		var a16 : boolean = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, "", "16.2 Wrong value","");
	verify (a16.GetType(), "System.Boolean", "16.3 Wrong data type","");


	apInitScenario("17. var a : boolean = true;");
	try {
		var a17 : boolean = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, true, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Boolean", "17.3 Wrong data type","");


	apInitScenario("18. var a : boolean = ReturnbooleanVal(...);");
	try {
		var a18 : boolean = ReturnbooleanVal("5");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, true, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Boolean", "18.3 Wrong data type","");


	apEndTest();
}

typann25();


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
