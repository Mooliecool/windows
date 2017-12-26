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


var iTestID = 82064;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";



// -----------------------------------------------------------------------
class Person1
{
   public var age = 40;
                  
   public function GetAge()
   {
      return age;
   }
}


// -----------------------------------------------------------------------
class Person2
{
   public var age = 23;
}


// -----------------------------------------------------------------------
class Person3_1
{
   public var age = 23;
}

class Student3_1 extends Person3_1
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 23;
                     
      try
      {  
         eval ("actual = age;");
      }
      catch (error)
      {
         apLogFailInfo ("Public fields not accessible from a subclass (1)", "Shouldn't throw an exception", error, "75880");
      }
      if (actual != expected)
         apLogFailInfo ("Public fields not accessible from a subclass (2)", expected, actual, "75880");
   }
}


// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      public var age = 12;
   }
         
   public class Student3_2 extends Person3_2
   {
      public function AccessAge()
      {
         var actual = 0;
         var expected = 12;
               
         actual = age;
         if (actual != expected)
            apLogFailInfo ("Public fields not accessible from a subclass -- inside the package", expected, actual, "");
      }
   }
}    


// -----------------------------------------------------------------------
package GroupA3_3
{
   public class Person3_3
   {
      public var age = 13;
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
            apLogFailInfo ("Public fields not accessible from a subclass -- outside the package", expected, actual, "");
      }
   }
}   
        

// -----------------------------------------------------------------------
class Person4
{
   public var age = 18;
}                  

class Student4 extends Person4
{
}

class CollegeStudent4 extends Student4
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 18;
                     
      try
      {
         eval ("actual = age;");
      }
      catch (error)
      {
         apLogFailInfo ("Public fields not accessible from a sub-subclass (1)", "Shouldn't throw an exception", error, "75880");
      }
      if (actual != expected)
         apLogFailInfo ("Public fields not accessible from a sub-subclass (2)", expected, actual, "75880");
   }
}


// -----------------------------------------------------------------------
package Group5
{
   class Person5
   {
      public var age = 16;
   }
                                 
   public class Student5
   {
      public function AccessAge ()
      {
         var actual = 0;
         var expected = 16;
         var person5: Person5;
                        
         person5 = new Person5();
         actual = person5.age;
         if (actual != expected)
            apLogFailInfo ("Public fields not accessible inside the package", expected, actual, "");
      }
   }
}


// -----------------------------------------------------------------------
package Group6
{
   public class Person6
   {
      public var age = 27;
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
      public var age = 26;   
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
         var expected = 26;
         var person7: Person7;
                        
         person7 = new Person7();
         actual = person7.age;
         if (actual != expected)
            apLogFailInfo ("Public fields not accessible from nested packages", expected, actual, "71717");
      }
   }
}
*/


/*
This generates a compile error starting at JS 7.0.8905

// -----------------------------------------------------------------------
class Person9_1
{
   public var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student9_1 extends Person9_1
{
   public var age = 16;
}


// -----------------------------------------------------------------------
class Person9_2
{
   public var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student9_2 extends Person9_2
{
   public var age = 16;
}


// -----------------------------------------------------------------------
class Person9_3
{
   public var age = 40;
}

class Student9_3 extends Person9_3
{
   public var age = 16;

   public function GetAge()
   {
      return age;
   }
}
         

// -----------------------------------------------------------------------
class Person9_4
{
   public var age = 40;
}

class Student9_4 extends Person9_4
{
   public var age = 16;
                  
   public function GetAge()
   {
      return age;
   }
}
*/


// -----------------------------------------------------------------------
class Person10
{
   public var age = 10;
}

class Student10 extends Person10
{
}


// -----------------------------------------------------------------------
/*
This is for Scenario 11: Subclass defines a field with the same name as its superclass
This is done to get around Bug 156617: 
   JS7 gives the wrong error message when a subclass defines a field with the same name as its superclass' and this is done in a function
*/

var sameNameActualError = "";
var sameNameExceptionThrown = false;

try
{
   eval ("  class Person11                   " +
         "  {                                " +
         "     public var age = 10;          " +
         "  }                                " +
         "                                   " +
         "  class Student11 extends Person11 " +
         "  {                                " +
         "     public var age = 15;          " +
         "  }                                ");
}
catch (error)
{
   sameNameExceptionThrown = true;
   sameNameActualError = error;
}




import Group3_2;
import GroupB3_3;
import Group5;
import Group6;



function accesscontroltopublicfields()
{
   apInitTest ("AccessControlToPublicFields");
   

   // -----------------------------------------------------------------------
   apInitScenario ("1. Private fields accessible within the class itself?");
   // Yes

   var person1: Person1;

   person1 = new Person1();
   expected = 40;
   actual = 0;

   actual = person1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Public fields not accessible within the class itself", expected, actual, "");                  
                                    
                                    
   // -----------------------------------------------------------------------
   apInitScenario ("2. Public fields accessible from outside the class?");
   // Yes

   var person2: Person2;

   person2 = new Person2();
   expected = 23;
   actual = 0;

   actual = person2.age;

   if (actual != expected)
      apLogFailInfo ("Public fields not accessible from outside the class", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Public fields accessible from a subclass?  Default package.");
   // Yes

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.AccessAge();


   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Public fields accessible from a subclass?  Inside the package.");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();


   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Public fields accessible from a subclass?  Outside the package.");
   // Yes

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();


   // -----------------------------------------------------------------------
   apInitScenario ("4. Public fields accessible from a sub-subclass?");
   // Yes
                                    
   var collegeStudent4: CollegeStudent4;

   collegeStudent4 = new CollegeStudent4();
   collegeStudent4.AccessAge();

                                    
   // -----------------------------------------------------------------------
   apInitScenario ("5. Public fields accessible inside the package?");
   // Yes

   var student5: Student5;

   student5 = new Student5();
   student5.AccessAge();


   /*
   HermanV:  Functions will no longer be supported in packages.

   // -----------------------------------------------------------------------
   // apInitScenario ("5.2 Public fields accessible inside the package? -- From a function?");
   // Yes

   package People5_2
   {
      class Person5_2
      {
         public var age = 28;
      }
                     
      public function AccessAge()
      {
         var person = new Person5_2();
         return person.age;
      }
   }

   import People5_2;

   var expected = 28;
   var actual;

   actual = AccessAge();

   if (actual != expected)
      apLogFailInfo ("Error in accessing public fields from a function within a package", expected, actual, "");
   */


   // -----------------------------------------------------------------------
   apInitScenario ("6. Public fields accessible outside the package?");
   // Yes

   var person6: Person6;

   person6 = new Person6();
   expected = 27;
   actual = 0;

   actual = person6.age;
   if (actual != expected)
      apLogFailInfo ("Public fields not accessible outside the package.", expected, actual, "");


   // -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 
   
   // apInitScenario ("7. Public fields accessible inside a nested package?");
   // Yes

   import Group7.InnerGroup7;

   var student7: Student7;

   student7 = new Student7();
   student7.AccessAge();
*/   
                                    
                                 
   /*
   HermanV:  Functions will no longer be allowed in packages

   // -----------------------------------------------------------------------
   // apInitScenario ("7.2 Public fields accessible inside a nested package? -- From a function?");
   // Yes

   package People7_2
   {
      class Person7_2
      {
         public var age = 28;
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

   var expected = 28;
   var actual;

   actual = AccessAge();

   if (actual != expected)
      apLogFailInfo ("Public fields not accessible from a function inside a nested package.", expected, actual, "");
   */   


   /*
   PTorr: Nested classes moved to JS 8
   // -----------------------------------------------------------------------
   // apInitScenario ("8. Public field accessible by an enclosing package?");
   // Yes

   package Group8
   {
      public class Person8
      {
         public function AccessAge()
         {  
            var actual = 0;
            var expected = 18;
            var student8: Student8;
                  
            student8 = new Student8();
            actual = student8.age;
            if (actual != expected)
               apLogFailInfo ("Public field not accessible by an enclosing package", expected, actual, "");
         }
      }
   }

   import Group8;

   package Group8.InnerGroup8
   {
      public class Student8
      {
         public var age = 18;
      }
   }


   var person8: Person8;

   person8 = new Person8();
   person8.AccessAge();
   */


   /*
   This generates a compile error starting at JS 7.0.8905

   // -----------------------------------------------------------------------
   // apInitScenario ("9.1 Subclass defines a public field with the same name as superclass' -- Access method in the superclass.  Superclass is instantiated");
                            
   var person9_1: Person9_1;

   person9_1 = new Person9_1();
   expected = 40;
   actual = 0;
   actual = person9_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");



   // -----------------------------------------------------------------------
   // apInitScenario ("9.2 Subclass defines a public field with the same name as superclass' -- Access method in the superclass.  Subclass is instantiated");

   var student9_2: Student9_2;

   student9_2 = new Student9_2();
   expected = 40;
   actual = 0;
   actual = student9_2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");          

                     
                     
   // -----------------------------------------------------------------------
   // apInitScenario ("9.3 Subclass defines a public field with the same name as superclass' -- Access method in the subclass.  Subclass is instantiated");

   var student9_3: Student9_3;

   student9_3 = new Student9_3();
   expected = 16;
   actual = 0;
   actual = student9_3.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");
                    

                                
   // -----------------------------------------------------------------------
   // apInitScenario ("9.4 Subclass defines a public field with the same name as superclass' -- Access method in the subclass.  Superclass is instantiated");

   var person9_4: Person9_4;

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: Function expected";
   person9_4 = new Person9_4();
                     
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
   apInitScenario ("10. Inherited public fields accessible by an instance of a subclass?");
   // Yes

   var student10: Student10;

   student10 = new Student10();
   actual = 0;
   expected = 10;
   actual = student10.age;
   if (actual != expected)
      apLogFailInfo ("Inherited public fields not accessible by an instance of a subclass", expected, actual, "82345");
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("11. Subclass defines a field with the same name as its superclass");
   // Compile error
   
   expectedError = "Error: Base class member 'Person11.age' is hidden by this declaration";
   
   if (sameNameExceptionThrown == false)
      apLogFailInfo ("No error thrown when a subclass defines a field with the same name as its superclass", true, sameNameExceptionThrown, "");
   if (sameNameActualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, sameNameActualError, "156617, 145569");                     
                       

   apEndTest();
}



accesscontroltopublicfields();


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
