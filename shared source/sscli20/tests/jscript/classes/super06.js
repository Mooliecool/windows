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


var iTestID = 194186;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";


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
var exceptionThrown6_3 = false;
var actualError6_3 = "";
var expectedError6_3 = "ReferenceError: 'super' is not accessible from this scope";

try
{
   eval ("  class Alpha6_3                               " +
         "  {                                            " +
         "     protected function GetName(): String      " +
         "     {                                         " +
         "        return \"Alpha6_3\";                   " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta6_3 extends Alpha6_3               " +
         "  {                                            " +
         "     protected function GetName(): String      " +
         "     {                                         " +
         "        return \"Beta6_3\";                    " +
         "     }                                         " +
         "  }                                            " +
         "  class Charlie6_3 extends Beta6_3             " +
         "  {                                            " +
         "     static function Bar()                     " +
         "     {                                         " +
         "        var x = \"\";                          " +
         "        x = super.GetName();                   " +
         "     }                                         " +
         "  }                                            ", "unsafe");
}
catch (error)
{
   exceptionThrown6_3 = true;
   actualError6_3 = error;
}


// -----------------------------------------------------------------------
class Alpha7_1
{
   protected var name: String;
   
   public function Alpha7_1()
   {
      name = "Alpha7_1";
   }
}

class Beta7_1 extends Alpha7_1
{
   public function Bar()
   {
      var localVariable = "";
      
      expected = "Alpha7_1";
      localVariable = super.name;
      if (localVariable != expected)
         apLogFailInfo ("Error in 7.1", expected, localVariable, "");
   }
}


// -----------------------------------------------------------------------
class Alpha7_2
{
   protected var name: String;
   
   public function Alpha7_2()
   {
      name = "Alpha7_2";
   }
}

class Beta7_2 extends Alpha7_2
{
}

class Charlie7_2 extends Beta7_2
{
   public function Bar()
   {
      var localVariable = "";
      
      expected = "Alpha7_2";
      localVariable = super.name;
      if (localVariable != expected)
         apLogFailInfo ("Error in 7.2", expected, localVariable, "");
   }
}


// -----------------------------------------------------------------------
class Alpha8_1
{
   protected var name: String;
   
   public function Alpha8_1()
   {
      name = "Alpha8_1";
   }
}

class Beta8_1 extends Alpha8_1
{
   private var foo: String;
   
   public function Bar()
   {
      foo = super.name;
      expected = "Alpha8_1";
      if (foo != expected)
         apLogFailInfo ("Error in 8.1", expected, foo, "");
   }
}


// -----------------------------------------------------------------------
class Alpha8_2
{
   protected var name: String;
   
   public function Alpha8_2()
   {
      name = "Alpha8_2";
   }
}

class Beta8_2 extends Alpha8_2
{
}

class Charlie8_2 extends Beta8_2
{
   private var foo: String;
   
   public function Bar()
   {
      foo = super.name;
      expected = "Alpha8_2";
      if (foo != expected)
         apLogFailInfo ("Error in 8.2", expected, foo, "");
   }
}


// -----------------------------------------------------------------------
class Alpha9_1
{
   public var name: String;
   
   public function Alpha9_1()
   {
      name = "Alpha9_1";
   }
}

class Beta9_1 extends Alpha9_1
{
   protected var foo: String;
   
   public function Bar()
   {
      foo = super.name;
      expected = "Alpha9_1";
      if (foo != expected)
         apLogFailInfo ("Error in 9.1", expected, foo, "");
   }
}


// -----------------------------------------------------------------------
class Alpha9_2
{
   public var name: String;
   
   public function Alpha9_2()
   {
      name = "Alpha9_2";
   }
}

class Beta9_2 extends Alpha9_2
{
}

class Charlie9_2 extends Beta9_2
{
   protected var foo: String;
   
   public function Bar()
   {  
      foo = super.name;
      expected = "Alpha9_2";
      if (foo != expected)
         apLogFailInfo ("Error in 9.2", expected, foo, "");
   }  
}


// -----------------------------------------------------------------------
var expectedError10_1, actualError10_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha10_1                              " +
         "  {                                            " +
         "     private var name: String;                 " +
         "     public function Alpha10_1()               " +
         "     {                                         " +
         "        name = \"Alpha10_1\";                  " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta10_1 extends Alpha10_1             " +
         "  {                                            " +
         "     public var foo: String;                   " +
         "     public function Bar()                     " +
         "     {                                         " +
         "        foo = super.name;                      " +
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

actualError10_1 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError10_1 = "test.js(1,624) : error JS1123: 'super.name' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError10_1 = "test.js(1,624) : error JS1123: 'super.name' is not accessible from this scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError10_2, actualError10_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha10_2                        " +
         "  {                                      " +
         "     private var name: String;           " +
         "     public function Alpha10_2()         " +
         "     {                                   " +
         "        name = \"Alpha10_2\";            " +
         "     }                                   " +
         "  }                                      " +
         "  class Beta10_2 extends Alpha10_2       " +
         "  {  }                                   " +
         "  class Charlie10_2 extends Beta10_2     " +
         "  {                                      " +
         "     public var foo: String;             " +
         "     public function Bar()               " +
         "     {                                   " +
         "        foo = super.name;                " +
         "     }                                   " +
         "  }                                      ";
@if(@rotor)
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

actualError10_2 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError10_2 = "test.js(1,628) : error JS1123: 'super.name' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError10_2 = "test.js(1,628) : error JS1123: 'super.name' is not accessible from this scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError11_1, actualError11_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha11_1                     " +
         "  {                                   " +
         "     static var name: String;         " +
         "     public function Alpha11_1()      " +
         "     {                                " +
         "        name = \"Alpha11_1\";         " +
         "     }                                " +
         "  }                                   " +
         "  class Beta11_1 extends Alpha11_1    " +
         "  {                                   " +
         "     private var foo: String;         " +
         "     public function Bar()            " +
         "     {                                " +
         "        foo = super.name;             " +
         "     }                                " +
         "  }                                   ";
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

actualError11_1 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError11_1 = "test.js(1,513) : error JS1244: The static member 'name' must be accessed with the class name" + Environment.NewLine + "";
@else
expectedError11_1 = "test.js(1,513) : error JS1244: The static member 'name' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha11_2
{
   static var name: String;
   
   public function Alpha11_2()
   {
      name = "Alpha11_2";
   }
}

class Beta11_2 extends Alpha11_2
{
}

class Charlie11_2 extends Beta11_2
{
   private var foo: String;
   
   public function Bar()
   {
      exceptionThrown = false
      
      try
      {
         eval ("foo = super.name;");
      }
      catch (error)
      {
         exceptionThrown = true;
      }
      
      if (exceptionThrown == false)
         apLogFailInfo ("Scenario 11.2 does not give a compile error", "Should give a compile error", exceptionThrown, "186473");
      if (foo == "Alpha11_2")
         apLogFailInfo ("Error in 11.2", "Shouldn't be accessible", foo, "186473");              
   }
}


// -----------------------------------------------------------------------
class Alpha12_1
{
   protected var name: String;
   
   public function Alpha12_1()
   {
      name = "Alpha12_1";
   }
}

class Beta12_1 extends Alpha12_1
{
   static var foo: String;
   
   public function Bar()
   {
      expected = "Alpha12_1";
      foo = super.name;
      if (foo != expected) 
         apLogFailInfo ("Error in 12.1", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Alpha12_2
{
   protected var name: String;
   
   public function Alpha12_2()
   {
      name = "Alpha12_2";
   }
}

class Beta12_2 extends Alpha12_2
{
}

class Charlie12_2 extends Beta12_2
{
   static var foo: String;
   
   public function Bar()
   {
      expected = "Alpha12_2";
      foo = super.name;
      if (foo != expected)
         apLogFailInfo ("Error in 12.2", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Alpha13_1
{
   private var name: String;
   
   public function Alpha13_1()
   {
      name = "Alpha13_1";
   }
}

class Beta13_1 extends Alpha13_1
{
   public function Bar()
   {
      exceptionThrown = false;
      actualError = "";
      expectedError = "ReferenceError: Objects of type 'System.Object' do not have such a member";
      
      try
      {
         eval ("super.name = \"Foo\";");      
      }
      catch (error)
      {
         exceptionThrown = true;
         actualError = error;
      }
      
      if (exceptionThrown == false)
         apLogFailInfo ("Modifying the value of super.field (private) does not give a compile error (13.1)", "Should give a compile error", exceptionThrown, "");
      if (actualError != expectedError)
         apLogFailInfo ("Wrong error message (13.1)", expectedError, actualError, "");                        
   }
}


// -----------------------------------------------------------------------
class Alpha13_2
{
   private var name: String;
   
   public function Alpha13_2()
   {
      name = "Alpha13_2";
   }
}

class Beta13_2 extends Alpha13_2
{
}

class Charlie13_2 extends Beta13_2
{
   public function Bar()
   {
      exceptionThrown = false;
      actualError = "";
      expectedError = "ReferenceError: Objects of type 'System.Object' do not have such a member";
      
      try
      {
         eval ("super.name = \"Foo\";");
      }
      catch (error)
      {
         exceptionThrown = true;
         actualError = error;
      }
      
      if (exceptionThrown == false)
         apLogFailInfo ("Modifying the value of super.field (private) does not give a compile error (13.2)","Should give a compile error", exceptionThrown, "");
      if (actualError != expectedError)
         apLogFailInfo ("Wrong error message (13.2)", expectedError, actualError, "");
   }
}


// -----------------------------------------------------------------------
class Alpha14_1
{
   protected var name: String;
   
   public function Alpha14_1()
   {
      name = "Alpha14_1";
   }
}

class Beta14_1 extends Alpha14_1
{
   public function Bar()
   {
      actual = "";
      expected = "Foo";
      super.name = "Foo";
      actual = super.name;
      if (actual != expected)
         apLogFailInfo ("Error in 14.1", expected, actual, "189234");
   }
}


// -----------------------------------------------------------------------
class Alpha14_2
{
   protected var name: String;
   
   public function Alpha14_2()
   {
      name = "Alpha14_2";
   }
}

class Beta14_2 extends Alpha14_2
{
}

class Charlie14_2 extends Beta14_2
{
   public function Bar()
   {
      actual = "";
      expected = "Foo";
      super.name = "Foo";
      actual = super.name;
      if (actual != expected)
         apLogFailInfo ("Error in 14.2", expected, actual, "189234");
   }
}



// -----------------------------------------------------------------------
function super06()
{
   apInitTest ("super06");


   // -----------------------------------------------------------------------
   apInitScenario ("6.3 Subclass calls protected super.GetName() from a different method (static) - 3rd level in the inheritance tree.  GetName() in the 1st and 2nd level.");
   // Compile error: super cannot be referenced from a static context               

   if (exceptionThrown6_3 == false)
      apLogFailInfo ("No compile error in 6.3", "Should give a compile error", exceptionThrown6_3, "");
   if (actualError6_3 != expectedError6_3)
      apLogFailInfo ("Wrong compile error in 6.3", expectedError6_3, actualError6_3, "200532");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("7.1 Subclass assigns super.field to a local variable - 2nd level in the inheritance tree.");
      
   var beta7_1: Beta7_1 = new Beta7_1();
   beta7_1.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("7.2 Subclass assigns super.field to a local variable. - 3rd level in the inheritance tree.");
   
   var charlie7_2: Charlie7_2 = new Charlie7_2();
   charlie7_2.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("8.1 Subclass assigns super.field (protected) to its private field - 2nd level in the inheritance tree.");
   
   var beta8_1: Beta8_1 = new Beta8_1();
   beta8_1.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("8.2 Subclass assigns super.field (protected) to its private field - 3rd level in the inheritance tree.");
   
   var charlie8_2: Charlie8_2 = new Charlie8_2();
   charlie8_2.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("9.1 Subclass assigns super.field (public) to its protected field - 2nd level in the inheritance tree.");
                       
   var beta9_1: Beta9_1 = new Beta9_1();
   beta9_1.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("9.2 Subclass assigns super.field (public) to its protected field - 3rd level in the inheritance tree.");
   
   var charlie9_2: Charlie9_2 = new Charlie9_2();
   charlie9_2.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("10.1 Subclass assigns super.field (private) to its public field - 2nd level in the inheritance tree.");
                          
   if (actualError10_1 != expectedError10_1)
      apLogFailInfo ("Error in 10.1", expectedError10_1, actualError10_1, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("10.2 Subclass assigns super.field (private) to its public field - 3rd level in the inheritance tree");
   
   if (actualError10_2 != expectedError10_2)
      apLogFailInfo ("Error in 10.2", expectedError10_2, actualError10_2, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("11.1 Subclass assigns super.field (static) to its private field - 2nd level in the inheritance tree");
   
   if (actualError11_1 != expectedError11_1)
      apLogFailInfo ("Error in 11.1", expectedError11_1, actualError11_1, "");


   // -----------------------------------------------------------------------
   apInitScenario ("11.2 Subclass assigns super.field (static) to its private field - 3rd level in the inheritance tree");
   // Compile error
   
   var charlie11_2: Charlie11_2 = new Charlie11_2();
   charlie11_2.Bar();


   // -----------------------------------------------------------------------
   apInitScenario ("12.1 Subclass assigns super.field (protected) to its static field - 2nd level in the inheritance tree.");
                        
   var beta12_1: Beta12_1 = new Beta12_1();
   beta12_1.Bar();


   // -----------------------------------------------------------------------
   apInitScenario ("12.2 Subclass assigns super.field (protected) to its static field - 3rd level in the inheritance tree.");
   
   var charlie12_2: Charlie12_2 = new Charlie12_2();
   charlie12_2.Bar();

   
   // -----------------------------------------------------------------------
   apInitScenario ("13.1 Subclass modifies the value of super.field (private) - 2nd level in the inheritance tree.");

   var beta13_1: Beta13_1 = new Beta13_1();
   beta13_1.Bar();


   // -----------------------------------------------------------------------
   apInitScenario ("13.2 Subclass modifies the value of super.field (private) - 3rd level in the inheritance tree.");

   var charlie13_2: Charlie13_2 = new Charlie13_2();
   charlie13_2.Bar();


   // -----------------------------------------------------------------------
   apInitScenario ("14.1 Subclass modifies the value of super.field (protected) - 2nd level in the inheritance tree.");

   var beta14_1: Beta14_1 = new Beta14_1();
   beta14_1.Bar();


   // -----------------------------------------------------------------------
   apInitScenario ("14.2 Subclass modifies the value of super.field (protected) - 3rd level in the inheritance tree.");

   var charlie14_2: Charlie14_2 = new Charlie14_2();
   charlie14_2.Bar();


   apEndTest();
}


super06();


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
