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


var iTestID = 194182;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown6_1 = false;
var actualError6_1 = "";
var expectedError6_1 = "ReferenceError: 'super' is not accessible from this scope";

try
{
   eval ("  class Alpha6_1                            " +
         "  {                                         " +
         "     protected function GetName(): String   " +
         "     {                                      " +
         "        return \"Alpha6_1\";                " +
         "     }                                      " +
         "  }                                         " +
         "  class Beta6_1 extends Alpha6_1            " +
         "  {                                         " +
         "     static function Bar()                  " +
         "     {                                      " +
         "        var x = \"\";                       " +
         "        x = super.GetName();                " +
         "     }                                      " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown6_1 = true;
   actualError6_1 = error;
}


// -----------------------------------------------------------------------
function super04()
{
   apInitTest ("super04");


   // -----------------------------------------------------------------------
   apInitScenario ("6.1 Subclass calls protected super.GetName() from a different method (static) - 2nd level in the inheritance tree.");
   // Compile error: super cannot be referenced from a static context               
   
   if (exceptionThrown6_1 == false)
      apLogFailInfo ("No compile error in 6.1", "Should give a compile error", exceptionThrown6_1, "");
   if (actualError6_1 != expectedError6_1)
      apLogFailInfo ("Wrong compile error in 6.1", expectedError6_1, actualError6_1, "200532");
   
   
   apEndTest();
}


super04();


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
