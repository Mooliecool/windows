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


var iTestID = 194183;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown6_2 = false;
var actualError6_2 = "";
var expectedError6_2 = "ReferenceError: 'super' is not accessible from this scope";

try
{
   eval ("  class Alpha6_2                            " +
         "  {                                         " +
         "     protected function GetName(): String   " +
         "     {                                      " +
         "        return \"Alpha6_2\";                " +
         "     }                                      " +
         "  }                                         " +
         "  class Beta6_2 extends Alpha6_2            " +
         "  {                                         " +
         "  }                                         " +
         "  class Charlie6_2 extends Beta6_2          " +
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
   exceptionThrown6_2 = true;
   actualError6_2 = error;
}


// -----------------------------------------------------------------------
function super05()
{
   apInitTest ("super05");


   // -----------------------------------------------------------------------
   apInitScenario ("6.2 Subclass calls protected super.GetName() from a different method (static) - 3rd level in the inheritance tree.  GetName() in the 1st level only.");
   // Compile error: super cannot be referenced from a static context               
   
   if (exceptionThrown6_2 == false)
      apLogFailInfo ("No compile error in 6.2", "Should give a compile error", exceptionThrown6_2, "");
   if (actualError6_2 != expectedError6_2)
      apLogFailInfo ("Wrong compile error in 6.2", expectedError6_2, actualError6_2, "200532");


   apEndTest();
}


super05();


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
