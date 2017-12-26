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


var iTestID = 161333;

/* -------------------------------------------------------------------------
  Test: 	TYPCON14
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion on literal values
 
  Keywords:	type conversion object data literal values
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  [objectName]() -- empty

		2.  [objectName](null)

		3.  [objectName](undefined)

		4.  [objectName](true)

		5.  [objectName](/\s\w+/) - regexp literal

		6.  [objectName](eval('123'))

		7.  [objectName](eval(Number('123')))

		8.  [objectName]([complex math expression])

		9.  [objectName]('123' + 5) - coercion result


  Abstract:	 Testing that the type conversion special cases
			 produce the expected results.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


					1.3 From 'System.Double' to 'System.Int32'
					1.4 From 'NaN' to '0'
					1.6 From 'undefined' to ''
					was failing on NT platforms
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
@cc_on

function typcon14() {

    apInitTest("typcon14: Type conversion -- Special cases"); 

	var result;

	apInitScenario("1.  [objectName]() -- empty");
	result = Boolean();
	verify (result.GetType(), "System.Boolean", "1.1 Wrong data type","");
	verify (result, false, "1.2 Wrong value","");
	result = Number();
	@if (@_fast)
		verify (result.GetType(), "System.Double", "1.3 Wrong data type",""); // [01]
		verify (result.toString(), "NaN", "1.4 Wrong value",""); // [01]
		result = String();
		verify (result.GetType(), "System.String", "1.5 Wrong data type","");
		verify (result.toString(), "undefined", "1.6 Wrong value",""); // [01]
	@else
		verify (result.GetType(), "System.Int32", "1.3 Wrong data type",""); // [01]
		verify (result.toString(), "0", "1.4 Wrong value",""); // [01]
		result = String();
		verify (result.GetType(), "System.String", "1.5 Wrong data type","");
		verify (result.toString(), "", "1.6 Wrong value",""); // [01]
	@end
	result = RegExp();
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "1.7 Wrong data type","");
	verify (result.toString(), new RegExp(""), "1.8 Wrong value","");


	apInitScenario("2.  [objectName](null)");
	result = Boolean(null);
	verify (result.GetType(), "System.Boolean", "2.1 Wrong data type","");
	verify (result, false, "2.2 Wrong value","");
	result = Number(null);
	verify (result.GetType(), "System.Double", "2.3 Wrong data type","");
	verify (result, 0, "2.4 Wrong value","");
	result = String(null);
	verify (result.GetType(), "System.String", "2.5 Wrong data type","");
	verify (result, "null", "2.6 Wrong value","");
	result = RegExp(null);
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "2.7 Wrong data type","");
	verify (result.toString(), "/null/", "2.8 Wrong value","");


	apInitScenario("3.  [objectName](undefined)");
	result = Boolean(undefined);
	verify (result.GetType(), "System.Boolean", "3.1 Wrong data type","");
	verify (result, false, "3.2 Wrong value","");
	result = Number(undefined);
	verify (result.GetType(), "System.Double", "3.3 Wrong data type","");
	verify (result.toString(), "NaN", "3.4 Wrong value","");
	result = String(undefined);
	verify (result.GetType(), "System.String", "3.5 Wrong data type","");
	verify (result, "undefined", "3.6 Wrong value","");
	result = RegExp(undefined);
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "3.7 Wrong data type","");
	verify (result.toString(), new RegExp(""), "3.8 Wrong value","");


	apInitScenario("4.  [objectName](true)");
	result = Boolean(true);
	verify (result.GetType(), "System.Boolean", "4.1 Wrong data type","");
	verify (result, true, "4.2 Wrong value","");
	result = Number(true);
	verify (result.GetType(), "System.Double", "4.3 Wrong data type","");
	verify (result, 1, "4.4 Wrong value","");
	result = String(true);
	verify (result.GetType(), "System.String", "4.5 Wrong data type","");
	verify (result, "true", "4.6 Wrong value","");
	result = RegExp(true);
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "4.7 Wrong data type","");
	verify (result.toString(), "/true/", "4.8 Wrong value","");


	apInitScenario("5.  [objectName](/\s\w+/) - regexp literal");
	result = Boolean(/\s\w+/);
	verify (result.GetType(), "System.Boolean", "5.1 Wrong data type","");
	verify (result, true, "5.2 Wrong value","");
	result = Number(/\s\w+/);
	verify (result.GetType(), "System.Double", "5.3 Wrong data type","");
	verify (result.toString(), "NaN", "5.4 Wrong value","");
	result = String(/\s\w+/);
	verify (result.GetType(), "System.String", "5.5 Wrong data type","");
	verify (result, "/\\s\\w+/", "5.6 Wrong value","");
	result = RegExp(/\s\w+/);
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "5.7 Wrong data type","");
	verify (result.toString(), "/\\s\\w+/", "5.8 Wrong value","");


	apInitScenario("6.  [objectName](eval('123'))");
	result = Boolean(eval("123"));
	verify (result.GetType(), "System.Boolean", "6.1 Wrong data type","");
	verify (result, true, "6.2 Wrong value","");
	result = Number(eval("123"));
	verify (result.GetType(), "System.Double", "6.3 Wrong data type","");
	verify (result, 123, "6.4 Wrong value","");
	result = String(eval("123"));
	verify (result.GetType(), "System.String", "6.5 Wrong data type","");
	verify (result, "123", "6.6 Wrong value","");
	result = RegExp(eval("123"));
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "6.7 Wrong data type","");
	verify (result.toString(), "/123/", "6.8 Wrong value","");


	apInitScenario("7.  [objectName](eval(Number('123')))");
	result = Boolean(eval(Number('123')));
	verify (result.GetType(), "System.Boolean", "7.1 Wrong data type","");
	verify (result, true, "7.2 Wrong value","");
	result = Number(eval(Number('123')));
	verify (result.GetType(), "System.Double", "7.3 Wrong data type","");
	verify (result, 123, "7.4 Wrong value","");
	result = String(eval(Number('123')));
	verify (result.GetType(), "System.String", "7.5 Wrong data type","");
	verify (result, "123", "7.6 Wrong value","");
	result = RegExp(eval(Number('123')));
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "7.7 Wrong data type","");
	verify (result.toString(), "/123/", "7.8 Wrong value","");


	apInitScenario("8.  [objectName]([complex math expression])");
	result = Boolean(Math.floor(10000*(3.415/2.13)*((4-27.33)/(-6)+36)));
	verify (result.GetType(), "System.Boolean", "8.1 Wrong data type","");
	verify (result, true, "8.2 Wrong value","");
	result = Number(Math.floor(10000*(3.415/2.13)*((4-27.33)/(-6)+36)));
	verify (result.GetType(), "System.Double", "8.3 Wrong data type","");
	verify (result, 639524, "8.4 Wrong value","");
	result = String(Math.floor(10000*(3.415/2.13)*((4-27.33)/(-6)+36)));
	verify (result.GetType(), "System.String", "8.5 Wrong data type","");
	verify (result, "639524", "8.6 Wrong value","");
	result = RegExp(Math.floor(10000*(3.415/2.13)*((4-27.33)/(-6)+36)));
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "8.7 Wrong data type","");
	verify (result.toString(), "/639524/", "8.8 Wrong value","");


	apInitScenario("9.  [objectName]('123' + 5) - coercion result");
	result = Boolean('123' + 5);
	verify (result.GetType(), "System.Boolean", "9.1 Wrong data type","");
	verify (result, true, "9.2 Wrong value","");
	result = Number('123' + 5);
	verify (result.GetType(), "System.Double", "9.3 Wrong data type","");
	verify (result, 1235, "9.4 Wrong value","");
	result = String('123' + 5);
	verify (result.GetType(), "System.String", "9.5 Wrong data type","");
	verify (result, "1235", "9.6 Wrong value","");
	result = RegExp('123' + 5);
	verify (result.GetType(), "Microsoft.JScript.RegExpObject", "9.7 Wrong data type","");
	verify (result.toString(), "/1235/", "9.8 Wrong value","");


	apEndTest();
}


typcon14();


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
