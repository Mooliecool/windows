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


var iTestID = 193767;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;




// -----------------------------------------------------------------------
var exceptionThrown31_6 = false;
var actualError31_6 = "";
var expectedError31_6 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha31_6                     " +
         "  {                                   " +
         "     public function Alpha31_6()      " +
         "     {                                " +
         "     }                                " +
         "  }                                   " +
         "  class Beta31_6 extends Alpha31_6    " +
         "  {                                   " +
         "     public function Beta31_6()       " +
         "     {                                " +
         "        var x;                        " +
         "        super();                      " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown31_6 = true;
   actualError31_6 = error;
}





// -----------------------------------------------------------------------
function constructors12()
{
   apInitTest ("Constructors12");
    
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.6 Calling superclass' constructors using 'super'. Call to super() from a default constructor is not the first line of a constructor");
      
   if (exceptionThrown31_6 == false)
      apLogFailInfo ("No compile error in 31.6", "Should give a compile error", exceptionThrown31_6, "");
   if (actualError31_6 != expectedError31_6)
      apLogFailInfo ("Wrong compile error in 31.6", expectedError31_6, actualError31_6, "192201");

          

   
   apEndTest();
}



constructors12();


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
