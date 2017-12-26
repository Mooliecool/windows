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


var iTestID = 178206;

/* -------------------------------------------------------------------------
  Test: 	tcatch07
   
  
 
  Component:	JScript
 
  Major Area:	try/catch
 
  Test Area:	try/catch using hard-typed variables
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  Throw standard runtime error

		2.  Throw a non-error

		3.  Nested try-catch


  Abstract:	 Testing that scope works correctly with hard-typed variables.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/
@if(!@aspx)
   import System
@end

/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes,sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp+" ", sAct, sBug);
}


function throwUndef(){
@if(@_fast)
	var obj : Object = new ReferenceError();
	obj.number = -2146823279;
	obj.message = obj.description = "'undef' is undefined";
	throw obj;
@else
	return undef;
@end
}

function throwNum(){
	var x : double = 123.456;
	throw x;
}

function throwObjExp1(){
	var a : Object;
	return a.undef;
}

/*----------
/
/  Global variables
/
----------*/



function tcatch07() {

    apInitTest("tcatch07: Try/catch involving hard-typed variables"); 

	var resNum : int;
	var resDesc : String;

	apInitScenario("1.  Throw standard runtime error");
	try {
		throwUndef();
	}
	catch (e) {
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	verify(resNum,-2146823279,"1.1 Wrong error.","");
	verify(resDesc,"'undef' is undefined","1.2 Wrong error.","");


	apInitScenario("2.  Throw a non-error");
	try{throwNum();}
	catch(e){
		if(e instanceof Error){
			apLogFailInfo("2.1 Error - not member of error.","","","");
		}
		verify(e,123.456,"2.2 Wrong value for error.","");
	}


	apInitScenario("3.  Nested try-catch");
	resNum = 0;
	resDesc = "unchanged";
	try{
		try{
			try{throwObjExp1();}
			catch(e){
				resNum = e.number;
				resDesc = e.description;
			}

			@if(@_jscript_version < 7)
				verify(resNum,-2146823281,"3.1 Wrong error.","");
				verify(resDesc,"'undef' is null or not an object","3.2 Not correct error.","");
			@else
				verify(resNum,-2146827864,"3.3 Wrong error.","");
				verify(resDesc,"Object required","3.4 Not correct error.","");
			@end
			try{throwNum();}
			catch(e){
				if(e instanceof Error){
					apLogFailInfo( "3.5 Error - not member of error.","","","");
				}
				verify(e,123.456,"3.6 Wrong value for error.","");
			}
		}
		catch(e){
			apLogFailInfo( "3.7 Error - caught in wrong nesting.","","","");
		}
	}
	catch(e){
		apLogFailInfo( "3.8 Error - caught in wrong nesting.","","","");
	}


	apEndTest();
}


tcatch07();


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
