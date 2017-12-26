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


var iTestID = 194191;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown17 = false;
var actualError17 = "";
var expectedError17 = "ReferenceError: Objects of type 'Alpha17' do not have such a member";

try
{
   eval ("  class Alpha17                    " +
         "  {                                " +
         "     public function Alpha17()     " +
         "     { }                           " +
         "  }                                " +
         "  class Beta17 extends Alpha17     " +
         "  {                                " +
         "     public function Bar()         " +
         "     {                             " +
         "        super.Alpha17();           " +
         "     }                             " +
         "  }                                ", "unsafe");
}
catch (error)
{
   exceptionThrown17 = true;
   actualError17 = error;
}


// -----------------------------------------------------------------------
class Alpha18
{
   public var value: String = "none";
   
   public function Alpha18()
   {
      value = "Alpha18.Alpha18()";
   }
}

class Beta18 extends Alpha18
{
   public function Alpha18()
   {
      value = "Beta18.Alpha18()";
   }
}

class Charlie18 extends Beta18
{
   public function Bar()
   {
      super.Alpha18();
   }
}


// -----------------------------------------------------------------------
class Alpha19
{
   public var value: String = "none";
   
   public function Beta19()
   {
      value = "Alpha19.Beta19()";
   }
}

class Beta19 extends Alpha19
{
   public function Beta19()
   {
      value = "Beta19.Beta19()";
   }
}

class Charlie19 extends Beta19
{
   public function Bar()
   {
      super.Beta19();
   }
}



// -----------------------------------------------------------------------
function super10()
{
   apInitTest ("super10");


   // -----------------------------------------------------------------------
   apInitScenario ("17. Beta.Bar() calls Alpha.Alpha() with super.Alpha()");
      
   if (exceptionThrown17 == false)
      apLogFailInfo ("No compile error in 17.", "Should give a compile error", exceptionThrown17, "");
   if (actualError17 != expectedError17)
      apLogFailInfo ("Wrong compile error in 17.", expectedError17, actualError17, "");
                   
                   
   // -----------------------------------------------------------------------
   apInitScenario ("18. Charlie.Bar() calls super.Alpha(); Alpha.Alpha() and Beta.Alpha() exists");
                        
   var charlie18: Charlie18 = new Charlie18();
   
   expected = "Beta18.Alpha18()";
   actual = "";
   charlie18.Bar();
   actual = charlie18.value;
   if (actual != expected)
      apLogFailInfo ("Error in 18.", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19. Charlie.Bar() calls super.Beta(); Alpha.Beta() and Beta.Beta() exists");
      
   var charlie19: Charlie19 = new Charlie19();
   
   expected = "Alpha19.Beta19()";
   actual = "";
   charlie19.Bar();
   actual = charlie19.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19.", expected, actual, "");



   
   apEndTest();
}


super10();


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
