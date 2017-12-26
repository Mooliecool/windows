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


var iTestID = 194403;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";



// -----------------------------------------------------------------------
var actualError5 = "";
var exceptionThrown5 = false;

try
{
   eval ("class Person5                      " +
         "{                                  " +
         "   public function GetAge(): int   " +
         "   {                               " +
         "      return 30;                   " +
         "   }                               " +
         "                                   " +
         "   static Person5                  " +
         "   {                               " +
         "      var x;                       " +
         "      x = GetAge();                " +
         "   }                               " +
         "}                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown5 = true;
   actualError5 = error;
}


// -----------------------------------------------------------------------
function staticinitializers02()
{
   apInitTest ("StaticInitializers02");


   // -----------------------------------------------------------------------
   apInitScenario ("5. Can static initializers access instance methods?");
   // No
   
   expectedError = "ReferenceError: Non-static member 'GetAge' is not accessible from a static scope";
   if (exceptionThrown5 == false)
      apLogFailInfo ("Static initializers accessing instance methods does not give a compile error","Should give a compile error", exceptionThrown5, "");
   if (actualError5 != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError5, "");                     
   
         
   apEndTest();
}



staticinitializers02();


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
