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


var iTestID = 163579;

/* -------------------------------------------------------------------------
  Test: 	TYPANN27
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for byte type
 
  Keywords:	type annotation annotate value byte
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : byte;
		
		2.  var a : byte = 3;

		3.  var a : byte = (date.parse ("3/2/2000"));

		4.  var a : byte = (value greater than byte.MaxValue);

		5.  var a : byte = undefined;

		6.  var a : byte; a = 3;

		7.  var a : byte = "hello";

		8.  var a : byte = "5";

		9.  var a : byte = "5abc";

		10. var a : byte = " 5";

		11. var a : byte = "5 ";

		12. var a : byte = byte.MaxValue;

		13. var a : byte = byte.MinValue;

		14. var a : byte = Number.POSITIVE_INFINITY;

		15. var a : byte = Number.NEGATIVE_INFINITY;

		16. var a : byte = NaN;

		17. var a : byte = true;

		18. var a : byte = ReturnbyteVal(...);


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


function ReturnbyteVal(str) {
	var byteVal : byte;

	byteVal = byte(str);

	return byteVal;
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

function typann27() {

    apInitTest("typann27: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : byte;");
	try {
		var a1 : byte;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an byte", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Byte", "1.3 Wrong data type","");


	apInitScenario("2.  var a : byte = 3;");
	try {
		var a2 : byte = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an byte", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Byte", "2.3 Wrong data type","");


	apInitScenario("3.  var a : byte = (date.parse ('3/2/2000'));");

/*
	try {
		var a3 : byte = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an byte", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Byte", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : byte = (value greater than byte.MaxValue);");
	try {
		var a4 : byte = eval(1.7976931348623157e+308 * 2);  // byte.MaxValue * 2
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, 0, "4.2 Wrong value","");
	verify (a4.GetType(), "System.Byte", "4.3 Wrong data type","");


	apInitScenario("5.  var a : byte = undefined;");
	try {
		var a5 : byte = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an byte", "error", "");
		}
	}
	verify (a5, "", "5.2 Wrong value","");
	verify (a5.GetType(), "System.Byte", "5.3 Wrong data type","");


	apInitScenario("6.  var a : byte; a = 3;");
	try {
		var a6 : byte; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an byte", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Byte", "6.3 Wrong data type","");


	apInitScenario("7.  var a : byte = 'hello';");
	try {
		var a7 : byte = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, 0, "7.2 Wrong value","");
	verify (a7.GetType(), "System.Byte", "7.3 Wrong data type","");


	apInitScenario("8.  var a : byte = '5';");
	try {
		var a8 : byte = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Byte", "8.3 Wrong data type","");


	apInitScenario("9.  var a : byte = '5abc';");
	try {
		var a9 : byte = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, 0, "9.2 Wrong value","");
	verify (a9.GetType(), "System.Byte", "9.3 Wrong data type","");


	apInitScenario("10. var a : byte = ' 5';");
	try {
		var a10 : byte = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Byte", "10.3 Wrong data type","");


	apInitScenario("11. var a : byte = '5 ';");
	try {
		var a11 : byte = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Byte", "11.3 Wrong data type","");


	apInitScenario("12. var a : byte = byte.MaxValue;");
	try {
		var a12 : byte = byte.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a12, 255, "12.2 Wrong value","");
	verify (a12.GetType(), "System.Byte", "12.3 Wrong data type","");


	apInitScenario("13. var a : byte = byte.MinValue;");
	try {
		var a13 : byte = byte.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, 0, "13.2 Wrong value","");
	verify (a13.GetType(), "System.Byte", "13.3 Wrong data type","");


	apInitScenario("14. var a : byte = Number.POSITIVE_INFINITY");
	var a14 : byte;
	try {
		eval ("a14 = Number.POSITIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, 0, "14.2 Wrong value","");
	verify (a14.GetType(), "System.Byte", "14.3 Wrong data type","");


	apInitScenario("15. var a : byte = Number.NEGATIVE_INFINITY");
	var a15 : byte;
	try {
		eval("a15 = Number.NEGATIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, 0, "15.2 Wrong value","");
	verify (a15.GetType(), "System.Byte", "15.3 Wrong data type","");


	apInitScenario("16. var a : byte = NaN;");
	try {
		var a16 : byte = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, "", "16.2 Wrong value","");
	verify (a16.GetType(), "System.Byte", "16.3 Wrong data type","");


	apInitScenario("17. var a : byte = true;");
	try {
		var a17 : byte = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, true, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Byte", "17.3 Wrong data type","");


	apInitScenario("18. var a : byte = ReturnbyteVal(...);");
	try {
		var a18 : byte = ReturnbyteVal("5");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, 5, "18.2 Wrong value","");
	verify (a18.GetType(), "System.Byte", "18.3 Wrong data type","");


	apEndTest();
}

typann27();


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
