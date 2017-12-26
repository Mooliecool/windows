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


var iTestID = 82048;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var expectedError = "";
var actualError = "";
var exceptionThrown = false;
var version = Number(@_jscript_version).ToString("0.0")


@if(!@aspx)
	import System
	import System.IO
	import System.Diagnostics
@else
	</script>
	<%@ import namespace="System.IO" %>
	<%@ import namespace="System.Diagnostics" %>
	<script language=jscript runat=server>
@end

var JSCode: String;
var fileStream: FileStream;
var streamWriter: StreamWriter;
var process: Process;
var processStartInfo: ProcessStartInfo;

// -----------------------------------------------------------------------
class Person1
{
   private var age = 1;
         
   public function SetAge (newAge)
   {
      age = newAge;
   }
            
   public function GetAge()
   {
      return age;
   }      
}

// -----------------------------------------------------------------------
class Person2
{
   private var age = 23;
}

// -----------------------------------------------------------------------
var expectedError3_1, actualError3_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha                      " +
         "  {                                " +
         "     private var value = 24;       " +
         "  }                                " +
         "  class Beta extends Alpha         " +
         "  {                                " +
         "     public function Bar()         " +
         "     {                             " +
         "        var x = 0;                 " +
         "        x = value;                 " +
         "     }                             " +
         "  }                                ";


@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError3_1 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError3_1 = "test.js(1,328) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError3_1 = "test.js(1,328) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@end
// -----------------------------------------------------------------------
var expectedError3_2, actualError3_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  package Group                    " +
         "  {                                " +
         "     class Alpha                   " +
         "     {                             " +
         "        private var value = 23;    " +
         "     }                             " +
         "     class Beta extends Alpha      " +
         "     {                             " +
         "        public function Bar()      " +
         "        {                          " +
         "           var x = 0;              " +
         "           x = value;              " +
         "        }                          " +
         "     }                             " +
         "  }                                ";

@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError3_2 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError3_2 = "test.js(1,401) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError3_2 = "test.js(1,401) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError3_3, actualError3_3: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  package GroupA                         " +
         "  {                                      " +
         "     public class Alpha                  " +
         "     {                                   " +
         "        private var value = 13;          " +
         "     }                                   " +
         "  }                                      " +
         "  import GroupA;                         " +
         "  package GroupB                         " +
         "  {                                      " +
         "     public class Beta extends Alpha     " +
         "     {                                   " +
         "        public function Bar()            " +
         "        {                                " +
         "           var x = 0;                    " +
         "           x = value;                    " +
         "        }                                " +
         "     }                                   " +
         "  }                                      ";


@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError3_3 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError3_3 = "test.js(1,631) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError3_3 = "test.js(1,631) : error JS1123: 'value' is not accessible from this scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
package Group4
{
   class Person4
   {
      private var age = 23;
   }
         
   public class Student4
   {
      public function AccessAge()
      {
         var person4: Person4 = new Person4();
         
         actual = 0;
         exceptionThrown = false;
         actualError = "";
         if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'person4.age' is not accessible from this scope";
         else expectedError = "ReferenceError: Objects of type 'Group4.Person4' do not have such a member";
        
         
         try
         {     
            eval ("actual = person4.age;");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }

         if (exceptionThrown == false)
            apLogFailInfo ("Compile error not thrown", "Should give a compile error", exceptionThrown, "164934");
         if (actualError != expectedError)
            apLogFailInfo ("Wrong error message", expectedError, actualError, "164934");
         if (actual == 23)
            apLogFailInfo ("Private fields accessible inside a package", "shouldn't be accessible", actual, "61982");
      }   
   }
}

// -----------------------------------------------------------------------
package Group5
{
   public class Person5
   {
      private var age = 23;
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
      private var age = 23;
   }
}

import Group6;    

package Group6.InnerGroup6
{
   public class Student6
   {
      public function AccessAge()
      {
         var exceptionThrown = false;
         var expectedError = "blah";
         var actualError = "";
         var actual = 0;
         var person6: Person6;
               
         person6 = new Person6();

         try
         {
            eval ("actual = person6.age;");
         }
         catch (error)
         {
            exceptionThrown = true;
            actualError = error;
         }
               
         if (actual == 23)
            apLogFailInfo ("Private fields accessible by a nested package", "should not be accessible", actual, "60951");
         if (exceptionThrown == true)
            apLogFailInfo ("Accessing private fields from a nested package throws an exception", "It should fail silently", exceptionThrown, "70599");
      }

   }
}

// -----------------------------------------------------------------------

PTorr -- Nested classes will be supported in JScript 8

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
         var expectedError = "blah";
            
         student7 = new Student7();
         try
         {
            eval ("actual = student7.age;");
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
      private var age = 17;
   }
}

This generates a compile error starting at JS 7.0.8905

// -----------------------------------------------------------------------
class Person8_1
{
   private var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student8_1 extends Person8_1
{
   private var age = 16;
}

// -----------------------------------------------------------------------
class Person8_2
{
   private var age = 40;

   public function GetAge()
   {
      return age;
   }
}

class Student8_2 extends Person8_2
{
   private var age = 16;
}

// -----------------------------------------------------------------------
class Person8_3
{
   private var age = 40;
}

class Student8_3 extends Person8_3
{
   private var age = 16;

   public function GetAge()
   {
      return age;
   }
}

// -----------------------------------------------------------------------
class Person8_4
{
   private var age = 40;
}

class Student8_4 extends Person8_4
{
   private var age = 16;
         
   public function GetAge()
   {
      return age;
   }
}
*/

// -----------------------------------------------------------------------
class Alpha9
{
   private var counter: int = 5;
   public function GetCounter()
   {
      return counter;
   }
}

class Beta9 extends Alpha9
{
   private var counter: int = 10;
   public function GetCounter()
   {
      return counter;
   }
} 

// -----------------------------------------------------------------------
var exceptionThrown10_2 = false;
var actualError10_2 = "";
var expectedError10_2 = "ReferenceError: 'counter' is read-only";

try
{
   eval ("  class Alpha10_2                     " +
         "  {                                   " +
         "     private const counter: int = 5;  " +
         "     public function SetCounter()     " +
         "     {                                " +
         "        counter = 10;                 " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown10_2 = true;
   actualError10_2 = error;
}

// -----------------------------------------------------------------------
import Group3_2;
import GroupB3_3;
import Group4;
import Group5;

// -----------------------------------------------------------------------
function accesscontroltoprivatefields1(){

   apInitTest ("AccessControlToPrivateFields1");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Private fields are accessible within the class itself");

   var person1: Person1;

   person1 = new Person1();
   actual = 0;
   expected = 23; 
         
   person1.SetAge(23);
   actual = person1.GetAge();
         
   if (actual != expected)
      apLogFailInfo ("Private fields not accessible within the class itself", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("2. Private fields not accessible from outside a class");

   var person2: Person2 = new Person2();

   actual = 0;
   if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'person2.age' is not accessible from this scope";
   else expectedError = "ReferenceError: Objects of type 'Person2' do not have such a member";
   actualError = "";
   exceptionThrown = false;
   
   try
   {
      eval ("actual = person2.age;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   if (actual == 23)
      apLogFailInfo ("Private fields are accessible outside the class", "shouldn't be accessible", actual, "164934, 61982");
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing private fields does not give a compile error", "Should give a compile error", exceptionThrown, "164934, 61982");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934, 61982");

   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Private fields accessible not from a subclass, Default package.");

   if (actualError3_1 != expectedError3_1)
      apLogFailInfo ("Error in 3.1", expectedError3_1, actualError3_1, "");

   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Private fields not accessible from a subclass, Inside the package.");

   if (actualError3_2 != expectedError3_2)
      apLogFailInfo ("Error in 3.2", expectedError3_2, actualError3_2, "");

   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Private fields not accessible from a subclass, Outside the package.");

   if (actualError3_3 != expectedError3_3)
      apLogFailInfo ("Error in 3.3", expectedError3_3, actualError3_3, "");

   // -----------------------------------------------------------------------
   apInitScenario ("4. Private fields not accessible inside a package?");
   
   var student4: Student4;

   student4 = new Student4();
   student4.AccessAge();

   // -----------------------------------------------------------------------
   apInitScenario ("5. Private fields not accessible outside the package?");

   var person5: Person5 = new Person5();

   actual = 0;
   exceptionThrown = false;
   actualError = "";
   if ((version == "7.0")||(version == "8.0")) expectedError = "ReferenceError: 'person5.age' is not accessible from this scope";
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
   
   if (exceptionThrown == false)
      apLogFailInfo ("Accessing private fields outside the package does not throw any error", "Should throw an error", exceptionThrown, "164934, 150458");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "164934, 150458");
   if (actual == 23)
      apLogFailInfo ("Private fields accessible outside the package", "shouldn't be accessible", actual, "164934, 150458, 61982");

   // -----------------------------------------------------------------------
/*
PTorr:
   The initial implementation of packages in JScript 7 will be limited; only classes will be valid 
   inside a package; functions, code, and nested packages will not be allowed. This restriction 
   may be lifted in future versions. 
   
   // apInitScenario ("6. Private fields accessible by a nested package?");
   // No

   import Group6.InnerGroup6;

   var student6: Student6;

   student6 = new Student6();
   student6.AccessAge();

   // -----------------------------------------------------------------------

   HermanV: Functions will no longer be supported inside packages.

   // apInitScenario ("6.2 Private fields accessible by a nested package?  From a function?");
   // No

   package People6_2
   {
      class Person6_2
      {
         private var age;
      }
         
      public package InnerPeople6_2
      {
         public function AccessAge()
         {
            var person = new Person6_2();
            person.age = 23;
         }
      }
   }

   import People6_2.InnerPeople6_2;
   actualError = "";
   expectedError = "blah";    // no error description yet
   exceptionThrown = false;

   try
   {
      AccessAge();
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }

   if (exceptionThrown == false)
      apLogFailInfo ("Accessing private fields from a function inside a nested package does not throw any errors", exceptionThrown, true, "58310_2");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");                  

   // -----------------------------------------------------------------------

   PTorr -- Nested packages will be supported in JScript 8
   
   // apInitScenario ("7. Private field accessible by an enclosing package?");
   // No

   import Group7;
   
   var person7: Person7;

   person7 = new Person7();
   person7.AccessAge();

   //This generates a compile error starting at JS 7.0.8905
   // -----------------------------------------------------------------------
   // apInitScenario ("8.1 Subclass defines a private field with the same name as superclass'   -- Access method in the superclass.  Superclass is instantiated");

   var person8_1: Person8_1;

   person8_1 = new Person8_1();
   expected = 40;
   actual = 0;

   actual = person8_1.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("8.2 Subclass defines a private field with the same name as superclass'  -- Access method in the superclass.  Subclass is instantiated");


   var student8_2: Student8_2;

   student8_2 = new Student8_2();
   expected = 40;
   actual = student8_2.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("8.3 Subclass defines a private field with the same name as superclass'  -- Access method in the subclass.  Subclass is instantiated");

   var student8_3: Student8_3;

   student8_3 = new Student8_3();
   expected = 16;
   actual = student8_3.GetAge();
   if (actual != expected)
      apLogFailInfo ("Returned the wrong value", expected, actual, "");

   // -----------------------------------------------------------------------
   // apInitScenario ("8.4 Subclass defines a private field with the same name as superclass' \n  -- Access method in the subclass.  Superclass is instantiated");


   var person8_4: Person8_4;

   person8_4 = new Person8_4();
   exceptionThrown = false;
   expectedError = "TypeError: Object doesn't support this property or method";
   actualError = "";
   actual = 0;
         
   try
   {
      eval ("person8_4.GetAge();");
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
   apInitScenario ("9. Subclass defines a field with the same name as its superclass");
   
   var alpha9: Alpha9 = new Alpha9();
   var beta9: Beta9 = new Beta9();
   
   actual = 0;
   expected = 5;
   actual = alpha9.GetCounter();
   if (actual != expected)
      apLogFailInfo ("Error when subclass defines a field with the same name as its superclass (1)", expected, actual, "");

   actual = 0;
   expected = 10;
   actual = beta9.GetCounter();
   if (actual != expected)
      apLogFailInfo ("Error when subclass defines a field with the same name as its superclass (2)", expected, actual, "");
         
   // -----------------------------------------------------------------------
   apInitScenario ("10.1 Constants.  Type annotated; not initialized.");
   // Compile error
   
   exceptionThrown = false;
   expectedError = "SyntaxError: Expected '='";
   actualError = "";
   
   try
   {
      eval ("  class Alpha10_1                  " +
            "  {                                " +
            "     private const counter: int;   " +
            "  }                                ");
   }
   catch (error)
   {  
      exceptionThrown = true;
      actualError = error;      
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("Compile error not thrown in 10.1", "Should throw a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 10.1", expectedError, actualError, "");

   // -----------------------------------------------------------------------
   apInitScenario ("10.2 Constants.  Type annotated; initialized.");
   
   if (exceptionThrown10_2 == false)
      apLogFailInfo ("No compile error in 10.2", "Should give a compile error", exceptionThrown10_2, "");
   if (actualError10_2 != expectedError10_2)
      apLogFailInfo ("Wrong compile error in 10.2", expectedError10_2, actualError10_2, "");

   // -----------------------------------------------------------------------
   apInitScenario ("10.3 Constants.  Not type annotated; not initialized.");
   // Compile error


   exceptionThrown = false;
   expectedError = "SyntaxError: Expected '='";
   actualError = "";
   
   try
   {
      eval ("  class Alpha10_3            " +
            "  {                          " +
            "     private const counter;  " +
            "  }                          ");
   }
   catch (error)
   {  
      exceptionThrown = true;
      actualError = error;      
   }

   if (exceptionThrown == false)
      apLogFailInfo ("Compile error not thrown in 10.3", "Should throw a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 10.3", expectedError, actualError, "");


   apEndTest();
}


accesscontroltoprivatefields1();


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
