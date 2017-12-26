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


var iTestID = 82061;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";
var version = ScriptEngineMajorVersion() + "." + ScriptEngineMinorVersion()

// -----------------------------------------------------------------------
class Person1
{
   protected var age = 23;
                  
   public function AccessAge()
   {
      var actual = 0;
      var expected = 23;
                     
      actual = age;
      if (actual != expected)
         apLogFailInfo ("Protected fields not accessible within the class itself", expected, actual, "");
   }   
}


// -----------------------------------------------------------------------
class Person2_1
{
   protected var age = 24;
}


// -----------------------------------------------------------------------
class Person2_2
{
   protected var age = 20;
   public function GetAge()
   {
      return age;
   }
}


// -----------------------------------------------------------------------
class Person3_1
{
   protected var age = 23;
}

class Student3_1 extends Person3_1
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 23;
         
      actual = age;
      if (actual != expected)
         apLogFailInfo ("Protected fields not accessible from a subclass", expected, actual, "80164");
   }
}


// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      protected var age = 12;
   }
      
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 12;
            
         actual = age;
         if (actual != expected)
            apLogFailInfo ("Protected fields not accessible from a subclass -- inside the package", expected, actual, "");
      }
   }
}      


// -----------------------------------------------------------------------
package GroupA3_3
{
   public class Person3_3
   {
      protected var age = 13;
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
            
         actual = age;
         if (actual != expected)
            apLogFailInfo ("Protected fields accessible from a subclass? -- outside the package", expected, actual, "");
      }
   }
}   



// -----------------------------------------------------------------------
class Person4
{
   protected var age = 17;
}

class Student4 extends Person4
{
}

class One extends Student4
{
}

class Two extends One
{
}

class Three extends Two
{
}

class Four extends Three
{
}

class CollegeStudent4 extends Four
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 17;
         
      actual = age;
      if (actual != expected)
         apLogFailInfo ("Protected fields not accessible from a sub-subclass", expected, actual, "80164");
   }
}


// -----------------------------------------------------------------------
package Group5
{
   class Person5
   {
      protected var age = 23;
   }
                                 
   public class Student5
   {
      public function AccessAge()
      {
         var exceptionThrown = false;
         var expectedError = "ReferenceError: 'person5.age' is not accessible from this scope";
         var actualError = "";
         var actual = 0;
         var person5: Person5;
                        
         person5 = new Person5();
                        
         try
         {
            eval ("actual = person5.age;", "unsafe");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
                        
         if (actual == 23)
            apLogFailInfo ("Protected fields accessible inside a package", "shouldn't be accessible", actual, "61982");
         if (exceptionThrown == false)
            apLogFailInfo ("Accessing protected fields from a method inside a package Should throw an exception", false, exceptionThrown, "164934, 150458, 80092, 77272");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164934, 150458");                  
      }
   }
}


// -----------------------------------------------------------------------
package Group6
{
   public class Person6
   {
      protected var age = 23;
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
      protected var age = 24;
   }
}

import Group7;

package Group7.InnerGroup7
{
   public class Student7
   {
      public function AccessAge()
      {
         var exceptionThrown = false;
         var expectedError = "blah";
         var actualError = "";
         var actual = 0;
         var person7: Person7;
                        
         person7 = new Person7();
                        
         try
         {
            eval ("actual = person7.age;");
         }
         catch (error)
         {
            exceptionThrown = true;   
            actualError = error;
         }
                       
         if (actual == 24)
            apLogFailInfo ("Protected fields accessible from a nested package", "shouldn't be accessible", actual, "61982");
         if (exceptionThrown == false)
            apLogFailInfo ("Accessing protected fields from a method inside a nested package Should throw an exception", exceptionThrown, "77272");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");                  
      }
   }
}
*/


// -----------------------------------------------------------------------
/*
PTorr -- Nested classes will be supported in JScript 8

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
         var expectedError = "blah";
            
         student8 = new Student8();
         try
         {
            eval ("actual = student8.age;");
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
      protected var age = 17
   }
}
*/


/*
This generates a compile error starting at JS 7.0.8905
   
// -----------------------------------------------------------------------
class Person9_1
{
   protected var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student9_1 extends Person9_1
{
   protected var age = 16;
}


// -----------------------------------------------------------------------
class Person9_2
{
   protected var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student9_2 extends Person9_2
{
   protected var age = 16;
}


// -----------------------------------------------------------------------
class Person9_3
{
   private var age = 40;
}

class Student9_3 extends Person9_3
{
   private var age = 16;

   public function GetAge()
   {
      return age;
   }
}


// -----------------------------------------------------------------------
class Person9_4
{
   protected var age = 40;
}

class Student9_4 extends Person9_4
{
   protected var age = 16;
                  
   public function GetAge()
   {
      return age;
   }
}
*/


// -----------------------------------------------------------------------
/*
This is for Scenario 10: Subclass defines a field with the same name as its superclass
This is done to get around Bug 156617: 
   JS7 gives the wrong error message when a subclass defines a field with the same name as its superclass' and this is done in a function
*/

var sameNameActualError = "";
var sameNameExceptionThrown = false;

try
{
   eval ("  class Person10                   " +
         "  {                                " +
         "     protected var age = 10;       " +
         "  }                                " +
         "                                   " +
         "  class Student10 extends Person10 " +
         "  {                                " +
         "     protected var age = 15;       " +
         "  }                                ");
}
catch (error)
{
   sameNameExceptionThrown = true;
   sameNameActualError = error;
}


// -----------------------------------------------------------------------





// -----------------------------------------------------------------------
import Group3_2;
import GroupB3_3;
import Group5;
import Group6;




// -----------------------------------------------------------------------
function accesscontroltoprotectedfields()
{
   apInitTest ("AccessControlToProtectedFields");


   // -----------------------------------------------------------------------
   apInitScenario ("1. Protected fields accessible within the class itself?");
   // Yes

   var person1: Person1;

   person1 = new Person1();
   person1.AccessAge();
                                 

   // -----------------------------------------------------------------------
   apInitScenario ("2.1 Protected fields accessible outside a class?");
   // No

   var person2_1: Person2_1;

   person2_1 = new Person2_1();
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: 'person2_1.age' is not accessible from this scope";
   actual = 0;

   try
   {
      eval ("actual = person2_1.age;", "unsafe");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 24)
      apLogFailInfo ("Protected fields are accessible outside the class", "Shouldn't be accessible", actual, "61988");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing protected fields outside a class does not throw any errors", "An error should be thrown", exceptionThrown, "164934, 145569, 77272");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934, 77272");                  

                                 
   // -----------------------------------------------------------------------
   apInitScenario ("2.2 Protected fields accessible from outside a class through the expando property?");
   // No

   var person2_2: Person2_2;

   person2_2 = new Person2_2();
   actualError = "";
   expectedError = "ReferenceError: 'person2_2.age' is not accessible from this scope";
   exceptionThrown = false;
   expected = 0;
   actual = 0;

   expected = person2_2.GetAge();
   try
   {
      eval ("person2_2.age = 10;", "unsafe");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }   
   actual = person2_2.GetAge();

   if (actual != expected)
      apLogFailInfo ("Protected fields are accessible outside the class through the expando property.", expected, actual, "62402");
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "Exception should be thrown", exceptionThrown, "164934");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934");                  

                                
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Protected fields accessible from a subclass?  Default package.");
   // Yes

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.AccessAge();
                     
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Protected fields accessible from a subclass?  Inside the package.");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Protected fields accessible from a subclass?  Outside the package.");
   // Yes

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();

                     

   // -----------------------------------------------------------------------
   apInitScenario ("4. Protected fields accessible from a sub-subclass?");
   // Yes

   var collegeStudent4: CollegeStudent4;

   collegeStudent4 = new CollegeStudent4();
   collegeStudent4.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("5. Protected fields accessible inside a package?");
   // No

   var student5: Student5;

   student5 = new Student5();
   student5.AccessAge();



   /*
   HermanV:  Functions will no longer be supported inside packages.

   // -----------------------------------------------------------------------
   // apInitScenario ("5.2 Protected fields accessible inside a package? -- From a function?");
   // No

   package People5_2
   {
      class Person5_2
      {
         protected var age = 40;
      }
                  
      public function AccessAge()
      {
         var person = new Person5_2();
         return person.age;
      }
   }


   import People5_2;

   var exceptionThrown = false;
   var actualError = "";
   var expectedError = "blah";    // this is not the real error message
   var actual;

   try
   {
      actual = AccessAge();
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 40)
      apLogFailInfo ("Protected fields accessible inside a package", "shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing protected fields from a function inside a package does not throw any errors", exceptionThrown, true, "60951");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");                  
   */

   // -----------------------------------------------------------------------
   apInitScenario ("6. Protected fields accessible outside a package?");
   // No               

   var person6: Person6;

   person6 = new Person6();
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: 'person6.age' is not accessible from this scope";    // this is not the real error message
   actual = 0;

   try
   {
      eval ("actual = person6.age;", "unsafe");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 23)
      apLogFailInfo ("Protected fields accessible outside a package", "shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing protected fields inside a package does not throw any errors", "Should throw an exception", exceptionThrown, "164934, 77272");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934");                  

   
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 

   // -----------------------------------------------------------------------
   // apInitScenario ("7. Protected fields accessible from a nested package?");
   // No

   import Group7.InnerGroup7;

   var student7: Student7;

   student7 = new Student7();
   student7.AccessAge();

//HermanV:  Functions will no longer be supported in packages.
   // -----------------------------------------------------------------------
   // apInitScenario ("7.2 Protected fields accessible from a nested package? -- from a function? \n");
   // No

   package People7_2
   {
      class Person7_2
      {
         protected var age = 40;
      }
                  
      public package InnerPeople7_2
      {
         public function AccessAge()
         {
            var person = new Person7_2();
            return person.age;
         }
      }
   }

   import People7_2.InnerPeople7_2;

   var exceptionThrown = false;
   var actualError = "";
   var expectedError = "blah";    // this is not the real error message
   var actual;

   try
   {
      actual = AccessAge();
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (actual == 40)
      apLogFailInfo ("Protected fields accessible from a nested package", "shouldn't be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing protected fields from a function inside a nested package does not throw any errors", exceptionThrown, true, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");                  

//PTorr -- Nested classes will be supported in JScript 8
   // -----------------------------------------------------------------------
   // apInitScenario ("8. Protected field accessible by an enclosing package?");
   // No

   import Group8;
   
   var person8: Person8;

   person8 = new Person8();
   person8.AccessAge();

   //This generates a compile error starting at JS 7.0.8905
   // -----------------------------------------------------------------------
   // apInitScenario ("9.1 Subclass defines a protected field with the same name as superclass'  -- Access method in the superclass.  Superclass is instantiated");

   var person9_1: Person9_1;

   person9_1 = new Person9_1();
   expected = 40;
   actual = person9_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("9.2 Subclass defines a protected field with the same name as superclass'" +
   //                 " -- Access method in the superclass.  Subclass is instantiated");

   var student9_2: Student9_2;

   student9_2 = new Student9_2();
   expected = 40;
   actual = student9_2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("9.3 Subclass defines a protected field with the same name as superclass' -- Access method in the subclass.  Subclass is instantiated");

   var student9_3: Student9_3;

   student9_3 = new Student9_3();
   expected = 16;
   actual = student9_3.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("9.4 Subclass defines a protected field with the same name as superclass' -- Access method in the subclass.  Superclass is instantiated");
   
   var person9_4: Person9_4;

   person9_4 = new Person9_4();
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: Function expected";
                  
   try
   {
      eval ("person9_4.GetAge();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
                  
   if (exceptionThrown == false)
      apLogFailInfo ("Exception not thrown", "Exception should be thrown", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
   */

   // -----------------------------------------------------------------------
   apInitScenario ("10. Subclass defines a field with the same name as its superclass");
   // Compile error
   
   expectedError = "Error: Base class member 'Person10.age' is hidden by this declaration";
   
   if (sameNameExceptionThrown == false)
      apLogFailInfo ("No error thrown when a subclass defines a field with the same name as its superclass", true, sameNameExceptionThrown, "");
   if (sameNameActualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, sameNameActualError, "156617, 145577");                     

   apEndTest();
}
   


accesscontroltoprotectedfields();


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
