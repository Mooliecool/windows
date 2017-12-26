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


var iTestID = 160220;

/* -------------------------------------------------------------------------
  Test: 	TYPANN20
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  function foo (I : int)

		2.  function foo (l : long)

		3.  function foo (f : float)

		4.  function foo (d : double)

		5.  function foo (fso : ActiveXObject)

		6.  function foo (arr : Array)

		7.  function foo (b : Boolean)

		8.  function foo (d : Date)

		9.  function foo (enum : Enumerator)

		10. function foo (e : Error)

		11. function foo (func : Function)

		12. function foo (n : Number)

		13. function foo (re : RegExp)

		14. function foo (str : String)

		15. function foo (o : Person)

		16. function foo (i16 : int16)

		17. function foo (strS : System.String)


  Abstract:	 Testing variable assignment through type annotation..
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


-------------------------------------------------------------------------*/

import System

/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


function ReturnSysStringVal() {
	var n : System.String = "42";

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

	class Student extends Person {
		private var iGPA : float;
	
		public function GetGPA() {
			return iGPA;
		}
		
		public function SetGPA(iNewGPA) {
			iGPA = iNewGPA;
		}
	}




function foo01 (I : int) { if ((I % 2) == 0) { return true; } else { return false; } }
function foo02 (l : long) { if ((l % 2) == 0) { return true; } else { return false; } }
function foo03 (f : float) { if (Math.floor(f) != f) { return true; } else { return false; } }
function foo04 (d : double) { if (d == System.Double.MaxValue) { return true; } else { return false; } }
@if(!@rotor)
function foo05 (fso : ActiveXObject) { return fso.DriveExists("C:"); }
@end
function foo06 (a : Array) { if (a.length == 4) { return true; } else { return false; } }
function foo07 (b : Boolean) { return b; }
function foo08 (d : Date) { if (d.valueOf() > 0) { return true; } else { return false; } }

function foo10 (e : Error) { if (e.number == 2047) { return true; } else { return false; } }
//function foo11 (f : Function) { return f(new Error(2047)); }
function foo12 (n : Number) { if (n == 42) { return true; } else { return false; } }
function foo13 (r : RegExp) { if (("hello world".split(r)).length == 2) { return true; } else { return false; } }
function foo14 (s : String) { if (s.length == 4) { return true; } else { return false; } }
function foo15 (o : Person) { if (o.GetAge() == 34) { return true; } else { return false; } }
function foo16 (i : Int16) { if (i == 32765) { return true; } else { return false; } }
function foo17 (s : System.String) { if (s == "Boo yah") { return true; } else { return false; } }


function typann20() {

    apInitTest("typann20: Type annotation -- Object types"); 

	apInitScenario("1.  function foo (I : int)");
	verify (foo01(34), true, "1.1 Wrong value","");
	try {
		eval("foo01('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 1.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("2.  function foo (l : long)");
	verify (foo02((new Date("1/1/2001")).valueOf()), true, "2.1 Wrong value","");
	try {
		eval("foo02('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 2.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("3.  function foo (f : float)");
	verify (foo03(3.14159), true, "3.1 Wrong value","");
	try {
		eval("foo03('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 3.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("4.  function foo (d : double)");
	verify (foo04(1.7976931348623157e+308), true, "4.1 Wrong value","");
	try {
		eval("foo04('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 4.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("5.  function foo (fso : ActiveXObject)");
@if(!@rotor)
	verify (foo05(new ActiveXObject("Scripting.FileSystemObject")), true, "5.1 Wrong value","");
	try {
		eval("foo05('hi')");
	}
	catch (e) {
		if (e.description != "Function expected") {
	        apLogFailInfo( "*** Scenario failed: 5.2 Wrong Error returned", e.description, "error", "");
		}
	}
@end

	apInitScenario("6.  function foo (arr : Array)");
	verify (foo06(new Array(3,4,5,6)), true, "6.1 Wrong value","");
	try {
		eval("foo06('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 6.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("7.  function foo (b : Boolean)");
	verify (foo07(new Boolean(true)), true, "7.1 Wrong value","");
	try {
		eval("foo07('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 7.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("8.  function foo (d : Date)");
	verify (foo08(new Date()), true, "8.1 Wrong value","");
	try {
		eval("foo08('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 8.2 Wrong Error returned", e.description, "error", "");
		}
	}


//	apInitScenario("9.  function foo (enum : Enumerator)");

/*
		Spec issue pending
*/


	apInitScenario("10. function foo (e : Error)");
	verify (foo10(new Error(2047)), true, "10.1 Wrong value","");
	try {
		eval("foo10('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 10.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("11. function foo (func : Function)");


/*	verify (foo11(foo10), true, "11.1 Wrong value","");
	try {
		eval("foo11('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 11.2 Wrong Error returned", e.description, "error", "");
		}
	}
*/

	apInitScenario("12. function foo (n : Number)");
	verify (foo12(new Number(42)), true, "12.1 Wrong value","");
	try {
		eval("foo12('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 12.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("13. function foo (re : RegExp)");
	verify (foo13(new RegExp("\\s")), true, "13.1 Wrong value","");
	try {
		eval("foo13('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 13.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("14. function foo (str : String)");
	verify (foo14(new String("Blah")), true, "14.1 Wrong value","");
	try {
		eval("foo14(34)");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 14.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("15. function foo (o : Person)");
	var o15 : Person = new Person();
	o15.SetAge(34);
	verify (foo15(o15), true, "15.1 Wrong value","");
	try {
		eval("foo15('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 15.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("16. function foo (i16 : int16)");
	var i16 : Int16 = 32765;
	verify (foo16(i16), true, "16.1 Wrong value","");
	try {
		eval("foo16('hi')");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 16.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apInitScenario("17. function foo (strS : System.String)");
	var s17 : System.String = "Boo yah";
	verify (foo17(s17), true, "17.1 Wrong value","");
	try {
		eval("foo17(34)");
	}
	catch (e) {
		if (e.description != "Type mismatch") {
	        apLogFailInfo( "*** Scenario failed: 17.2 Wrong Error returned", e.description, "error", "");
		}
	}


	apEndTest();
}


typann20();


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
