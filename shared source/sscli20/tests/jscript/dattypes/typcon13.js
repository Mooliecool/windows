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


var iTestID = 89104;

/* -------------------------------------------------------------------------
  Test: 	TYPCON13
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion on function return values
 
  Keywords:	type conversion object data variant function return
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Conversion from Boolean return value to Boolean Object
		2.  Conversion from Boolean return value to Number Object
		3.  Conversion from Boolean return value to String Object
		4.  Conversion from Boolean return value to RegExp Object

		5.  Conversion from Number return value to Boolean Object
		6.  Conversion from Number return value to Number Object
		7.  Conversion from Number return value to String Object
		8.  Conversion from Number return value to RegExp Object

		9.  Conversion from String return value to Boolean Object
		10. Conversion from String return value to Number Object
		11. Conversion from String return value to String Object
		12. Conversion from String return value to RegExp Object

		13. Conversion from RegExp return value to Boolean Object
		14. Conversion from RegExp return value to Number Object
		15. Conversion from RegExp return value to String Object
		16. Conversion from RegExp return value to RegExp Object

		17. Conversion from Date return value to Boolean Object
		18. Conversion from Date return value to Number Object
		19. Conversion from Date return value to String Object
		20. Conversion from Date return value to RegExp Object


  Abstract:	 Testing that the type conversion of function return values
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

function RetBool() {
	return true;
}

function RetNum() {
	return new Number(34);
}

function RetString() {
	return "My dog has fleas.";
}

function RetString2() {
	return "34";
}

function RetRegExp() {
	return new RegExp ("abc");
}

function RetRegExp2() {
	return new RegExp ("34");
}

function RetDate() {
	return new Date(2001,1,14);
}


/*----------
/
/  Global variables
/
----------*/


@if(!@aspx)
	import System
@end

function typcon13() {

    apInitTest("typcon13: Type conversion -- Conversion of function return values"); 

	var bResult : Boolean;
	var dResult : Date;
	var iResult : int;
	var strResult : String;
	var reResult : RegExp;


	apInitScenario("1.  Conversion from Boolean return value to Boolean Object");
	bResult = Boolean(RetBool());
	verify (String(bResult.GetType()), "System.Boolean", "1.1 Wrong data type","");
	verify (bResult, true, "1.2 Wrong value","");
	
	apInitScenario("2.  Conversion from Boolean return value to Number Object");
	iResult = Number(RetBool());
	verify (String(iResult.GetType()), "System.Int32", "2.1 Wrong data type","");
	verify (iResult, 1, "2.2 Wrong value","");

	apInitScenario("3.  Conversion from Boolean return value to String Object");
	strResult = String(RetBool());
	verify (String(strResult.GetType()), "System.String", "3.1 Wrong data type","");
	verify (strResult, "true", "3.2 Wrong value","");

	apInitScenario("4.  Conversion from Boolean return value to RegExp Object");
	reResult = RegExp(RetBool());
	verify (String(reResult.GetType()), "Microsoft.JScript.RegExpObject", "4.1 Wrong data type","");
	verify (reResult, "/true/", "4.2 Wrong value","");


	apInitScenario("5.  Conversion from Number return value to Boolean Object");
	bResult = Boolean(RetNum());
	verify (String(bResult.GetType()), "System.Boolean", "5.1 Wrong data type","");
	verify (bResult, true, "5.2 Wrong value","");
	
	apInitScenario("6.  Conversion from Number return value to Number Object");
	iResult = Number(RetNum());
	verify (String(iResult.GetType()), "System.Int32", "6.1 Wrong data type","");
	verify (iResult, 34, "6.2 Wrong value","");

	apInitScenario("7.  Conversion from Number return value to String Object");
	strResult = String(RetNum());
	verify (String(strResult.GetType()), "System.String", "7.1 Wrong data type","");
	verify (strResult, "34", "7.2 Wrong value","");

	apInitScenario("8.  Conversion from Number return value to RegExp Object");
	reResult = RegExp(RetNum());
	verify (String(reResult.GetType()), "Microsoft.JScript.RegExpObject", "8.1 Wrong data type","");
	verify (reResult, "/34/", "8.2 Wrong value","");


	apInitScenario("9.  Conversion from String return value to Boolean Object");
	bResult = Boolean(RetString());
	verify (String(bResult.GetType()), "System.Boolean", "9.1 Wrong data type","");
	verify (bResult, true, "9.2 Wrong value","");
	
	apInitScenario("10. Conversion from String return value to Number Object");
	try {
		iResult = Number(RetString());
	} catch (e) {
		verify (e.description, "Type mismatch","10.1 Wrong error","");
	}
	iResult = Number(RetString2());
	verify (String(iResult.GetType()), "System.Int32", "10.1 Wrong data type","");
	verify (iResult, 34, "10.2 Wrong value","");

	apInitScenario("11. Conversion from String return value to String Object");
	strResult = String(RetString());
	verify (String(strResult.GetType()), "System.String", "11.1 Wrong data type","");
	verify (strResult, "My dog has fleas.", "11.2 Wrong value","");

	apInitScenario("12. Conversion from String return value to RegExp Object");
	reResult = RegExp(RetString());
	verify (String(reResult.GetType()), "Microsoft.JScript.RegExpObject", "12.1 Wrong data type","");
	verify (reResult, "/My dog has fleas./", "12.2 Wrong value","");


	apInitScenario("13. Conversion from RegExp return value to Boolean Object");
	bResult = Boolean(RetRegExp());
	verify (String(bResult.GetType()), "System.Boolean", "13.1 Wrong data type","");
	verify (bResult, true, "13.2 Wrong value","");
	
	apInitScenario("14. Conversion from RegExp return value to Number Object");
	try {
		iResult = Number(RetRegExp2());
	} catch (e) {
		verify (e.description, "Type mismatch", "14.0 Wrong error", "");
	}

	apInitScenario("15. Conversion from RegExp return value to String Object");
	strResult = String(RetRegExp());
	verify (String(strResult.GetType()), "System.String", "15.1 Wrong data type","");
	verify (strResult, "/abc/", "15.2 Wrong value","");

	apInitScenario("16. Conversion from RegExp return value to RegExp Object");
	reResult = RegExp(RetRegExp());
	verify (String(reResult.GetType()), "Microsoft.JScript.RegExpObject", "16.1 Wrong data type","");
	verify (reResult, "/abc/", "16.2 Wrong value","");


	apInitScenario("17. Conversion from Date return value to Boolean Object");
	bResult = Boolean(RetDate());
	verify (String(bResult.GetType()), "System.Boolean", "17.1 Wrong data type","");
	verify (bResult, true, "17.2 Wrong value","");
	
/*	apInitScenario("18. Conversion from Date return value to Number Object");
	iResult = Number(RetDate());
	verify (String(iResult.GetType()), "System.Int32", "18.1 Wrong data type","");
	verify (iResult, 1, "18.2 Wrong value","");
*/
        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario("19. Conversion from Date return value to String Object");
	strResult = String(RetDate());
	verify (String(strResult.GetType()), "System.String", "19.1 Wrong data type","");
	verify (strResult, "Wed Feb 14 00:00:00 PST 2001", "19.2 Wrong value","");

	apInitScenario("20. Conversion from Date return value to RegExp Object");
	reResult = RegExp(RetDate());
	verify (String(reResult.GetType()), "Microsoft.JScript.RegExpObject", "20.1 Wrong data type","");
	verify (reResult, "/Wed Feb 14 00:00:00 PST 2001/", "20.2 Wrong value","");
        }

	apEndTest();
}

typcon13();


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
