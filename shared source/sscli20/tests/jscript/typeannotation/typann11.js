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


var iTestID = 160211;

/* -------------------------------------------------------------------------
  Test: 	TYPANN11
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var foo : Function;

		2.  var foo : Function = new Function ('return 3;');

		3.  var foo : Function = new Function ('return this.myVar = 3;');

		4.  var foo : Function = Function ('return 5;');

		5.  var foo : Function = function (x,y,z) { return x + y + z; };

		6.  var foo : Function = 3;

		7.  var foo : Function = 'hello';

		8.  var foo : Function = true;

		9.  var foo : Function = ReturnFunctionVal(...);

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


function ReturnFunctionVal() {
	return new Function("return 42;");
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

function typann11() {

    apInitTest("typann11: Type annotation -- Object types"); 

	apInitScenario("1.  var foo : Function;");
	try {
		var f1 : Function;
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 1.1 Error returned", e.description, "error", "");
		}
	}
	verify (f1, undefined, "1.2 Wrong value","");


	apInitScenario("2.  var foo : Function = new Function ('return 3;');");
	try {
		var f2 : Function = new Function ("return 3;");
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 2.1 Error returned", "a is an double", "error", "");
		}
	}
	verify (escape(f2), "function%20anonymous%28%29%20%7B%0Areturn%203%3B%0A%7D", "2.2 Wrong value","");
	verify (f2.GetType(), "Microsoft.JScript.Closure", "2.3 Wrong data type","");


	apInitScenario("3.  var foo : Function = new Function ('return this.myVar = 3;');");
	try {
		var f3 : Function = new Function ('return this.myVar = 3;');
	}
	catch (e) {
		if (e.description != "An exception of type System.InvalidCastException was thrown.") {
	        apLogFailInfo( "*** Scenario failed: 3.1 Error returned", e.description, "error", "");
		}
	}
	verify (escape(f3), "function%20anonymous%28%29%20%7B%0Areturn%20this.myVar%20%3D%203%3B%0A%7D", "3.2 Wrong value","");
	verify (f3.GetType(), "Microsoft.JScript.Closure", "3.3 Wrong data type","");


	apInitScenario("4.  var foo : Function = Function ('return 5;');");
	try {
		var f4 : Function = Function ('return 5;');
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 4.1 Error returned", e.description, "error", "");
		}
	}
	verify (escape(f4), "function%20anonymous%28%29%20%7B%0Areturn%205%3B%0A%7D", "4.2 Wrong value","");
	verify (f4.GetType(), "Microsoft.JScript.Closure", "4.3 Wrong data type","");


	apInitScenario("5.  var foo : Function = function (x,y,z) { return x + y + z; };");
	try {
		var f5 : Function = function (x,y,z) { return x + y + z; };
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 5.1 Error returned", e.description, "error", "");
		}
	}
	verify (escape(f5), "function%20%28x%2Cy%2Cz%29%20%7B%20return%20x%20+%20y%20+%20z%3B%20%7D", "5.2 Wrong value","");
	verify (f5.GetType(), "Microsoft.JScript.Closure", "5.3 Wrong data type","");


	apInitScenario("6.  var foo : Function = 3;");
	try {
		var f6 : Function = eval(3);
	}
	catch (e) {
		verify (e.number, ICError, "6.1 Wrong error","");
	}
	verify (f6, undefined, "6.2 Wrong value","");


	apInitScenario("7.  var foo : Function = 'hello';");
	try {
		var f7 : Function = eval("new String('hello')");
	}
	catch (e) {
		verify (e.number, ICError, "7.1 Wrong error","");
	}
	verify (f7, undefined, "7.2 Wrong value","");


	apInitScenario("8.  var foo : Function = true;");
	try {
		var f8 : Function = eval("true");
	}
	catch (e) {
		verify (e.number, ICError, "8.1 Wrong error","");
	}
	verify (f8, undefined, "8.2 Wrong value","");


	apInitScenario("9.  var foo : Function = ReturnFunctionVal(...);");
	try {
		var f9 : Function = ReturnFunctionVal();
	}
	catch (e) {
		if (e.number != 0) {
	        apLogFailInfo( "*** Scenario failed: 9.1 Error returned", e.description, "error", "");
		}
	}
	verify (escape(f9), "function%20anonymous%28%29%20%7B%0Areturn%2042%3B%0A%7D", "9.2 Wrong value","");


	apEndTest();
}


typann11();


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
