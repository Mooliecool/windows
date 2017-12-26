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


var iTestID = 89099;

/* -------------------------------------------------------------------------
  Test: 	TYPCON08
   
  
 
  Component:	JScript
 
  Major Area:	Type Conversion
 
  Test Area:	Type conversion to Number object
 
  Keywords:	type conversion object data variant number
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Conversion from ActiveXObject to Number Object

		2.  Conversion from Array object to Number Object

		3.  Conversion from Boolean object to Number Object

		4.  Conversion from Date object to Number Object

		5.  Conversion from Enumerator object to Number Object

		6.  Conversion from Error object to Number Object

		7.  Conversion from Function object to Number Object

		8.  Conversion from Number object to Number Object

		9.  Conversion from Regexp object to Number Object

		10. Conversion from String object to Number Object

		11. Conversion from VBArray object to Number Object

		12. Conversion from User-defined object to Number Object

		13. Conversion from String type to Number Object

		14. Conversion from Boolean type to Number Object

		15. Conversion from Number type to Number Object

		16. Conversion from COM+ data type to Number Object


  Abstract:	 Testing that the correct errors are returned for type conversion to
			 a Number object.
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


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, "");
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

function typcon08() {

    apInitTest("typcon08: Type conversion -- Conversion to Number Object"); 

	var objResult : Number;
	var strResult : String;

	apInitScenario("1.  Conversion from ActiveXObject to Number Object");

/*	var obj1 = new ActiveXObject("Scripting.FileSystemObject");
	objResult = Number(obj1);
	verify (String(objResult.GetType()), "System.Double", "1.1 Wrong data type");
	verify (String(objResult), "NaN", "1.2 Wrong data type");
	verify (String(obj1.GetType()), "System.__ComObject", "1.3 Wrong data type");
*/	

	apInitScenario("2.  Conversion from Array object to Number Object");
	var obj2 = new Array(3,4,5);
	objResult = Number(obj2);
	verify (String(objResult.GetType()), "System.Double", "2.1 Wrong data type");
	verify (String(objResult), "NaN", "2.2 Wrong data type");
	verify (String(obj2.GetType()), "Microsoft.JScript.ArrayObject", "2.3 Wrong data type");


	apInitScenario("3.  Conversion from Boolean object to Number Object");
	var obj3 = new Boolean(false);
	objResult = Number(obj3);
	verify (String(objResult.GetType()), "System.Double", "3.1 Wrong data type");
	verify (objResult, 0, "3.2 Wrong data type");
	obj3 = new Boolean(true);
	objResult = Number(obj3);
	verify (objResult, 1, "3.3 Wrong data type");
	verify (String(obj3.GetType()), "Microsoft.JScript.BooleanObject", "3.4 Wrong data type");


        if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	apInitScenario("4.  Conversion from Date object to Number Object");
	var obj4 = new Date("11-22-1939");
	objResult = Number(obj4);
	verify (String(objResult.GetType()), "System.Double", "4.1 Wrong data type");
	verify (objResult, -950198400000, "4.2 Wrong data type");
	verify (String(obj4.GetType()), "Microsoft.JScript.DateObject", "4.3 Wrong data type");
        }

	apInitScenario("5.  Conversion from Enumerator object to Number Object");

	var obj5 = new Enumerator("Eric", "Ian", "Kirk", "Vernon");
	objResult = Number(obj5);
	verify (String(objResult.GetType()), "System.Double", "5.1 Wrong data type");
	verify (String(objResult), "NaN", "5.2 Wrong data type");


	apInitScenario("6.  Conversion from Error object to Number Object");
	var obj6 = new Error(32764, "Kirk's runtime error");
	objResult = Number(obj6);
	verify (String(objResult.GetType()), "System.Double", "6.1 Wrong data type");
	verify (String(objResult), "NaN", "6.2 Wrong data type");
	verify (String(obj6.GetType()), "Microsoft.JScript.ErrorObject", "6.3 Wrong data type");


	apInitScenario("7.  Conversion from Function object to Number Object");
	var obj7 = new Function("return 3");
	objResult = Number(obj7);
	verify (String(objResult.GetType()), "System.Double", "7.1 Wrong data type");
	verify (String(objResult), "NaN", "7.2 Wrong data type");
	verify (String(obj7.GetType()), "Microsoft.JScript.Closure", "7.3 Wrong data type");


	apInitScenario("8.  Conversion from Number object to Number Object");

	// Note that this is the Number OBJECT as opposed to the Number data type.
	// All objects resolve to boolean true.
	var obj8 = new Number(0);
	objResult = Number(obj8);
	verify (String(objResult.GetType()), "System.Double", "8.1 Wrong data type");
	verify (objResult, 0, "8.2 Wrong data type");
	verify (String(obj8.GetType()), "Microsoft.JScript.NumberObject", "8.3 Wrong data type");
	obj8 = new Number(123);
	objResult = Number(obj8);
	verify (String(objResult.GetType()), "System.Double", "8.4 Wrong data type");
	verify (objResult, 123, "8.5 Wrong data type");


	apInitScenario("9.  Conversion from Regexp object to Number Object");
	var obj9 = new RegExp("(\w+)\s");
	objResult = Number(obj9);
	verify (String(objResult.GetType()), "System.Double", "9.1 Wrong data type");
	verify (String(objResult), "NaN", "9.2 Wrong data type");
	verify (String(obj9.GetType()), "Microsoft.JScript.RegExpObject", "9.3 Wrong data type");


	apInitScenario("10. Conversion from String object to Number Object");
	var obj10 = new String("My dog has fleas.");
	objResult = Number(obj10);
	verify (String(objResult.GetType()), "System.Double", "10.1 Wrong data type");
	verify (String(objResult), "NaN", "10.2 Wrong data type");
	verify (String(obj10.GetType()), "Microsoft.JScript.StringObject", "10.3 Wrong data type");


	apInitScenario("11. Conversion from VBArray object to Number Object");

/* VBArrays need to come from some VB script
	var obj11 = new VBArray(3,4,1);
	objResult = Number(obj11);
	verify (String(objResult.GetType()), "System.Double", "11.1 Wrong data type");
	verify (objResult, 0, "11.2 Wrong data value");
	verify (String(obj11.GetType()), "Microsoft.JScript.StringObject", "11.3 Wrong data type");
*/

	apInitScenario("12. Conversion from User-defined object to Number Object");
	var obj12 = new Person();
	obj12.SetAge(31);
	objResult = Number(obj12);
	verify (String(objResult.GetType()), "System.Double", "12.1 Wrong data type");
	verify (String(objResult), "NaN", "12.2 Wrong data value");
	verify (String(obj12.GetType()), "Person", "12.3 Wrong data type");


	apInitScenario("13. Conversion from String type to Number Object");

	// Note that this is a String data type as opposed to a String object.
	var obj13 = "My dog has fleas.";
	objResult = Number(obj13);
	verify (String(objResult.GetType()), "System.Double", "13.1 Wrong data type");
	verify (String(objResult), "NaN", "13.2 Wrong data type");
	verify (String(obj13.GetType()), "System.String", "13.3 Wrong data type");


	apInitScenario("14. Conversion from Boolean type to Number Object");

	// Note that this is a String data type as opposed to a String object.
	var obj14 = true;
	objResult = Number(obj14);
	verify (String(objResult.GetType()), "System.Double", "14.1 Wrong data type");
	verify (objResult, 1, "14.2 Wrong data type");
	verify (String(obj14.GetType()), "System.Boolean", "14.3 Wrong data type");
	obj14 = false;
	objResult = Number(obj14);
	verify (objResult, 0, "14.4 Wrong data type");


	apInitScenario("15. Conversion from Number type to Number Object");

	// Note that this is a Number data type as opposed to a Number object.
	var obj15 = 123;
	objResult = Number(obj15);
	verify (String(objResult.GetType()), "System.Double", "15.1 Wrong data type");
	verify (objResult, 123, "15.2 Wrong data type");
	verify (String(obj15.GetType()), "System.Double", "15.3 Wrong data type");


	apInitScenario("16. Conversion from COM+ data type to Number Object");

	// Note that this is a Number data type as opposed to a Number object.
	var obj16 : Int16;
	obj16 = 32767;
	objResult = Number(obj16);
	verify (String(objResult.GetType()), "System.Double", "16.1 Wrong data type");
	verify (objResult, 32767, "16.2 Wrong data type");
	verify (String(obj16.GetType()), "System.Int16", "16.3 Wrong data type");


	apEndTest();
}

typcon08();


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
