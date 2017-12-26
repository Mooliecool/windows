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


var iTestID = 194180;



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
var exceptionThrown3_3 = false;
var actualError3_3 = "";
var expectedError3_3 = "ReferenceError: 'super' is not accessible from this scope";

try
{
   eval ("  class Alpha3_3                                  " +
         "  {                                               " +
         "     protected function GetName(): String         " +
         "     {                                            " +
         "        return \"Alpha3_3\";                      " +
         "     }                                            " +
         "  }                                               " +
         "  class Beta3_3 extends Alpha3_3                  " +
         "  {                                               " +
         "     protected function GetName(): String         " +
         "     {                                            " +
         "        return \"Beta3_3\";                       " +
         "     }                                            " +
         "  }                                               " +
         "  class Charlie3_3 extends Beta3_3                " +
         "  {                                               " +
         "     public static function GetName(): String     " +
         "     {                                            " +
         "        var x = \"\";                             " +
         "        x = super.GetName();                      " +
         "     }                                            " +
         "  }                                               ", "unsafe");
}
catch (error)
{
   exceptionThrown3_3 = true;
   actualError3_3 = error;
}


// -----------------------------------------------------------------------
class Alpha4_1
{
   protected function GetName(): String
   {
      return "Alpha4_1";
   }
}

class Beta4_1 extends Alpha4_1
{
   public function Bar()
   {
      actual = "";
      expected = "Alpha4_1";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 4.1", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Alpha4_2
{
   protected function GetName(): String
   {
      return "Alpha4_2";
   }
}

class Beta4_2 extends Alpha4_2
{
}

class Charlie4_2 extends Beta4_2
{
   public function Bar()
   {
      actual = "";
      expected = "Alpha4_2";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 4.2", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
class Alpha4_3
{
   protected function GetName(): String
   {
      return "Alpha4_3";
   }
}

class Beta4_3 extends Alpha4_3
{
   protected function GetName(): String
   {
      return "Beta4_3";
   }
}

class Charlie4_3 extends Beta4_3
{
   public function Bar()
   {
      actual = "";
      expected = "Beta4_3";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 4.3", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
var expectedError5_1, actualError5_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha5_1                                     " +
         "  {                                                  " +
         "     static function GetName(): String               " +
         "     {                                               " +
         "        return \"Alpha5_1\";                         " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta5_1 extends Alpha5_1                     " +
         "  {                                                  " +
         "     public function Bar()                           " +
         "     {                                               " +
         "        var x = \"\";                                " +
         "        x = super.GetName();                         " +
         "     }                                               " +
         "  }                                                  ";

@if (!@rotor)
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

actualError5_1 = process.StandardOutput.ReadToEnd();
@if (!@rotor)
	expectedError5_1 = "test.js(1,651) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@else
	expectedError5_1 = "test.js(1,651) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError5_2, actualError5_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha5_2                                     " +
         "  {                                                  " +
         "     static function GetName(): String               " +
         "     {                                               " +
         "        return \"Alpha5_2\";                         " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta5_2 extends Alpha5_2                     " +
         "  {  }                                               " +
         "  class Charlie5_2 extends Beta5_2                   " +
         "  {                                                  " +
         "     public function Bar()                           " +
         "     {                                               " +
         "        var x = \"\";                                " +
         "        x = super.GetName();                         " +
         "     }                                               " +
         "  }                                                  ";

@if (!@rotor)
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

actualError5_2 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
	expectedError5_2 = "test.js(1,757) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@else
	expectedError5_2 = "test.js(1,757) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@end



// -----------------------------------------------------------------------
function super03()
{
   apInitTest ("super03");


   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Subclass calls protected super.GetName() from static GetName() - 3rd level in the inheritance tree.  GetName() in the 1st and 2nd level.");
   // Compile error: super cannot be referenced from a static context               

   if (exceptionThrown3_3 == false)
      apLogFailInfo ("No compile error in 3.3", "Should give a compile error", exceptionThrown3_3, "");
   if (actualError3_3 != expectedError3_3)
      apLogFailInfo ("Wrong compile error in 3.3", expectedError3_3, actualError3_3, "200532");


   // -----------------------------------------------------------------------
   apInitScenario ("4.1 Subclass calls protected super.GetName() from a different method - 2nd level in the inheritance tree.");

   var beta4_1: Beta4_1 = new Beta4_1();
   beta4_1.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("4.2 Subclass calls protected super.GetName() from a different method - 3rd level in the inheritance tree.  GetName() in the 1st level only.");
   
   var charlie4_2: Charlie4_2 = new Charlie4_2();
   charlie4_2.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("4.3 Subclass calls protected super.GetName() from a different method - 3rd level in the inheritance tree.  GetName() in the 1st and 2nd level.");
   
   var charlie4_3: Charlie4_3 = new Charlie4_3();
   charlie4_3.Bar();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("5.1 Subclass calls static super.GetName() from a different method - 2nd level in the inheritance tree.");
   
   if (actualError5_1 != expectedError5_1)
      apLogFailInfo ("Error in 5.1", expectedError5_1, actualError5_1, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("5.2 Subclass calls static super.GetName() from a different method - 3rd level in the inheritance tree.");
   // Compile error
   
   if (actualError5_2 != expectedError5_2)
      apLogFailInfo ("Error in 5.2", expectedError5_2, actualError5_2, "");


   apEndTest();
}


super03();


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
