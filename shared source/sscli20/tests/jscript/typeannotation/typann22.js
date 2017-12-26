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


var iTestID = 160222;

/* -------------------------------------------------------------------------
  Test: 	TYPANN22
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  function foo (...) : int

		2.  function foo (...) : long

		3.  function foo (...) : float

		4.  function foo (...) : double

		5.  function foo (...) : ActiveXObject

		6.  function foo (...) : Array

		7.  function foo (...) : Boolean

		8.  function foo (...) : Date

		9.  function foo (...) : Enumerator

		10. function foo (...) : Error

		11. function foo (...) : Function

		12. function foo (...) : Number

		13. function foo (...) : RegExp

		14. function foo (...) : String

		15. function foo (...) : Person

		16. function foo (...) : Int16

		17. function foo (...) : System.String

		18. function foo (...) : void


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




function foo01 (T) : int { return T; }
function foo02 (T) : long { return T; }
function foo03 (T) : float { return T; }
function foo04 (T) : double { return T; }
@if(!@rotor)
function foo05 (T) : ActiveXObject { return new ActiveXObject("Scripting.FileSystemObject"); }
@end
function foo06 (T) : Array { return new Array(); }
function foo07 (T) : Boolean { return false; }
function foo08 (T) : Date {	return new Date(); }

function foo10 (T) : Error { return new Error(2047); }
/*function foo11 (T) : Function { return foo10; }*/
function foo12 (T) : Number { return new Number(3.14159); }
function foo13 (T) : RegExp { return new RegExp("abc"); }
function foo14 (T) : String { return new String("def"); }
function foo15 (T) : Person { return new Person(); }
function foo16 (T) : Int16 { var x : Int16 = 3; return x; }
function foo17 (T) : System.String { var s : System.String = "Boo yah"; return s; }
function foo18 (T) : void { return; }

function typann22() {

    apInitTest("typann22: Type annotation -- Object types"); 

	apInitScenario("1.  function foo (...) : int");
	verify (foo01(34).GetType(), "System.Int32", "1.1 Wrong type","");


	apInitScenario("2.  function foo (...) : long");
	verify (foo02(34).GetType(), "System.Int64", "2.1 Wrong type","");


	apInitScenario("3.  function foo (...) : float");
	verify (foo03(34).GetType(), "System.Single", "3.1 Wrong type","");


	apInitScenario("4.  function foo (...) : double");
	verify (foo04(34).GetType(), "System.Double", "4.1 Wrong type","");

@if(!@rotor)
	apInitScenario("5.  function foo (...) : ActiveXObject");
	verify (foo05(34).GetType(), "System.__ComObject", "5.1 Wrong type","");
@end

	apInitScenario("6.  function foo (...) : Array");
	verify (foo06(34).GetType(), "Microsoft.JScript.ArrayObject", "6.1 Wrong type","");


	apInitScenario("7.  function foo (...) : Boolean");
	verify (foo07(34).GetType(), "System.Boolean", "7.1 Wrong type","");


	apInitScenario("8.  function foo (...) : Date");
	verify (foo08(34).GetType(), "Microsoft.JScript.DateObject", "8.1 Wrong type","");


//	apInitScenario("9.  function foo (...) : Enumerator");

/*
		Spec issue pending
*/


	apInitScenario("10. function foo (...) : Error");
	verify (foo10(34).GetType(), "Microsoft.JScript.ErrorObject", "10.1 Wrong type","");


/*	apInitScenario("11. function foo (...) : Function");
	verify (foo11(34).GetType(), "Microsoft.JScript.Closure", "11.1 Wrong type","");
*/

	apInitScenario("12. function foo (...) : Number");
	verify (foo12(34).GetType(), "System.Double", "12.1 Wrong type","");


	apInitScenario("13. function foo (...) : RegExp");
	verify (foo13(34).GetType(), "Microsoft.JScript.RegExpObject", "13.1 Wrong type","");


	apInitScenario("14. function foo (...) : String");
	verify (foo14(34).GetType(), "System.String", "14.1 Wrong type","");


	apInitScenario("15. function foo (...) : Person");
	verify (foo15(34).GetType(), "Person", "15.1 Wrong type","");


	apInitScenario("16. function foo (...) : Int16");
	verify (foo16(34).GetType(), "System.Int16", "16.1 Wrong type","");


	apInitScenario("17. function foo (...) : System.String");
	verify (foo17(34).GetType(), "System.String", "17.1 Wrong type","");


	apEndTest();
}


typann22();


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
