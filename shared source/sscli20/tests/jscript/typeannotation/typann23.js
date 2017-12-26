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


var iTestID = 160223;

/* -------------------------------------------------------------------------
  Test: 	TYPANN23
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  var int : int;

		2.  var long : long;

		3.  var float : float;

		4.  var double : double;

		5.  var q : void;

		6.  var q : int[3];

		7.  var q : int[] = [];


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

var ArrayTypeString = "System.Int32[]";

function typann23() {

    apInitTest("typann23: Type annotation -- Object types"); 

	apInitScenario("1.  var int : int;");
	var int : int;
	int = 3;
	verify (int, 3, "1.1 Wrong value","");
	verify (int.GetType(), "System.Int32", "1.2 Wrong value","");
	var i1 : int = 4;
	verify (i1, 4, "1.3 Wrong value","");
	verify (i1.GetType(), "System.Int32", "1.4 Wrong value","");


	apInitScenario("2.  var long : long;");
	var long : long;
	long = 34;
	verify (long, 34, "2.1 Wrong value","");
	verify (long.GetType(), "System.Int64", "2.2 Wrong value","");
	var i2 : long = 4;
	verify (i2, 4, "2.3 Wrong value","");
	verify (i2.GetType(), "System.Int64", "2.4 Wrong value","");


	apInitScenario("3.  var float : float;");
	var float : float;
	float = 34;
	verify (float, 34, "3.1 Wrong value","");
	verify (float.GetType(), "System.Single", "3.2 Wrong value","");
	var i3 : float = 4;
	verify (i3, 4, "3.3 Wrong value","");
	verify (i3.GetType(), "System.Single", "3.4 Wrong value","");


	apInitScenario("4.  var double : double;");
	var double : double;
	double = 34;
	verify (double, 34, "4.1 Wrong value","");
	verify (double.GetType(), "System.Double", "4.2 Wrong value","");
	var i4 : double = 4;
	verify (i4, 4, "4.3 Wrong value","");
	verify (i4.GetType(), "System.Double", "4.4 Wrong value","");


	apInitScenario("5.  var q : void;");
	try {
		var q5 : void;
	}
	catch (e) {
		verify (e.number, 0, "5.1 Wrong error","");
	}


	apInitScenario("6.  var q : int[] = [...];");
	var q6 : int[] = [34,42,51];
	verify (q6[0], 34, "6.1 Wrong value","");
	verify (q6[1], 42, "6.2 Wrong value","");
	q6[1] = 34;
	verify (q6[1], 34, "6.3 Wrong value","");
	verify (q6[2], 51, "6.4 Wrong value","");
	verify (q6.GetType(), ArrayTypeString, "6.5 Wrong value","");


	apInitScenario("7.  var q : int[] = [];");
	var q7 : int[] = [];
	verify (q7.GetType(), ArrayTypeString, "7.2 Wrong value","");


	apEndTest();
}


typann23();


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
