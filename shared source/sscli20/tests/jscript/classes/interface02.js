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


var iTestID = 197617;



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
@else
	</script>
	<%@ import namespace="System.IO" %>
	<%@ import namespace="System.Diagnostics" %>
	<script language=jscript runat=server>
@end

import Letters;

var JSCode: String;
var fileStream: FileStream;
var streamWriter: StreamWriter;
var process: Process;
var processStartInfo: ProcessStartInfo;


// -----------------------------------------------------------------------
var exceptionThrown26 = false;
var actualError26 = "";
var expectedError26 = "Error: No implementation provided for 'Group26.InterfaceA26.Bar() : System.String'"

package Group26
{
   interface InterfaceA26
   {
      public function Bar(): String;
   }
}

import Group26;

try
{
   eval ("  class Alpha26 implements InterfaceA26  " +
         "  {  }                                   ");
}
catch (error)
{
   exceptionThrown26 = true;
   actualError26 = error;
}


// -----------------------------------------------------------------------
interface InterfaceA27
{
   public function Bar(): String;
}

class Alpha27
{
   class Beta27 implements InterfaceA27
   {
      public function Bar(): String
      {
         return "Alpha27.Beta27.Bar()";
      }
   }
}

class Charlie27
{
   public function Foo (interfaceA27: InterfaceA27)
   {
      var temp: String;
      temp = interfaceA27.Bar();
      temp = temp + "!";
      return temp;
   }
}

function Zoo27 (interfaceA27: InterfaceA27)
{
   var temp: String;
   temp = interfaceA27.Bar();
   temp = temp + "?";
   return temp;
}


// -----------------------------------------------------------------------
interface InterfaceA28
{
   public function Bar(): String;
}

class Alpha28
{
   static class Beta28 implements InterfaceA28
   {
      public function Bar(): String
      {
         return "Alpha28.Beta28.Bar()";
      }
   }
}

class Charlie28
{
   public function Foo (interfaceA28: InterfaceA28)
   {
      var temp: String;
      temp = interfaceA28.Bar();
      temp = temp + "!";
      return temp;
   }
}

function Zoo28 (interfaceA28: InterfaceA28)
{
   var temp: String;
   temp = interfaceA28.Bar();
   temp = temp + "?";
   return temp;
}


// -----------------------------------------------------------------------
interface InterfaceA29
{
   public function Bar(): String;
}

class Alpha29 implements InterfaceA29
{
   class Beta29
   {
   }
   
   public function Bar(): String
   {
      return "Alpha29.Bar()";
   }
}

class Charlie29 
{
   public function Foo (interfaceA: InterfaceA29)
   {
      var temp: String;
      temp = interfaceA.Bar();
      temp = temp + "!";
      return temp;
   }
}

function Zoo29 (interfaceA: InterfaceA29)
{
   var temp: String;
   temp = interfaceA.Bar();
   temp = temp + "?";
   return temp;
}


// -----------------------------------------------------------------------
var exceptionThrown30 = false;
var actualError30 = "";
var expectedError30 = "Error: No implementation provided for 'InterfaceA30.Bar() : System.String'"

try
{
   eval ("  interface InterfaceA30                    " +
         "  {                                         " +
         "     public function Bar(): String;         " +
         "  }                                         " +
         "  class Alpha30 implements InterfaceA30     " +
         "  {                                         " +
         "     class Beta30 implements InterfaceA30   " +
         "     {  }                                   " +
         "  }                                         ",
         "unsafe"
         );
}
catch (error)
{
   exceptionThrown30 = true;
   actualError30 = error;
}


// -----------------------------------------------------------------------
interface InterfaceA31
{
   public function Bar(): String
}

class Alpha31 implements InterfaceA31
{ 
   public function Bar(): String
   {
      return "Alpha31.Bar()";
   }
   
   class Beta31 implements InterfaceA31
   {
      public function Bar(): String
      {
         return "Alpha31.Beta31.Bar()";
      }
   }
}

class Charlie31
{
   public function Foo (interfaceA31: InterfaceA31)
   {
      var temp: String;
      temp = interfaceA31.Bar();
      temp = temp + "!";
      return temp;
   }
}

function Zoo31 (interfaceA31: InterfaceA31)
{
   var temp: String;
   temp = interfaceA31.Bar();
   temp = temp + "?";
   return temp;
}  


// -----------------------------------------------------------------------
interface InterfaceA32
{
   public function Bar(): String;
}

class Alpha32 implements InterfaceA32
{
   public function Bar(): String
   {
      return "Alpha32.Bar()";
   }
   
   public function Foo (interfaceA32: InterfaceA32)
   {
      var temp: String;
      temp = interfaceA32.Bar();
      temp = temp + "!";
      return temp;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown33 = false;
var actualError33 = "";
var expectedError33 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  interface InterfaceA33                 " +
         "  {                                      " +
         "     private function Bar(): String;     " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown33 = true;
   actualError33 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown34 = false;
var actualError34 = "";
var expectedError34 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  interface InterfaceA34                 " +
         "  {                                      " +
         "     protected function Bar(): String;   " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown34 = true;
   actualError34 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown35 = false;
var actualError35 = "";
var expectedError35 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  interface InterfaceA35                 " +
         "  {                                      " +
         "     protected function Bar(): String;   " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown35 = true;
   actualError35 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown36 = false;
var actualError36 = "";
var expectedError36 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  package Group36                           " +
         "  {                                         " +
         "     interface InterfaceA36                 " +
         "     {                                      " +
         "        protected function Bar(): String;   " +
         "     }                                      " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown36 = true;
   actualError36 = error;
}


// -----------------------------------------------------------------------
interface InterfaceA37
{
   public function InterfaceA37();
}

class Alpha37 implements InterfaceA37
{
   public var value: String = "none";
   
   public function InterfaceA37()
   {
      value = "One";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown38 = false;
var actualError38 = "";
var expectedError38 = "Error: No implementation provided for 'InterfaceA38.Bar()'"

try
{
   eval ("  interface InterfaceA38                          " +
         "  {                                               " +
         "     public function Bar();                       " +
         "  }                                               " +
         "  abstract class Alpha38 implements InterfaceA38  " +
         "  {  }                                            " +
         "  class Beta38 extends Alpha38                    " +
         "  {  }                                            ",
         "unsafe"
         );
}
catch (error)
{
   exceptionThrown38 = true;
   actualError38 = error;
}


// -----------------------------------------------------------------------
var expectedError39, actualError39: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA39                             " +
         "  {                                                  " +
         "     public function Bar(): String;                  " +
         "  }                                                  " +
         "  abstract class Alpha39 implements InterfaceA39     " +
         "  {                                                  " +
         "  }                                                  " +
         "  class Beta39 extends Alpha39                       " +
         "  {                                                  " +
         "     public function Bar(): String                   " +
         "     {                                               " +
         "        return \"One\";                              " +
         "     }                                               " +
         "  }                                                  ";
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end

process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError39 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError39 = "test.js(1,215) : error JS1128: No implementation provided for 'InterfaceA39.Bar() : System.String'" + Environment.NewLine + "";
@else
expectedError39 = "test.js(1,215) : error JS1128: No implementation provided for 'InterfaceA39.Bar() : System.String'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
// This declaration is used by Scenarios 40.x
interface InterfaceA40
{
   public function Bar(): String;
}

abstract class Alpha40 implements InterfaceA40
{
   abstract public function Bar(): String
   static public function Foo (alpha40: Alpha40): String
   {
      var temp: String = "";
      temp = alpha40.Bar();
      return temp;
   }
}

class Beta40 extends Alpha40
{
   public function Bar(): String
   {
      return "hello";
   }
}

class Charlie40 extends Beta40
{
   public function Bar(): String
   {
      return "One";
   }
}

function FooAlpha40 (alpha40: Alpha40): String
{
   var temp: String = "";
   temp = alpha40.Bar();
   return temp;
}

function FooInterfaceA40 (interfaceA40: InterfaceA40): String
{
   var temp: String = "";
   temp = interfaceA40.Bar();
   return temp;
}



// -----------------------------------------------------------------------
var exceptionThrown41 = false;
var actualError41 = "";
var expectedError41 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  interface InterfaceA41           " +
         "  {                                " +
         "     final public function Bar();  " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown41 = true;
   actualError41 = error;
}


// -----------------------------------------------------------------------
var expectedError42, actualError42: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA42                 " +
         "  {                                      " +
         "     protected internal function Bar();  " +
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError42 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError42 = "test.js(1,88) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError42 = "test.js(1,88) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError43, actualError43: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  package Group43                        " +
         "  {                                      " +
         "     interface InterfaceA43              " +
         "     {                                   " +
         "        protected internal function Bar();  " +
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError43 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError43 = "test.js(1,173) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError43 = "test.js(1,173) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
interface InterfaceA44
{
   public function Bar(): String;
}

interface InterfaceB44
{
   public function Foo(): String;
}

class Alpha44 implements InterfaceA44, InterfaceB44
{
   public function Bar(): String
   {
      return "Alpha44.Bar()";
   }
   
   public function Foo(): String
   {
      return "Alpha44.Foo()";
   }
}

function Boo44 (interfaceA44: InterfaceA44): String
{
   var temp: String = "";
   temp = interfaceA44.Bar();
   return temp;
}

function Coo44 (interfaceB44: InterfaceB44): String
{
   var temp: String = "";
   temp = interfaceB44.Foo();
   return temp;
}


// -----------------------------------------------------------------------
interface InterfaceA45
{
   public function Bar(): String;
}

interface InterfaceB45
{
   public function Bar(): String;
}

interface InterfaceC45
{
   public function Bar(): String;
}

class Alpha45 implements InterfaceA45, InterfaceB45, InterfaceC45
{
   public function Bar(): String
   {
      return "Alpha45.Bar()";
   }
}

function Foo45 (interfaceA45: InterfaceA45): String
{
   var temp: String = "";
   temp = interfaceA45.Bar();
   return temp;
}

function Zoo45 (interfaceC45: InterfaceC45): String
{
   var temp: String = "";
   temp = interfaceC45.Bar();
   return temp;
}


// -----------------------------------------------------------------------
interface InterfaceA46
{
   public function Bar(): String;
}

interface InterfaceB46
{
   public function Bar(): String;
}

interface InterfaceC46
{
   public function Bar(): String;
}

class Alpha46 implements InterfaceA46, InterfaceB46, InterfaceC46
{
   public function Bar(): String
   {
      return "Alpha46.Bar()";
   }
   
   public function InterfaceB46.Bar(): String
   {
      return "InterfaceB46.Bar()";
   }
}


// -----------------------------------------------------------------------
var expectedError47, actualError47: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA47                                   " +
         "  {                                                        " +
         "     public function Bar(): String;                        " +
         "  }                                                        " +
         "  interface InterfaceB47                                   " +
         "  {                                                        " +
         "     public function Bar(): String;                        " +
         "  }                                                        " +
         "  class Alpha47 implements InterfaceA47, InterfaceB47      " +
         "  {                                                        " +
         "     public function InterfaceB47.Bar(): String            " +
         "     {                                                     " +
         "        return \"InterfaceB47.Bar()\";                     " +
         "     }                                                     " +
         "  }                                                        ";
         
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError47 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError47 = "test.js(1,475) : error JS1128: No implementation provided for 'InterfaceA47.Bar() : System.String'" + Environment.NewLine + "";
@else
expectedError47 = "test.js(1,475) : error JS1128: No implementation provided for 'InterfaceA47.Bar() : System.String'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError48, actualError48: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA48                             " +
         "  {                                                  " +
         "     public function Bar(): String;                  " +
         "  }                                                  " +
         "  interface InterfaceB48                             " +
         "  {                                                  " +
         "     public function Bar(): String;                  " +
         "  }                                                  " +
         "  class Alpha48 implements InterfaceA48, InterfaceB48   " +
         "  {                                                  " +
         "     public function InterfaceA48.Bar(): String      " +
         "     {                                               " +
         "        return \"InterfaceA48.Bar()\";               " +
         "     }                                               " +
         "     public function InterfaceB48.Bar(): String      " +
         "     {                                               " +
         "        return \"InterfaceB48.Bar()\";               " +
         "     }                                               " +
         "  }                                                  " +
         "  var alpha48: Alpha48 = new Alpha48();              " +
         "  alpha48.Bar();                                     ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError48 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError48 = "test.js(1,1070) : error JS1151: Objects of type 'Alpha48' do not have such a member" + Environment.NewLine + "";
@else
expectedError48 = "test.js(1,1070) : error JS1151: Objects of type 'Alpha48' do not have such a member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError49, actualError49: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA49 extends InterfaceC49  {  }  " +
         "  interface InterfaceB49 extends InterfaceA49  {  }  " +
         "  interface InterfaceC49 extends InterfaceB49  {  }  ";
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError49 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError49 = "test.js(1,109) : error JS1120: Circular definition" + Environment.NewLine + "" +
                  "test.js(1,56) : error JS1120: Circular definition" + Environment.NewLine + "" +
                  "test.js(1,3) : error JS1120: Circular definition" + Environment.NewLine + "";
@else
expectedError49 = "test.js(1,109) : error JS1120: Circular definition" + Environment.NewLine + "" +
                  "test.js(1,56) : error JS1120: Circular definition" + Environment.NewLine + "" +
                  "test.js(1,3) : error JS1120: Circular definition" + Environment.NewLine + "";
@end
   
// -----------------------------------------------------------------------
var expectedError50, actualError50: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA50 { }                   " +
         "  interface InterfaceB50 extends InterfaceA50  " +
         "  {                                            " +
         "     function InterfaceA50.Bar();              " +
         "  }                                            ";
         
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError50 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError50 = "test.js(1,168) : error JS1002: Syntax error" + Environment.NewLine + "" +
                  "test.js(1,147) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "";                  
@else
expectedError50 = "test.js(1,168) : error JS1002: Syntax error" + Environment.NewLine + "" +
                  "test.js(1,147) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "";                  
@end

// -----------------------------------------------------------------------
var expectedError51, actualError51: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA51                          " +
         "  {                                               " +
         "     public function Bar(): String;               " +
         "  }                                               " +
         "  interface InterfaceB51 extends InterfaceA51     " +
         "  {                                               " +
         "     public function InterfaceA51.Bar(): String;  " +
         "  }                                               ";
         
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError51 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError51 = "test.js(1,334) : error JS1002: Syntax error" + Environment.NewLine + "" +
                  "test.js(1,313) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "";
@else
expectedError51 = "test.js(1,334) : error JS1002: Syntax error" + Environment.NewLine + "" +
                  "test.js(1,313) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError52, actualError52: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA52 extends Nothing { } ";
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError52 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError52 = "test.js(1,34) : error JS1135: Variable 'Nothing' has not been declared" + Environment.NewLine + "";
@else
expectedError52 = "test.js(1,34) : error JS1135: Variable 'Nothing' has not been declared" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError53, actualError53: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha53 { }                            " +
         "  interface InterfaceA53 extends Alpha53 { }   ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError53 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError53 = "test.js(1,81) : error JS1129: Interface name expected" + Environment.NewLine + "";
@else
expectedError53 = "test.js(1,81) : error JS1129: Interface name expected" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError54, actualError54: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA54 { }             " +
         "  class Alpha54 extends InterfaceA54 { } ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError54 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError54 = "test.js(1,66) : error JS1134: Type: 'InterfaceA54' cannot be extended" + Environment.NewLine + "";
@else
expectedError54 = "test.js(1,66) : error JS1134: Type: 'InterfaceA54' cannot be extended" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError55, actualError55: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface A55                 " +
         "  {                             " +
         "  }                             " +
         "  class Alpha55 implements A55  " +
         "  {                             " +
         "     public function A55.Bar()  " +
         "     {  }                       " +
         "  }                             ";
         
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError55 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError55 = "test.js(1,166) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@else
expectedError55 = "test.js(1,166) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError56, actualError56: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface A56                 " +
         "  {                             " +
         "  }                             " +
         "  class Alpha56                 " +
         "  {                             " +
         "     public function A56.Bar()  " +
         "     {  }                       " +
         "  }                             ";
         
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError56 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError56 = "test.js(1,166) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@else
expectedError56 = "test.js(1,166) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
interface A57                            
{                                         
   public function get Bar(): int;
}

class Alpha57 implements A57
{
   function get A57.Bar(): int
   {
      return 1;
   }
   
   public function Bar(): int
   {
      return 2;
   }
}
         

// -----------------------------------------------------------------------
var expectedError58, actualError58: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA58                                " +
         "  {                                                     " +
         "     public function Bar(): String;                     " +
         "  }                                                     " +
         "  abstract class Alpha58 implements InterfaceA58        " +
         "  {                                                     " +
         "     abstract public function InterfaceA58.Bar(): String;  " +
         "     abstract public function Bar(): String;               " +
         "  }                                                     " +
         "  class Beta58 extends Alpha58                          " +
         "  {                                                     " +
         "     public function Bar(): String                      " +
         "     {                                                  " +
         "        return \"One\";                                 " +
         "     }                                                  " +
         "  }                                                     ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError58 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError58 = "test.js(1,342) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "" +
                  "test.js(1,513) : error JS1128: No implementation provided for 'Alpha58.InterfaceA58.Bar() : System.String'" + Environment.NewLine + "";
@else
expectedError58 = "test.js(1,342) : error JS1157: An abstract method cannot be private" + Environment.NewLine + "" +
                  "test.js(1,513) : error JS1128: No implementation provided for 'Alpha58.InterfaceA58.Bar() : System.String'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError59, actualError59: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA59                       " +
         "  {                                            " +
         "     public function Bar(): String;            " +
         "  }                                            " +
         "  abstract class Alpha59 implements InterfaceA59  " +
         "  {                                            " +
         "     abstract public function Bar(): String;   " +
         "  }                                            " +
         "  class Beta59 extends Alpha59                 " +
         "  {                                            " +
         "     public function Bar(): String             " +
         "     {                                         " +
         "        return \"One\";                        " +
         "     }                                         " +
         "     public function InterfaceA59.Bar(): String   " +
         "     {                                         " +
         "        return \"Two\";                        " +
         "     }                                         " +
         "  }                                            ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError59 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError59 = "test.js(1,665) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@else
expectedError59 = "test.js(1,665) : error JS1180: There is no matching method in a base class to override" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError60, actualError60: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA60        " +
         "  {                             " +
         "     private function Bar(): int;  " +
         "  }                             ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError60 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError60 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError60 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError61, actualError61: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA61        " +
         "  {                             " +
         "     protected function Bar(): int;  " +
         "  }                             ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError61 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError61 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError61 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError62, actualError62: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA62        " +
         "  {                             " +
         "     internal function Bar(): int;  " +
         "  }                             ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError62 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError62 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError62 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError63, actualError63: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA63        " +
         "  {                             " +
         "     internal protected function Bar(): int;  " +
         "  }                             ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError63 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError63 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError63 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError64, actualError64: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA64        " +
         "  {                             " +
         "     static function Bar(): int;  " +
         "  }                             ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError64 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError64 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@else
expectedError64 = "test.js(1,70) : error JS1224: This modifier cannot be used on an interface member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError65, actualError65: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA65                       " +
         "  {                                            " +
         "     public function Bar (x: int): int;        " +
         "     public function Bar (x: String): int;     " +
         "  }                                            " +
         "  class Alpha65 implements InterfaceA65        " +
         "  {                                            " +
         "  }                                            ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError65 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError65 = "test.js(1,238) : error JS1128: No implementation provided for 'InterfaceA65.Bar(System.Int32) : System.Int32'" + Environment.NewLine + "";
@else
expectedError65 = "test.js(1,238) : error JS1128: No implementation provided for 'InterfaceA65.Bar(System.Int32) : System.Int32'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError66, actualError66: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA66                       " +
         "  {                                            " +
         "     public function Bar (x: int): int;        " +
         "     public function Bar (x: String): int;     " +
         "  }                                            " +
         "  class Alpha66 implements InterfaceA66        " +
         "  {                                            " +
         "     public function Bar (x: String): int      " +
         "     {                                         " +
         "     }                                         " +
         "  }                                            ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError66 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError66 = "test.js(1,238) : error JS1128: No implementation provided for 'InterfaceA66.Bar(System.Int32) : System.Int32'" + Environment.NewLine + "";
@else
expectedError66 = "test.js(1,238) : error JS1128: No implementation provided for 'InterfaceA66.Bar(System.Int32) : System.Int32'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
interface InterfaceA67
{
   public function Bar (x: int): int;
   public function Bar (x: String): int;
}

class Alpha67 implements InterfaceA67
{
   public function Bar (x: int): int
   {
      return 10;
   }
   
   public function Bar (x: String): int
   {
      return 20;
   }
}


// -----------------------------------------------------------------------
var expectedError68, actualError68: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA68                       " +
         "  {                                            " +
         "     public function Bar (x: int): int;        " +
         "     public function Bar (x: int): String;     " +
         "  }                                            ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError68 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError68 = "test.js(1,107) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "" +
                  "test.js(1,154) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "";
@else
expectedError68 = "test.js(1,107) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "" +
                  "test.js(1,154) : error JS1251: This method has the same name and parameter types as another method in this class" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
interface InterfaceA69 
{
   public function Bar (x: int): int;
}

interface InterfaceB69 
{
   public function Bar (x: int): int;
}

interface InterfaceC69 
{
   public function Bar (x: int): int;
}

class Alpha69 
{
   public function Foo (x: int): int 
   {
      var beta69: Beta69 = new Beta69();
      var interfaceA69: InterfaceA69 = InterfaceA69 (beta69);
      var interfaceB69: InterfaceB69 = InterfaceB69 (beta69);
      var interfaceC69: InterfaceC69 = InterfaceC69 (beta69);
      var temp = 0;
      
      temp = interfaceA69.Bar(x) + interfaceB69.Bar(x) - interfaceC69.Bar(x);
      return temp;
   }
}

class Beta69 implements InterfaceA69, InterfaceB69, InterfaceC69 
{
   public function InterfaceA69.Bar (x: int): int 
   {
      return x;
   }
   
   public function Bar (x: int): int 
   {
      return x + 5;
   }
}


// -----------------------------------------------------------------------
var expectedError70, actualError70: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface Alpha70 { }   " +
         "  class Alpha70 { }       ";

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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError70 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError70 = "test.js(1,35) : error JS1111: 'Alpha70' is already defined" + Environment.NewLine + "";
@else
expectedError70 = "test.js(1,35) : error JS1111: 'Alpha70' is already defined" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError71, actualError71: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA71                 " +
         "  {                                      " +
         "     public function Bar();              " +
         "  }                                      " +
         "  class Alpha71 implements InterfaceA71  " +
         "  {                                      " +
         "     public function Bar(): int { }      " +
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError71 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError71 = "test.js(1,252) : error JS1176: Method 'Bar' is already defined in a base class with a different return type" + Environment.NewLine + "" +
                  "test.js(1,167) : error JS1128: No implementation provided for 'InterfaceA71.Bar()'" + Environment.NewLine + "";
@else
expectedError71 = "test.js(1,252) : error JS1176: Method 'Bar' is already defined in a base class with a different return type" + Environment.NewLine + "" +
                  "test.js(1,167) : error JS1128: No implementation provided for 'InterfaceA71.Bar()'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError72, actualError72: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  interface InterfaceA72                 " +
         "  {                                      " +
         "     public function Alpha72();          " +
         "  }                                      " +
         "  class Alpha72 implements InterfaceA72  " +
         "  {                                      " +
         "     public function Alpha72() { }       " +
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
@if(@rotor)
processStartInfo.FileName = "clix";
processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
processStartInfo.RedirectStandardOutput = true;
@else
processStartInfo.FileName = "jsc";
processStartInfo.Arguments = "-nologo test.js";
processStartInfo.RedirectStandardOutput = true;
processStartInfo.UseShellExecute = false;
@end 
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();

actualError72 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError72 = "test.js(1,167) : error JS1128: No implementation provided for 'InterfaceA72.Alpha72()'" + Environment.NewLine + "";
@else
expectedError72 = "test.js(1,167) : error JS1128: No implementation provided for 'InterfaceA72.Alpha72()'" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Charlie74 extends Beta74
{
   public function Bar(): String
   {
      return "one";
   }
}

abstract class Beta74 implements IAlpha74
{
   abstract public function Bar(): String;
}

interface IAlpha74
{
   public function Bar(): String;
}  


// -----------------------------------------------------------------------
function interface02()
{
   apInitTest ("Interface02");


   // -----------------------------------------------------------------------
   apInitScenario ("26. Interface inside a package; Alpha does not implement a method of the package");         

   if (exceptionThrown26 == false)
      apLogFailInfo ("No compile error in 26.", "Should give a compile error", exceptionThrown26, "");
   if (actualError26 != expectedError26)
      apLogFailInfo ("Wrong compile error in 26.", expectedError26, actualError26, "");

   // -----------------------------------------------------------------------
   apInitScenario ("27. A nested class implements an interface");         

   var alpha27: Alpha27 = new Alpha27();
   var beta27: Alpha27.Beta27 = new alpha27.Beta27();
   
   expected = "Alpha27.Beta27.Bar()";
   actual = "";
   actual = beta27.Bar();
   if (actual != expected) 
      apLogFailInfo ("Error in 27(1)", expected, actual, "");
      
   var charlie27: Charlie27 = new Charlie27();
   expected = "Alpha27.Beta27.Bar()!";   
   actual = "";
   actual = charlie27.Foo (beta27);
   if (actual != expected) 
      apLogFailInfo ("Error in 27(2)", expected, actual, "");
      
   expected = "Alpha27.Beta27.Bar()?";   
   actual = "";
   actual = Zoo27 (beta27);
   if (actual != expected) 
      apLogFailInfo ("Error in 27(3)", expected, actual, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("28. A static nested class implements an interface");         
      
   var beta28: Alpha28.Beta28 = new Alpha28.Beta28();
   
   expected = "Alpha28.Beta28.Bar()";
   actual = "";
   actual = beta28.Bar();
   if (actual != expected) 
      apLogFailInfo ("Error in 28.", expected, actual, "");   
      
   var charlie28: Charlie28 = new Charlie28();
   expected = "Alpha28.Beta28.Bar()!";   
   actual = "";
   actual = charlie28.Foo (beta28);
   if (actual != expected) 
      apLogFailInfo ("Error in 28(2)", expected, actual, "");
      
   expected = "Alpha28.Beta28.Bar()?";   
   actual = "";
   actual = Zoo28 (beta28);
   if (actual != expected) 
      apLogFailInfo ("Error in 28(3)", expected, actual, "");   
      

   // -----------------------------------------------------------------------
   apInitScenario ("29. An outer class implements an interface");         
      
   var alpha29: Alpha29 = new Alpha29();
   
   expected = "Alpha29.Bar()";
   actual = "";
   actual = alpha29.Bar();   
   if (actual != expected)
      apLogFailInfo ("Error in 29(1)", expected, actual, "");

   var charlie29: Charlie29 = new Charlie29();
   expected = "Alpha29.Bar()!";
   actual = "";
   actual = charlie29.Foo (alpha29);
   if (actual != expected)
      apLogFailInfo ("Error in 29(2)", expected, actual, "");

   expected = "Alpha29.Bar()?";
   actual = "";
   actual = Zoo29 (alpha29);
   if (actual != expected)
      apLogFailInfo ("Error in 29(3)", expected, actual, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("30. Both outer and nested class implement the same interface; no implemetation provided");         
                       
   if (exceptionThrown30 == false)
      apLogFailInfo ("No compile error in 30.", "Should give a compile error", exceptionThrown30, "");
   if (actualError30 != expectedError30)
      apLogFailInfo ("Wrong compile error in 30.", expectedError30, actualError30, "");
                       
      
   // -----------------------------------------------------------------------
   apInitScenario ("31. Both outer and nested class implement the same interface; implementation provided");    
                       
   var alpha31: Alpha31 = new Alpha31();
   var beta31: Alpha31.Beta31 = new alpha31.Beta31();
   
   expected = "Alpha31.Bar()";
   actual = "";
   actual = alpha31.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 31(1)", expected, actual, "");
      
   expected = "Alpha31.Beta31.Bar()";
   actual = "";
   actual = beta31.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 31(2)", expected, actual, "");
                               
   var charlie31: Charlie31 = new Charlie31();
   
   expected = "Alpha31.Bar()!";
   actual = "";
   actual = charlie31.Foo (alpha31);
   if (actual != expected)
      apLogFailInfo ("Error in 31(3)", expected, actual, "");
      
   expected = "Alpha31.Beta31.Bar()!";
   actual = "";
   actual = charlie31.Foo (beta31)
   if (actual != expected)
      apLogFailInfo ("Error in 31(4)", expected, actual, "");

   expected = "Alpha31.Bar()?";
   actual = "";
   actual = Zoo31 (alpha31);
   if (actual != expected)
      apLogFailInfo ("Error in 31(5)", expected, actual, "");
      
   expected = "Alpha31.Beta31.Bar()?";
   actual = "";
   actual = Zoo31 (beta31);
   if (actual != expected)
      apLogFailInfo ("Error in 31(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("32. Alpha implements InterfaceA; instantiate alphaA & alphaB of type Alpha; pass alphaA to a method of alphaB that accepts InterfaceA as a parameter");    

   var alphaA: Alpha32 = new Alpha32();
   var alphaB: Alpha32 = new Alpha32();
   
   expected = "Alpha32.Bar()!";
   actual = "";
   actual = alphaB.Foo (alphaA);
   if (actual != expected)
      apLogFailInfo ("Error in 32.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("33. Private method inside an interface");    
      
   if (exceptionThrown33 == false)
      apLogFailInfo ("No compile error in 33.", "Should give a compile error", exceptionThrown33, "");
   if (actualError33 != expectedError33)
      apLogFailInfo ("Wrong compile error in 33.", expectedError33, actualError33, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("34. Protected method inside an interface");    
                             
   if (exceptionThrown34 == false)
      apLogFailInfo ("No compile error in 34.", "Should give a compile error", exceptionThrown34, "");
   if (actualError34 != expectedError34)
      apLogFailInfo ("Wrong compile error in 34.", expectedError34, actualError34, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35. Protected method inside an interface");    
                             
   if (exceptionThrown35 == false)
      apLogFailInfo ("No compile error in 35.", "Should give a compile error", exceptionThrown35, "");
   if (actualError35 != expectedError35)
      apLogFailInfo ("Wrong compile error in 35.", expectedError35, actualError35, "");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Internal method inside an interface & inside a package");    
      
   if (exceptionThrown36 == false)
      apLogFailInfo ("No compile error in 36.", "Should give a compile error", exceptionThrown36, "");
   if (actualError36 != expectedError36)
      apLogFailInfo ("Wrong compile error in 36.", expectedError36, actualError36, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("37. Declare a constructor in an interface (method has the same name as the interface name)");    
      
   var alpha37: Alpha37 = new Alpha37();
   
   expected = "One";
   actual = "";
   alpha37.InterfaceA37();
   actual = alpha37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37.", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("38. Alpha implements InterfaceA; Alpha is abstract; Beta extends Alpha; Beta does not implement InterfaceA.Bar()");    
      
   if (exceptionThrown38 == false)
      apLogFailInfo ("No compile error in 38.", "Should give a compile error", exceptionThrown38, "");
   if (actualError38 != expectedError38)
      apLogFailInfo ("Wrong compile error in 38.", expectedError38, actualError38, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("39. Alpha implements InterfaceA; Alpha is abstract; Beta extends Alpha; Beta implements InterfaceA.Bar()");
                           
   if (actualError39 != expectedError39)
      apLogFailInfo ("Error in 39", expectedError39, actualError39, "305205, 302200");



   // -----------------------------------------------------------------------
   apInitScenario ("40.1 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "Instantiate Charlie and call Bar();");
                       
   var charlie40_1: Charlie40 = new Charlie40();
   
   expected = "One";
   actual = "";
   actual = charlie40_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 40.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40.2 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "Instantiate Beta and call Bar();");
                        
   var beta40_2: Beta40 = new Beta40();
   
   expected = "hello";
   actual = "";
   actual = beta40_2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 40.2", expected, actual, "");        
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40.3 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "var beta: Beta = new Charlie();");
                        
   var beta40_3: Beta40 = new Charlie40();
   
   expected = "One";
   actual = "";
   actual = beta40_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 40.3", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40.4 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "Instantiate Charlie and pass to function Foo (Alpha)");
                              
   var charlie40_4: Charlie40 = new Charlie40();
   
   expected = "One";
   actual = "";
   actual = FooAlpha40 (charlie40_4);                        
   if (actual != expected)
      apLogFailInfo ("Error in 40.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40.5 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "Instantiate Charlie and pass to function Foo (InterfaceA)");
      
   var charlie40_5: Charlie40 = new Charlie40();
   
   expected = "One";
   actual = "";
   actual = FooInterfaceA40 (charlie40_5);                        
   if (actual != expected)
      apLogFailInfo ("Error in 40.5", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40.6 InterfaceA has a method Bar(); Alpha implements InterfaceA; Alpha is abstract; " +
                        "Alpha declares an abstract method Bar(); Beta extends Alpha; Charlie extends Beta; " +
                        "Charlie implements Bar(); " +
                        "Instantiate Charlie and pass to static method Alpha.Foo (Alpha)");
                        
   var charlie40_6: Charlie40 = new Charlie40();
   
   expected = "One";
   actual = "";
   actual = Alpha40.Foo (charlie40_6);                        
   if (actual != expected)
      apLogFailInfo ("Error in 40.6", expected, actual, "");                          
                         
      
   // -----------------------------------------------------------------------
   apInitScenario ("41. Final method inside an interface");    
   
   if (exceptionThrown41 == false)
      apLogFailInfo ("No compile error in 41.", "Should give a compile error", exceptionThrown41, "");
   if (actualError41 != expectedError41)
      apLogFailInfo ("Wrong compile error in 41.", expectedError41, actualError41, "");


   // -----------------------------------------------------------------------
   apInitScenario ("42. Protected internal method in an interface ");    
      
   if (actualError42 != expectedError42)
      apLogFailInfo ("Error in 42", expectedError42, actualError42, "");


   // -----------------------------------------------------------------------
   apInitScenario ("43. Protected internal method in an interface (inside a package)");    
      
   if (actualError43 != expectedError43)
      apLogFailInfo ("Error in 43", expectedError43, actualError43, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("44. Alpha implements InterfaceA.Bar() and InterfaceB.Foo()");    
      
   var alpha44: Alpha44 = new Alpha44();
   
   expected = "Alpha44.Bar()";
   actual = "";
   actual = alpha44.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 44(1)", expected, actual, "");
      
   expected = "Alpha44.Bar()";
   actual = "";
   actual = Boo44 (alpha44);
   if (actual != expected)
      apLogFailInfo ("Error in 44(2)", expected, actual, "");
      
   expected = "Alpha44.Foo()";
   actual = "";
   actual = alpha44.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 44(3)", expected, actual, "");

   expected = "Alpha44.Foo()";
   actual = "";
   actual = Coo44 (alpha44);
   if (actual != expected)
      apLogFailInfo ("Error in 44(4)", expected, actual, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("45. Alpha implements InterfaceA.Bar, InterfaceB.Bar(), and InterfaceC.Bar(); all share the same implementation");    
      
   var alpha45: Alpha45 = new Alpha45();
   
   expected = "Alpha45.Bar()";
   actual = "";
   actual = alpha45.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 45(1)", expected, actual, "");
      
   expected = "Alpha45.Bar()";
   actual = "";
   actual = Foo45 (alpha45);
   if (actual != expected)
      apLogFailInfo ("Error in 45(2)", expected, actual, "");      
      
   expected = "Alpha45.Bar()";
   actual = "";
   actual = Zoo45 (alpha45);
   if (actual != expected)
      apLogFailInfo ("Error in 45(3)", expected, actual, "");   
      

   // -----------------------------------------------------------------------
   apInitScenario ("46. Alpha implements InterfaceA.Bar, InterfaceB.Bar(), and InterfaceC.Bar(); InterfaceA & InterfaceC get the default implementation; InterfaceB gets its own implementation");
      
   var alpha46: Alpha46;
   var interfaceA46: InterfaceA46;
   var interfaceB46: InterfaceB46;
   var interfaceC46: InterfaceC46;
   
   alpha46 = new Alpha46();
   expected = "Alpha46.Bar()";
   actual = "";
   actual = alpha46.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 46(1)", expected, actual, "304909, 310266");
      
   interfaceA46 = new Alpha46();
   expected = "Alpha46.Bar()";
   actual = "";
   actual = interfaceA46.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 46(2)", expected, actual, "304909, 310266");
      
   interfaceB46 = new Alpha46();
   expected = "InterfaceB46.Bar()";
   actual = "";
   actual = interfaceB46.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 46(3)", expected, actual, "302430, 304909, 310266");

   interfaceC46 = new Alpha46();
   expected = "Alpha46.Bar()";
   actual = "";
   actual = interfaceC46.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 46(2)", expected, actual, "304909, 310266");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("47. Alpha implements InterfaceA and InterfaceB but provides implementation for InterfaceB only");    

   if (actualError47 != expectedError47)
      apLogFailInfo ("Error in 47", expectedError47, actualError47, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("48. Alpha implements InterfaceA.Bar() and InterfaceB.Bar(); no implementation for Alpha.Bar()");    
      
   if (actualError48 != expectedError48)
      apLogFailInfo ("Error in 48", expectedError48, actualError48, "");
   
     
   // -----------------------------------------------------------------------
   apInitScenario ("49. Circular inheritance of interfaces");    
   
   if (actualError49 != expectedError49)
      apLogFailInfo ("Error in 49", expectedError49, actualError49, "302422");      
      

   // -----------------------------------------------------------------------
   apInitScenario ("50. InterfaceB uses InterfaceA as a qualified name; InterfaceA does not have Bar()");
   
   if (actualError50 != expectedError50)
      apLogFailInfo ("Error in 50", expectedError50, actualError50, "302560, 305028");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51. InterfaceB uses InterfaceA as a qualified name; InterfaceA has Bar()");
      
   if (actualError51 != expectedError51)
      apLogFailInfo ("Error in 51", expectedError51, actualError51, "302560, 305028");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("52. Interface extends a non-existing Interface");
      
   if (actualError52 != expectedError52)
      apLogFailInfo ("Error in 52", expectedError52, actualError52, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("53. Interface extends a class");
      
   if (actualError53 != expectedError53)
      apLogFailInfo ("Error in 53", expectedError53, actualError53, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("54. A class extends an interface");
   
   if (actualError54 != expectedError54)
      apLogFailInfo ("Error in 54", expectedError54, actualError54, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55. class Alpha implements IntefaceA; Alpha implements InterfaceA.Bar(); but InterfaceA does not have Bar()");      
      
   if (actualError55 != expectedError55)
      apLogFailInfo ("Error in 55", expectedError55, actualError55, "304250, 303811");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56. class Alpha does not implement InterfaceA; Alpha implements InterfaceA.Bar(); but InterfaceA does not have Bar()");      
      
   if (actualError56 != expectedError56)
      apLogFailInfo ("Error in 56", expectedError56, actualError56, "304250, 303811");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("58. Alpha implements InterfaceA; Alpha is abstract; " +
                       "Alpha declares abstract InterfaceA.Bar() and abstract Bar(); " +
                       "Beta implements Bar()");
 
   if (actualError58 != expectedError58)
      apLogFailInfo ("Error in 58", expectedError58, actualError58, "302738");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("59. Alpha implements InterfaceA; Alpha is abstract; Alpha declares abstract Bar(); " +
                       "Beta implements Bar() and InterfaceA.Bar()");

   if (actualError59 != expectedError59)
      apLogFailInfo ("Error in 59", expectedError59, actualError59, "307401");

   // -----------------------------------------------------------------------
   apInitScenario ("60. Private interface method");
      
   if (actualError60 != expectedError60)
      apLogFailInfo ("Error in 60", expectedError60, actualError60, "");

   // -----------------------------------------------------------------------
   apInitScenario ("61. Protected interface method");
      
   if (actualError61 != expectedError61)
      apLogFailInfo ("Error in 61", expectedError61, actualError61, "");
 
   // -----------------------------------------------------------------------
   apInitScenario ("62. Internal interface method");
      
   if (actualError62 != expectedError62)
      apLogFailInfo ("Error in 62", expectedError62, actualError62, "");
      
                       
   // -----------------------------------------------------------------------
   apInitScenario ("63. Internal protected interface method");
      
   if (actualError63 != expectedError63)
      apLogFailInfo ("Error in 63", expectedError63, actualError63, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("64. Static interface method");
      
   if (actualError64 != expectedError64)
      apLogFailInfo ("Error in 64", expectedError64, actualError64, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("65. Interface has overloaded methods; Alpha doesn't implement any method");
      
   if (actualError65 != expectedError65)
      apLogFailInfo ("Error in 65", expectedError65, actualError65, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("66. Interface has overloaded methods; Alpha doesn't implement any method");
      
   if (actualError66 != expectedError66)
      apLogFailInfo ("Error in 66", expectedError66, actualError66, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("67. Interface had overloaded methods; Alpha implements all methods");
      
   var alpha67: Alpha67 = new Alpha67();
   
   actual = 0;
   expected = 10;
   actual = alpha67.Bar(1);
   if (actual != expected)
      apLogFailInfo ("Error in 67(1)", expected, actual, "");
      
   actual = 0;
   expected = 20;
   actual = alpha67.Bar("hello");
   if (actual != expected) 
      apLogFailInfo ("Error in 67(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("68. Interface has overloaded methods that differ only in return type");
      
   if (actualError68 != expectedError68)
      apLogFailInfo ("Error in 68", expectedError68, actualError68, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("69. Cast on object to an interface");
      
   var alpha69: Alpha69 = new Alpha69();
   
   expected = 5;
   actual = 0;
   actual = alpha69.Foo(5);
   if (actual != expected) 
      apLogFailInfo ("Error in 69.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("70. Class has the same name as an interface.");
      
   if (actualError70 != expectedError70)
      apLogFailInfo ("Error in 70", expectedError70, actualError70, "215760");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("71. A class implements an interface but has a different signature for the method");
   
   if (actualError71 != expectedError71)
      apLogFailInfo ("Error in 71", expectedError71, actualError71, "215759");  
      

   // -----------------------------------------------------------------------
   apInitScenario ("72. Class implements an interface and has the same constructor name as the method");
        
   if (actualError72 != expectedError72)
      apLogFailInfo ("Error in 72", expectedError72, actualError72, "215759");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("74. A class implements an interface using forward referencing");
      
   var charlie74: Charlie74 = new Charlie74();
   
   expected = "one";
   actual = "";
   actual = charlie74.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 74.", expected, actual, "");
      
      
      
      
   apEndTest();
}



interface02();


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
