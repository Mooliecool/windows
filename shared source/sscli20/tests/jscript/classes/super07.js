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


var iTestID = 194187;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown15_1 = false;
var actualError15_1 = "";
var expectedError15_1 = "ReferenceError: The static member 'name' must be accessed with the class name";

try
{
   eval ("  class Alpha15_1                     " +
         "  {                                   " +
         "     static var name: String;         " +
         "  }                                   " +
         "  class Beta15_1 extends Alpha15_1    " +
         "  {                                   " +
         "     public function Bar()            " +
         "     {                                " +
         "        super.name = \"Foo\";         " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown15_1 = true;
   actualError15_1 = error;
}



// -----------------------------------------------------------------------
function super07()
{
   apInitTest ("super07");


   // -----------------------------------------------------------------------
   apInitScenario ("15.1 Subclass modifies the value of super.field (static) - 2nd level in the inheritance tree.");

   if (exceptionThrown15_1 == false)
      apLogFailInfo ("No compile error in 15.1", "Should give a compile error", exceptionThrown15_1, "200532");
   if (actualError15_1 != expectedError15_1)
      apLogFailInfo ("Wrong compile error in 15.1", expectedError15_1, actualError15_1, "");


   apEndTest();
}


super07();


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
