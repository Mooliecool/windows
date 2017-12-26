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


var iTestID = 83986;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";
var version = Number(@_jscript_version).ToString("0.0")


// -----------------------------------------------------------------------
package Group1
{
   public class Person1
   {
      public var age = 5;
      public function GetAge()
      {
         return 10;
      }
   }
}


// -----------------------------------------------------------------------
package Group2
{
   internal class Person2
   {
   }
}


// -----------------------------------------------------------------------
package Group3
{
   public class Person3
   {
   }
}


// -----------------------------------------------------------------------
package Group4
{
   internal class Person4
   {
   }
}


// -----------------------------------------------------------------------
package Group5
{
   class Person5
   {
      public var age = 15;
      public function GetAge()
      {
         return 16;
      }
   }
}


// -----------------------------------------------------------------------
package Group6
{
   public class Person6
   {
      public var age = 10;
      public function GetAge()
      {
         return 11;
      }
   }
}


class Person6
{
   public var age = 15;
   public function GetAge()
   {
      return 16;
   }
}


// -----------------------------------------------------------------------

package Group7
{
   public class Person7
   {
      public var age = 1;
      public function GetAge()
      {
         return 2;
      }
   }
}

class Person7
{
   public var age = 5;
   public function GetAge()
   {
      return 6;
   }
}


// -----------------------------------------------------------------------

package Group8_1A
{
   public class Person8_1
   {
      public var age = 10;
      public function GetAge()
      {
         return 11;
      }
   }
}

package Group8_1B
{
   public class Person8_1
   {
      public var age = 15;
      public function GetAge()
      {
         return 16;
      }
   }
}


// -----------------------------------------------------------------------

package Group8_2A
{
   public class Person8_2
   {
      public var age = 10;
      public function GetAge()
      {
         return 11;
      }
   }
}

package Group8_2B
{
   public class Person8_2
   {
      public var age = 15;
      public function GetAge()
      {
         return 16;
      }
   }
}


// -----------------------------------------------------------------------

package Group9A
{
   public class Person9
   {
      public var age = 15;
      public function GetAge()
      {
         return 16;
      }   
   }
}

package Group9B
{
   public class Person9
   {
      public var age = 20;
      public function GetAge()
      {
         return 21;
      }
   }
}


// -----------------------------------------------------------------------
// Package A class member and Package B class member have similar names; qualifier used – B

package Group10A
{
   public class Person10
   {
      public var age = 5;
      public function GetAge()
      {  
         return 6;
      }  
   }
}

package Group10B
{
   public class Person10
   {
      public var age = 10;
      public function GetAge()
      {  
         return 11;
      }  
   }
}


// -----------------------------------------------------------------------
package Group11A
{
   public class Person11
   {
      public var age = 5;
      public function GetAge()
      {
         return 6;
      }
   }
}

class Person11
{
   public var age = 10;
   public function GetAge()
   {
      return 11;
   }
}

package Group11B
{
   public class Person11
   {
      public var age = 15;
      public function GetAge()
      {
         return 16;
      }
   }
}



// -----------------------------------------------------------------------
package Group12A
{
   public class Person12
   {
      public var age = 20;
      public function GetAge()
      {
         return 21;
      }
   }
}

class Person12
{
   public var age = 25;
   public function GetAge()
   {
      return 26;
   }
}

package Group12B
{
   public class Person12
   {
      public var age = 30;
      public function GetAge()
      {
         return 31;
      }
   }
}


// -----------------------------------------------------------------------
package Group13A
{
   public class Person13
   {
      public var age = 30;
      public function GetAge()
      {
         return 31;
      }   
   }
}

class Person13
{
   public var age = 35;
   public function GetAge()
   {
      return 36;
   }   
}

package Group13B
{
   public class Person13
   {
      public var age = 40;
      public function GetAge()
      {
         return 41;
      }   
   }
}


// -----------------------------------------------------------------------

import Group1;
import Group2;
import Group5;
import Group6;
import Group7;

import Group8_1A;
import Group8_1B;

import Group8_2B;    // This is intentional.  
import Group8_2A;    // Do not change the order.

import Group9A;
import Group9B;
import Group10A;
import Group10B;
import Group11A;
import Group11B;
import Group12A;
import Group13B;

   

// -----------------------------------------------------------------------
function packages()
{
   apInitTest ("Packages");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Import the package; public class");
   // OK

   var person1: Person1 = new Person1();
   
   expected = 5;
   actual = 0;
   actual = person1.age;
   if (actual != expected)
      apLogFailInfo ("Public class inside a package is not accessible (1)", expected, actual, "");
   
   expected = 10;
   actual = 0;
   actual = person1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Public class inside a package is not accessible (2)", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("2. Import the package; internal class");
   // Error
   
   exceptionThrown = false
   actualError = "";
   if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'Person2' is not accessible from this scope";
   @if (@_fast)
     else expectedError = "ReferenceError: Variable 'Person2' has not been declared";
   @else
     else expectedError = "ReferenceError: Expression must be a compile time constant";
   @end

   try
   {
      eval ("var person2: Person2;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("Error not thrown", "An error should be thrown", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Did not import the package; public class");
   // Error
   
   exceptionThrown = false;
   actualError = "";
   
   expectedError = "";
   @cc_on 
      @if (@_fast)  
            expectedError = "ReferenceError: Variable 'Person3' has not been declared";
        @else
            expectedError = "ReferenceError: Expression must be a compile time constant";
       
   @end       
   
   try
   {
      eval ("var person3: Person3;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("Error not thrown", "An error should be thrown", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.  Did not import the package; internal class");
   // Error
   
   exceptionThrown = false;
   actualError = "";
   
   expectedError = "";
   @cc_on 
      @if (@_fast)  
            expectedError = "ReferenceError: Variable 'Person4' has not been declared";
        @else
            expectedError = "ReferenceError: Expression must be a compile time constant";
       
   @end       
   
   try
   {
      eval ("var person4: Person4;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }   
   
   if (exceptionThrown == false)
      apLogFailInfo ("Error not thrown", "An error should be thrown", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.  Public is the default modifier of package members?");
   // Yes
   
   var person5: Person5 = new Person5();
   
   expected = 15;
   actual = 0;
   actual = person5.age;
   if (actual != expected)
      apLogFailInfo ("Public is not the default modifier of package members (1)", expected, actual, "");
      
   expected = 16;
   actual = 0;
   actual = person5.GetAge();
   if (actual != expected) 
      apLogFailInfo ("Public is not the default modifier of package members (2)", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("6.  Package class member and local class have similar names; qualifier not used");
   // Local class takes precedence.  Local class will be used.
   
   var person6: Person6 = new Person6();
   
   expected = 15;
   actual = 0;
   actual = person6.age;
   if (actual != expected) 
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "");
   
   expected = 16;
   actual = 0;
   actual = person6.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("7.  Package class member and local class have similar names; qualifier used");
   // Package class member will be used.
   
   var person7: Group7.Person7 = new Group7.Person7();
   
   expected = 1;
   actual = 0;
   actual = person7.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "152296");
   
   expected = 2;
   actual = 0;
   actual = person7.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "152296");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("8.1  Package A class member and Package B class member have similar names; qualifier not used; Group8_1A is imported first");
   // The class of Group8_1A will be used
   // JScript will give a warning

   var person8_1: Person8_1 = new Person8_1();
   
   actual = 0;
   expected = 10;
   actual = person8_1.age;
   if (actual != expected)
      apLogFailInfo ("Error when 2 packages have the same class name within", expected, actual, "87896");

   actual = 0;
   expected = 11;
   actual = person8_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Error when 2 packages have the same class name within", expected, actual, "87896");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.2  Package A class member and Package B class member have similar names; qualifier not used; Group8_2B is imported first");
   // The class of Group8_2B will be used
   // JScript will give a warning

   var person8_2: Person8_2 = new Person8_2();
   
   actual = 0;
   expected = 15;
   actual = person8_2.age;
   if (actual != expected)
      apLogFailInfo ("Error when 2 packages have the same class name within", expected, actual, "87896");

   actual = 0;
   expected = 16;
   actual = person8_2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Error when 2 packages have the same class name within", expected, actual, "87896");
       
           
   
   // -----------------------------------------------------------------------
   apInitScenario ("9.  Package A class member and Package B class member have similar names; qualifier used – A");
   // Class in Package A will be used.
   
   var person9: Group9A.Person9 = new Group9A.Person9();
   
   expected = 15;
   actual = 0;
   actual = person9.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "152296");
      
   expected = 16;
   actual = 0;
   actual = person9.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "152296");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("10.  Package A class member and Package B class member have similar names; qualifier used – B");
   // Class in Package B will be used.
   
   var person10: Group10B.Person10 = new Group10B.Person10();
   
   expected = 10;
   actual = 0;
   actual = person10.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "152296");
   
   expected = 11;
   actual = 0;
   actual = person10.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "152296");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11.  Package A class member, Package B class member, and local class have similar names; qualifier not used");
   // Local class takes precedence.  Local class will be used.
   var person11: Person11 = new Person11();
   
   expected = 10;
   actual = 0;
   actual = person11.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "");
      
   expected = 11;
   actual = 0;
   actual = person11.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("12.  Package A class member, Package B class member, and local class have similar names; qualifier used – A");
   // Class in Package A will be used.
   var person12: Group12A.Person12 = new Group12A.Person12();
   
   expected = 20;
   actual = 0;
   actual = person12.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "");
   
   expected = 21;
   actual = 0;
   actual = person12.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("13.  Package A class member, Package B class member, and local class have similar names; qualifier used – B");
   // Class in Package B will be used.
   var person13: Group13B.Person13 = new Group13B.Person13();
   
   expected = 40;
   actual = 0;
   actual = person13.age;
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (1)", expected, actual, "");
       
   expected = 41;
   actual = 0;
   actual = person13.GetAge();
   if (actual != expected)
      apLogFailInfo ("Wrong class instantiated (2)", expected, actual, "");
    

   apEndTest();
}



packages();


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
