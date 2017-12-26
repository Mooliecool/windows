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


var iTestID = 215881;



var actual = "";
var expected = "";
var expectedError: String = "";
var actualError: String = "";
var errorThrown = false;


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



//--------------------------------------------------------------
class Alpha1
{
   public function Bar (b: Boolean)
   {  return 1;   }
   
   public function Bar (s: String)
   {  return 3;   }

   public function Bar (i: int)
   {  return 4;   }
   
   public function Bar (l: long)
   {  return 5;   }
   
   public function Bar (f: float)
   {  return 6;   }
   
   public function Bar (d: double)
   {  return 7;   }
}


//--------------------------------------------------------------
var expectedError2, actualError2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha2                                 " +
         "  {                                            " +
         "     public function Bar (x: int): String      " +
         "     {                                         " +
         "        return \"one\";                        " +
         "     }                                         " +
         "     public function Bar (x: int): String      " +
         "     {                                         " +
         "        return \"two\";                        " +
         "     }                                         " +
         "  }                                            ";


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

actualError2 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError2 = "test.js(1,100) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "" +
                 "test.js(1,286) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "";
@else
expectedError2 = "test.js(1,100) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "" +
                 "test.js(1,286) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "";
@end

//--------------------------------------------------------------
class Alpha3
{
   public function Bar (x: int, y: String, z: boolean)
   {
      return "One";
   }
   
   public function Bar (x: String, y: boolean, z: int)
   {
      return "Two";
   }
}   


//--------------------------------------------------------------
class Alpha4
{
   public function Bar (x: int, y: String, z: boolean)
   {
      return "Three";
   }
}

class Beta4 extends Alpha4
{   
   public function Bar (x: String, y: boolean, z: int)
   {
      return "Four";
   }
}  


//--------------------------------------------------------------
class Alpha5
{
   public function Bar (x: int, y: String, z: boolean)
   {
      return "Five";
   }
}

class Beta5 extends Alpha5
{   
}     

class Charlie5 extends Beta5
{
   public function Bar (x: String, y: boolean, z: int)
   {
      return "Six";
   }
}


//--------------------------------------------------------------
class Alpha6
{
   public function Bar (x: int)
   {
      return "One";
   }
   
   public function Bar (x: int, y: String)
   {
      return "Two";
   }
   
   public function Bar (x: int, y: String, z: boolean)
   {
      return "Three";
   }
}


//--------------------------------------------------------------
class Alpha7
{
   public function Bar (x: int)
   {
      return "Four";
   }

   public function Bar (x: int, y: String, z: boolean)
   {
      return "Six";
   }
}

class Beta7 extends Alpha7
{
   public function Bar (x: int, y: String)
   {
      return "Five";
   }
}


//--------------------------------------------------------------
class Alpha8
{
   public function Bar (x: int)
   {
      return "Seven";
   }
}

class Beta8 extends Alpha8
{
   public function Bar (x: int, y: String)
   {
      return "Eight";
   }
}

class Charlie8 extends Beta8
{
   public function Bar (x: int, y: String, z: boolean)
   {
      return "Nine";
   }
}


//--------------------------------------------------------------
var exceptionThrown9 = false;
var actualError9 = "";
var expectedError9 = "ReferenceError: More than one method or property matches this argument list"

try
{
   eval ("  class Alpha9                                 " +
         "  {                                            " +
         "     public function Bar (x: String, y: char)  " +
         "     {  }                                      " +
         "     public function Bar (x: char, y: String)  " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha9: Alpha9 = new Alpha9();           " +
         "  alpha9.Bar (\"hello\", 'a');                   ", "unsafe");
}
catch (error)
{
   exceptionThrown9 = true;
   actualError9 = error;
}


//--------------------------------------------------------------
var exceptionThrown10 = false;
var actualError10 = "";
var expectedError10 = "ReferenceError: More than one method or property matches this argument list"

try
{
   eval ("  class Alpha10                                 " +
         "  {                                            " +
         "     public function Bar (x: String, y: char)  " +
         "     {  }                                      " +
         "     public function Bar (x: char, y: String)  " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha10: Alpha10 = new Alpha10();           " +
         "  alpha10.Bar ('a', \"hello\");                   ", "unsafe");
}
catch (error)
{
   exceptionThrown10 = true;
   actualError10 = error;
}


//--------------------------------------------------------------
var exceptionThrown11 = false;
var actualError11 = "";
var expectedError11 = "ReferenceError: More than one method or property matches this argument list"

try
{
   eval ("  class Alpha11                                " +
         "  {                                            " +
         "     public function Bar (x: String, y: char)  " +
         "     {  }                                      " +
         "     public function Bar (x: char, y: String)  " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha11: Alpha11 = new Alpha11();        " +
         "  alpha11.Bar ('a', 'b');                      ", "unsafe");
}
catch (error)
{
   exceptionThrown11 = true;
   actualError11 = error;
}


//--------------------------------------------------------------
var exceptionThrown12 = false;
var actualError12 = "";
var expectedError12 = "ReferenceError: More than one method or property matches this argument list"

try
{
   eval ("  class Alpha12                                " +
         "  {                                            " +
         "     public function Bar (x: String, y: char)  " +
         "     {  }                                      " +
         "     public function Bar (x: char, y: String)  " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha12: Alpha12 = new Alpha12();        " +
         "  alpha12.Bar (\"hello\", \"world\");          ", "unsafe");
}
catch (error)
{
   exceptionThrown12 = true;
   actualError12 = error;
}


//--------------------------------------------------------------
class Alpha13
{
   class Beta13
   {
      public function Bar (x: int, y: String): boolean
      {
         return false;
      }
      
      public function Bar (x: String): String
      {
         return x;
      }
   }
}


//--------------------------------------------------------------
class Alpha14
{
   static class Beta14
   {
      public function Bar (x: int, y: String): boolean
      {
         return false;
      }
      
      public function Bar (x: String): String
      {
         return x;
      }
   }
}


//--------------------------------------------------------------
class Outer15
{
   class Alpha15
   {
      public function Bar (x: boolean): int
      {
         return 20;
      }
   }
   
   class Beta15 extends Alpha15
   {
      public function Bar (x: int): String
      {
         return "hello";
      }
   }
}


//--------------------------------------------------------------
class Outer16
{
   static class Alpha16
   {
      public function Bar (x: boolean): int
      {
         return 20;
      }
   }
   
   static class Beta16 extends Alpha16
   {
      public function Bar (x: int): String
      {
         return "hello";
      }
   }
}

//--------------------------------------------------------------
function methodoverloading()
{
   apInitTest ("MethodOverloading");
   
   //--------------------------------------------------------------
   apInitScenario ("1. Single parameters, different types");
   
   var alpha1: Alpha1 = new Alpha1();
   
   expected = 1
   actual = 0;
   actual = alpha1.Bar (true);
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with Boolean parameter", expected, actual, "");
      
   expected = 4;
   actual = 0;
   actual = alpha1.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with Number parameter", expected, actual, "");
      
   expected = 3;
   actual = 0;
   actual = alpha1.Bar ("hello");
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with String parameter", expected, actual, "");
   
   expected = 4;
   actual = 0;
   actual = alpha1.Bar (1);
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with integer parameter", expected, actual, "");
      
   expected = 4;
   actual = 0;
   actual = alpha1.Bar (60);
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with long parameter", expected, actual, "");      
      
   expected = 7;
   actual = 0;
   actual = alpha1.Bar (3.2);
   if (actual != expected)
      apLogFailInfo ("Did not call the Bar method with double parameter", expected, actual, "");
      
      
   //--------------------------------------------------------------
   apInitScenario ("2. Alpha has 2 methods with the same signature");
   
   if (actualError2 != expectedError2)
      apLogFailInfo ("Error in 2", expectedError2, actualError2, "233627");      
      
      
   //--------------------------------------------------------------
   apInitScenario ("3. Same types in a different order: Alpha.Bar (int, String, boolean) & Alpha.Bar (String, boolean, int)");
      
   var alpha3: Alpha3 = new Alpha3();
   
   expected = "One";
   actual = "";
   actual = alpha3.Bar (3, "hello", false)
   if (actual != expected)
      apLogFailInfo ("Error in 3(1)", expected, actual, "");
   
   expected = "Two";
   actual = "";
   actual = alpha3.Bar ("hello", false, 3)
   if (actual != expected)
      apLogFailInfo ("Error in 3(2)", expected, actual, "");
      
      
   //--------------------------------------------------------------
   apInitScenario ("4. Same types in a different order: Alpha.Bar (int, String, boolean) & Beta.Bar (String, boolean, int)");
      
   var beta4: Beta4 = new Beta4();
   
   expected = "Three";
   actual = "";
   actual = beta4.Bar (3, "hello", false)
   if (actual != expected)
      apLogFailInfo ("Error in 4(1)", expected, actual, "");
   
   expected = "Four";
   actual = "";
   actual = beta4.Bar ("hello", false, 3)
   if (actual != expected)
      apLogFailInfo ("Error in 4(2)", expected, actual, "");   
      
      
   //--------------------------------------------------------------
   apInitScenario ("5. Same types in a different order: Alpha.Bar (int, String, boolean) & Charlie.Bar (String, boolean, int)");
      
   var charlie5: Charlie5 = new Charlie5();
   
   expected = "Five";
   actual = "";
   actual = charlie5.Bar (3, "hello", false)
   if (actual != expected)
      apLogFailInfo ("Error in 5(1)", expected, actual, "");
   
   expected = "Six";
   actual = "";
   actual = charlie5.Bar ("hello", false, 3)
   if (actual != expected)
      apLogFailInfo ("Error in 5(2)", expected, actual, "");   
      

   //--------------------------------------------------------------
   apInitScenario ("6. Different number of arguments: Alpha.Bar (int); Alpha.Bar (int, String); Alpha.Bar (int, String, boolean)");
                      
   var alpha6: Alpha6 = new Alpha6();
   
   expected = "One";
   actual = "";
   actual = alpha6.Bar (5);
   if (actual != expected)
      apLogFailInfo ("Error in 6(1)", expected, actual, "");   

   expected = "Two";
   actual = "";
   actual = alpha6.Bar (5, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 6(2)", expected, actual, "");   

   expected = "Three";
   actual = "";
   actual = alpha6.Bar (5, "hello", true);
   if (actual != expected)
      apLogFailInfo ("Error in 6(3)", expected, actual, "");   
      
      
   //--------------------------------------------------------------
   apInitScenario ("7. Different number of arguments: Alpha.Bar (int); Beta.Bar (int, String); Alpha.Bar (int, String, boolean)");
   
   var beta7: Beta7 = new Beta7();
   
   expected = "Four";
   actual = "";
   actual = beta7.Bar (5);
   if (actual != expected)
      apLogFailInfo ("Error in 7(1)", expected, actual, "");   
      
   expected = "Six";
   actual = "";
   actual = beta7.Bar (5, "hello", true);
   if (actual != expected)
      apLogFailInfo ("Error in 7(2)", expected, actual, "");   

   expected = "Five";
   actual = "";
   actual = beta7.Bar (5, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 7(3)", expected, actual, "");   
      
      
   //--------------------------------------------------------------
   apInitScenario ("8. Different number of arguments: Alpha.Bar (int); Beta.Bar (int, String); Charlie.Bar (int, String, boolean)");
      
   var charlie8: Charlie8 = new Charlie8();
   
   expected = "Seven";
   actual = "";
   actual = charlie8.Bar (5);
   if (actual != expected)
      apLogFailInfo ("Error in 8(1)", expected, actual, "");   
      
   expected = "Nine";
   actual = "";
   actual = charlie8.Bar (5, "hello", true);
   if (actual != expected)
      apLogFailInfo ("Error in 8(2)", expected, actual, "");   

   expected = "Eight";
   actual = "";
   actual = charlie8.Bar (5, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 8(3)", expected, actual, "");    
      
      
   //--------------------------------------------------------------
   apInitScenario ("9. Method Bar (String, char); method Bar (char, String); call Bar (\"hello\", 'a');");
   
   if (exceptionThrown9 == false)
      apLogFailInfo ("No compile error in 9.", "Should give a compile error", exceptionThrown9, "");
   if (actualError9 != expectedError9)
      apLogFailInfo ("Wrong compile error in 9.", expectedError9, actualError9, "");
              
      
   //--------------------------------------------------------------
   apInitScenario ("10. Method Bar (String, char); method Bar (char, String); call Bar ('a', \"hello\");");
   
   if (exceptionThrown10 == false)
      apLogFailInfo ("No compile error in 10.", "Should give a compile error", exceptionThrown10, "");
   if (actualError10 != expectedError10)
      apLogFailInfo ("Wrong compile error in 10.", expectedError10, actualError10, "");
      
                 
   //--------------------------------------------------------------
   apInitScenario ("11. Method Bar (String, char); method Bar (char, String); call Bar ('a', 'b');");
   
   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11.", "Should give a compile error", exceptionThrown11, "");
   if (actualError11 != expectedError11)
      apLogFailInfo ("Wrong compile error in 11.", expectedError11, actualError11, "");
      
      
   //--------------------------------------------------------------
   apInitScenario ("12. Method Bar (String, char); method Bar (char, String); call Bar (\"hello\", \"world\");");
   
   if (exceptionThrown12 == false)
      apLogFailInfo ("No compile error in 12.", "Should give a compile error", exceptionThrown12, "");
   if (actualError12 != expectedError12)
      apLogFailInfo ("Wrong compile error in 12.", expectedError12, actualError12, "");
      
      
   //--------------------------------------------------------------
   apInitScenario ("13. Method overloading inside a nested class");
   
   var alpha13: Alpha13 = new Alpha13();
   var beta13: Alpha13.Beta13 = new alpha13.Beta13();
   
   expected = false;
   actual = true;
   actual = beta13.Bar (5, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 13(1)", expected, actual, "");
      
   expected = "hello";
   actual = "";
   actual = beta13.Bar ("hello");
   if (actual != expected)
      apLogFailInfo ("Error in 13(2)", expected, actual, "");
      
      
   //--------------------------------------------------------------
   apInitScenario ("14. Method overloading inside a static nested class");
   
   var beta14: Alpha14.Beta14 = new Alpha14.Beta14();
   
   expected = false;
   actual = true;
   actual = beta14.Bar (5, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 14(1)", expected, actual, "");
      
   expected = "hello";
   actual = "";
   actual = beta14.Bar ("hello");
   if (actual != expected)
      apLogFailInfo ("Error in 14(2)", expected, actual, "");  
      
      
   //--------------------------------------------------------------
   apInitScenario ("15. Method overloading with a subclass inside a nested class");
   
   var outer15: Outer15 = new Outer15();
   var alpha15: Outer15.Alpha15; 
   var beta15: Outer15.Beta15;
   
   // ---------------------------
   alpha15 = new outer15.Alpha15();
   
   expected = 20;
   actual = 0;
   actual = alpha15.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 15(1)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha15.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 15(2)", expected, actual, "308653");

   // ---------------------------
   alpha15 = new outer15.Beta15();
   
   expected = 20;
   actual = 0;
   actual = alpha15.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 15(3)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha15.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 15(4)", expected, actual, "308653");   
   
   // ---------------------------
   beta15 = new outer15.Beta15();
   
   expected = 20;
   actual = 0;
   actual = alpha15.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 15(5)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha15.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 15(6)", expected, actual, "308653");   
      
      
   //--------------------------------------------------------------
   apInitScenario ("16. Method overloading with a subclass inside a static nested class");
      
   var alpha16: Outer16.Alpha16;
   var beta16: Outer16.Beta16;
   
   // ---------------------------
   alpha16 = new Outer16.Alpha16();
   
   expected = 20;
   actual = 0;
   actual = alpha16.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 16(1)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha16.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 16(2)", expected, actual, "308653");

   // ---------------------------
   alpha16 = new Outer16.Beta16();
   
   expected = 20;
   actual = 0;
   actual = alpha16.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 16(3)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha16.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 16(4)", expected, actual, "308653");   
   
   // ---------------------------
   beta16 = new Outer16.Beta16();
   
   expected = 20;
   actual = 0;
   actual = alpha16.Bar (false);
   if (actual != expected)
      apLogFailInfo ("Error in 16(5)", expected, actual, "308653");
   
   expected = 20;   
   actual = 0;
   actual = alpha16.Bar (2);
   if (actual != expected)
      apLogFailInfo ("Error in 16(6)", expected, actual, "308653");   


   apEndTest();   
}


methodoverloading();


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
