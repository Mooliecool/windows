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


var iTestID = 193769;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown31_8 = false;
var actualError31_8 = "";
var expectedError31_8 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha31_8                           " +
         "  {                                         " +
         "     public function Alpha31_8 (x: String)  " +
         "     {                                      " +
         "     }                                      " +
         "  }                                         " +
         "  class Beta31_8 extends Alpha31_8          " +
         "  {                                         " +
         "     public function Beta31_8()             " +
         "     {                                      " +
         "        var x = 10 + 30;                    " +
         "        super (\"hello\");                  " +
         "     }                                      " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown31_8 = true;
   actualError31_8 = error;
}


// -----------------------------------------------------------------------
function constructors14()
{
   apInitTest ("Constructors14");
    
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.8 Calling superclass' constructors using 'super'. Call to super(String) from a default constructor is not the first line of a constructor");
                        
   if (exceptionThrown31_8 == false)
      apLogFailInfo ("No compile error in 31.8", "Should give a compile error", exceptionThrown31_8, "");
   if (actualError31_8 != expectedError31_8)
      apLogFailInfo ("Wrong compile error in 31.8", expectedError31_8, actualError31_8, "192201");
           
           
   apEndTest();
}



constructors14();


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
