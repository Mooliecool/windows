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


var iTestID = 192367;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var expectedError = "";
var actualError = "";
var exceptionThrown = false;


// -----------------------------------------------------------------------
var exceptionThrown10_4 = false;
var actualError10_4 = "";
var expectedError10_4 = "ReferenceError: 'counter' is read-only";

try
{
   eval ("  class Alpha10_4                     " +
         "  {                                   " +
         "     private const counter = 10;      " +
         "     public function SetCounter()     " +
         "     {                                " +
         "        counter = 10;                 " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown10_4 = true;
   actualError10_4 = error;
}


// -----------------------------------------------------------------------
function accesscontroltoprivatefields2()
{
   apInitTest ("AccessControlToPrivateFields2");


   // -----------------------------------------------------------------------
   apInitScenario ("10.4 Constants.  Not type annotated; initialized.");
   
   if (exceptionThrown10_4 == false)
      apLogFailInfo ("No compile error in 10.4", "Should give a compile error", exceptionThrown10_4, "300501");
   if (actualError10_4 != expectedError10_4)
      apLogFailInfo ("Wrong compile error in 10.4", expectedError10_4, actualError10_4, "300501");
  

         

   apEndTest();
}



accesscontroltoprivatefields2();


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
