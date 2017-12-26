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


var iTestID = 163576;

/* -------------------------------------------------------------------------
  Test: 	TYPANN24
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for char type
 
  Keywords:	type annotation annotate value char
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var a : char;
		
		2.  var a : char = 3;

		3.  var a : char = (date.parse ("3/2/2000"));

		4.  var a : char = (value greater than char.MaxValue);

		5.  var a : char = undefined;

		6.  var a : char; a = 3;

		7.  var a : char = "hello";

		8.  var a : char = "5";

		9.  var a : char = "5abc";

		10. var a : char = " 5";

		11. var a : char = "5 ";

		12. var a : char = char.MaxValue;

		13. var a : char = char.MinValue;

		14. var a : char = Number.POSITIVE_INFINITY;

		15. var a : char = Number.NEGATIVE_INFINITY;

		16. var a : char = NaN;

		17. var a : char = true;

		18. var a : char = ReturncharVal(...);


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


function ReturncharVal(str) {
	var charVal : char;

	charVal = char(str);

	return charVal;
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

var CharMaxValue;

//if (ScriptEngineBuildVersion() >= 9254) {
  if (ScriptEngineMajorVersion() >= 7) {
	CharMaxValue = -1;
}
else {
	CharMaxValue = 65535;
}

function typann24() {

    apInitTest("typann24: Type annotation -- Literal types"); 

	apInitScenario("1.  var a : char;");
	try {
		var a1 : char;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", "a is an char", "error", "");
		}
	}
	verify (a1, 0, "1.2 Wrong value","");
	verify (a1.GetType(), "System.Char", "1.3 Wrong data type","");


	apInitScenario("2.  var a : char = 3;");
	try {
		var a2 : char = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an char", "error", "");
		}
	}
	verify (a2, 3, "2.2 Wrong value","");
	verify (a2.GetType(), "System.Char", "2.3 Wrong data type","");


	apInitScenario("3.  var a : char = (date.parse ('3/2/2000'));");

/*
	try {
		var a3 : char = (Date.parse ("3/2/2000"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", "a is an char", "error", "");
		}
	}
	verify (a3, 3, "3.2 Wrong value","");
	verify (a3.GetType(), "System.Char", "3.3 Wrong data type","");
*/


	apInitScenario("4.  var a : char = (value greater than char.MaxValue);");
	try {
		var a4 : char = eval(1.7976931348623157e+308 * 2);  // char.MaxValue * 2
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a4, "", "4.2 Wrong value","");
	verify (a4.GetType(), "System.Char", "4.3 Wrong data type","");


	apInitScenario("5.  var a : char = undefined;");
	try {
		var a5 : char = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", "a is an char", "error", "");
		}
	}
	verify (a5, "", "5.2 Wrong value","");
	verify (a5.GetType(), "System.Char", "5.3 Wrong data type","");


	apInitScenario("6.  var a : char; a = 3;");
	try {
		var a6 : char; a6 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", "a is an char", "error", "");
		}
	}
	verify (a6, 3, "6.2 Wrong value","");
	verify (a6.GetType(), "System.Char", "6.3 Wrong data type","");


	apInitScenario("7.  var a : char = 'hello';");
	try {
		var a7 : char = eval("new String('hello')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a7, "", "7.2 Wrong value","");
	verify (a7.GetType(), "System.Char", "7.3 Wrong data type","");


	apInitScenario("8.  var a : char = '5';");
	try {
		var a8 : char = eval("new String('5')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a8, 5, "8.2 Wrong value","");
	verify (a8.GetType(), "System.Char", "8.3 Wrong data type","");


	apInitScenario("9.  var a : char = '5abc';");
	try {
		var a9 : char = eval(new String("5abc"));
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 9.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a9, "", "9.2 Wrong value","");
	verify (a9.GetType(), "System.Char", "9.3 Wrong data type","");


	apInitScenario("10. var a : char = ' 5';");
	try {
		var a10 : char = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a10, 5, "10.2 Wrong value","");
	verify (a10.GetType(), "System.Char", "10.3 Wrong data type","");


	apInitScenario("11. var a : char = '5 ';");
	try {
		var a11 : char = eval(new String(" 5"));
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a11, 5, "11.2 Wrong value","");
	verify (a11.GetType(), "System.Char", "11.3 Wrong data type","");


	apInitScenario("12. var a : char = char.MaxValue;");
	try {
		var a12 : char = char.MaxValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.number, e.description, "");
		}
	}
	verify (System.Char.GetNumericValue(a12), CharMaxValue, "12.2 Wrong value","215460");
	verify (a12.GetType(), "System.Char", "12.3 Wrong data type","");


	apInitScenario("13. var a : char = char.MinValue;");
	try {
		var a13 : char = char.MinValue;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a13, "", "13.2 Wrong value","");
	verify (a13.GetType(), "System.Char", "13.3 Wrong data type","");


	apInitScenario("14. var a : char = Number.POSITIVE_INFINITY");
	var a14 : char;
	try {
		eval ("a14 = Number.POSITIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a14, "", "14.2 Wrong value","");
	verify (a14.GetType(), "System.Char", "14.3 Wrong data type","");


	apInitScenario("15. var a : char = Number.NEGATIVE_INFINITY");
	var a15 : char;
	try {
		eval("a15 = Number.NEGATIVE_INFINITY;");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a15, "", "15.2 Wrong value","");
	verify (a15.GetType(), "System.Char", "15.3 Wrong data type","");


	apInitScenario("16. var a : char = NaN;");
	try {
		var a16 : char = eval(NaN);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a16, "", "16.2 Wrong value","");
	verify (a16.GetType(), "System.Char", "16.3 Wrong data type","");


	apInitScenario("17. var a : char = true;");
	try {
		var a17 : char = eval(true);
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.1 Wrong error returned", "Type mismatch", e.description, "");
		}
	}
	verify (a17, 1, "17.2 Wrong value","");
	verify (a17.GetType(), "System.Char", "17.3 Wrong data type","");


	apInitScenario("18. var a : char = ReturncharVal(...);");
	try {
		var a18 : char = ReturncharVal("5");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.number, e.description, "");
		}
	}
	verify (a18, "5", "18.2 Wrong value","");
	verify (a18.GetType(), "System.Char", "18.3 Wrong data type","");


	apEndTest();
}


typann24();


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
