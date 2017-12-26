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


var iTestID = 198983;

/* -------------------------------------------------------------------------
  Test: 	typearr01
   
  
 
  Component:	JScript
 
  Major Area:	Data Types
 
  Test Area:	Hard Typed System Array tests
 
  Keywords:	Hard Typed System Array tests
 
 ---------------------------------------------------------------------------
  Purpose:	Test the general functionality of typed arrays
 
  Scenarios:



  Abstract:	 Hard Typed System Array tests
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 ------------------------------------------------------------------*/


/*----------
/
/  Helper functions
/
----------*/





/*----------
/
/  Class definitions
/
----------*/


@if(!@aspx)
    import System
@end

function typearr01() {

  apInitTest("typearr01: Typed arrays"); 

    var objResult : Boolean;
    var strResult : String;

    apInitScenario("test array declarations");
    try{
      var a : ulong = 10;				//OK
      var a1 : long[] = new long[10];		//OK
      var a2 : ulong[] = new ulong[10];		//error JS0013: Type mismatch
    }catch(e){
      apLogFailInfo("exception occured durring declarations", "", e.description,252823)
    }
	


    apEndTest();
}
 

typearr01();


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
