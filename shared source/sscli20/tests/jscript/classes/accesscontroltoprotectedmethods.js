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


var iTestID = 82062;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";

// -----------------------------------------------------------------------
class Person1
{
   protected function ProtectedGetAge()
   {
      return 20;
   }
            
   public function PublicGetAge()
   {
      var actual = 0;
      var expected = 20;
               
      actual = ProtectedGetAge();
      if (actual != expected)
         apLogFailInfo ("Protected methods not accessible within the class itself", expected, actual, "");
   }
}

// -----------------------------------------------------------------------
class Person2
{
   protected function GetAge()
   {
      return 21;
   }
}

// -----------------------------------------------------------------------
class Person3_1
{
   protected function ProtectedGetAge()
   {
      return 22;
   }
}

class Student3_1 extends Person3_1
{
   public function PublicGetAge()
   {
      var actual = 0;
      var expected = 22;
      
      actual = ProtectedGetAge();
      if (actual != expected)
         apLogFailInfo ("Protected methods not accessible from a subclass", expected, actual, "75880, 119190");
   }
}

// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      protected function GetAge()
      {
         return 12;
      }
   }
      
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 12;
            
         actual = GetAge();
         if (actual != expected)
            apLogFailInfo ("Protected methods not accessible from a subclass -- inside the package", expected, actual, "");
      }
   }
} 

// -----------------------------------------------------------------------
package GroupA3_3
{
   public class Person3_3
   {
      protected function GetAge()
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
         var expected = 13;
            
         actual = GetAge();
         if (actual != expected)
            apLogFailInfo ("Protected fields accessible from a subclass? -- outside the package", expected, actual, "");
      }
   }
}   

// -----------------------------------------------------------------------
class Person4
{
   protected function GetAge()
   {
      return 23;
   }
}

class Student4 extends Person4
{
}

class CollegeStudent4 extends Student4
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 23;
      
      actual = GetAge();
      if (actual != expected)
         apLogFailInfo ("Protected methods not accessible from a sub-subclass", expected, actual, "190290, 75880");
   }
}

// -----------------------------------------------------------------------
package Group5
{
   class Person5
   {
      protected function GetAge()
      {
         return 24;
      }
   }
            
   public class Student5
   {
      public function AccessAge()
      {
         var actual = 0;
         var exceptionThrown = false;
         var expectedError = "ReferenceError: 'person5.GetAge' is not accessible from this scope";
         var actualError = "";
         var person5: Person5;
                  
         person5 = new Person5();
                  
         try
         {
            eval ("actual = person5.GetAge();", "unsafe");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
                  
         if (actual == 24)
            apLogFailInfo ("Protected methods accessible inside the package", "shouldn't be accessible", actual, "164696, 150458, 61982");
         if (exceptionThrown == false)
            apLogFailInfo ("Accessing protected methods inside the package does not throw an exception", "An exception should be thrown", exceptionThrown, "164696, 150458, 80092, 77272");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164696, 150458");
      }   
   }
}

// -----------------------------------------------------------------------
package Group6
{
   public class Person6
   {
      protected function GetAge()
      {
         return 26;
      }
   }
}

// -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 

package Group7
{
   public class Person7
   {
      protected function GetAge()   
      {
         return 27;
      }
   }
}

import Group7;

package Group7.InnerGroup7
{
   public class Student7
   {
      public function AccessAge()
      {
         var actual = 0;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "TypeError: Function expected";
         var person7: Person7;
                           
         person7 = new Person7();

         try
         {
            eval ("actual = person7.GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
         if (actual == 27)
            apLogFailInfo ("Protected methods accessible by a nested package", "shouldn't be accessible", actual, "61982");
         if (exceptionThrown == false)
            apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");                           
      }
   }
}

// -----------------------------------------------------------------------
//PTorr:  Nested packages will be supported in JScript 8

package Group8
{
   public class Person8
   {
      public function AccessAge()
      {
         var actual = 0;
         var student8: Student8;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "TypeError: Function expected";
            
         student8 = new Student8();
         try
         {
            eval ("actual = student8.GetAge();");
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

import Group8;

package Group8.InnerGroup8
{
   public class Student8
   {
      protected function GetAge()
      {
         return 17;
      }
   }
}
*/

// -----------------------------------------------------------------------
class Person9
{
   protected function ProtectedGetName()
   {
      return "person";
   }
            
   public function PublicGetName()
   {
      var tempName = "";
      tempName = ProtectedGetName();
      return tempName;
   }
}

class Student9 extends Person9
{
   protected function ProtectedGetName()
   {
      return "student";
   }
            
   public function PublicGetName()
   {
      var tempName = "";
      tempName = ProtectedGetName();
      return tempName;
   }
}

class CollegeStudent9 extends Student9
{
}

import Group3_2;
import GroupB3_3;
import Group5;
import Group6;

// -----------------------------------------------------------------------
function accesscontroltoprotectedmethods()
{
   apInitTest ("AccessControlToProtectedMethods");


   // -----------------------------------------------------------------------
   apInitScenario ("1. Protected methods accessible within the class itself?");
   // Yes

   var person1: Person1;

   person1 = new Person1();
   person1.PublicGetAge();

   // -----------------------------------------------------------------------
   apInitScenario ("2. Protected methods accessible from outside a class?");
   // No

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: 'person2.GetAge' is not accessible from this scope";

   try
   {
      eval ("actual = person2.GetAge()", "unsafe");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 21) 
      apLogFailInfo ("Protected methods are accessible from outside a class.", "shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "164696, 77272");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");

   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Protected methods accessible from a subclass?  Default package");
   // Yes

   var student3_1: Student3_1 

   student3_1 = new Student3_1();
   student3_1.PublicGetAge();

   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Protected methods accessible from a subclass?  Inside the package.");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();

   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Protected method accessible from a subclass?  Outside the package.");
   // Yes

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();

   // -----------------------------------------------------------------------
   apInitScenario ("4. Protected methods accessible from a sub-subclass?");
   // Yes

   var collegeStudent4: CollegeStudent4 

   collegeStudent4 = new CollegeStudent4();
   collegeStudent4.AccessAge();

   // -----------------------------------------------------------------------
   apInitScenario ("5. Protected methods accessible inside the package?");
   // No

   var student5: Student5;

   student5 = new Student5();
   student5.AccessAge();

   /*
   HermanV: Functions will no longer be supported in packages.
   // -----------------------------------------------------------------------
   // apInitScenario ("5.2 Protected methods accessible inside the package? From a function?");
   // No

   package People5_2
   {
      class Person5_2
      {
         protected function GetAge()
         {
            return 25;
         }   
      }
            
      public function AccessAge()
      {
         var person = new Person5_2();
         var tempAge = 0;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "blah";
               
         try
         {
            eval ("tempAge = person.GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
               
         if (exceptionThrown == false)
            apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");
                                 
         return tempAge;     
      }
   }

   import People5_2;

   var actual = 0;

   actual = AccessAge();
   if (actual == 25)
      apLogFailInfo ("Protected methods are accessible inside a package - from a function", "Protected methods should not be accessible inside a package", actual, "");
   */

   // -----------------------------------------------------------------------
   apInitScenario ("6. Protected methods accessible outside the package?");
   // No

   var person6: Person6;

   person6 = new Person6();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: 'person6.GetAge' is not accessible from this scope";

   try
   {
      eval ("actual = person6.GetAge();", "unsafe");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 26)
      apLogFailInfo ("Protected methods are accessible outside the package", "shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "164696");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");

/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 
   // -----------------------------------------------------------------------
   // apInitScenario ("7. Protected methods accessible by a nested package?");
   // No
   
   import Group7.InnerGroup7;

   var student7 = new Student7();
   student7.AccessAge();

//HermanV: Functions will no longer be supported in packages.                         
   // -----------------------------------------------------------------------
   // apInitScenario ("7.2 Protected methods accessible by a nested package?  From a function?");
   // No

   package People7_2
   {
      class Person7_2
      {
         protected function GetAge()
         {
            return 28;
         }
      }
            
      public package InnerPeople7_2
      {
         public function AccessAge()
         {
            var person = new Person7_2();
            var tempAge = 0;
            var exceptionThrown = false;
            var actualError = "";
            var expectedError = "blah";
                  
            try
            {
               eval ("tempAge = person.GetAge();");
            }
            catch (error)
            {
               exceptionThrown = true;
               actualError = error;
            }
                  
            if (exceptionThrown == false)
               apLogFailInfo ("Exception not thrown", "An exception should be thrown", exceptionThrown, "");
            if (actualError != expectedError)
               apLogFailInfo ("Wrong error message", expectedError, actualError, "");
                  
            return tempAge;
         }   
      } 
   }

   import People7_2;

   var actual = 0;

   actual = AccessAge();
   if (actual == 28)
      apLogFailInfo ("Protected methods are accessible by a nested package - from a function", "Protected methods should not be accessible by a nested package", actual, "");

//PTorr:  Nested packages will be supported in JScript 8
   // -----------------------------------------------------------------------
   // apInitScenario ("8. Protected method accessible by an enclosing package?");
   // No

   import Group8;
   
   var person8: Person8;

   person8 = new Person8();
   person8.AccessAge();
*/

   // -----------------------------------------------------------------------
   apInitScenario ("9. Subclass defines a method with the same name as the superclass' method");
   // Check which method will be called -- should be lowest subclass' definition

   var person9: Person9;
   var student9: Student9;
   var collegeStudent9: CollegeStudent9;

   person9 = new Person9();
   student9 = new Student9();
   collegeStudent9 = new CollegeStudent9();
   actual = "";
   expected = "";

   expected = "person";
   actual = person9.PublicGetName();
   if (actual != expected)
      apLogFailInfo ("Superclass returns the wrong value", expected, actual, "");

   expected = "student";
   actual = student9.PublicGetName();
   if (actual != expected)
      apLogFailInfo ("Subclass returns the wrong value", expected, actual, "");
            
   expected = "student";
   actual = collegeStudent9.PublicGetName();
   if (actual != expected)
      apLogFailInfo ("Sub-subclass returns the wrong value", expected, actual, "");

   apEndTest();
}


accesscontroltoprotectedmethods();


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
