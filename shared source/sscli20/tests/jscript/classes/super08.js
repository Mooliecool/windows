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


var iTestID = 194188;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown15_2 = false;
var actualError15_2 = "";
var expectedError15_2 = "ReferenceError: The static member 'name' must be accessed with the class name";

try
{
   eval ("  class Alpha15_2                     " +
         "  {                                   " +
         "     static var name: String;         " +
         "  }                                   " +
         "  class Beta15_2 extends Alpha15_2    " +
         "  {                                   " +
         "  }                                   " +
         "  class Charlie15_2 extends Beta15_2  " +
         "  {                                   " +
         "     public function Bar()            " +
         "     {                                " +
         "        super.name = \"Zoo\";         " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown15_2 = true;
   actualError15_2 = error;
}




// -----------------------------------------------------------------------
function super08()
{
   apInitTest ("super08");


   // -----------------------------------------------------------------------
   apInitScenario ("15.2 Subclass modifies the value of super.field (static). " +
                        "3rd level in the inheritance tree.");
                        
   if (exceptionThrown15_2 == false)
      apLogFailInfo ("No compile error in 15.2", "Should give a compile error", exceptionThrown15_2, "200532");
   if (actualError15_2 != expectedError15_2)
      apLogFailInfo ("Wrong compile error in 15.2", expectedError15_2, actualError15_2, "");
      
      
   apEndTest();
}

super08();


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
