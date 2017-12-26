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


var iTestID = 193759;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;



// -----------------------------------------------------------------------
var exceptionThrown18_6 = false;
var actualError18_6 = "";
var expectedError18_6 = "ReferenceError: 'Alpha18_6' is not accessible from this scope";

try
{
   eval ("  class Alpha18_6                                       " +
         "  {                                                     " +
         "     private function Alpha18_6 (x: boolean, y: String) " +
         "     {                                                  " +
         "     }                                                  " +
         "  }                                                     " +
         "  var alpha18_6: Alpha18_6 = new Alpha18_6(true,5);     ", "unsafe");
}
catch (error)
{
   exceptionThrown18_6 = true;
   actualError18_6 = error;   
}     




// -----------------------------------------------------------------------
var exceptionThrown18_7 = false;
var actualError18_7 = "";
var expectedError18_7 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_7                                    " +
         "  {                                                  " +
         "     private function Alpha18_7 (x: int, y: boolean) " +
         "     { }                                             " +
         "  }                                                  " +
         "  class Beta18_7 extends Alpha18_7                   " +
         "  {                                                  " +
         "  }                                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown18_7 = true;
   actualError18_7 = error;   
}


// -----------------------------------------------------------------------
function constructors06()
{
   apInitTest ("Constructors06");


   // -----------------------------------------------------------------------
   apInitScenario ("18.6 Private constructor -- Simple class with private constructor w/ parameters");
   
   if (exceptionThrown18_6 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_6, "");
   if (actualError18_6 != expectedError18_6)
      apLogFailInfo ("Wrong compile error", expectedError18_6, actualError18_6, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("18.7 Private constructor -- Superclass with private constructor w/ parameters");
   
   if (exceptionThrown18_7 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_7, "200532");
   if (actualError18_7 != expectedError18_7)
      apLogFailInfo ("Wrong compile error", expectedError18_7, actualError18_7, "200532");


   apEndTest();
}



constructors06();


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
