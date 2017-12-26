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


var iTestID = 89101;

/* -------------------------------------------------------------------------
  Test: 	TYPCON10
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion to String object
 
  Keywords:	type conversion object data variant string
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Conversion from ActiveXObject to String Object

		2.  Conversion from Array object to String Object

		3.  Conversion from Boolean object to String Object

		4.  Conversion from Date object to String Object

		5.  Conversion from Enumerator object to String Object

		6.  Conversion from Error object to String Object

		7.  Conversion from Function object to String Object

		8.  Conversion from Number object to String Object

		9.  Conversion from Regexp object to String Object

		10. Conversion from String object to String Object

		11. Conversion from VBArray object to String Object

		12. Conversion from User-defined object to String Object

		13. Conversion from String type to String Object

		14. Conversion from Boolean type to String Object

		15. Conversion from Number type to String Object

		16. Conversion from COM+ data type to String Object


  Abstract:	 Testing that the correct errors are returned for type conversion to
			 a String object.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


				changes in anonymous function layout
 -------------------------------------------------------------------------*/


/*----------
/
/  Helper functions
/
----------*/

function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
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



@if(!@aspx)
	import System
@end

function typcon10() {

    apInitTest("typcon10: Type conversion -- Conversion to String Object"); 

	var objResult : String;

	apInitScenario("1.  Conversion from ActiveXObject to String Object");

/*	var obj1 = new ActiveXObject("Scripting.FileSystemObject");
	objResult = String(obj1);
	verify (String(objResult.GetType()), "System.String", "1.1 Wrong data type", "VS7 87865");
	verify (objResult, "ActiveXObject", "1.2 Wrong data type", "VS7 87865");
	verify (String(obj1.GetType()), "System.__ComObject", "1.3 Wrong data type", "VS7 87865");
*/

	apInitScenario("2.  Conversion from Array object to String Object");
	var obj2 = new Array(4,5,6);
	objResult = String(obj2);
	verify (String(objResult.GetType()), "System.String", "2.1 Wrong data type", "");
	verify (objResult, "4,5,6", "2.2 Wrong data type", "");
	verify (String(obj2.GetType()), "Microsoft.JScript.ArrayObject", "2.3 Wrong data type", "");


	apInitScenario("3.  Conversion from Boolean object to String Object");
	var obj3 = new Boolean(false);
	objResult = String(obj3);
	verify (String(objResult.GetType()), "System.String", "3.1 Wrong data type", "");
	verify (objResult, "false", "3.2 Wrong data type", "");
	obj3 = new Boolean(true);
	objResult = String(obj3);
	verify (objResult, "true", "3.3 Wrong data type", "");
	verify (String(obj3.GetType()), "Microsoft.JScript.BooleanObject", "3.4 Wrong data type", "");


        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario("4.  Conversion from Date object to String Object");
	var obj4 = new Date(1939,10,11);
	objResult = String(obj4);
	verify (String(objResult.GetType()), "System.String", "4.1 Wrong data type", "");
	verify (String(objResult), "Sat Nov 11 00:00:00 PST 1939", "4.2 Wrong data type", "");
	verify (String(obj4.GetType()), "Microsoft.JScript.DateObject", "4.3 Wrong data type", "");
        }

	apInitScenario("5.  Conversion from Enumerator object to String Object");

	var obj5 = new Enumerator("Eric", "Ian", "Kirk", "Vernon");
	objResult = String(obj5);
	verify (String(objResult.GetType()), "System.String", "5.1 Wrong data type", "");
	verify (String(objResult), "[object Object]", "5.2 Wrong data type", "");


	apInitScenario("6.  Conversion from Error object to String Object");
	var obj6 = new Error(32764, "Kirk's runtime error");
	objResult = String(obj6);
	verify (String(objResult.GetType()), "System.String", "6.1 Wrong data type", "");
	verify (objResult, "Error: Kirk's runtime error", "6.2 Wrong data type", "");
	verify (String(obj6.GetType()), "Microsoft.JScript.ErrorObject", "6.3 Wrong data type", "");


	apInitScenario("7.  Conversion from Function object to String Object");
	var obj7 = new Function("return 3");
	objResult = String(obj7);
	verify (String(objResult.GetType()), "System.String", "7.1 Wrong data type", "");
	verify (encodeURI(objResult), "function%20anonymous()%20%7B%0Areturn%203%0A%7D", "7.2 Wrong data type","");
	verify (String(obj7.GetType()), "Microsoft.JScript.Closure", "7.3 Wrong data type", "");


	apInitScenario("8.  Conversion from Number object to String Object");

	// Note that this is the Number OBJECT as opposed to the Number data type.
	var obj8 = new Number(0);
	objResult = String(obj8);
	verify (String(objResult.GetType()), "System.String", "8.1 Wrong data type", "");
	verify (objResult, "0", "8.2 Wrong data type", "");
	verify (String(obj8.GetType()), "Microsoft.JScript.NumberObject", "8.3 Wrong data type", "");
	obj8 = new Number(123);
	objResult = String(obj8);
	verify (String(objResult.GetType()), "System.String", "8.4 Wrong data type", "");
	verify (objResult, "123", "8.5 Wrong data type", "");


	apInitScenario("9.  Conversion from Regexp object to String Object");
	var obj9 = new RegExp("(\w+)\s");
	objResult = String(obj9);
	verify (String(objResult.GetType()), "System.String", "9.1 Wrong data type", "");
	verify (String(objResult), "/(\w+)\s/", "9.2 Wrong data type", "VS7 87846");
	verify (String(obj9.GetType()), "Microsoft.JScript.RegExpObject", "9.3 Wrong data type", "");


	apInitScenario("10. Conversion from String object to String Object");
	var obj10 = new String("My dog has fleas.");
	objResult = String(obj10);
	verify (String(objResult.GetType()), "System.String", "10.1 Wrong data type", "");
	verify (String(objResult), "My dog has fleas.", "10.2 Wrong data type", "");
	verify (String(obj10.GetType()), "Microsoft.JScript.StringObject", "10.3 Wrong data type", "");


	apInitScenario("11. Conversion from VBArray object to String Object");

/* VBArrays need to come from some VB script
	var obj11 = new VBArray(3,4,1);
	objResult = String(obj11);
	verify (String(objResult.GetType()), "System.String", "11.1 Wrong data type", "");
	verify (objResult, 0, "11.2 Wrong data value", "");
	verify (String(obj11.GetType()), "Microsoft.JScript.StringObject", "11.3 Wrong data type", "");
*/

	apInitScenario("12. Conversion from User-defined object to String Object");
	var obj12 = new Person();
	obj12.SetAge(31);
	objResult = String(obj12);
	verify (String(objResult.GetType()), "System.String", "12.1 Wrong data type", "");
	verify (objResult, "Person", "12.2 Wrong data value", "");
	verify (String(obj12.GetType()), "Person", "12.3 Wrong data type", "");


	apInitScenario("13. Conversion from String type to String Object");

	// Note that this is a String data type as opposed to a String object.
	var obj13 = "My dog has fleas.";
	objResult = String(obj13);
	verify (String(objResult.GetType()), "System.String", "13.1 Wrong data type", "");
	verify (String(objResult), "My dog has fleas.", "13.2 Wrong data type", "");
	verify (String(obj13.GetType()), "System.String", "13.3 Wrong data type", "");


	apInitScenario("14. Conversion from Boolean type to String Object");

	// Note that this is a String data type as opposed to a String object.
	var obj14 = true;
	objResult = String(obj14);
	verify (String(objResult.GetType()), "System.String", "14.1 Wrong data type", "");
	verify (String(objResult), "true", "14.2 Wrong data type","");
	verify (String(obj14.GetType()), "System.Boolean", "14.3 Wrong data type", "");
	obj14 = false;
	objResult = String(obj14);
	verify (String(objResult), "false", "14.4 Wrong data type", "");


	apInitScenario("15. Conversion from Number type to String Object");

	// Note that this is a Number data type as opposed to a Number object.
	var obj15 = 123;
	objResult = String(obj15);
	verify (String(objResult.GetType()), "System.String", "15.1 Wrong data type", "");
	verify (String(objResult), "123", "15.2 Wrong data type", "");
	verify (String(obj15.GetType()), "System.Double", "15.3 Wrong data type", "");


	apInitScenario("16. Conversion from COM+ data type to String Object");

	// Note that this is a Number data type as opposed to a Number object.
	var obj16 : Int16;
	obj16 = 32767;
	objResult = String(obj16);
	verify (String(objResult.GetType()), "System.String", "16.1 Wrong data type", "");
	verify (objResult, "32767", "16.2 Wrong data type", "");
	verify (String(obj16.GetType()), "System.Int16", "16.3 Wrong data type", "");


	apEndTest();
}

typcon10();


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
