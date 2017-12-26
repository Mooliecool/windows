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


var iTestID = 142524;



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
class Alpha1_1
{
   public function GetName(): String
   {
      return "Alpha1_1";
   }
}

class Beta1_1 extends Alpha1_1
{
   public function GetName(): String
   {
      actual = "";
      expected = "Alpha1_1";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 1.1", expected, actual, "186461");
   }
}


// -----------------------------------------------------------------------
class Alpha1_2
{
   public function GetName(): String
   {
      return "Alpha1_2";
   }
}

class Beta1_2 extends Alpha1_2
{
   public function GetName(): String
   {
      return "Beta1_2";
   }
}

class Charlie1_2 extends Beta1_2
{
   public function GetName(): String
   {
      actual = "";
      expected = "Beta1_2";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 1.2", expected, actual, "186461");
   }
}


// -----------------------------------------------------------------------
class Alpha1_3
{
   public function GetName(): String
   {
      return "Alpha1_3";
   }
}

class Beta1_3 extends Alpha1_3
{
}

class Charlie1_3 extends Beta1_3
{
   public function GetName(): String
   {
      actual = "";
      expected = "Alpha1_3";
      actual = super.GetName();
      if (actual != expected)
         apLogFailInfo ("Error in 1.3", expected, actual, "");
   }
}


// -----------------------------------------------------------------------
var expectedError2_1, actualError2_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha2_1                                  " +
         "  {                                               " +
         "     public static function GetName(): String     " +
         "     {                                            " +
         "        return \"Alpha2_1\";                      " +
         "     }                                            " +
         "  }                                               " +
         "  class Beta2_1 extends Alpha2_1                  " +
         "  {                                               " +
         "     public function GetName(): String            " +
         "     {                                            " +
         "        var x = \"\";                             " +
         "        x = super.GetName();                      " +
         "     }                                            " +
         "  }                                               ";
         

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

actualError2_1 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError2_1 = "test.js(1,615) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@else
expectedError2_1 = "test.js(1,615) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError2_2, actualError2_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha2_2                                     " +
         "  {                                                  " +
         "     public static function GetName(): String        " +
         "     {                                               " +
         "        return \"Alpha2_2\";                         " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta2_2 extends Alpha2_2                     " +
         "  {  }                                               " +
         "  class Charlie2_2 extends Beta2_2                   " +
         "  {                                                  " +
         "     public function GetName(): String               " +
         "     {                                               " +
         "        var x = \"\";                                " +
         "        x = super.GetName();                         " +
         "     }                                               " +
         "  }                                                  ";
         

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

actualError2_2 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError2_2 = "test.js(1,757) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@else
expectedError2_2 = "test.js(1,757) : error JS1244: The static member 'GetName' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var exceptionThrown3_1 = false;
var actualError3_1 = "";
var expectedError3_1 = "ReferenceError: 'super' is not accessible from this scope";

try
{
   eval ("  class Alpha3_1                                     " +
         "  {                                                  " +
         "     protected function GetName(): String            " +
         "     {                                               " +
         "        return \"Alpha3_1\";                         " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta3_1 extends Alpha3_1                     " +
         "  {                                                  " +
         "     public static function GetName(): String        " +
         "     {                                               " +
         "        super.GetName();                             " +
         "     }                                               " +
         "  }                                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown3_1 = true;
   actualError3_1 = error;
}



// -----------------------------------------------------------------------
function super01()
{
   apInitTest ("super01");

   // -----------------------------------------------------------------------
   apInitScenario ("1.1 Subclass calls super.GetName() from GetName() - 2nd level in the inheritance tree");
   
   var beta1_1: Beta1_1 = new Beta1_1();
   beta1_1.GetName();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("1.2 Subclass calls super.GetName() from GetName() - 3rd level in the inheritance tree. GetName() in the 1st and 2nd level.");

   var charlie1_2: Charlie1_2 = new Charlie1_2();
   charlie1_2.GetName();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("1.3 Subclass calls super.GetName() from GetName() - 3rd level in the inheritance tree. GetName() in the 1st level only.");

   var charlie1_3: Charlie1_3 = new Charlie1_3();
   charlie1_3.GetName();
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("2.1 Subclass calls static super.GetName() from public GetName() - 2nd level in the inheritance tree.");

   if (actualError2_1 != expectedError2_1)
      apLogFailInfo ("Error in 2.1", expectedError2_1, actualError2_1, "");
                             

   // -----------------------------------------------------------------------
   apInitScenario ("2.2 Subclass calls static super.GetName() from public GetName() - 3rd level in the inheritance tree.");
   
   if (actualError2_2 != expectedError2_2)
      apLogFailInfo ("Error in 2.2", expectedError2_2, actualError2_2, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Subclass calls protected super.GetName() from static GetName() - 2nd level in the inheritance tree.");
   // Compile error: super cannot be referenced from a static context               
   
   if (exceptionThrown3_1 == false)
      apLogFailInfo ("No compile error in 3.1", "Should give a compile error", exceptionThrown3_1, "");
   if (actualError3_1 != expectedError3_1)
      apLogFailInfo ("Wrong compile error in 3.1", expectedError3_1, actualError3_1, "");


   apEndTest();
}


super01();


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
