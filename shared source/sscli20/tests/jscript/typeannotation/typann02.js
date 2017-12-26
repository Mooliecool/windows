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


var iTestID = 160204;

/* -------------------------------------------------------------------------
  Test: 	TYPANN02
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for long type
 
  Keywords:	type annotation annotate value long
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : long;
		
		2.  var a : long = 3;

		3.  var a : long = (date.parse ("3/2/2000"));

		4.  var a : long = (value greater than long.MaxValue);

		5.  var a : long = undefined;

		6.  var a : long; a = 3;

		7.  var a : long = "hello";

		8.  var a : long = "5";

		9.  var a : long = "5abc";

		10. var a : long = " 5";

		11. var a : long = "5 ";

		12. var a : long = long.MaxValue;

		13. var a : long = long.MinValue;

		14. var a : long = Number.POSITIVE_INFINITY;

		15. var a : long = Number.NEGATIVE_INFINITY;

		16. var a : long = NaN;

		17. var a : long = true;

		18. var a : long = ReturnlongVal(...);


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


function ReturnLongVal() {
	var longVal : long;

	longVal = 5;

	return longVal;
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

@if(!@aspx)
import System
@end

function typann02() {

    apInitTest("typann02: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : long;");

	try {
		var a1 : long;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an long", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Int64", "1.3 Wrong data type","");


	apInitScenario("2.  var a : long = 3;");

	try {
		var a2 : long = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an long", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Int64", "2.3 Wrong data type","");


	apInitScenario("3.  var a : long = (date.parse ('3/2/2000'));");

/*	try {
		var a3 : long = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an long", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Int64", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : long = (value greater than long.MaxValue);");

	try {
		var a4 : long = eval(9223372036854775807 + 10);  // long.MaxValue + 10
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, 0, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Int64", "4.3 Wrong data type","");


	apInitScenario("5.  var a : long = undefined;");

	try {
		var a5 : long = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an long", "error", "");
		}
	}
	verify (a5, 0, "5.2 Wrong value","");
	verify (a5.GetType(), "System.Int64", "5.3 Wrong data type","");


	apInitScenario("6.  var a : long; a = 3;");

	try {
		var a6 : long; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an long", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Int64", "6.3 Wrong data type","");


	apInitScenario("7.  var a : long = 'hello';");

	try {
		var a7 : long = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, 0, "7.2 Wrong value","");
	verify (a7.GetType(), "System.Int64", "7.3 Wrong data type","");


	apInitScenario("8.  var a : long = '5';");

	try {
		var a8 : long = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Int64", "8.3 Wrong data type","");


	apInitScenario("9.  var a : long = '5abc';");

	try {
		var a9 : long = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, 0, "9.2 Wrong value","");
	verify (a9.GetType(), "System.Int64", "9.3 Wrong data type","");


	apInitScenario("10. var a : long = ' 5';");

	try {
		var a10 : long = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Int64", "10.3 Wrong data type","");


	apInitScenario("11. var a : long = '5 ';");

	try {
		var a11 : long = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Int64", "11.3 Wrong data type","");


	apInitScenario("12. var a : long = long.MaxValue;");

	try {
		var a12 : long = long.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, 9223372036854775807, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Int64", "12.3 Wrong data type","");


	apInitScenario("13. var a : long = long.MinValue;");

	try {
		var a13 : long = long.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, -9223372036854775808, "13.2 Wrong value","");
	verify (a13.GetType(), "System.Int64", "13.3 Wrong data type","");


	apInitScenario("14. var a : long = Number.POSITIVE_INFINITY");

	try {
		var a14 : long = eval("Number.POSITIVE_INFINITY");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, 0, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Int64", "14.3 Wrong data type","");


	apInitScenario("15. var a : long = Number.NEGATIVE_INFINITY");

	try {
		var a15 : long = eval("Number.NEGATIVE_INFINITY");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, 0, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Int64", "15.3 Wrong data type","");


	apInitScenario("16. var a : long = NaN;");

	try {
		var a16 : long = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, 0, "16.2 Wrong value","");
	verify (a16.GetType(), "System.Int64", "16.3 Wrong data type","");


	apInitScenario("17. var a : long = true;");

	try {
		var a17 : long = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, 1, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Int64", "17.3 Wrong data type","");


	apInitScenario("18. var a : long = ReturnLongVal(...);");

	try {
		var a18 : long = ReturnLongVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, 5, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Int64", "18.3 Wrong data type","");


	apEndTest();
}


typann02();


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
