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


var iTestID = 74877;

/* -------------------------------------------------------------------------
  Test: 	DTTDS01
   
  
 
  Component:	JScript
 
  Major Area:	Date Object
 
  Test Area:	Changes in functionality from 5.1 to 5.5/7.0
 
  Keywords:	new Date object constructor UTC toDateString toTimeString
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features in Date.toDateString()
 
  Scenarios:
		1.  12/31/1 BC

		2.  1/1/1 AD

		3.  12/31/1999

		4.  1/1/2000

		5.  January

		6.  February

		7.  March

		8.  April

		9.  May

		10. June

		11. July

		12. August

		13. September

		14. October

		15. November

		16. December

		17. Sunday

		18. Monday

		19. Tuesday

		20. Wednesday

		21. Thursday

		22. Friday

		23. Saturday

		24. Date.valueOf() == 8640000000000000 (largest AD date value)

		25. Date.valueOf() == -8640000000000000 (largest BC date value)


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



function dttds01() {


    apInitTest("dttdc01: Date.toDateString() -- new method for JScript 5.5"); 


	apInitScenario("1.  12/31/1 BC");
	dDate1 = new Date(-1,23,31);
	verify (dDate1.toDateString(), "Sun Dec 31 1 B.C.", "1 Wrong date string returned");	


	apInitScenario("2.  1/1/1 AD");
	dDate1 = new Date(-1,24,1);
	verify (dDate1.toDateString(), "Mon Jan 1 1", "2 Wrong date string returned");	


	apInitScenario("3.  12/31/1999");
	dDate1 = new Date(1999,11,31);
	verify (dDate1.toDateString(), "Fri Dec 31 1999", "3 Wrong date string returned");	


	apInitScenario("4.  12/31/1999");
	dDate1 = new Date(2000,0,1);
	verify (dDate1.toDateString(), "Sat Jan 1 2000", "4 Wrong date string returned");	


	apInitScenario("5.  January");
	dDate1 = new Date(2000,0,1);
	verify ((dDate1.toDateString()).substr(4,3), "Jan", "5 Wrong date string returned");	


	apInitScenario("6.  February");
	dDate1 = new Date(2000,1,1);
	verify ((dDate1.toDateString()).substr(4,3), "Feb", "6 Wrong date string returned");	


	apInitScenario("7.  March");
	dDate1 = new Date(2000,2,1);
	verify ((dDate1.toDateString()).substr(4,3), "Mar", "7 Wrong date string returned");	


	apInitScenario("8.  April");
	dDate1 = new Date(2000,3,1);
	verify ((dDate1.toDateString()).substr(4,3), "Apr", "8 Wrong date string returned");	


	apInitScenario("9.  May");
	dDate1 = new Date(2000,4,1);
	verify ((dDate1.toDateString()).substr(4,3), "May", "9 Wrong date string returned");	


	apInitScenario("10.  June");
	dDate1 = new Date(2000,5,1);
	verify ((dDate1.toDateString()).substr(4,3), "Jun", "10 Wrong date string returned");	


	apInitScenario("11.  July");
	dDate1 = new Date(2000,6,1);
	verify ((dDate1.toDateString()).substr(4,3), "Jul", "11 Wrong date string returned");	


	apInitScenario("12.  August");
	dDate1 = new Date(2000,7,1);
	verify ((dDate1.toDateString()).substr(4,3), "Aug", "12 Wrong date string returned");	


	apInitScenario("13.  September");
	dDate1 = new Date(2000,8,1);
	verify ((dDate1.toDateString()).substr(4,3), "Sep", "13 Wrong date string returned");	


	apInitScenario("14.  October");
	dDate1 = new Date(2000,9,1);
	verify ((dDate1.toDateString()).substr(4,3), "Oct", "14 Wrong date string returned");	


	apInitScenario("15.  November");
	dDate1 = new Date(2000,10,1);
	verify ((dDate1.toDateString()).substr(4,3), "Nov", "15 Wrong date string returned");	


	apInitScenario("16.  December");
	dDate1 = new Date(2000,11,1);
	verify ((dDate1.toDateString()).substr(4,3), "Dec", "16 Wrong date string returned");	


	apInitScenario("17.  Sunday");
	dDate1 = new Date(2000,0,2);
	verify ((dDate1.toDateString()).substr(0,3), "Sun", "17 Wrong date string returned");	


	apInitScenario("18.  Monday");
	dDate1 = new Date(2000,0,3);
	verify ((dDate1.toDateString()).substr(0,3), "Mon", "18 Wrong date string returned");	


	apInitScenario("19.  Tuesday");
	dDate1 = new Date(2000,0,4);
	verify ((dDate1.toDateString()).substr(0,3), "Tue", "19 Wrong date string returned");	


	apInitScenario("20.  Wednesday");
	dDate1 = new Date(2000,0,5);
	verify ((dDate1.toDateString()).substr(0,3), "Wed", "20 Wrong date string returned");	


	apInitScenario("21.  Thursday");
	dDate1 = new Date(2000,0,6);
	verify ((dDate1.toDateString()).substr(0,3), "Thu", "21 Wrong date string returned");	


	apInitScenario("22.  Friday");
	dDate1 = new Date(2000,0,7);
	verify ((dDate1.toDateString()).substr(0,3), "Fri", "22 Wrong date string returned");	


	apInitScenario("23.  Saturday");
	dDate1 = new Date(2000,0,8);
	verify ((dDate1.toDateString()).substr(0,3), "Sat", "23 Wrong date string returned");	


	apInitScenario("24. Date.valueOf() == 8640000000000000 (largest AD date value)");
	dDate1 = new Date(8640000000000000);
	verify (dDate1.toDateString(), "Fri Sep 12 275760", "24 Wrong date string returned");	


	apInitScenario("25. Date.valueOf() == -8640000000000000 (largest BC date value)");
	dDate1 = new Date(-8640000000000000);
	verify (dDate1.toDateString(), "Mon Apr 19 271822 B.C.", "25 Wrong date string returned");	


	apEndTest();
}

dttds01();


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
