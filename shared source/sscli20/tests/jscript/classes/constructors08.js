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


var iTestID = 193762;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown18_11_1 = false;
var actualError18_11_1 = "";
var expectedError18_11_1 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_11_1                                    " +
         "  {                                                     " +
         "     public var value: int;                             " +
         "                                                        " +
         "     private function Alpha18_11_1()                    " +
         "     { }                                                " +
         "                                                        " +
         "     private function Alpha18_11_1 (x: int, y: boolean) " +
         "     {                                                  " +
         "        value = x;                                      " +
         "     }                                                  " +
         "  }                                                     " +
         "                                                        " +
         "  class Beta18_11_1 extends Alpha18_11_1                " +
         "  {                                                     " +
         "     public function Beta18_11_1 (x: int, y: boolean)   " +
         "     {                                                  " +
         "        super (x, y);                                   " +
         "     }                                                  " +
         "  }                                                     ", "unsafe");
}
catch (error)
{
   exceptionThrown18_11_1 = true;
   actualError18_11_1 = error;
}


// -----------------------------------------------------------------------
function constructors08()
{
   apInitTest ("Constructors08");

   // -----------------------------------------------------------------------
   apInitScenario ("18.11.1 Private constructor ");
//                 "        Superclass with private default constructor and private constructor w/ parameters \n" +
//                 "        Subclass constructor calls superclass' constructor w/ parameters using super (…)");
   
   if (exceptionThrown18_11_1 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_11_1, "");
   if (actualError18_11_1 != expectedError18_11_1)
      apLogFailInfo ("Wrong compile error", expectedError18_11_1, actualError18_11_1, "");
      
      
   apEndTest();
}



constructors08();


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
