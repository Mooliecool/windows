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


var iTestID = 194194;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = ""; 
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown26 = false;
var actualError26 = "";
var expectedError26 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha26                       " +
         "  {                                   " +
         "     public function Alpha26()        " +
         "     { }                              " +
         "  }                                   " +
         "  class Beta26 extends Alpha26        " +
         "  {                                   " +
         "     public function Beta26()         " +
         "     {                                " +
         "        var x;                        " +
         "        x = 20;                       " +
         "        super();                      " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown26 = true;
   actualError26 = error;
}


// -----------------------------------------------------------------------
class Alpha27
{
   public var value: String = "none";
   
   public function GetName()
   {
      return "Alpha27.GetName()";
   }
   
}

class Beta27 extends Alpha27
{
   public function Beta27()
   {
      value = super.GetName();
   }
}


// -----------------------------------------------------------------------
class Alpha28
{
   public var value: String = "none";
   
   public function GetName()
   {
      return "Alpha28.GetName()";
   }
   
}

class Beta28 extends Alpha28
{
   public function Beta28 (x: String)
   {
      value = super.GetName();
   }
}


// -----------------------------------------------------------------------
class Alpha29
{
   public var value: String = "none";
   
   public function GetName()
   {
      return "Alpha29.GetName()";
   }
   
}

class Beta29 extends Alpha29
{
}

class Charlie29 extends Beta29
{
   public function Charlie29()
   {
      value = super.GetName();
   }
}


// -----------------------------------------------------------------------
class Alpha30
{
   public var value: String = "none";
   
   public function GetName()
   {
      return "Alpha30.GetName()";
   }
   
}

class Beta30 extends Alpha30
{
}

class Charlie30 extends Beta30
{
   public function Charlie30 (x: String)
   {
      value = super.GetName();
   }
}


// -----------------------------------------------------------------------
abstract class Alpha31
{
   public var value: String = "none";
   abstract public function Bar(): String;
}

class Beta31 extends Alpha31
{
   public function Bar(): String
   {
      return "Beta31.Bar()";
   }
}

class Charlie31 extends Beta31
{
   public function Foo()
   {
      value = super.Bar();
   }
}


// -----------------------------------------------------------------------
function super13()
{
   apInitTest ("super13");


   // -----------------------------------------------------------------------
   apInitScenario ("26. Call to super must be first statement in constructor ");
      
   if (exceptionThrown26 == false)
      apLogFailInfo ("No compile error in 26.", "Should give a compile error", exceptionThrown26, "");
   if (actualError26 != expectedError26)
      apLogFailInfo ("Wrong compile error in 26.", expectedError26, actualError26, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27. Beta calls super.GetName() from a default constructor");
      
   var beta27: Beta27 = new Beta27();
   
   expected = "Alpha27.GetName()";
   actual = "";
   actual = beta27.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 27.", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("28. Beta calls super.GetName() from a non-default constructor");

   var beta28: Beta28 = new Beta28 ("hello");
   
   expected = "Alpha28.GetName()";
   actual = "";
   actual = beta28.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 28.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. Charlie calls super.GetName() from a default constructor");
      
   var charlie29: Charlie29 = new Charlie29();
   
   expected = "Alpha29.GetName()";
   actual = "";
   actual = charlie29.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 29.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30. Charlie calls super.GetName() from a non-default constructor");
      
   var charlie30: Charlie30 = new Charlie30 ("hello");
   
   expected = "Alpha30.GetName()";
   actual = "";
   actual = charlie30.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 30.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31. Beta implements the abstract method Alpha.Bar(); Charlie calls super.Bar()");
      
   var charlie31: Charlie31 = new Charlie31();
   
   expected = "Beta31.Bar()";
   actual = "";
   charlie31.Foo();
   actual = charlie31.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 31.", expected, actual, "");
      
      

   apEndTest();
}


super13();


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
