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


var iTestID = 194189;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown16 = false;
var actualError16 = "";
var expectedError16 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha16                    " +
         "  {                                " +
         "     public function Alpha16()     " +
         "     { }                           " +
         "  }                                " +
         "  class Beta16 extends Alpha16     " +
         "  {                                " +
         "     public function Bar()         " +
         "     {                             " +
         "        super();                   " +
         "     }                             " +
         "  }                                ", "unsafe");
}
catch (error)
{
   exceptionThrown16 = true;
   actualError16 = error;
}




// -----------------------------------------------------------------------
function super09()
{
   apInitTest ("super09");


   // -----------------------------------------------------------------------
   apInitScenario ("16. Beta.Bar() calls Alpha.Alpha() with super()");
      
   if (exceptionThrown16 == false)
      apLogFailInfo ("No compile error in 16.", "Should give a compile error", exceptionThrown16, "");
   if (actualError16 != expectedError16)
      apLogFailInfo ("Wrong compile error in 16.", expectedError16, actualError16, "");
      
      
   apEndTest();
}


super09();


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
