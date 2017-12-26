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


var iTestID = 193757;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;



// -----------------------------------------------------------------------
var exceptionThrown18_1 = false;
var actualError18_1 = "";
var expectedError18_1 = "ReferenceError: 'Alpha18_1' is not accessible from this scope";

try
{
   eval ("  class Alpha18_1                              " +
         "  {                                            " +
         "     private function Alpha18_1()              " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha18_1: Alpha18_1 = new Alpha18_1();  ", "unsafe");
}
catch (error)
{
   exceptionThrown18_1 = true;
   actualError18_1 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown18_2 = false;
var actualError18_2 = "";
var expectedError18_2 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_2                        " +
         "  {                                      " +
         "     private function Alpha18_2() { }    " +
         "  }                                      " +
         "  class Beta18_2 extends Alpha18_2       " +
         "  {                                      " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown18_2 = true;
   actualError18_2 = error;   
}



// -----------------------------------------------------------------------
function constructors04()
{
   apInitTest ("Constructors04");


   // -----------------------------------------------------------------------
   apInitScenario ("18.1 Private constructor -- Simple class with private default constructor");
   
   if (exceptionThrown18_1 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_1, "");
   if (actualError18_1 != expectedError18_1)
      apLogFailInfo ("Wrong compile error", expectedError18_1, actualError18_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18.2 Private constructor -- Superclass with private default constructor");
   
   if (exceptionThrown18_2 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_2, "");
   if (actualError18_2 != expectedError18_2)
      apLogFailInfo ("Wrong compile error", expectedError18_2, actualError18_2, "200532");
      
      
   apEndTest();
}



constructors04();


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
