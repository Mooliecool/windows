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


var iTestID = 161334;

/* -------------------------------------------------------------------------
  Test: 	TYPCON15
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion round trips
 
  Keywords:	type conversion object round trips
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Number object > String object > Number object

		2.  String (numeric) object > Number object > String object

		3.  Boolean object > String object > Boolean object

		4.  String object ('false') > Boolean object > String object

		5.  Number object (0) > Boolean object >  Number object

		6.  Boolean object > Number object >  Boolean object


  Abstract:	 Testing that type conversion round trips
			 produce the expected results.
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
	if (sBug == undefined) {
		sBug = "";
	}
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


/*----------
/
/  Global variables
/
----------*/


@if(!@aspx)
	import System
@end

function typcon15() {

    apInitTest("typcon15: Type conversion -- Conversion round trips"); 

	var obj1;
	var obj2;
	var result;

	apInitScenario("1.  Number object > String object > Number object");
	obj1 = new Number(34.56);
	obj2 = String(obj1);
	verify (obj2.GetType(), "System.String", "1.1 Wrong data type","");
	verify (obj2, "34.56", "1.2 Wrong value","");
	result = Number(obj2);
	verify (result.GetType(), "System.Double", "1.3 Wrong data type","");
	verify (result, 34.56, "1.4 Wrong value","");


	apInitScenario("2.  String (numeric) object > Number object > String object");
	obj1 = new String("34.56");
	obj2 = Number(obj1);
	verify (obj2.GetType(), "System.Double", "2.1 Wrong data type","");
	verify (obj2, 34.56, "2.2 Wrong value","");
	result = String(obj2);
	verify (result.GetType(), "System.String", "2.3 Wrong data type","");
	verify (result, "34.56", "2.4 Wrong value","");


	apInitScenario("3.  Boolean object > String object > Boolean object");
	obj1 = new Boolean(false);
	obj2 = String(obj1);
	verify (obj2.GetType(), "System.String", "3.1 Wrong data type","");
	verify (obj2, "false", "3.2 Wrong value","");
	result = Boolean(obj2);
	verify (result.GetType(), "System.Boolean", "3.3 Wrong data type","");
	verify (result, true, "3.4 Wrong value","");
	// This is the correct result because any string that has length > 0
	// will be evaluated as boolean true


	apInitScenario("4.  String object ('false') > Boolean object > String object");
	obj1 = new String("false");
	obj2 = Boolean(obj1);
	verify (obj2.GetType(), "System.Boolean", "4.1 Wrong data type","");
	verify (obj2, true, "4.2 Wrong value","");
	// This is the correct result because any string that has length > 0
	// will be evaluated as boolean true
	result = String(obj2);
	verify (result.GetType(), "System.String", "4.3 Wrong data type","");
	verify (result, "true", "4.4 Wrong value","");


	apInitScenario("5.  Number object (0) > Boolean object >  Number object");
	obj1 = new Number(0);
	obj2 = Boolean(obj1);
	verify (obj2.GetType(), "System.Boolean", "5.1 Wrong data type","");
	verify (obj2, true, "5.2 Wrong value","");
	result = Number(obj2);
	verify (result.GetType(), "System.Double", "5.3 Wrong data type","");
	verify (result, "1", "5.4 Wrong value","");


	apInitScenario("6.  Boolean object > Number object >  Boolean object");
	obj1 = new Boolean(true);
	obj2 = Number(obj1);
	verify (obj2.GetType(), "System.Double", "6.1 Wrong data type","");
	verify (obj2, 1, "6.2 Wrong value","");
	result = Boolean(obj2);
	verify (result.GetType(), "System.Boolean", "6.3 Wrong data type","");
	verify (result, true, "6.4 Wrong value","");


	apEndTest();
}

typcon15();


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
