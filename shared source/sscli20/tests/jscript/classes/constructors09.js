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


var iTestID = 193763;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown18_11_2 = false;
var actualError18_11_2 = "";
var expectedError18_11_2 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_11_2                                       " +
         "  {                                                        " +
         "     public var value: String = \"three\";                 " +
         "                                                           " +
         "     private function Alpha18_11_2()                       " +
         "     {                                                     " +
         "        value = \"one\";                                   " +
         "     }                                                     " +
         "                                                           " +
         "     public function Alpha18_11_2 (x: double, y: String)   " +
         "     {                                                     " +
         "        value = \"two\";                                   " +
         "     }                                                     " +
         "  }                                                        " +
         "                                                           " +
         "  class Beta18_11_2 extends Alpha18_11_2                   " +
         "  {                                                        " +
         "     public function Beta18_11_2 (x: double, y: String)    " +
         "     {                                                     " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
}
catch (error)
{
   exceptionThrown18_11_2 = true;
   actualError18_11_2 = error;
}


// -----------------------------------------------------------------------
function constructors09()
{
   apInitTest ("Constructors09");


   // -----------------------------------------------------------------------
   apInitScenario ("18.11.2 Private constructor " );
//                   "        Superclass with private default constructor and public constructor w/ parameters \n" +
//                   "        Subclass constructor does not call any superclass constructor");
                  
   if (exceptionThrown18_11_2 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_11_2, "");
   if (actualError18_11_2 != expectedError18_11_2)
      apLogFailInfo ("Wrong compile error", expectedError18_11_2, actualError18_11_2, "");
      
      
   apEndTest();
}



constructors09();


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
