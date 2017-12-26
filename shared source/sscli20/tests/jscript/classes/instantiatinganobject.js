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


var iTestID = 82153;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
class Person1
{
   public var age = 31;
}


// -----------------------------------------------------------------------
class Person2
{
   public var age = 0;
   public function Person2()
   {
      age = 32;
   }
}


// -----------------------------------------------------------------------
class Person3
{
   public var age = 0;
   public function Person3 (age)
   {
      this.age = age;
   }
}
            

// -----------------------------------------------------------------------
class Person4
{
   static public var counter = 0;
            
   static Person4
   {
      counter = 0;
   }
            
   public function Person4()
   {
      counter++;
   }
   
   public function GetCounter()
   {
      return counter;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown6 = false;
var actualError6 = "";
var expectedError6 = "ReferenceError: Non-static member 'age' is not accessible from a static scope";

try
{
   eval ("  class Person6        " +
         "  {                    " +
         "     public var age;   " +
         "     static Person6    " +
         "     {                 " +
         "        age = 36;      " +
         "     }                 " +
         "  }                    " , "unsafe");
}
catch (error)
{
   exceptionThrown6 = true;
   actualError6 = error;
}



// -----------------------------------------------------------------------
class Person7
{
   public static var age = 0;

   static Person7
   {
      age = 37;
   }  
   
   public function GetAge()
   {
      return age;
   }
}


// -----------------------------------------------------------------------
class Person12
{
   public var booleanVar: Boolean;
   public var numberVar: Number;
   public var stringVar: String;
   
   public function Person12()
   {
      stringVar = new String();
   }
}


// -----------------------------------------------------------------------
class Person13
{
   public var numPerson13: Number;
   public var strPerson13: String;
   public var objPerson13: Object;
   public var boolPerson13: Boolean;
}

class Student13 extends Person13
{
   public var numStudent13: Number;
   public var strStudent13: String;
   public var objStudent13: Object;
   public var boolStudent13: Boolean;
}

class CollegeStudent13 extends Student13
{
   public var numCollegeStudent13: Number;
   public var strCollegeStudent13: String;
   public var objCollegeStudent13: Object;
   public var boolCollegeStudent13: Boolean;
}


// -----------------------------------------------------------------------

            

// -----------------------------------------------------------------------
function instantiatinganobject()
{
   apInitTest ("InstantiatingAnObject");


   // -----------------------------------------------------------------------
   apInitScenario ("1. Initialization in the declaration");
   // Check initialization of attributes in the declaration

   var moe: Person1;
   
   moe = new Person1();
   actual = 0;
   expected = 31;

   actual = moe.age;
   if (actual != expected)
      apLogFailInfo ("Error in intialization in the declaration (1)", expected, actual, "");                 


   var larry: Person1;
   
   larry = new Person1();
   actual = 0;
   expected = 31;

   actual = larry.age;
   if (actual != expected)
      apLogFailInfo ("Error in intialization in the declaration (2)", expected, actual, "");


   var curly: Person1;

   curly = new Person1();
   actual = 0;
   expected = 31;

   actual = curly.age;
   if (actual != expected)
      apLogFailInfo ("Error in intialization in the declaration (3)", expected, actual, "");

            

   // -----------------------------------------------------------------------
   apInitScenario ("2. Default constructor");
   // Object's fields properly initialized?

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   expected = 32;

   actual = person2.age;
   if (actual != expected)
      apLogFailInfo ("Error in default constructor", expected, actual, "");
      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Constructor with parameters");
   // Object's fields properly initialized?

   var person3: Person3;

   person3 = new Person3 (33);
   actual = 0;
   expected = 33;

   actual = person3.age;
   if (actual != expected)
      apLogFailInfo ("Error in constructor with parameters", expected, actual, "");



   // -----------------------------------------------------------------------
   apInitScenario ("4. Class initializer: fields properly updated?");

   var person4: Person4;

   person4 = new Person4();
   actual = 0;
   expected = 1;

   actual = person4.GetCounter();
   if (actual != expected)
      apLogFailInfo ("Fields not properly initialized by class the class initializer",expected, actual, "179249, 161976");    
                     
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("5. Class initializers invoked only once?");
   // This test scenario continues from the code above.

   var anotherPerson: Person4;

   anotherPerson = new Person4();
   actual = 0;
   expected = 2;

   actual = anotherPerson.GetCounter();
   if (actual != expected)
      apLogFailInfo ("Class initializers are not invoked only once", expected, actual, "167207");
 
   // -----------------------------------------------------------------------
   apInitScenario ("6. Can a class initializer update an instance variable?");
   // No

   if (exceptionThrown6 == false)
      apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown6, "144915");
   if (actualError6 != expectedError6)
      apLogFailInfo ("Wrong error message", expectedError6, actualError6, "144915");    
       
   // -----------------------------------------------------------------------
   apInitScenario ("7. Can a class initializer update a class variable?");
   // Yes

   var person7: Person7;

   person7 = new Person7();
   actual = 0;
   expected = 37;

   actual = person7.GetAge();
   if (actual != expected)
      apLogFailInfo ("Class initializer can't update a class variable",expected, actual, "167207");


   // -----------------------------------------------------------------------
   apInitScenario ("12. Implicit constructors");

   var person12: Person12 = new Person12();

   expected = false;
   actual = person12.booleanVar;
   if (actual != expected)
      apLogFailInfo ("Wrong value for Boolean", expected, actual, "");
      
   expected = "boolean";
   actual = "";
   actual = typeof (person12.booleanVar);
   if (actual != expected)
      apLogFailInfo ("Wrong type for Boolean", expected, actual, "");
      
   expected = 0;
   actual = -1;
   actual = person12.numberVar;
   if (actual != expected)
      apLogFailInfo ("Wrong value for Number", expected, actual, "");
      
   expected = "number";
   actual = "";
   actual = typeof (person12.numberVar);
   if (actual != expected)
      apLogFailInfo ("Wrong type for Number", expected, actual, "");
      
   expected = "";
   actual = "yes";
   actual = person12.stringVar;
   if (actual != expected)
      apLogFailInfo ("Wrong value for String", expected, actual, "");
      
   expected = "string";
   actual = "";
   actual = typeof (person12.stringVar);
   if (actual != expected) 
      apLogFailInfo ("Wrong type for String", expected, actual, "");
      
 

/*
   expected = 0;
   actual = person12.num;
   if (actual != expected)
      apLogFailInfo ("Wrong value for number", expected, actual, "");
         
   expected = null;
   actual = person12.str;
   if (actual != expected)
      apLogFailInfo ("Wrong value for string", expected, actual, "");
         
   expected = null;
   actual = person12.obj;
   if (actual != expected)
      apLogFailInfo ("Wrong value for object", expected, actual, "");
         
   expected = false;
   actual = person12.bool;
   if (actual != expected) 
      apLogFailInfo ("Wrong value for boolean", expected, actual, "");
*/         
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("13. Implicit constructors w/ inheritance \n");

   var collegeStudent13: CollegeStudent13;
   collegeStudent13 = new CollegeStudent13();

   expected = 0;
   actual = collegeStudent13.numPerson13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.numPerson13", expected, actual, "");
         
   expected = null;
   actual = collegeStudent13.strPerson13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.strPerson13", expected, actual, "");
         
   expected = null;
   actual = collegeStudent13.objPerson13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.objPerson13", expected, actual, "");

   expected = false;
   actual = collegeStudent13.boolPerson13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.boolPerson13", expected, actual, "");
         
   // ------------------------------------
         
   expected = 0;
   actual = collegeStudent13.numStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.numStudent13", expected, actual, "");
         
   expected = null;
   actual = collegeStudent13.strStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.strStudent13", expected, actual, "");
         
   expected = null;
   actual = collegeStudent13.objStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.objStudent13", expected, actual, "");

   expected = false;
   actual = collegeStudent13.boolStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.boolStudent13", expected, actual, "");

   // ------------------------------------

   expected = 0;
   actual = collegeStudent13.numCollegeStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.numCollegeStudent13", expected, actual, "");

   expected = null;
   actual = collegeStudent13.strCollegeStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.strCollegeStudent13", expected, actual, "");

   expected = null;
   actual = collegeStudent13.objCollegeStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.objCollegeStudent13", expected, actual, "");

   expected = false;
   actual = collegeStudent13.boolCollegeStudent13;
   if (actual != expected)
      apLogFailInfo ("Wrong value for collegeStudent13.boolCollegeStudent13", expected, actual, "");

         
   apEndTest();  

}



instantiatinganobject();


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
