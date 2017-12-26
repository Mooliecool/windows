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


var iTestID = 74874;

/* -------------------------------------------------------------------------
  Test: 	DTFUNC01
   
  
 
  Component:	JScript
 
  Major Area:	Date Object
 
  Test Area:	Changes in functionality from 5.1 to 5.5/7.0
 
  Keywords:	date function object
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Zero arguments

		2.  One argument

		3.  Two arguments

		4.  Three arguments

		5.  Four arguments

		6.  Five arguments

		7.  Six arguments

		8.  Seven arguments

		9.  Eight arguments


  Abstract:	 All arguments are optional; any arguments supplied
			 are accepted but completely ignored. A string is created
			 and returned as if by the expression (new Date()).toString().
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp , sAct, "");
}

function DateCompare (d1, d2) {
	if (Math.abs(d2 - d1) > 1000) {
		return false;
	}
	else {
		return true;
	}
}

//////////
//
// Global variables


var dDate1;
var dDate2;


function dtfunc01() {

    apInitTest("dtfunc01: Date() function -- all arguments are optional and accepted, but completely ignored"); 

        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	dDate1 = new Date();

	apInitScenario("1 No arguments");
	dDate2 = Date();
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "1.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "1.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "1.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "1.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "1.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "1.7 Wrong date returned");


	apInitScenario("2 One argument");
	dDate2 = Date(2000);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "2.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "2.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "2.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "2.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "2.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "2.7 Wrong date returned");


	apInitScenario("3 Two arguments");
	dDate2 = Date(2000,0);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "3.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "3.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "3.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "3.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "3.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "3.7 Wrong date returned");


	apInitScenario("4 Three arguments");
	dDate2 = Date(2000,0,3);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "4.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "4.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "4.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "4.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "4.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "4.7 Wrong date returned");


	apInitScenario("5 Four arguments");
	dDate2 = Date(2000,0,3,8);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "5.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "5.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "5.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "5.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "5.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "5.7 Wrong date returned");


	apInitScenario("6 Five arguments");
	dDate2 = Date(2000,0,3,8,15);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "6.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "6.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "6.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "6.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "6.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "6.7 Wrong date returned");


	apInitScenario("7 Six arguments");
	dDate2 = Date(2000,0,3,8,15,31);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "7.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "7.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "7.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "7.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "7.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "7.7 Wrong date returned");


	apInitScenario("8 Seven arguments");
	dDate2 = Date(2000,0,3,8,15,31,686);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "8.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "8.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "8.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "8.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "8.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "8.7 Wrong date returned");


	apInitScenario("9 Eight arguments");
	dDate2 = Date(2000,0,3,8,15,31,686,40);
	verify (dDate1.getFullYear(), (new Date(Date.parse(dDate2))).getFullYear(), "9.1 Wrong date returned");
	verify (dDate1.getMonth(), (new Date(Date.parse(dDate2))).getMonth(), "9.2 Wrong date returned");
	verify (dDate1.getDate(), (new Date(Date.parse(dDate2))).getDate(), "9.3 Wrong date returned");
	verify (dDate1.getHours(), (new Date(Date.parse(dDate2))).getHours(), "9.4 Wrong date returned");
	verify (dDate1.getMinutes(), (new Date(Date.parse(dDate2))).getMinutes(), "9.5 Wrong date returned");
	verify (true, DateCompare(dDate1.valueOf(), Date.parse(dDate2)), "9.7 Wrong date returned");

        }
	apEndTest();
}

dtfunc01();


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
