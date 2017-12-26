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


var iTestID = 194192;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown20 = false;
var actualError20 = "";
var expectedError20 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha20                          " +
         "  {                                      " +
         "     public function Alpha20 (x: String) " +
         "     {  }                                " +
         "  }                                      " +
         "  class Beta20 extends Alpha20           " +
         "  {                                      " +
         "     public function Bar()               " +
         "     {                                   " +
         "        super (\"hello\");               " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown20 = true;
   actualError20 = error;
}


// -----------------------------------------------------------------------
function super11()
{
   apInitTest ("super11");


   // -----------------------------------------------------------------------
   apInitScenario ("20. Beta.Bar() calls Alpha.Alpha(String) with super('hello')");

   if (exceptionThrown20 == false)
      apLogFailInfo ("No compile error in 20.", "Should give a compile error", exceptionThrown20, "200532");
   if (actualError20 != expectedError20)
      apLogFailInfo ("Wrong compile error in 20.", expectedError20, actualError20, "");


   apEndTest();
}


super11();


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
