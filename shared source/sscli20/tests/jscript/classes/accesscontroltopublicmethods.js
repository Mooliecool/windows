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


var iTestID = 82065;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";


// -----------------------------------------------------------------------
class Person1
{
   public function GetAge1()
   {
      return 11;
   }
         
   public function GetAge2()
   {  
      var actual = 0;
      var expected = 11;
            
      actual = GetAge1();
      if (actual != expected)
         apLogFailInfo ("Public methods not accessible within the class itself", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Person2
{
   public function GetAge()
   {
      return 12;
   }
}


// -----------------------------------------------------------------------
class Person3_1
{
   public function GetAge()
   {
      return 13;
   }
}               

class Student3_1 extends Person3_1
{
   public function AccessAge()
   {
      var actual = 0;
      var expected = 13;
      
      actual = GetAge();
      if (actual != expected)
         apLogFailInfo ("Public methods not accessible from a subclass", expected, actual, "75880, 119047");
   }
}   



// -----------------------------------------------------------------------
package Group3_2
{
   class Person3_2
   {
      public function GetAge()
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
            apLogFailInfo ("Public fields not accessible from a subclass -- inside the package", expected, actual, "119047");
      }
   }
}      


// -----------------------------------------------------------------------
package GroupA3_3
{
   public class Person3_3
   {
      public function GetAge()
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
            apLogFailInfo ("Public fields not accessible from a subclass -- outside the package", expected, actual, "");
      }
   }
}   


// -----------------------------------------------------------------------
class Person4
{
   public function GetAge()
   {
      return 14;
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
      var expected = 14;
            
      try
      {
         eval ("actual = GetAge();");
      }
      catch (error)
      {
         apLogFailInfo ("Public methods not accessible from a sub-subclass (1)", "Shouldn't throw an exception", error, "75880");
      }
      if (actual != expected)
         apLogFailInfo ("Public methods not accessible from a sub-subclass (2)", expected, actual, "75880");
   }
}


// -----------------------------------------------------------------------
package Group5
{
   class Person5
   {
      public function GetAge()
      {
         return 15;
      }
   }
         
   public class Student5
   {
      public function AccessAge()
      {
         var person5: Person5;
         var actual = 0;
         var expected = 15;

         person5 = new Person5();
         actual = person5.GetAge();
         if (actual != expected)
            apLogFailInfo ("Public methods not accessible inside the package", expected, actual, "");
      }
   }
}


// -----------------------------------------------------------------------
package Group6
{
   public class Person6
   {
      public function GetAge()
      {
         return 17;
      }
   }
}


// -----------------------------------------------------------------------
/*


package Group7
{
   public class Person7
   {
      public function GetAge()
      {
         return 18;
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
         try
         {
            var person7: Person7;
            var actual = 0;
            var expected = 18;
                  
            person7 = new Person7();
            actual = person7.GetAge();
            if (actual != expected)
               apLogFailInfo ("Public methods not accessible by a nested package", expected, actual, "");
         }
         catch (error)
         {
            apLogFailInfo ("Instantiating a class in another class' method causes an exception", "this shouldn't cause an exception", error, "73234");
         }
      }
   }
}
*/


// -----------------------------------------------------------------------
class Person9
{
   public function GetName()
   {
      return "person";
   }  
}

class Student9 extends Person9
{
   public function GetName()
   {
      return "student";
   }
}

class CollegeStudent9 extends Student9
{
}


// -----------------------------------------------------------------------
class Person10
{
   public function GetAge()
   {
      return 10;
   }
}

class Student10 extends Person10
{
}



import Group3_2;
import GroupB3_3;
import Group5;
import Group6;


 
function accesscontroltopublicmethods()
{
   apInitTest ("AccessControlToPublicMethods");


   // -----------------------------------------------------------------------
   apInitScenario ("1. Public methods accessible within the class itself?");
   // Yes

   var person1: Person1;

   person1 = new Person1();
   person1.GetAge2();
                                 


   // -----------------------------------------------------------------------
   apInitScenario ("2. Public methods accessible from outside the class?");
   // Yes

   var person2: Person2;

   person2 = new Person2();
   actual = 0;
   expected = 12;

   actual = person2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Public methods not accessible from outside the class", expected, actual, "");


                                 
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Public methods accessible from a subclass?  Default package");
   // Yes

   var student3_1: Student3_1;

   student3_1 = new Student3_1();
   student3_1.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Public methods accessible from a subclass?  Inside the package.");
   // Yes

   var student3_2: Student3_2;

   student3_2 = new Student3_2();
   student3_2.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Public methods accessible from a subclass?  Outside the package.");
   // Yes

   var student3_3: Student3_3;

   student3_3 = new Student3_3();
   student3_3.AccessAge();



   // -----------------------------------------------------------------------
   apInitScenario ("4. Public methods accessible from a sub-subclass?");
   // Yes
                                 
   var collegeStudent4: CollegeStudent4;

   collegeStudent4 = new CollegeStudent4();
   collegeStudent4.AccessAge();

                                 

   // -----------------------------------------------------------------------
   apInitScenario ("5. Public methods accessible inside the package?");
   // Yes

   var student5: Student5;

   student5 = new Student5();
   student5.AccessAge();



   /*

   // -----------------------------------------------------------------------
   // apInitScenario ("5.2 Public methods accessible inside the package? From a function?");
   // Yes

   package People5_2
   {
      class Person5_2
      {
         public function GetAge()
         {
            return 16;
         }
      }
                  
      public function AccessAge()
      {
         var person = new Person5_2();
         var tempAge = 0;
                     
         tempAge = person.GetAge();
         return tempAge;     
      }
   }

   import People5_2;

   var actual = 0;
   var expected = 16;

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Public methods not accessible inside the package -- from a function", expected, actual, "");
   */

                                 
                                 
   // -----------------------------------------------------------------------
   apInitScenario ("6. Public methods accessible outside the package?");
   // Yes

   var person6: Person6;

   person6 = new Person6();
   actual = 0;
   expected = 17;

   actual = person6.GetAge();
   if (actual != expected)
      apLogFailInfo ("Public methods not accessible outside the package", expected, actual, "");



   // -----------------------------------------------------------------------
/*

   // apInitScenario ("7. Public methods accessible by a nested package?");
   // Yes

   import Group7.InnerGroup7;

   var student7: Student7;

   student7 = new Student7();
   student7.AccessAge();
*/   
                                 


   /*                  


   // -----------------------------------------------------------------------
   // apInitScenario ("7.2 Public methods accessible by a nested package -- from a function?");
   // Yes

   package People7_2
   {
      class Person7_2
      {
         public function GetAge()
         {
            return 19;
         }
      }
                  
      public package InnerPeople7_2
      {
         public function AccessAge()
         {
            var person = new Person7_2();
            var tempAge = 0;
                        
            tempAge = person.GetAge();
            return tempAge;
         }
      }
   }

   import People7_2.InnerPeople7_2;

   var actual = 0;
   var expected = 19;                  

   actual = AccessAge();
   if (actual != expected)
      apLogFailInfo ("Public methods not accessible by a nested package - from a function", expected, actual, "");
   */



   /* 
   // -----------------------------------------------------------------------
   // apInitScenario ("8. Public method accessible by an enclosing package?");
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
            actual = student8.GetAge();
            if (actual != expected)
               apLogFailInfo ("Public method not accessible by an enclosing package", expected, actual, "");
         }
      }
   }

   import Group8;

   package Group8.InnerGroup8
   {
      public class Student8
      {
         public function GetAge()
         {
            return 18;
         }
      }
   }

   var person8: Person8;

   person8 = new Person8();
   person8.AccessAge();
   */

                                 

   // -----------------------------------------------------------------------
   apInitScenario ("9. Subclass defines a method with the same name as superclass' method?");
   // Yes

   var person9: Person9;
   var student9: Student9;
   var collegeStudent9: CollegeStudent9;

   person9 = new Person9();
   student9  = new Student9();
   collegeStudent9 = new CollegeStudent9();

   actual = "";
   expected = "person";
   actual = person9.GetName();
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "");

   actual = "";
   expected = "student";
   actual = student9.GetName();
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "");

   actual = "";
   expected = "student";
   actual = collegeStudent9.GetName();
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "");
                  
                  
                  
   // -----------------------------------------------------------------------
   apInitScenario ("10. Inherited public methods accessible by an instance of a subclass?");
   // Yes

   var student10: Student10;

   student10 = new Student10();
   actual = 0;
   expected = 10;
   actual = student10.GetAge();
   if (actual != expected)
      apLogFailInfo ("Inherited public methods not accessible by an instance of a subclass", expected, actual, "58377");
   
                           
   apEndTest();
}



accesscontroltopublicmethods();


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
