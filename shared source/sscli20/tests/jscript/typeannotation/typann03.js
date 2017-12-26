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


var iTestID = 160205;

/* -------------------------------------------------------------------------
  Test: 	TYPANN03
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for float type
 
  Keywords:	type annotation annotate value float
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : float;
		
		2.  var a : float = 3;

		3.  var a : float = (date.parse ("3/2/2000"));

		4.  var a : float = (value greater than float.MaxValue);

		5.  var a : float = undefined;

		6.  var a : float; a = 3;

		7.  var a : float = "hello";

		8.  var a : float = "5";

		9.  var a : float = "5abc";

		10. var a : float = " 5";

		11. var a : float = "5 ";

		12. var a : float = float.MaxValue;

		13. var a : float = float.MinValue;

		14. var a : float = Number.POSITIVE_INFINITY;

		15. var a : float = Number.NEGATIVE_INFINITY;

		16. var a : float = NaN;

		17. var a : float = true;

		18. var a : float = ReturnFloatVal(...);


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


function ReturnFloatVal(str) {
	var floatVal : float;

	floatVal = float(str);

	return floatVal;
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

function typann03() {

    apInitTest("typann03: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : float;");

	try {
		var a1 : float;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an float", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Single", "1.3 Wrong data type","");


	apInitScenario("2.  var a : float = 3;");

	try {
		var a2 : float = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an float", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Single", "2.3 Wrong data type","");


	apInitScenario("3.  var a : float = (date.parse ('3/2/2000'));");

/*	try {
		var a3 : float = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an float", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Single", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : float = (value greater than float.MaxValue);");

	try {
		var a4 : float = eval(3.4028234663852886e+38 * 2);  // float.MaxValue * 2
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, Infinity, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Single", "4.3 Wrong data type","");


	apInitScenario("5.  var a : float = undefined;");

	try {
		var a5 : float = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an float", "error", "");
		}
	}
	verify (a5.toString(), "NaN", "5.2 Wrong value","");
	verify (a5.GetType(), "System.Single", "5.3 Wrong data type","");


	apInitScenario("6.  var a : float; a = 3;");

	try {
		var a6 : float; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an float", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Single", "6.3 Wrong data type","");


	apInitScenario("7.  var a : float = 'hello';");

	try {
		var a7 : float = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7.toString(), "NaN", "7.2 Wrong value","");
	verify (a7.GetType(), "System.Single", "7.3 Wrong data type","");


	apInitScenario("8.  var a : float = '5';");

	try {
		var a8 : float = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Single", "8.3 Wrong data type","");


	apInitScenario("9.  var a : float = '5abc';");

	try {
		var a9 : float = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9.toString(), "NaN", "9.2 Wrong value","");
	verify (a9.GetType(), "System.Single", "9.3 Wrong data type","");


	apInitScenario("10. var a : float = ' 5';");

	try {
		var a10 : float = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Single", "10.3 Wrong data type","");


	apInitScenario("11. var a : float = '5 ';");

	try {
		var a11 : float = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Single", "11.3 Wrong data type","");


	apInitScenario("12. var a : float = float.MaxValue;");

	try {
		var a12 : float = float.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, 3.4028234663852886e+38, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Single", "12.3 Wrong data type","");


	apInitScenario("13. var a : float = float.MinValue;");

	try {
		var a13 : float = float.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, -3.4028234663852886e+38, "13.2 Wrong value","");
	verify (a13.GetType(), "System.Single", "13.3 Wrong data type","");


	apInitScenario("14. var a : float = Number.POSITIVE_INFINITY");

	try {
		var a14 : float = Number.POSITIVE_INFINITY;
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, Infinity, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Single", "14.3 Wrong data type","");


	apInitScenario("15. var a : float = Number.NEGATIVE_INFINITY");

	try {
		var a15 : float = Number.NEGATIVE_INFINITY;
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, -Infinity, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Single", "15.3 Wrong data type","");


	apInitScenario("16. var a : float = NaN;");

	try {
		var a16 : float = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16.toString(), "NaN", "16.2 Wrong value","");
	verify (a16.GetType(), "System.Single", "16.3 Wrong data type","");


	apInitScenario("17. var a : float = true;");

	try {
		var a17 : float = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, 1, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Single", "17.3 Wrong data type","");


	apInitScenario("18. var a : float = ReturnFloatVal(...);");

	try {
		var a18 : float = ReturnFloatVal("5");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, 5, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Single", "18.3 Wrong data type","");


	apEndTest();
}


typann03();


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
