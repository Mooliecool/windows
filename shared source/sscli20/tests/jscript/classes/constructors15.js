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


var iTestID = 193770;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


@if(!@aspx)
	import System
	import System.IO
	import System.Diagnostics
	import System.Reflection
@else
	</script>
	<%@ import namespace="System.IO" %>
	<%@ import namespace="System.Diagnostics" %>
	<%@ import namespace="System.Reflection" %>
	<script language=jscript runat=server>
@end

var JSCode: String;
var fileStream: FileStream;
var streamWriter: StreamWriter;
var process: Process;
var processStartInfo: ProcessStartInfo;


// -----------------------------------------------------------------------
var exceptionThrown31_9 = false;
var actualError31_9 = "";
var expectedError31_9 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha31_9                           " +
         "  {                                         " +
         "     public function Alpha31_9 (x: String)  " +
         "     {                                      " +
         "     }                                      " +
         "  }                                         " +
         "  class Beta31_9 extends Alpha31_9          " +
         "  {                                         " +
         "     public function Beta31_9 (x: int)      " +
         "     {                                      " +
         "        var x = 10;                         " +
         "        super (\"hello\");                  " +
         "     }                                      " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown31_9 = true;
   actualError31_9 = error;
}


// -----------------------------------------------------------------------
class Alpha31_10
{
   public var value: String = "none";
   
   public function Alpha31_10()
   {
      super();
      value = "one";
   }
}


// -----------------------------------------------------------------------
class Alpha31_11
{
   public var value: String = "none";
   
   public function Alpha31_11()
   {
      super ("Hello");
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha31_12
{
   public var value: String = "none";
}

class Beta31_12 extends Alpha31_12
{
   public function Beta31_12()
   {
      super();
   }
}


// -----------------------------------------------------------------------
class Alpha31_13
{
   public var value: String = "none";
   
   public function Alpha31_13 (x: boolean)
   {
      value = "two";
   }
}

class Beta31_13 extends Alpha31_13
{
   public function Beta31_13()
   {
      super();
   }
}


// -----------------------------------------------------------------------
class Alpha31_14
{
   public var value: String = "none";
}

class Beta31_14 extends Alpha31_14
{
   public function Beta31_14()
   {
      super (true);
   }
}


// -----------------------------------------------------------------------
class Alpha31_15
{
   public var value: String = "none";
   
   public function Alpha31_15 (x: int)
   {
      value = "one";
   }
   
   public function Alpha31_15 (x: int, y: String)
   {
      value = "two";
   }
}

class Beta31_15 extends Alpha31_15
{
   public function Beta31_15()
   {
      super (25);
   }
}


// -----------------------------------------------------------------------
class Alpha31_16
{
   public var value: String = "none";
   
   public function Alpha31_16 (x: int)
   {
      value = "three";
   }
   
   public function Alpha31_16 (x: int, y: String)
   {
      value = "four";
   }
}

class Beta31_16 extends Alpha31_16
{
   public function Beta31_16()
   {
      super (25, "hello");
   }
}


// -----------------------------------------------------------------------
class Alpha31_17
{
   public var value: String = "none";
   
   public function Alpha31_17 (x: int)
   {
      value = "five";
   }
   
   public function Alpha31_17 (x: double)
   {
      value = "six";
   }
}

class Beta31_17 extends Alpha31_17
{
   public function Beta31_17()
   {
      super(50);
   }
}


// -----------------------------------------------------------------------
class Alpha32
{
   public function Beta32()
   {
      return 1;
   }
}

class Beta32 extends Alpha32
{
   public function Beta32()
   {
   }
}

class Charlie32 extends Beta32
{
   public function Beta32()
   {
      return 2;
   }
}


// -----------------------------------------------------------------------
class Alpha33
{
   public var value: int = 0;
   public function Alpha33 (x: int)
   {
      value = 5;   
   }
}

class Beta33 extends Alpha33
{
   public function Beta33()
   {
   }
}


// -----------------------------------------------------------------------
var expectedError34, actualError34: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha34                             " +
         "  {                                         " +
         "     public function Alpha34 (x: int) { }   " +
         "     public function Alpha34 (x: float) { } " +
         "  }                                         " +
         "  class Beta34 extends Alpha34              " +
         "  {                                         " +
         "     public function Beta34 () { }          " +
         "  }                                         ";
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
actualError34 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError34 = "test.js(1,314) : error JS1184: More than one constructor matches this argument list" + Environment.NewLine + "";
@else
expectedError34 = "test.js(1,314) : error JS1184: More than one constructor matches this argument list" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha35
{
   public var value : Number;
   public function Alpha35 (newValue: Number)
   {
      this.value = newValue;
   }
}


// -----------------------------------------------------------------------
class Beta36 extends Alpha36
{
   public function Beta36()
   {
      super();
   }
}

class Alpha36
{
   public var value: String;
   
   public function Alpha36()
   {
      value = "five";
   }
}


// -----------------------------------------------------------------------
function constructors15()
{
   apInitTest ("Constructors15");
    
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.9 Calling superclass' constructors using 'super'. Call to super(String) from a non-default constructor is not the first line of a constructor"); 

   if (exceptionThrown31_9 == false)
      apLogFailInfo ("No compile error in 31.9", "Should give a compile error", exceptionThrown31_9, "");
   if (actualError31_9 != expectedError31_9)
      apLogFailInfo ("Wrong compile error in 31.9", expectedError31_9, actualError31_9, "192201");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.10 Calling superclass' constructors using 'super'. Alpha() calls super() and it doesn't have a superclass");
   // This will call Object's default constructor 
   
   var alpha31_10: Alpha31_10 = new Alpha31_10();
   
   expected = "one";
   actual = "";
   actual = alpha31_10.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.10", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.11 Calling superclass' constructors using 'super'. Alpha() calls super(String) and it doesn't have a superclass");
   // A warning will be given saying that the extra parameters will be ignored.   
   
   var alpha31_11: Alpha31_11 = new Alpha31_11();
   
   expected = "two";
   actual = "";
   actual = alpha31_11.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.11", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.12 Calling superclass' constructors using 'super'. Beta() calls super() and it doesn't exist");
      
   var beta31_12: Beta31_12 = new Beta31_12();
   
   expected = "none";
   actual = "";
   actual = beta31_12.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.12", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.13 Calling superclass' constructors using 'super'. Beta() calls super() but only Alpha (boolean) exists");
      
   var beta31_13: Beta31_13 = new Beta31_13();
   
   expected = "two";
   actual = "";
   actual = beta31_13.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.13", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.14 Calling superclass' constructors using 'super'. Beta() calls super(boolean) and it doesn't exist");
   // A warning will be given saying that the extra parameters will be ignored.
      
   var beta31_14: Beta31_14 = new Beta31_14();
   
   expected = "none";
   actual = "";
   actual = beta31_14.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.14", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.15 Calling superclass' constructors using 'super'. Beta() calls super(25) with Alpha(int) & Alpha(int,String) available");
      
   var beta31_15: Beta31_15 = new Beta31_15();
   
   expected = "one";
   actual = "";
   actual = beta31_15.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.15", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.16 Calling superclass' constructors using 'super'. Beta() calls super(25,'hello') with Alpha(int) & Alpha(int,String) available");
      
   var beta31_16: Beta31_16 = new Beta31_16();
   
   expected = "four";
   actual = "";
   actual = beta31_16.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.16", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.17 Calling superclass' constructors using 'super'. Beta() calls super(50) with Alpha(int) and Alpha(double) present");
      
   var beta31_17: Beta31_17 = new Beta31_17();
   
   expected = "five";
   actual = "";
   actual = beta31_17.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.17", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("32. Subclass has a constructor with the same name as a method in the superclass");
   
   var alpha32: Alpha32;
   var beta32: Beta32;
   var charlie32: Charlie32;

   // alpha32
   alpha32 = new Alpha32();
   expected = 1;
   actual = 0;
   actual = alpha32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(1)", expected, actual, "");
      
   alpha32 = new Beta32();
   expected = 1;
   actual = 0;
   actual = alpha32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(2)", expected, actual, "");

   alpha32 = new Charlie32();
   expected = 2;
   actual = 0;
   actual = alpha32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(3)", expected, actual, "");
   
   // beta32
   beta32 = new Beta32();
   expected = 1;
   actual = 0;
   actual = beta32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(4)", expected, actual, "");

   beta32 = new Charlie32();
   expected = 2;
   actual = 0;
   actual = beta32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(5)", expected, actual, "");
      
   // charlie32   
   charlie32 = new Charlie32();
   expected = 2;
   actual = 0;
   actual = charlie32.Beta32();
   if (actual != expected)
      apLogFailInfo ("Error in 32(6)", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("33. Alpha has Alpha(int) constructor; Beta has Beta() constructor");
   
   var beta33: Beta33 = new Beta33();
   
   actual = 0;
   expected = 5;
   actual = beta33.value;
   if (actual != expected)
      apLogFailInfo ("Error in 33", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("34. Alpha has Alpha(int) and Alpha(float) constructors; Beta has Beta() constructor");
   
   if (actualError34 != expectedError34)
      apLogFailInfo ("Error in 34", expectedError34, actualError34, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35. allows CTOR to be called twice from untrusted code");
   
   var one: Object = new Alpha35(123);
   var type: Type = Alpha35;
   var two: Object = type.GetMember (".ctor", BindingFlags.Public | BindingFlags.Instance);
   var args: Object[] = [456];

   expected = 123;
   actual = 0;
   actual = one.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(1)", expected, actual, "318316");
      
   two[0].Invoke (one, args);

   expected = 456;
   actual = 0;
   actual = one.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(2)", expected, actual, "318316");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Call a forward referenced base class constructor using super");
   
   var beta36: Beta36 = new Beta36();
   
   expected = "five";
   actual = "";
   actual = beta36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36.", expected, actual, "");
      
      
   apEndTest();
}



constructors15();


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
