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


var iTestID = 163892;



// -----------------------------------------------------------------------

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

var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown1 = false;
var actualError1 = "";
var expectedError1 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  abstract class Alpha1               " +
         "  {  }                                " +
         "  var alpha1: Alpha1 = new Alpha1();  ");
}
catch (error)
{
   exceptionThrown1 = true;
   actualError1 = error;
}         


// -----------------------------------------------------------------------
var exceptionThrown2 = false;
var actualError2 = "";
var expectedError2 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class Alpha2                           " +
         "  {  }                                   " +
         "  abstract class Beta2 extends Alpha2    " +
         "  {  }                                   " +
         "  var beta2: Beta2 = new Beta2();        ");
}
catch (error)
{
   exceptionThrown2 = true;
   actualError2 = error;
}         


// -----------------------------------------------------------------------
var exceptionThrown3 = false;
var actualError3 = "";
var expectedError3 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class Alpha3                           " +
         "  {  }                                   " +
         "  abstract class Beta3 extends Alpha3    " +
         "  {  }                                   " +
         "  abstract class Charlie3 extends Beta3  " +
         "  {  }                                   " +
         "  var charlie3: Charlie3 = new Charlie3();  ");
}
catch (error)
{
   exceptionThrown3 = true;
   actualError3 = error;
}         



// -----------------------------------------------------------------------
abstract class Alpha4
{
   public var value: String = "none";
   abstract public function Bar();
}

class Beta4 extends Alpha4
{
   public function Bar()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown5 = false;
var actualError5 = "";
var expectedError5 = "Error: 'Bar' cannot be abstract unless the declaring class is marked as abstract";

try
{
   eval ("  class Alpha5                        " +
         "  {                                   " +
         "     abstract public function Bar();  " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown5 = true;
   actualError5 = error;
}  


// -----------------------------------------------------------------------
var expectedError6, actualError6: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  abstract class Alpha6                     " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     public function Bar()                  " +
         "     {                                      " +
         "        value = \"three\";                  " +
         "     }                                      " +
         "  }                                         " +
         "  class Beta6 extends Alpha6 { }            " +
         "  var alpha6: Alpha6 = new Alpha6();        ";


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

actualError6 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError6 = "test.js(1,420) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine;
@else
expectedError6 = "test.js(1,420) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine;
@end
// -----------------------------------------------------------------------
abstract public class Alpha7
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "four";   
   }
}     

class Beta7 extends Alpha7
{
}


// -----------------------------------------------------------------------
var exceptionThrown8 = false;
var actualError8 = "";
var expectedError8 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  abstract private class Alpha8    " +
         "  {                                " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown8 = true;
   actualError8 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown9 = false;
var actualError9 = "";
var expectedError9 = "SyntaxError: Only valid inside a class definition";

try
{  
   eval ("  abstract protected class Alpha9  " +
         "  {                                " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown9 = true;
   actualError9 = error;
}


// -----------------------------------------------------------------------
abstract internal class Alpha10
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "five";   
   }
}     

class Beta10 extends Alpha10
{
}


// -----------------------------------------------------------------------
var exceptionThrown11 = false;
var actualError11 = "";
var expectedError11 = "Error: final and abstract cannot be used together";

try
{
   eval ("  abstract final class Alpha11  " +
         "  {                             " +
         "  }                             ");
}
catch (error)
{
   exceptionThrown11 = true;
   actualError11 = error;
}


// -----------------------------------------------------------------------
abstract expando class Alpha12
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "six";   
   }
}     

class Beta12 extends Alpha12
{
}


// -----------------------------------------------------------------------
var exceptionThrown13 = false;
var actualError13 = "";
var expectedError13 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  abstract static class Alpha13 " +
         "  {                             " +
         "  }                             ");
}
catch (error)
{
   exceptionThrown13 = true;
   actualError13 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown14 = false;
var actualError14 = "";
var expectedError14 = "SyntaxError: Abstract function cannot have body";

try
{  
   eval ("  abstract class Alpha14              " +
         "  {                                   " +
         "     abstract public function Bar()   " +
         "     {  }                             " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown14 = true;
   actualError14 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown15 = false;
var actualError15 = "";
var expectedError15 = "Error: Not valid for a constructor";

try
{
   eval ("  abstract class Alpha15                 " +
         "  {                                      " +
         "     public abstract function Alpha15(); " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown15 = true;
   actualError15 = error;
}      


// -----------------------------------------------------------------------
abstract class Alpha16
{
   public var value: String = "none";
   
   abstract public function Bar();
}

class Beta16 extends Alpha16
{
   public function Bar()
   {
      value = "one";   
   }
}
   
         
// -----------------------------------------------------------------------
var exceptionThrown17 = false;
var actualError17 = "";
var expectedError17 = "Error: No implementation provided for 'Alpha17.Bar()'";

try
{
   eval ("  abstract class Alpha17                 " +
         "  {                                      " +
         "     abstract public function Bar();     " +
         "  }                                      " +
         "  class Beta17 extends Alpha17           " +
         "  {  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown17 = true;
   actualError17 = error;
}     


// -----------------------------------------------------------------------
abstract class Alpha18
{
   static public var value: String = "none";
   abstract public function Bar();
}

abstract class Beta18 extends Alpha18
{
   abstract public function Bar();
}  


// -----------------------------------------------------------------------
abstract class Alpha19
{  
   public var value: String = "none";
   abstract public function Bar();
}

class Beta19 extends Alpha19
{
   public function Bar()
   {
      value = "one";
   }
}

class Charlie19 extends Beta19
{
   public function Bar()
   {
      value = "two";
   }
} 


// -----------------------------------------------------------------------
abstract class Alpha20
{  
   public var value: String = "none";
   abstract public function Bar();
}

abstract class Beta20 extends Alpha20
{
   abstract public function Bar();
}

class Charlie20 extends Beta20
{
   public function Bar()
   {
      value = "three";
   }
} 


// -----------------------------------------------------------------------
var exceptionThrown21 = false;
var actualError21 = "";
var expectedError21 = "Error: No implementation provided for 'Beta21.Bar()'";

try
{
   eval ("  abstract class Alpha21                    " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  abstract class Beta21 extends Alpha21     " +
         "  {                                         " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  class Charlie21 extends Beta21            " +
         "  {  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown21 = true;
   actualError21 = error;
}         


// -----------------------------------------------------------------------
abstract class Alpha22
{
   static public var value: String = "none";
   abstract public function Bar();
}

abstract class Beta22 extends Alpha22
{
   abstract public function Bar();
}  

abstract class Charlie22 extends Beta22
{
   abstract public function Bar();
}


// -----------------------------------------------------------------------
var exceptionThrown23 = false;
var actualError23 = "";
var expectedError23 = "Error: No implementation provided for 'Beta23.Bar()'";


abstract class Alpha23
{
   public var value: String = "none";
   abstract public function Bar();
}

class Beta23 extends Alpha23
{
   public function Bar()
   {
      value = "four";
   }
}

abstract class Charlie23 extends Beta23
{
   abstract public function Bar();
}


// -----------------------------------------------------------------------
var exceptionThrown24 = false;
var actualError24 = "";
var expectedError24 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class Alpha24                             " +
         "  {                                         " +
         "     public function Bar()                  " +
         "     {  }                                   " +
         "  }                                         " +
         "  abstract class Beta24 extends Alpha24     " +
         "  {                                         " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  var beta24: Beta24 = new Beta24();        ", "unsafe");
}
catch (error)
{
   exceptionThrown24 = true;
   actualError24 = error;
}         



// -----------------------------------------------------------------------
class Alpha25
{
   public var value: String = "none";
   
   public function Bar()
   { 
      value = "one";
   }
}

abstract class Beta25 extends Alpha25
{
   abstract public function Bar();
}

class Charlie25 extends Beta25
{
   public function Bar()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown26 = false;
var actualError26 = "";
var expectedError26 = "Error: No implementation provided for 'Beta26.Bar()'";

try
{
   eval ("  class Alpha26                             " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     public function Bar()                  " +
         "     {                                      " +
         "        value = \"one\";                    " +
         "     }                                      " +
         "  }                                         " +
         "  abstract class Beta26 extends Alpha26     " +
         "  {                                         " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  class Charlie26 extends Beta26            " +
         "  {  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown26 = true;
   actualError26 = error;
}         


// -----------------------------------------------------------------------
var exceptionThrown27 = false;
var actualError27 = "";
var expectedError27 = "TypeError: Invalid procedure call or argument";

try
{
   eval ("  abstract class Alpha27                 " +
         "  {                                      " +
         "     abstract public function Bar();     " +
         "  }                                      " +
         "  class Beta27 extends Alpha27           " +
         "  {                                      " +
         "     public function Bar()               " +
         "     {                                   " +
         "        super.Bar();                     " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown27 = true;
   actualError27 = error;
}


abstract class Alpha28
{
   public var value: String = "none";
   
   abstract public function Bar();
}

class Beta28 extends Alpha28
{
   public function Bar()
   {
      value = "ten";
   }
}

class Charlie28 extends Beta28
{
   public function Bar()
   {
      value = "eleven";
      super.Bar();
   }
}


// -----------------------------------------------------------------------
abstract class Alpha29
{
   public var value: String = "";
   abstract public function Bar();
}

class Beta29 extends Alpha29
{
   public function Bar()
   {
      value = "one";
   }
}

abstract class Charlie29 extends Beta29
{
   abstract public function Bar();
}

class Delta29 extends Charlie29
{
   public function Bar()
   {
      value = "two";
   }
}






// -----------------------------------------------------------------------
function abstract01()
{
   apInitTest ("Abstract01");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Instantiate an abstract class");
   // Compile error
   
   if (exceptionThrown1 == false)
      apLogFailInfo ("No compile error in 1.", "Should give a compile error", exceptionThrown1, "");
   if (actualError1 != expectedError1)
      apLogFailInfo ("Wrong compile error in 1.", expectedError1, actualError1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2. Instantiate a subclass that's abstract");
   // Compile error
   
   if (exceptionThrown2 == false)
      apLogFailInfo ("No compile error in 2.", "Should give a compile error", exceptionThrown2, "");
   if (actualError2 != expectedError2)
      apLogFailInfo ("Wrong compile error in 2.", expectedError2, actualError2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Instantiate a sub-subclass that's abstract");
   // Compile error
      
   if (exceptionThrown3 == false)
      apLogFailInfo ("No compile error in 3.", "Should give a compile error", exceptionThrown3, "");
   if (actualError3 != expectedError3)
      apLogFailInfo ("Wrong compile error in 3.", expectedError3, actualError3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4. Abstract class; abstract method");
   
   var beta4: Beta4 = new Beta4();
   
   expected = "one";
   actual = "";
   beta4.Bar();
   actual = beta4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 4.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5. Non-abstract class; abstract method");
      
   if (exceptionThrown5 == false)
      apLogFailInfo ("No compile error in 5.", "Should give a compile error", exceptionThrown5, "");
   if (actualError5 != expectedError5)
      apLogFailInfo ("Wrong compile error in 5.", expectedError5, actualError5, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("6. Abstract class; non-abstract method");
   
   if (actualError6 != expectedError6)
      apLogFailInfo ("Error in 6.", expectedError6, actualError6, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7. Abstract public class");
      
   var beta7: Beta7 = new Beta7();
   
   expected = "four";
   actual = "";
   beta7.Bar();
   actual = beta7.value;
   if (actual != expected)
      apLogFailInfo ("Error in 7.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8. Abstract private class");
      
   if (exceptionThrown8 == false)
      apLogFailInfo ("No compile error in 8.", "Should give a compile error", exceptionThrown8, "212468");
   if (actualError8 != expectedError8)
      apLogFailInfo ("Wrong compile error in 8.", expectedError8, actualError8, "212468");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9. Abstract protected class");

   if (exceptionThrown9 == false)
      apLogFailInfo ("No compile error in 9.", "Should give a compile error", exceptionThrown9, "");
   if (actualError9 != expectedError9)
      apLogFailInfo ("Wrong compile error in 9.", expectedError9, actualError9, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. Abstract internal class");

   var beta10: Beta10 = new Beta10();
   
   expected = "five";
   actual = "";
   beta10.Bar();
   actual = beta10.value;
   if (actual != expected)
      apLogFailInfo ("Error in 10.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. Abstract final class");

   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11.", "Should give a compile error", exceptionThrown11, "");
   if (actualError11 != expectedError11)
      apLogFailInfo ("Wrong compile error in 11.", expectedError11, actualError11, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12. Abstract expando class");
      
   var beta12: Beta12 = new Beta12();
   
   expected = "six";
   actual = "";
   beta12.Bar();
   actual = beta12.value;
   if (actual != expected)
      apLogFailInfo ("Error in 12.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13. Abstract static class");
      
   if (exceptionThrown13 == false)
      apLogFailInfo ("No compile error in 13.", "Should give a compile error", exceptionThrown13, "");
   if (actualError13 != expectedError13)
      apLogFailInfo ("Wrong compile error in 13.", expectedError13, actualError13, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14. Abstract method with body");
      
   if (exceptionThrown14 == false)
      apLogFailInfo ("No compile error in 14.", "Should give a compile error", exceptionThrown14, "");
   if (actualError14 != expectedError14)
      apLogFailInfo ("Wrong compile error in 14.", expectedError14, actualError14, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("15. Abstract constructors");
   
   if (exceptionThrown15 == false)
      apLogFailInfo ("No compile error in 15.", "Should give a compile error", exceptionThrown15, "");
   if (actualError15 != expectedError15)
      apLogFailInfo ("Wrong compile error in 15.", expectedError15, actualError15, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16. Beta implements an abstract method");
      
   var beta16: Beta16 = new Beta16();
   
   expected = "one";
   actual = "";
   beta16.Bar();
   actual = beta16.value;
   if (actual != expected)
      apLogFailInfo ("Error in 16.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17. Beta does not implement an abstract method");
      
   if (exceptionThrown17 == false)
      apLogFailInfo ("No compile error in 17.", "Should give a compile error", exceptionThrown17, "");
   if (actualError17 != expectedError17)
      apLogFailInfo ("Wrong compile error in 17.", expectedError17, actualError17, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18. Beta declares an abstract method as abstract again");
   
   expected = "none";
   actual = "";
   actual = Alpha18.value;
   if (actual != expected)
      apLogFailInfo ("Error in 18.", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("19. Charlie implements an abstract method (Beta implements the abstract method)");
      
   var charlie19: Charlie19 = new Charlie19();
   
   expected = "two";
   actual = "";
   charlie19.Bar();
   actual = charlie19.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20. Charlie implements an abstract method (Beta declares it abstract again)");
      
   var charlie20: Charlie20 = new Charlie20();
   
   expected = "three";
   actual = "";
   charlie20.Bar();
   actual = charlie20.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21. Charlie does not implement an abstract method");
      
   if (exceptionThrown21 == false)
      apLogFailInfo ("No compile error in 21.", "Should give a compile error", exceptionThrown21, "");
   if (actualError21 != expectedError21)
      apLogFailInfo ("Wrong compile error in 21.", expectedError21, actualError21, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22. Charlie declares an abstract method as abstract again");
      
   expected = "none";
   actual = "";
   actual = Alpha22.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23. Alpha declares an abstract method - Beta implements the abstract method - Charlie declares it abstract again");
                       
   var beta23: Beta23 = new Beta23();
   
   expected = "four";
   actual = "";
   beta23.Bar();
   actual = beta23.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.", expected, actual, "");
   

   // -----------------------------------------------------------------------
   apInitScenario ("24. Alpha does not declare Bar() as abstract - Beta declares Bar() as abstract");
   
   if (exceptionThrown24 == false)
      apLogFailInfo ("No compile error in 24.", "Should give a compile error", exceptionThrown24, "");
   if (actualError24 != expectedError24)
      apLogFailInfo ("Wrong compile error in 24.", expectedError24, actualError24, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("25. Alpha does not declare Bar() as abstract - Beta declares Bar() as abstract - Charlie implements Bar()");

   var charlie25: Charlie25 = new Charlie25();

   expected = "two";
   actual = "";
   charlie25.Bar();
   actual = charlie25.value;
   if (actual != expected)
      apLogFailInfo ("Error in 25.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26. Alpha does not declare Bar() as abstract - Beta declares Bar() as abstract - Charlie does not implement Bar()");

   if (exceptionThrown26 == false)
      apLogFailInfo ("No compile error in 26.", "Should give a compile error", exceptionThrown26, "");
   if (actualError26 != expectedError26)
      apLogFailInfo ("Wrong compile error in 26.", expectedError26, actualError26, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27. Alpha declares Bar() as abstract - Beta implements Bar() and calls super.Bar()");
                       
   if (exceptionThrown27 == false)
      apLogFailInfo ("No compile error in 27.", "Should give a compile error", exceptionThrown27, "");
   if (actualError27 != expectedError27)
      apLogFailInfo ("Wrong compile error in 27.", expectedError27, actualError27, "");
                       

   // -----------------------------------------------------------------------
   apInitScenario ("28. Alpha declares Bar() as abstract - Beta implements Bar() - Charlie() implements Bar() and calls super.Bar()");
   
   var charlie28: Charlie28 = new Charlie28();
   
   expected = "ten";
   actual = "";
   charlie28.Bar();
   actual = charlie28.value;
   if (actual != expected)
      apLogFailInfo ("Error in 28.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. Alpha declares Bar() as abstract - Beta implements Bar() - Charlie declares Bar() as abstract - Delta implements Bar()");
                       
   var delta29: Delta29 = new Delta29();
   
   expected = "two";
   actual = "";
   delta29.Bar();
   actual = delta29.value;
   if (actual != expected)
      apLogFailInfo ("Error in 29.", expected, actual, "");
      
      

      
      
      
   apEndTest();
}


abstract01();


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
