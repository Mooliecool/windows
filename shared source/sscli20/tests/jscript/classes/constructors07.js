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


var iTestID = 193761;




// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;



// -----------------------------------------------------------------------
var exceptionThrown18_8 = false;
var actualError18_8 = "";
var expectedError18_8 = "ReferenceError: 'Beta18_8' is not accessible from this scope";

try
{
   eval ("  class Alpha18_8                                          " +
         "  {  }                                                     " +
         "  class Beta18_8 extends Alpha18_8                         " +
         "  {                                                        " +
         "     private function Beta18_8 (x: boolean, y: double)     " +
         "     {  }                                                  " +
         "  }                                                        " +
         "  var beta18_8: Beta18_8 = new Beta18_8 (true, 456);       ", "unsafe");
}
catch (error)
{
   exceptionThrown18_8 = true;
   actualError18_8 = error;   
}

// -----------------------------------------------------------------------
class Alpha18_9
{
}

class Beta18_9 extends Alpha18_9
{
}

class Charlie18_9 extends Beta18_9
{
   private function Charlie18_9 (x: double, y: boolean, z: String)
   {
   }
}


// -----------------------------------------------------------------------
var exceptionThrown18_10 = false;
var actualError18_10 = "";
var expectedError18_10 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_10                                                  " +
         "  {                                                                 " +
         "     private function Alpha18_10 (x: String, y: boolean, z: int)    " +
         "     { }                                                            " +
         "  }                                                                 " +
         "  class Beta18_10 extends Alpha18_10                                " +
         "  {                                                                 " +
         "     private function Beta18_10 (x: int, y: String, z: Boolean)     " +
         "     { }                                                            " +
         "  }                                                                 ", "unsafe");
}
catch (error)
{
   exceptionThrown18_10 = true;
   actualError18_10 = error;
}


// -----------------------------------------------------------------------
function constructors07()
{
   apInitTest ("Constructors07");


   // -----------------------------------------------------------------------
   apInitScenario ("18.8 Private constructor -- Subclass with private constructor w/ parameters");
   
   if (exceptionThrown18_8 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_8, "");
   if (actualError18_8 != expectedError18_8)
      apLogFailInfo ("Wrong compile error", expectedError18_8, actualError18_8, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18.9 Private constructor -- Sub-subclass with private constructor w/ parameters");
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: No such constructor";
   
   try
   {
      eval ("var charlie18_9: Charlie18_9 = new Charlie18_9 (20, false, \"Microsoft\");");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18.10 Private constructor -- Superclass and subclass with private constructor w/ parameters");

   if (exceptionThrown18_10 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_10, "");
   if (actualError18_10 != expectedError18_10)
      apLogFailInfo ("Wrong compile error", expectedError18_10, actualError18_10, "");   
      
      
   apEndTest();
}



constructors07();


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
