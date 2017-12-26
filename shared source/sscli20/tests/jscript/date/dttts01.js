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


var iTestID = 74878;

/* -------------------------------------------------------------------------
  Test: 	DTTTS01
   
  
 
  Component:	JScript
 
  Major Area:	Date Object
 
  Test Area:	Changes in functionality from 5.1 to 5.5/7.0
 
  Keywords:	new Date object constructor UTC toDateString toTimeString
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features in Date.toTimeString()
 
  Scenarios:
		1.  00:00:00:000

		2.  11:59:59:999

		3.  12:00:00:00

		4.  23:59:59:999


  Abstract:	 Date.toDateString() -- New Method.
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
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}



//////////
//
// Global variables


var dDate1;



function dttts01() {


    apInitTest("dttts01: Date.toTimeString() -- new method for JScript 5.5"); 
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
 

	apInitScenario("1.  00:00:00:000");
	dDate1 = new Date(2000,0,1,0,0,0,0);
	verify (dDate1.toTimeString(), "00:00:00 PST", "1 Wrong date string returned");	


	apInitScenario("2.  11:59:59:999");
	dDate1 = new Date(2000,0,1,11,59,59,999);
	verify (dDate1.toTimeString(), "11:59:59 PST", "2 Wrong date string returned");	


	apInitScenario("3.  12:00:00:000");
	dDate1 = new Date(2000,0,1,12,0,0,0);
	verify (dDate1.toTimeString(), "12:00:00 PST", "3 Wrong date string returned");	


	apInitScenario("4.  23:59:59:999");
	dDate1 = new Date(2000,0,1,23,59,59,999);
	verify (dDate1.toTimeString(), "23:59:59 PST", "4 Wrong date string returned");	


        }
	apEndTest();
}

dttts01();


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
