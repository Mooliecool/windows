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


var iTestID = 85286;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";



// -----------------------------------------------------------------------
class Person1
{
   public static var age = 5;
   static Person1
   {
      age = 10;
      counter = 2;
   }
   static Person1
   {
      age = 15;
      counter = 4;
   }   
   public static var counter = 8;
}


// -----------------------------------------------------------------------
class Person2
{
   public static var age = 5;
         
   static Person2 
   {
      age = 10;
   }
}


// -----------------------------------------------------------------------
var actualError3 = "";     
var exceptionThrown3 = false;

try
{
   eval ("class Person3             " +
         "{                         " +
         "   public var age = 18;   " +
         "   static Person3         " +
         "   {                      " +
         "      var x;              " +
         "      x = age;            " +
         "   }                      " +
         "}                         ", "unsafe");
}
catch (error)
{
   exceptionThrown3 = true;
   actualError3 = error;
}         


// -----------------------------------------------------------------------
class Person4
{
   private static var age = 5;
   
   public static function SetAge (newAge: int)
   {
      age = newAge;
   }
   
   public function GetAge(): int
   {
      return age;
   }
   
   static Person4 
   {
      SetAge (20);
   }
}


// -----------------------------------------------------------------------
function staticinitializers01()
{
   apInitTest ("StaticInitializers01");


   // -----------------------------------------------------------------------
   apInitScenario ("1. Multiple static initializers allowed?");
   // Yes

   var person1: Person1;
   
   person1 = new Person1();
   actual = 0;
   expected = 15;
   actual = Person1.age;
   if (actual != expected)
      apLogFailInfo ("Error with multiple static initializers (1)", expected, actual, "167207, 85541");
      
   actual = 0;
   expected = 8;
   actual = Person1.counter;
   if (actual != expected)
      apLogFailInfo ("Error with multiple static initializers (2)", expected, actual, "167207, 85541");  
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("2. Can static initializers access class variables?");
   // Yes

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   expected = 10;
   actual = Person2.age;
   if (actual != expected)
      apLogFailInfo ("Static initializers cannot access class variables", expected, actual, "167207");
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("3. Can static initializers access instance variables?");
   // No

   expectedError = "ReferenceError: Non-static member 'age' is not accessible from a static scope";
   if (exceptionThrown3 == false)
      apLogFailInfo ("Static initializers accessing instance variables does not give a compile error","Should give a compile error", exceptionThrown3, "");
   if (actualError3 != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError3, "");
      
  
   // -----------------------------------------------------------------------
   apInitScenario ("4. Can static initializers access class methods?");
   // Yes
   
   var person4: Person4;
   
   person4 = new Person4();
   actual = 0;
   expected = 20;
   actual = person4.GetAge();
   if (actual != expected)
      apLogFailInfo ("Static initializers cannot access class methods", expected, actual, "167207");
   
       
   apEndTest();
}



staticinitializers01();


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
