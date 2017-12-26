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


var iTestID = 193752;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown14_1 = false;
var actualError14_1 = "";
var expectedError14_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha14_1                        " +
         "  {                                      " +
         "  }                                      " +
         "  class Beta14_1 extends Alpha14_1       " +
         "  {                                      " +
         "     public function Beta14_1(): boolean " +
         "     {                                   " +
         "        var i: boolean;                  " +
         "        i = false;                       " +
         "        return i;                        " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown14_1 = true;
   actualError14_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown14_2 = false;
var actualError14_2 = "";
var expectedError14_2 = "TypeError: Cannot return a value from a constructor or void function";

try
{
   eval ("  class Alpha14_2                        " +
         "  {                                      " +
         "  }                                      " +
         "  class Beta14_2 extends Alpha14_2       " +
         "  {                                      " +
         "     public function Beta14_2()          " +
         "     {                                   " +
         "        var i: double;                   " +
         "        i = 10.2;                        " +
         "        return i;                        " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown14_2 = true;
   actualError14_2 = error;
}




// -----------------------------------------------------------------------
function constructors02()
{
   apInitTest ("Constructors02");

      
   // -----------------------------------------------------------------------
   apInitScenario ("14.1 Subclass constructor returns a value; type annotated");
   // Compile error
   
   if (exceptionThrown14_1 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown14_1, "197528, 200532");
   if (actualError14_1 != expectedError14_1)
      apLogFailInfo ("Wrong compile error message", expectedError14_1, actualError14_1, "197528, 200532");     
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("14.2 Subclass constructor returns a value; not type annotated");
   // Compile error
         
   if (exceptionThrown14_2 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown14_2, "200532");
   if (actualError14_2 != expectedError14_2)
      apLogFailInfo ("Wrong compile error message", expectedError14_2, actualError14_2, "200532"); 
      
      
  
   apEndTest();
}



constructors02();


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
