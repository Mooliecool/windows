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


var iTestID = 82042;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var version = Number(@_jscript_version).ToString("0.0")


// -----------------------------------------------------------------------
class Person1_1 
{
   var age = 23;
}  


// -----------------------------------------------------------------------
package Group1_2
{
   class Person1_2
   {
      var age = 22;  // package by default?
   }
            
   public class Student1_2
   {
      public function AccessAge()
      {
         var person1_2: Person1_2;
                  
         person1_2 = new Person1_2();
         return person1_2.age;
      }
   }
}


// -----------------------------------------------------------------------
class Person2
{
   internal var age = 20;
               
   public function GetAge()
   {
      return age;
   }  
}


// -----------------------------------------------------------------------
class Person3_1
{
   internal var age = 11;
}

class Student3_1 extends Person3_1
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 11;
            
      actual = age;
      if (actual != expected)
         apLogFailInfo ("Package fields not accessible from a subclass -- default package", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      internal var age = 24;
   }
            
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 24;
               
         actual = age;
         if (actual != expected)
            apLogFailInfo ("Package field not accessible from a subclass -- inside the package", expected, actual, "");
      }
   }
}                  


// -----------------------------------------------------------------------
package A
{
   public class Person3_3
   {
      internal var age = 13;
   }
}

import A;

package B
{
   public class Student3_3 extends Person3_3
   {
      public function AccessAge()
      {
         var actual = 0;
         
         actualError = "";
         if ((version == "7.0")||(version == "8.0"))
           expectedError = "ReferenceError: 'age' is not accessible from this scope";
         else
           @if (@_fast) 
             expectedError = "ReferenceError: Variable 'age' has not been declared";
           @else
             expectedError = "ReferenceError: 'age' is undefined";
           @end
         exceptionThrown = false;
         
         try
         {
            eval ("actual = age;");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
         
         if (exceptionThrown == false)
            apLogFailInfo ("Error not thrown", "Should throw an error", exceptionThrown, "");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "");
         if (actual == 13) 
            apLogFailInfo ("Package fields accessible from a subclass", "Shouldn't be accessible", actual, "");
      }
   }
}


// -----------------------------------------------------------------------
package Group4
{
   class Person4
   {
      internal var age = 25;
   }
            
   public class Student4
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 25;
         var person4: Person4;
                  
         person4 = new Person4();
         actual = person4.age;
         if (actual != expected)
            apLogFailInfo ("Package fields not accessible inside the package", expected, actual, "");
      }  
   }
}                


// -----------------------------------------------------------------------
package Group5 
{
   public class Person5 
   {
      internal var age = 20;
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
      internal var age = 24;
   }
}   

import Group6;

package Group6.InnerGroup6
{
   public class Student6
   {
      public function AccessAge()
      {
         var person6: Person6;
                  
         person6 = new Person6();
         actual = 0;
         expected = 24;
         
         actual = person6.age;
         if (actual != expected) 
            apLogFailInfo ("Package fields not accessible by a nested package", expected, actual, "71717");
      }
   }
}
*/


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
      public function AccessAge()
      {
         var actual = 0;
         var exceptionThrown = false;
         var actualError = "";
         var expectedError = "SyntaxError: Type name expected";
            
         try
         {
            eval ("var student7: Student7;");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
            
         if (exceptionThrown == false)
            apLogFailInfo ("Exception not thrown", "Should throw an exception", exceptionThrown, "84566");
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
      internal var age = 17;
   }
}
*/


/*
This generates a compile error starting at JS 7.0.8905

// -----------------------------------------------------------------------
class Person8_1
{
   internal var age = 10;
            
   public function GetAge()
   {
      return age;
   }  
}

class Student8_1 extends Person8_1
{
   internal var age = 16;
}


// -----------------------------------------------------------------------
class Person8_2
{
   internal var age = 9;
            
   public function GetAge()
   {
      return age;
   }  
}

class Student8_2 extends Person8_2
{
   internal var age = 15;
}


// -----------------------------------------------------------------------
class Person8_3
{
   internal var age = 8;
}

class Student8_3 extends Person8_3
{
   internal var age = 14;
            
   public function GetAge()
   {
      return age;
   }
}          


// -----------------------------------------------------------------------
class Person8_4
{
   internal var age = 40;
}

class Student8_4 extends Person8_4
{
   internal var age = 16;
            
   public function GetAge()
   {
      return age;
   }
}
*/


// -----------------------------------------------------------------------
class Person9
{
   internal var age = 17;
}


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
         "     internal var age = 10;         " +
         "  }                                " +
         "                                   " +
         "  class Student10 extends Person10 " +
         "  {                                " +
         "     internal var age = 15;         " +
         "  }                                ");
}
catch (error)
{
   sameNameExceptionThrown = true;
   sameNameActualError = error;
}




import Group1_2;
import Group3_2;
import B;
import Group4;
import Group5;
import Group7;



// -----------------------------------------------------------------------
function accesscontroltopackagefields()
{
   apInitTest ("AccessControlToPackageFields");

   // -----------------------------------------------------------------------
   apInitScenario ("1.1  Package is the default? -- inside the default package");
   // Yes

   var person1_1: Person1_1;

   person1_1 = new Person1_1();
   actual = 0;
   expected = 23;

   actual = person1_1.age;
   if (actual != expected)
      apLogFailInfo ("Package is not the default access specifier within the default package.", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("1.2  Package is the default? -- inside a package.");
   // Yes

   var student1_2: Student1_2;

   student1_2 = new Student1_2();
   actual = 0;
   expected = 22;

   actual = student1_2.AccessAge();
   if (actual != expected)
      apLogFailInfo ("Package is not the default access specifier.", expected, actual, "");



   /*
   HermanV:  Functions will no longer be supported in packages
              
   // -----------------------------------------------------------------------
   // apInitScenario ("1.3  Package is the default?  Accessing from a function.");
   // Yes

   package People1_3
   {
      class Person1_3
      {
         var age = 21;
      }
            
      public function GetAge()
      {
         var person = new Person1_3();
         return person.age;
      }
   }     
              
   import People1_3;

   var expected = 21;
   var actual;

   actual = GetAge();
   if (actual != expected) 
      apLogFailInfo ("Package is not the default access specifier.", expected, actual, "");
   */


   // -----------------------------------------------------------------------
   apInitScenario ("2.  Accessible within the class itself? \n");
   // Yes

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   expected = 20;

   actual = person2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Package fields not accesssible within the class itself.", expected, actual, "");
                        
                        
                        
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Package fields accessible from a subclass?  Default package.");
   // Yes

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.AccessAge();
                        
                           
                           
   // -----------------------------------------------------------------------
   apInitScenario ("3.2  Package field accessible from a subclass?  Inside the package?");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.3  Package fields accessible from a subclass?  Outside the package");
   // No

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("4.  Package fields accessible inside the package?");
   // Yes

   var student4: Student4;

   student4 = new Student4();
   student4.AccessAge();



   /*

                           
   // -----------------------------------------------------------------------
   // apInitScenario ("4.2  Accessible inside the package?  From a function?");
   // Yes

   package People4_2
   {
      class Person4_2
      {
         internal var age = 19;
      }
            
      public function AccessAge()
      {
         var person = new Person4_2();
         return person.age;
      }
   }

   import People4_2;

   var expected = 19;
   var actual;

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Package fields not accessible inside the package from a function.", expected, actual, "");
   */



   // -----------------------------------------------------------------------
   apInitScenario ("5.  Package fields accessible outside the package?");
   // No
   
   var person5: Person5 = new Person5();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   if ((version == "7.0")||(version == "8.0"))  expectedError = "ReferenceError: 'person5.age' is not accessible from this scope";
   else expectedError = "ReferenceError: Objects of type 'Group5.Person5' do not have such a member";
   
   try
   {
      eval ("actual = person5.age;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (actual == 20)
      apLogFailInfo ("Package fields are accessible outside the package", "should not be accessible", actual, "61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing package fields outside the package does not throw any error", "Should throw an error", exceptionThrown, "164934, 150458");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934, 150458");                     



   // -----------------------------------------------------------------------
   /*
   PTorr:
      The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
      inside a package; functions, code, and nested packages will not be allowed. This restriction 
      may be lifted in future versions. 
   
   // apInitScenario ("6.  Package fields accessible by a nested package?");
   // Yes

   import Group6.InnerGroup6;

   var student6: Student6;

   student6 = new Student6();
   actual = student6.AccessAge();
   */



   // -----------------------------------------------------------------------
   /*
   PTorr:
      The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
      inside a package; functions, code, and nested packages will not be allowed. This restriction 
      may be lifted in future versions. 
   
   // apInitScenario ("7. Package field accessible by an enclosing package?");
   // No

   var person7: Person7;

   person7 = new Person7();
   person7.AccessAge();
   */


   /*
   This generates a compile error starting at JS 7.0.8905
   
   // -----------------------------------------------------------------------
   // apInitScenario ("8.1  Subclass defines a package field with the same name as superclass. Access method in the superclass; superclass is instantiated.");

   var person8_1: Person8_1;

   person8_1 = new Person8_1();
   actual = 0;
   expected = 10;

   actual = person8_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong result.", expected, actual, "");



   // -----------------------------------------------------------------------
   // apInitScenario ("8.2  Subclass defines a package field with the same name as superclass. Access method in the superclass; subclass is instantiated.");

   var student8_2: Student8_2;

   student8_2 = new Student8_2();
   actual = 0;
   expected = 9;

   actual = student8_2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong result", expected, actual, "");



   // -----------------------------------------------------------------------
   // apInitScenario ("8.3  Subclass defines a package field with the same name as superclass. Access method in the subclass; subclass is instantiated.");

   var student8_3: Student8_3;

   student8_3 = new Student8_3();
   actual = 0;
   expected = 14;

   actual = student8_3.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong value.", expected, actual, "");



   // -----------------------------------------------------------------------
   // apInitScenario ("8.4  Subclass defines a package field with the same name as superclass-- Access method in the subclass.  Superclass is instantiated");
      
   var person8_4: Person8_4;
      
   person8_4 = new Person8_4();
   actual = 0;

   try
   {
      eval ("actual = person8_4.GetAge();");
   }
   catch (error)
   {
   }
   if (actual == 16)
      apLogFailInfo ("Package field accessible", "Should give an error", actual, "");
   */
   

   // -----------------------------------------------------------------------
   apInitScenario ("9. Package field accessible within the default package?");
   // Yes

   var person9: Person9;

   person9 = new Person9();
   actual = 0;
   expected = 17;

   actual = person9.age;
   if (actual != expected)
      apLogFailInfo ("Package field not accessible within the default package", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. Subclass defines a field with the same name as its superclass");
   // Compile error
   
   expectedError = "Error: Base class member 'Person10.age' is hidden by this declaration";
      
   if (sameNameExceptionThrown == false)
      apLogFailInfo ("No error thrown when a subclass defines a field with the same name as its superclass", true, sameNameExceptionThrown, "");
   if (sameNameActualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, sameNameActualError, "164630");                     
                           
                 
                 
   apEndTest();
}


accesscontroltopackagefields();


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
