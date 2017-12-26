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


var iTestID = 82052;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";
var version = Number(@_jscript_version).ToString("0.0")



// -----------------------------------------------------------------------
class Person1
{
   private function PrivateGetAge()
   {
      return 13;
   }   
            
   public function PublicGetAge()
   {
      var actual = 0;
      var expected = 13;
               
      actual = PrivateGetAge();
      if (actual != expected)
         apLogFailInfo ("Private methods not accessible within the class itself", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Person2
{
   private function GetAge()
   {
      return 30;
   }
}


// -----------------------------------------------------------------------
class Person3_1
{
   private function GetAge()
   {
      return 31;
   }
}

class Student3_1 extends Person3_1
{
   public function PublicGetAge()
   {
      var exceptionThrown = false;
      var actualError = "";
      var expectedError = "ReferenceError: 'GetAge' is not accessible from this scope";
      var actual = 0;
               
      try
      {
         eval ("actual = GetAge();", "unsafe");
      }
      catch (error)
      {
         exceptionThrown = true;
         actualError = error;
      }
               
      if (actual == 31)
         apLogFailInfo ("Private methods accessible from a subclass", "Shouldn't be accessible", actual, "61982");
      if (exceptionThrown == false)
         apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "164696");
      if (actualError != expectedError)
         apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");
   }
}


// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      private function GetAge()
      {
         return 12;
      }
   }
      
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var actualError = "";
         var expectedError = "ReferenceError: 'GetAge' is not accessible from this scope";
         var exceptionThrown = false;
            
         try
         {
            eval ("actual = GetAge();", "unsafe");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
            
         if (actual == 12) 
            apLogFailInfo ("Private field accessible from a subclass", "Shouldn't be accessible", actual, "");
         if (exceptionThrown == false)
            apLogFailInfo ("Error not thrown", "Should throw an error", exceptionThrown, "164696");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");
      }
   }
}


// -----------------------------------------------------------------------
package GroupA3_3
{
   public class Person3_3
   {
      private function GetAge()
      {
         return 13;
      }
   }
}

import GroupA3_3;

package GroupB3_3
{
   public class Student3_3 extends Person3_3
   {
      public function AccessAge()
      {
         var actual = 0;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "ReferenceError: 'GetAge' is not accessible from this scope";
            
         try
         {
            eval ("actual = GetAge();", "unsafe");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
            
         if (actual == 13)
            apLogFailInfo ("Private fields accessbible from a subclass", "Shouldn't be accessible", actual, "");
         if (exceptionThrown == false)
            apLogFailInfo ("Error not thrown", "Should throw an error", exceptionThrown, "164696");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");
      }
   }
}


// -----------------------------------------------------------------------
package Group4
{
   class Person4
   {
      private function GetAge()
      {
         return 32;
      }
   }
            
   public class Student4
   {
      public function AccessAge()
      {
         var actual = 0;
         var errorThrown = false;
         var actualError = "";
         var expectedError = "ReferenceError: 'person4.GetAge' is not accessible from this scope";
         var person4: Person4;

         person4 = new Person4();
                  
         try
         {
            eval ("actual = person4.GetAge();", "unsafe");
         }
         catch (error)
         {
            errorThrown = true;
            actualError = error;
         }
                  
         if (actual == 32)
            apLogFailInfo ("Private methods accessible inside the package", "Shouldn't be accessible", actual, "61982");
         if (errorThrown == false)
            apLogFailInfo ("Accessing private methods inside the package does not throw an exception", "An exception should be thrown", errorThrown, "80092, 77272");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");
      }
   }
}


// -----------------------------------------------------------------------
package Group5
{
   public class Person5
   {
      private function GetAge()
      {
         return 33;
      }
   }
}


// -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 

package Group6
{
   public class Person6
   {
      private function GetAge()
      {
         return 34;
      }
   }
}


import Group6;

package Group6.InnerGroup6
{
   public class Student6
   {
      public function AccessAge()
      {
         var actual = 0;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "TypeError: Function expected";
         var person6: Person6;
                           
         person6 = new Person6();         
                  
         try
         {
            eval ("actual = person6.GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
                  
         if (actual == 34)
            apLogFailInfo ("Private methods accessible by a nested package", "shouldn't be accessible", actual, "61982");
         if (exceptionThrown == false)
            apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");
      }
   }
}
*/


// -----------------------------------------------------------------------
/*
PTorr: Nested classes will be supported in JScript 8

package Group7
{
   public class Person7
   {
      public function AccessAge()
      {
         var actual = 0;
         var student7: Student7;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "TypeError: Function expected";
            
         student7 = new Student7();
         try
         {
            eval ("actual = student7.GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
         if (exceptionThrown == false)
            apLogFailInfo ("Exception not thrown", "Should throw an exception", exceptionThrown, "77272");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");                           
      }
   }
}

import Group7;

package Group7.InnerGroup7
{
   public class Student7
   {
      private function GetAge()
      {
         return 17;
      }
   }
}
*/


// -----------------------------------------------------------------------
class Person8
{
   private function GetName()
   {
      return "person";
   }
   
   public function PublicGetName()
   {
      var tempString: String = "";
      tempString = GetName();
      return tempString;
   }
}

class Student8 extends Person8
{
   private function GetName()
   {
      return "student";
   }
   
   public function PublicGetName()
   {
      var tempString: String = "";
      tempString = GetName();
      return tempString;
   }
}



import Group3_2;
import GroupB3_3;
import Group4;
import Group5;



// -----------------------------------------------------------------------
function accesscontroltoprivatemethods()
{
   apInitTest ("AccessControlToPrivateMethods");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("1. Private methods accessible within the class itself?");
   // Yes

   var person1: Person1;

   person1 = new Person1();
   person1.PublicGetAge();



   // -----------------------------------------------------------------------
   apInitScenario ("2. Private methods accessible from outside the class?");
   // No

   var person2: Person2;

   person2 = new Person2();
   exceptionThrown = false;
   actualError = "";
   if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'person2.GetAge' is not accessible from this scope";
   else expectedError = "ReferenceError: Objects of type 'Person2' do not have such a member";
   actual = 0;

   try
   {
      eval ("actual = person2.GetAge();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 30)
      apLogFailInfo ("Private methods are accessible", "Private methods shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "Exception should be thrown", exceptionThrown, "164696, 61982");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");
            

            
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Private methods accessible from a subclass?  Default package");
   // No

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.PublicGetAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Private methods accessible from a subclass?  Inside the package");
   // No

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Private methods accessible from a subclass?  Outside the package");
   // No

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("4. Private methods accessible inside the package?");
   // No

   var student4: Student4;

   student4 = new Student4();
   student4.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("5. Private methods accessible outside the package?");
   // No

   var person5: Person5;

   person5 = new Person5();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'person5.GetAge' is not accessible from this scope";
   else expectedError = "ReferenceError: Objects of type 'Group5.Person5' do not have such a member";

   try
   {
      eval ("actual = person5.GetAge();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 33)
      apLogFailInfo ("Private methods accessible outside the package", "shouldn't be accessible", actual, "61982, 150458");
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "164696, 150458");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164696, 150458");



   // -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 
   
   // apInitScenario ("6. Private methods accessible by a nested package?");
   // No

   import Group6.InnerGroup6;

   var student6: Student6;

   student6 = new Student6();
   student6.AccessAge();
*/   


   // -----------------------------------------------------------------------
/*
PTorr -- Nested classes will be supported in JScript 8

   // apInitScenario ("7. Private method accessible by an enclosing package?");
   // No

   import Group7;

   var person7: Person7;

   person7 = new Person7();
   person7.AccessAge();
*/



   // -----------------------------------------------------------------------
   apInitScenario ("8. Subclass defines a method with the same name as superclass'");
   // Check if the right method is called.

   var person8: Person8;
   var student8: Student8;

   person8 = new Person8();
   student8 = new Student8();

   actual = "";
   expected = "person";

   actual = person8.PublicGetName();
   if (actual != expected)
      apLogFailInfo ("The wrong method was called", expected, actual, "");

   actual = "";
   expected = "student";

   actual = student8.PublicGetName();
   if (actual != expected)
      apLogFailInfo ("The wrong method was called", expected, actual, "");


   apEndTest();
}


accesscontroltoprivatemethods();


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
