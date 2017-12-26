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


var iTestID = 160213;

/* -------------------------------------------------------------------------
  Test: 	TYPANN13
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var n : Number;

		2.  var n : Number = 3;

		3.  var n : Number = Math.pow(3,7);

		4.  var n : Number = (date.parse ('3/2/2000'));

		5.  var n : Number = (Int32 value);

		6.  var n : Number = undefined;

		7.  var n : Number; n = 3;

		8.  var n : Number = 'hello';

		9.  var n : Number = '5';

		10. var n : Number = '5abc';

		11. var n : Number = ' 5';

		12. var n : Number = '5 ';

		13. var n : Number = MAX_VAL;

		14. var n : Number = MIN_VAL;

		15. var n : Number = POSITIVE_INFINITY;

		16. var n : Number = NEGATIVE_INFINITY;

		17. var n : Number = NaN;

		18. var n : Number = true;

		19. var n : Number = ReturnNumberVal(...);

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


function ReturnNumberVal() {
	var n = new Number(21);

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


import System

function typann13() {

    apInitTest("typann13: Type annotation -- Object types"); 

	apInitScenario("1.  var n : Number;");
	try {
		var n1 : Number;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (n1, 0, "1.2 Wrong value","");
	verify (n1.GetType(), "System.Double", "1.3 Wrong data type","");


	apInitScenario("2.  var n : Number = 3;");
	try {
		var n2 : Number = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (n2, 3, "2.2 Wrong value","");
	verify (n2.GetType(), "System.Double", "2.3 Wrong data type","");


	apInitScenario("3.  var n : Number = Math.pow(3,7);");
	try {
		var n3 : Number = Math.pow(3,7);
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", e.description, "error", "");
		}
	}
	verify (n3, 2187, "3.2 Wrong value","");
	verify (n3.GetType(), "System.Double", "3.3 Wrong data type","");


	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario("4.  var n : Number = (date.parse ('3/2/2000'));");
	try {
		var n4 : Number = Date.parse ('3/2/2000');
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (n4, 951984000000, "4.2 Wrong value","");
	verify (n4.GetType(), "System.Double", "4.3 Wrong data type","");
	}

	apInitScenario("5.  var n : Number = (Int32 value);");
	try {
		var n5 : Number = 2147483647;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", e.description, "error", "");
		}
	}
	verify (n5, 2147483647, "5.2 Wrong value","");
	verify (n5.GetType(), "System.Double", "5.3 Wrong data type","");


	apInitScenario("6.  var n : Number = undefined;");
	try {
		var n6 : Number = undefined;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 6.1 Error returned", e.description, "error", "");
		}
	}
	verify (n6.toString(), "NaN", "6.2 Wrong value","");
	verify (n6.GetType(), "System.Double", "6.3 Wrong data type","");


	apInitScenario("7.  var n : Number; n = 3;");
	try {
		var n7 : Number;
		n7 = 3;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 7.1 Error returned", e.description, "error", "");
		}
	}
	verify (n7, 3, "7.2 Wrong value","");
	verify (n7.GetType(), "System.Double", "7.3 Wrong data type","");


	apInitScenario("8.  var n : Number = 'hello';");
	try {
		var n8 : Number = eval ("new String('hello')");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 8.1 Error returned", e.description, "error", "");
		}
	}
	verify (n8.toString(), "NaN", "8.2 Wrong value","");
	verify (n8.GetType(), "System.Double", "8.3 Wrong data type","");


	apInitScenario("9.  var n : Number = '5';");
	try {
		var n9 : Number = '5';
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 9.1 Error returned", e.description, "error", "");
		}
	}
	verify (n9, 5, "9.2 Wrong value","");
	verify (n9.GetType(), "System.Double", "9.3 Wrong data type","");


	apInitScenario("10. var n : Number = '5abc';");
	try {
		var n10 : Number = '5abc';
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 10.1 Error returned", e.description, "error", "");
		}
	}
	verify (n10.toString(), "NaN", "10.2 Wrong value","");
	verify (n10.GetType(), "System.Double", "10.3 Wrong data type","");


	apInitScenario("11. var n : Number = ' 5';");
	try {
		var n11 : Number = ' 5';
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 11.1 Error returned", e.description, "error", "");
		}
	}
	verify (n11, 5, "11.2 Wrong value","");
	verify (n11.GetType(), "System.Double", "11.3 Wrong data type","");


	apInitScenario("12. var n : Number = '5 ';");
	try {
		var n12 : Number = ' 5';
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 12.1 Error returned", e.description, "error", "");
		}
	}
	verify (n12, 5, "12.2 Wrong value","");
	verify (n12.GetType(), "System.Double", "12.3 Wrong data type","");


	apInitScenario("13. var n : Number = Number.MAX_VALUE;");
	try {
		var n13 : Number = Number.MAX_VALUE;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 13.1 Error returned", e.description, "error", "");
		}
	}
	verify (n13, 1.7976931348623157e+308, "13.2 Wrong value","");
	verify (n13.GetType(), "System.Double", "13.3 Wrong data type","");


	apInitScenario("14. var n : Number = MIN_VAL;");
	try {
		var n14 : Number = Number.MIN_VALUE;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 14.1 Error returned", e.description, "error", "");
		}
	}
	verify (n14, 4.94065645841247e-324, "14.2 Wrong value","");
	verify (n14.GetType(), "System.Double", "14.3 Wrong data type","");


	apInitScenario("15. var n : Number = Number.POSITIVE_INFINITY;");
	try {
		var n15 : Number = Number.POSITIVE_INFINITY;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 15.1 Error returned", e.description, "error", "");
		}
	}
	verify (n15, Infinity, "15.2 Wrong value","");
	verify (n15.GetType(), "System.Double", "15.3 Wrong data type","");


	apInitScenario("16. var n : Number = Number.NEGATIVE_INFINITY;");
	try {
		var n16 : Number = Number.NEGATIVE_INFINITY;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 16.1 Error returned", e.description, "error", "");
		}
	}
	verify (n16, -Infinity, "16.2 Wrong value","");
	verify (n16.GetType(), "System.Double", "16.3 Wrong data type","");


	apInitScenario("17. var n : Number = NaN;");
	try {
		var n17 : Number = NaN;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 17.1 Error returned", e.description, "error", "");
		}
	}
	verify (n17.toString(), "NaN", "17.2 Wrong value","");
	verify (n17.GetType(), "System.Double", "17.3 Wrong data type","");


	apInitScenario("18. var n : Number = true;");
	try {
		var n18 : Number = true;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 18.1 Error returned", e.description, "error", "");
		}
	}
	verify (n18, 1, "18.2 Wrong value","");
	verify (n18.GetType(), "System.Double", "18.3 Wrong data type","");


	apInitScenario("19. var n : Number = ReturnNumberVal(...);");
	try {
		var n19 : Number = ReturnNumberVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 19.1 Error returned", e.description, "error", "");
		}
	}
	verify (n19, 21, "19.2 Wrong value","");
	verify (n19.GetType(), "System.Double", "19.3 Wrong data type","");


	apEndTest();
}

typann13();


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
