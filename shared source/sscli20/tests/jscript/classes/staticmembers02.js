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


var iTestID = 198978;

// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown5_2 = false;
var actualError5_2 = "";
var expectedError5_2 = "ReferenceError: The static member 'Foo' must be accessed with the class name";

try
{
   eval ("  class Alpha5_2                                  " +
         "  {                                               " +
         "     public static function Foo()                 " +
         "     {  }                                         " +
         "  }                                               " +
         "  class Beta5_2 extends Alpha5_2                  " +
         "  {                                               " +
         "     public function Bar()                        " +
         "     {                                            " +
         "        super.Foo();                              " +
         "     }                                            " +
         "  }                                               ", "unsafe");
}
catch (error)
{
   exceptionThrown5_2 = true;
   actualError5_2 = error;
}








// -----------------------------------------------------------------------
function staticmembers02()
{
   apInitTest ("StaticMembers02");

      
   // -----------------------------------------------------------------------
   apInitScenario ("5.2 Access a static member using super: Static method");
      
   if (exceptionThrown5_2 == false)
      apLogFailInfo ("No compile error in 5.2", "Should give a compile error", exceptionThrown5_2, "");
   if (actualError5_2 != expectedError5_2)
      apLogFailInfo ("Wrong compile error in 5.2", expectedError5_2, actualError5_2, "");
      
      
   
      
      
                       
  
         
   apEndTest();
}





staticmembers02();


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
