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


var iTestID = 82044;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";
var version = Number(@_jscript_version).ToString("0.0")

// -----------------------------------------------------------------------
class Person1_1
{
   function GetAge()
   {
      return 11;
   }
}


// -----------------------------------------------------------------------
package Group1_2
{
   class Person1_2
   {
      function GetAge()
      {
         return 12;
      }
   }
         
   public class Student1_2
   {
      public function AccessAge()
      {
         var person1_2: Person1_2;
               
         person1_2 = new Person1_2();
         actual = 0;
         expected = 12;
               
         actual = person1_2.GetAge();
         if (actual != expected)
            apLogFailInfo ("Package is not the default within a method", expected, actual, "");
      }
   }
}


// -----------------------------------------------------------------------
class Person2
{
   internal function PackageGetAge()
   {
      return 14;
   }
         
   public function PublicGetAge()
   {
      var tempAge = 0;
      tempAge = PackageGetAge();
      return tempAge;
   }
}


// -----------------------------------------------------------------------

class Person3_1
{
   internal function GetAge()
   {
      return 11;
   }
}

class Student3_1 extends Person3_1
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 11;
         
      actual = GetAge();
      if (actual != expected)
         apLogFailInfo ("Package methods not accessible from a subclass -- default package", expected, actual, "119047");
   }
}
                             

// -----------------------------------------------------------------------

package Group3_2
{
   class Person3_2
   {
      internal function GetAge()
      {
         return 20;
      }   
   }
         
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 20;
            
         actual = GetAge();
         if (actual != expected)
            apLogFailInfo ("Package methods not accessible from a subclass", expected, actual, "119047");
      }
   }
}


// -----------------------------------------------------------------------
package Alpha3_3
{
   public class Person3_3
   {
      internal function GetAge()
      {
         return 16;
      }
   }
} 

import Alpha3_3;

package Beta3_3
{
   public class Student3_3 extends Person3_3    
   {                                            
      public function AccessAge()               
      {
         actual = 0;
         exceptionThrown = false;
         actualError = "";
         if ((version == "7.0")||(version == "8.0"))
           expectedError = "ReferenceError: 'GetAge' is not accessible from this scope";
         else 
           @if (@_fast) 
             expectedError = "ReferenceError: Variable 'GetAge' has not been declared";
           @else
             expectedError = "TypeError: Function expected";
           @end

         try
         {               
            eval ("actual = GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }

         if (exceptionThrown == false)
            apLogFailInfo ("Compile error not thrown", "Should give a compile error", exceptionThrown, "164696");               
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164696");            
         if (actual == 16)
            apLogFailInfo ("Package methods accesssible from a subclass outside the package","shouldn't be accessible", actual, "61982");
      }
   }
}


// -----------------------------------------------------------------------
package Group4
{
   class Person4
   {
      internal function GetAge()
      {
         return 17;
      }
   }
         
   public class Student4
   {
      public function AccessAge()
      {
         var person4: Person4;
               
         person4 = new Person4();
         actual = 0;
         expected = 17;
               
         actual = person4.GetAge();
         if (actual != expected)
            apLogFailInfo ("Package methods not accessible inside the package",expected, actual, "");
      }
   }
}


// -----------------------------------------------------------------------
package Group5
{
   public class Person5
   {
      internal function GetAge()
      {
         return 19;
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
      internal function GetAge()
      {
         return 20;
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
         var person6: Person6;
               
         person6 = new Person6();      
         actual = 0;
         expected = 20;
         exceptionThrown = false;
            
         try
         {      
            eval ("actual = person6.GetAge();");
         }
         catch (error)
         {
            exceptionThrown = true;
         }

         if ( (actual != expected) || (exceptionThrown == true) )
            apLogFailInfo ("Package methods not accessible by a nested package",expected, actual, "71717");
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
            apLogFailInfo ("Exception not thrown","Should throw an exception", exceptionThrown, "84566");
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
      internal function GetAge()
      {
         return 17;
      }
   }
}
*/


// -----------------------------------------------------------------------
class Person8
{
   internal function GetName()
   {
      return "person";
   }
}

class Student8 extends Person8
{
   internal function GetName()
   {
      return "student";
   }
}

class CollegeStudent8 extends Student8
{
}


// -----------------------------------------------------------------------
class Person9
{
   internal function GetAge()
   {
      return 20;
   }  
}   



import Group1_2;
import Group3_2;
import Beta3_3;
import Group4;
import Group5;
import Group7;


// -----------------------------------------------------------------------
function accesscontroltopackagemethods()
{
   apInitTest ("AccessControlToPackageMethods");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("1.1 Package is the default?  Within the default package?");
   // Yes

   var person1_1: Person1_1;

   person1_1 = new Person1_1();
   actual = 0;
   expected = 11;

   actual = person1_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Package is not the default", expected, actual, "");


              
   // -----------------------------------------------------------------------
   apInitScenario ("1.2 Package is the default? -- inside a package.");
   // Yes

   var student1_2: Student1_2;

   student1_2 = new Student1_2();
   student1_2.AccessAge();



   /*
   HermanV:  Functions will no longer be supported inside packages.

   // -----------------------------------------------------------------------
   // apInitScenario ("1.3 Package is the default? -- from a function.");
   // Yes

   package People1_3
   {
      class Person1_3
      {
         function GetAge()
         {
            return 13;
         }
      }
         
      public function AccessAge()
      {
         var person = new Person1_3();
         var tempAge = 0;
            
         tempAge = person.GetAge();
         return tempAge;
      }
   }

   import People1_3;

   var actual = 0;
   var expected = 13;

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Package is not the default", expected, actual, "");
   */



   // -----------------------------------------------------------------------
   apInitScenario ("2. Package methods accessible within the class itself?");
   // Yes

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   expected = 14;

   actual = person2.PublicGetAge();
   if (actual != expected)
      apLogFailInfo ("Package methods not accessible within the class itself",expected, actual, "");
                     
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Package methods accessible from a subclass?  Default package.");
   // Yes

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.AccessAge();

  
                        
   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Package methods accessible from a subclass?  Inside the package");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge()
   


   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Package methods accessible from a subclass?  Outside the package");
   // No

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("4. Package methods accessible inside the package?");
   // Yes

   var student4: Student4;

   student4 = new Student4();
   student4.AccessAge();



   /*
   HermanV:  Functions will no longer be supported in packages.

   // -----------------------------------------------------------------------
   // apInitScenario ("4.2 Package methods accessible inside the package? -- from a function");
   // Yes

   package People4_2
   {
      class Person4_2
      {
         internal method GetAge()
         {
            return 18;
         }      
      }
         
      public function AccessAge()
      {
         var person = new Person4_2();
         var tempAge = 0;
            
         tempAge = person.GetAge();
         return tempAge;
      }
   }

   import People4_2;

   actual = 0;
   expected = 18;

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Package methods not accessible inside the package - from a function",expected, actual, "");
   */



   // -----------------------------------------------------------------------
   apInitScenario ("5. Package methods accessible outside the package?");
   // No

   var person5: Person5 = new Person5();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   if ((version == "7.0")||(version == "8.0"))  expectedError = "ReferenceError: 'person5.GetAge' is not accessible from this scope";
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

   if (actual == 19)
      apLogFailInfo ("Package methods accessible outside the package","Package methods should not be",actual, "150458");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing package methods outside the package does not thow any error","Should throw an error", exceptionThrown, "164696, 150458");
   if (actualError != expectedError)   
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164696, 150458");                     


         
   // -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 
   
   // apInitScenario ("6. Package methods accessible by a nested package?");
   // Yes

   import Group6.InnerGroup6;

   var student6: Student6;

   student6 = new Student6();
   student6.AccessAge();
*/   
          


   /*
   HermanV: Functions will no longer be supported in packages.
                        
   // -----------------------------------------------------------------------
   // apInitScenario ("6.2 Package methods accessible by a nested package? -- from a function");
   // Yes

   package People6_2
   {
      class Person6_2
      {
         internal function GetAge()
         {
            return 21;
         }
      }
         
      public package InnerPeople6_2
      {
         public function AccessAge()
         {
            var person = new Person6_2();
            var tempAge = 0;
               
            tempAge = person.GetAge();
            return tempAge;
         }
      }
   }

   import People6_2.InnerPeople6_2;

   actual = 0;
   expected = 21;

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Package methods not accessible by a nested package - from a function",expected, actual, "");
   */



   // -----------------------------------------------------------------------
   /*
   PTorr:
      The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
      inside a package; functions, code, and nested packages will not be allowed. This restriction 
      may be lifted in future versions. 
   
   // apInitScenario ("7. Package method accessible by an enclosing package?");
   // No

   var person7: Person7;

   person7 = new Person7();
   person7.AccessAge();    
   */



   // -----------------------------------------------------------------------
   apInitScenario ("8. Subclass defines a method with the same name as superclass' method");
   // Check if the right method is called

   var person8: Person8;
   var student8: Student8;
   var collegeStudent8: CollegeStudent8;

   person8 = new Person8();
   student8 = new Student8();
   collegeStudent8 = new CollegeStudent8();

   // ----------------------------------------------------------------

   actual = "";
   expected = "person";
   exceptionThrown = false;

   try
   {
      actual = person8.GetName();
   }
   catch (error)
   {
      exceptionThrown = true
   }

   if ( (actual != expected) || (exceptionThrown == true) )
      apLogFailInfo ("Wrong value", expected, actual, "76686");

   // ----------------------------------------------------------------
         
   actual = "";  
   expected = "student";
   exceptionThrown = false;

   try
   {
      actual = student8.GetName();
   }
   catch (error)
   {
      exceptionThrown = true;
   }

   if ( (actual != expected) || (exceptionThrown == true) )
      apLogFailInfo ("Wrong value", expected, actual, "76686");
         
   // ----------------------------------------------------------------
         
   actual = "";  
   expected = "student";
   exceptionThrown = false;

   try
   {
      actual = collegeStudent8.GetName();   
   }
   catch (error)
   {
      exceptionThrown = true
   }

   if ( (actual != expected) || (exceptionThrown == true) )
      apLogFailInfo ("Wrong value", expected, actual, "76686");
         
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("9. Package method accessible within the default package?");
   // Yes

   var person9: Person9;
         
   person9 = new Person9();
   actual = 0;
   expected = 20;
   exceptionThrown = false;

   try
   {
      actual = person9.GetAge();
   }
   catch (error)
   {
      exceptionThrown = true;
   }

   if ( (actual != expected) || (exceptionThrown == true) )
      apLogFailInfo ("Package method not accessible within the default package",expected, actual, "76686");
                         
                        
   apEndTest();
}                     



accesscontroltopackagemethods();


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
