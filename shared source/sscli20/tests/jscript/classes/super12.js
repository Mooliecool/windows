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


var iTestID = 194193;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = ""; 
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown21 = false;
var actualError21 = "";
var expectedError21 = "ReferenceError: Objects of type 'Alpha21' do not have such a member";

try
{
   eval ("  class Alpha21                          " +
         "  {                                      " +
         "     public function Alpha21 (x: String) " +
         "     { }                                 " +
         "  }                                      " +
         "  class Beta21 extends Alpha21           " +
         "  {                                      " +
         "     public function Bar()               " +
         "     {                                   " +
         "        super.Alpha21(\"hello\");        " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown21 = true;
   actualError21 = error;
}


// -----------------------------------------------------------------------
class Alpha22
{
   public var value: String = "none";
   
   public function Alpha22 (x: String)
   {
      value = "Alpha22.Alpha22(String)";
   }
}

class Beta22 extends Alpha22
{
   public function Alpha22 (x: String)
   {
      value = "Beta22.Alpha22(String)";
   }
}

class Charlie22 extends Beta22
{
   public function Bar()
   {
      super.Alpha22 ("hello");
   }
}


// -----------------------------------------------------------------------
class Alpha23
{
   public var value: String = "none";
   
   public function Beta23 (x: String)
   {
      value = "Alpha23.Beta23()";
   }
}

class Beta23 extends Alpha23
{
   public function Beta23 (x: String)
   {
      value = "Beta23.Beta23()";
   }
}

class Charlie23 extends Beta23
{
   public function Bar()
   {
      super.Beta23 ("hello");   
   }
}


// -----------------------------------------------------------------------
class Alpha24
{
   public var value: String = "none";
   
   public function Beta24()
   {
      value = "Alpha24.Beta24()";
   }
}

class Beta24 extends Alpha24
{
   public function Beta24 (x: String)
   {
      value = "Beta24.Beta24()";
   }
}

class Charlie24 extends Beta24
{
   public function Bar()
   {
      super.Beta24 ("hello");
   }
}


// -----------------------------------------------------------------------
class Alpha25
{
   public var value: String = "none";
   
   public function Alpha25 (x: String)
   {
      value = "Alpha25.Alpha25()";
   }
}

class Beta25 extends Alpha25
{
   public function Alpha25()
   {
      value = "Beta25.Alpha25()";
   }
}

class Charlie25 extends Beta25
{
   public function Bar()
   {
      super.Alpha25 ("hello");
   }
}



// -----------------------------------------------------------------------
function super12()
{
   apInitTest ("super12");


   // -----------------------------------------------------------------------
   apInitScenario ("21. Beta.Bar() calls Alpha.Alpha(String) with super.Alpha('hello')");

   if (exceptionThrown21 == false)
      apLogFailInfo ("No compile error in 21.", "Should give a compile error", exceptionThrown21, "");
   if (actualError21 != expectedError21)
      apLogFailInfo ("Wrong compile error in 21.", expectedError21, actualError21, "");


   // -----------------------------------------------------------------------
   apInitScenario ("22. Charlie.Bar() calls super.Alpha('hello'); Alpha.Alpha() and Beta.Alpha() exists");

   var charlie22: Charlie22 = new Charlie22();
   
   expected = "Beta22.Alpha22(String)";
   actual = "";
   charlie22.Bar();
   actual = charlie22.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 22.", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("23. Charlie.Bar() calls super.Beta('hello'); Alpha.Beta() and Beta.Beta() exists");

   var charlie23: Charlie23 = new Charlie23();
   
   expected = "Alpha23.Beta23()";
   actual = "";
   charlie23.Bar();
   actual = charlie23.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 23.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24. Charlie.Bar() calls super.Beta('hello'); Alpha.Beta() and Beta.Beta(String) exists");
      
   var charlie24: Charlie24 = new Charlie24();
   
   expected = "Alpha24.Beta24()";
   actual = "";
   charlie24.Bar();
   actual = charlie24.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 24.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("25. Charlie.Bar() calls super.Alpha(“hello”); Alpha.Alpha(String) and Beta.Alpha() exists");
      
   var charlie25: Charlie25 = new Charlie25();
   
   expected = "Beta25.Alpha25()";
   actual = "";
   charlie25.Bar();
   actual = charlie25.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 25.", expected, actual, "");
      
      

   
   apEndTest();
}


super12();


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
