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


var iTestID = 193758;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;



// -----------------------------------------------------------------------
var exceptionThrown18_3 = false;
var actualError18_3 = "";
var expectedError18_3 = "ReferenceError: 'Beta18_3' is not accessible from this scope";

try
{
   eval ("  class Alpha18_3                           " +
         "  {  }                                      " +
         "  class Beta18_3 extends Alpha18_3          " +
         "  {                                         " +
         "     private function Beta18_3()            " +
         "     {  }                                   " +
         "  }                                         " +
         "  var beta18_3: Beta18_3 = new Beta18_3();  ", "unsafe");
}
catch (error)
{
   exceptionThrown18_3 = true;
   actualError18_3 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown18_4 = false;
var actualError18_4 = "";
var expectedError18_4 = "ReferenceError: 'Charlie18_4' is not accessible from this scope";

try
{
   eval ("  class Alpha18_4                           " +
         "  {  }                                      " +
         "  class Beta18_4 extends Alpha18_4          " +
         "  {  }                                      " +
         "  class Charlie18_4 extends Beta18_4        " +
         "  {                                         " +
         "     private function Charlie18_4()         " +
         "     {  }                                   " +
         "  }                                         " +
         "  var charlie18_4: Charlie18_4 = new Charlie18_4();  ", "unsafe");
}
catch (error)
{
   exceptionThrown18_4 = true;
   actualError18_4 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown18_5 = false;
var actualError18_5 = "";
var expectedError18_5 = "ReferenceError: Base class constructor is not accessible from this scope";

try
{
   eval ("  class Alpha18_5                  " +
         "  {                                " +
         "     private function Alpha18_5()  " +
         "     { }                           " +
         "  }                                " +
         "  class Beta18_5 extends Alpha18_5 " +
         "  {                                " +
         "     private function Beta18_5()   " +
         "     { }                           " +
         "  }                                ", "unsafe");
}
catch (error)
{
   exceptionThrown18_5 = true;
   actualError18_5 = error;
}


// -----------------------------------------------------------------------
function constructors05()
{
   apInitTest ("Constructors05");


   // -----------------------------------------------------------------------
   apInitScenario ("18.3 Private constructor -- Subclass with private default constructor");
   
   if (exceptionThrown18_3 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_3, "");
   if (actualError18_3 != expectedError18_3)
      apLogFailInfo ("Wrong compile error", expectedError18_3, actualError18_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18.4 Private constructor -- Sub-subclass with private default constructor");
   
   if (exceptionThrown18_4 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_4, "");
   if (actualError18_4 != expectedError18_4)
      apLogFailInfo ("Wrong compile error", expectedError18_4, actualError18_4, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("18.5 Private constructor -- Superclass and subclass with private default constructors");

   if (exceptionThrown18_5 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown18_5, "200532");
   if (actualError18_5 != expectedError18_5)
      apLogFailInfo ("Wrong compile error", expectedError18_5, actualError18_5, "200532");


   apEndTest();
}


constructors05();


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
