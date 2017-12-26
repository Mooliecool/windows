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


var iTestID = 89098;

/* -------------------------------------------------------------------------
  Test: 	TYPCON07
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion to Function object
 
  Keywords:	type conversion object data variant function
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Conversion from ActiveXObject to Function Object

		2.  Conversion from Array object to Function Object

		3.  Conversion from Boolean object to Function Object

		4.  Conversion from Date object to Function Object

		5.  Conversion from Enumerator object to Function Object

		6.  Conversion from Error object to Function Object

		7.  Conversion from Function object to Function Object

		8. Conversion from Number object to Function Object

		9. Conversion from Regexp object to Function Object

		10. Conversion from String object to Function Object

		11. Conversion from VBArray object to Function Object

		12. Conversion from User-defined object to Function Object

		13. Conversion from String type to Function Object

		14. Conversion from Boolean type to Function Object

		15. Conversion from Number type to Function Object

		16. Conversion from COM+ data type to Function Object


  Abstract:	 Testing that the correct errors are returned for type conversion to
			 a Function object.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


				to reflect changes in anonymous function output
 -------------------------------------------------------------------------*/


/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
}



/*----------
/
/  Global variables
/
----------*/


@if(!@aspx)
	import System
@end

class Person {
	private var iAge : int;

	public function GetAge() {
		return iAge;
	}
	
	public function SetAge(iNewAge) {
		iAge = iNewAge;
	}
}


function typcon07() {

    apInitTest("typcon07: Type conversion -- Conversion to Function Object"); 

	var objResult : Function;
	var strResult : String;

	apInitScenario("1.  Conversion from ActiveXObject to Function Object");

/*	var obj1 = new ActiveXObject("Scripting.FileSystemObject");
	objResult = Function(obj1);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "1.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0AActiveXObject%0A%7D", "1.2 Wrong value");
	verify (String(obj1.GetType()), "System.__ComObject", "1.3 Wrong data type");
*/

	apInitScenario("2.  Conversion from Array object to Function Object");
	var obj2 = new Array(2,4,5);
	objResult = Function(obj2);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "2.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A2%2C4%2C5%0A%7D", "2.2 Wrong value");
	verify (String(obj2.GetType()), "Microsoft.JScript.ArrayObject", "2.3 Wrong data type");
	

	apInitScenario("3.  Conversion from Boolean object to Function Object");
	var obj3 = new Boolean(false);
	objResult = Function(obj3);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "3.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Afalse%0A%7D", "3.2 Wrong value");
	verify (String(obj3.GetType()), "Microsoft.JScript.BooleanObject", "3.3 Wrong data type");

/* 209411 */

	apInitScenario("4.  Conversion from Date object to Function Object");

/*	var obj4 = new Date("11/11/1939");
	objResult = Function(obj4);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "4.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A%20%20NaN%0A%7D", "4.2 Wrong value");
	verify (String(obj4.GetType()), "Microsoft.JScript.DateObject", "4.3 Wrong data type");
*/

	apInitScenario("5.  Conversion from Enumerator object to Function Object");

/*	var obj5 = new Enumerator("Eric", "Ian", "Kirk", "Vernon");
	objResult = Function(obj5);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "5.1 Wrong data type");
	verify (escape(String(objResult)), "", "5.2 Wrong data type");
*/

	apInitScenario("6.  Conversion from Error object to Function Object");

/*	var obj6 = new Error(32764, "Kirk's runtime error");
	objResult = Function(obj6);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "6.1 Wrong data type");
	verify (escape(String(objResult)), "", "6.2 Wrong data type");
	verify (String(obj6.GetType()), "Microsoft.JScript.ErrorObject", "6.3 Wrong data type");
*/

	apInitScenario("7.  Conversion from Function object to Function Object");
	var obj7 = new Function("return 3");
	objResult = Function(obj7);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "7.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Afunction%20anonymous%28%29%20%7B%0Areturn%203%0A%7D%0A%7D", "7.2 Wrong data type");
	verify (String(obj7.GetType()), "Microsoft.JScript.Closure", "7.3 Wrong data type");


	apInitScenario("8.  Conversion from Number object to Function Object");
	// Note that this is the Number OBJECT as opposed to the Number data type.
	// All objects resolve to boolean true.
	var obj8 = new Number(0);
	objResult = Function(obj8);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "8.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A0%0A%7D", "8.2 Wrong data type");
	verify (String(obj8.GetType()), "Microsoft.JScript.NumberObject", "8.3 Wrong data type");
	obj8 = new Number(123);
	objResult = Function(obj8);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "8.4 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A123%0A%7D", "8.5 Wrong data type");


	apInitScenario("9.  Conversion from Regexp object to Function Object");
	var obj9 = new RegExp("(\w+)\s");
	objResult = Function(obj9);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "9.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A/%28w+%29s/%0A%7D", "9.2 Wrong data type");
	verify (String(obj9.GetType()), "Microsoft.JScript.RegExpObject", "9.3 Wrong data type");


	apInitScenario("10. Conversion from String object to Function Object");
	var obj10 = new String("return 3;");
	objResult = Function(obj10);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "10.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Areturn%203%3B%0A%7D", "10.2 Wrong data type");
	verify (String(obj10.GetType()), "Microsoft.JScript.StringObject", "10.3 Wrong data type");


	apInitScenario("11. Conversion from VBArray object to Function Object");

/* VBArrays need to come from some VB script
	var obj11 = new VBArray(3,4,1);
	objResult = Function(obj11);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "11.1 Wrong data type");
	verify (escape(String(objResult)), "", "11.2 Wrong data value");
	verify (String(obj11.GetType()), "Microsoft.JScript.StringObject", "11.3 Wrong data type");
*/


	apInitScenario("12. Conversion from User-defined object to Function Object");
	var obj12 = new Person();
	obj12.SetAge(31);
	objResult = Function(obj12);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "12.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0APerson%0A%7D", "12.2 Wrong data value");
	verify (String(obj12.GetType()), "Person", "12.3 Wrong data type");


	apInitScenario("13. Conversion from String type to Function Object");
	// Note that this is a String data type as opposed to a String object.
	var obj13 = "return 34;";
	objResult = Function(obj13);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "13.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Areturn%2034%3B%0A%7D", "13.2 Wrong data type");
	verify (String(obj13.GetType()), "System.String", "13.3 Wrong data type");


	apInitScenario("14. Conversion from Boolean type to Function Object");
	// Note that this is a String data type as opposed to a String object.
	var obj14 = true;
	objResult = Function(obj14);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "14.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Atrue%0A%7D", "14.2 Wrong data type");
	verify (String(obj14.GetType()), "System.Boolean", "14.3 Wrong data type");
	obj14 = false;
	objResult = Function(obj14);
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0Afalse%0A%7D", "14.4 Wrong data type");


	apInitScenario("15. Conversion from Number type to Function Object");
	// Note that this is a Number data type as opposed to a Number object.
	var obj15 = 123;
	objResult = Function(obj15);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "15.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A123%0A%7D", "15.2 Wrong data type");
	verify (String(obj15.GetType()), "System.Double", "15.3 Wrong data type");


	apInitScenario("16. Conversion from COM+ data type to Function Object");
	// Note that this is a Number data type as opposed to a Number object.
	var obj16 : Int16;
	obj15 = 32767;
	objResult = Function(obj16);
	verify (String(objResult.GetType()), "Microsoft.JScript.Closure", "16.1 Wrong data type");
	verify (escape(String(objResult)), "function%20anonymous%28%29%20%7B%0A0%0A%7D", "16.2 Wrong data type");
	verify (String(obj16.GetType()), "System.Int16", "16.3 Wrong data type");


	apEndTest();
}


typcon07();


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
