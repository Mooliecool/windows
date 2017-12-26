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


var iTestID = 106468;



// -----------------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError: String = "";
var actualError: String = "";


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


// -----------------------------------------------------------------------------
var exceptionThrown1 = false;
var actualError1 = "";
var expectedError1 = "SyntaxError: Circular definition";

try
{
   eval ("  class x extends z { }   " +
         "  class y extends x { }   " +
         "  class z extends y { }   " );
}
catch (error)
{
   exceptionThrown1 = true;
   actualError1 = error;   
} 


// -----------------------------------------------------------------------------
class Alpha2
{
   public function GetName() 
   {
      return "Alpha2";
   }
   
   private var e;
}

class Beta2 extends Alpha2
{
   public function GetName() 
   {
      return "Beta2";
   }
   
   public var e;
}


// -----------------------------------------------------------------------------
package Alpha3
{
   public class Apple
   {
   }
}

package Beta3
{
   internal class Banana
   {
   }
}


// -----------------------------------------------------------------------------
class Alpha4
{
}


// -----------------------------------------------------------------------------
class Alpha5
{
   public function GetName()
   {
      return "Alpha5";
   }
}

class Beta5
{
   public function GetName()
   {
      return "Beta5";
   }
}

function Bar5 (alpha5: Alpha5): String
{
   return (alpha5.GetName());
}


// -----------------------------------------------------------------------------
class Beta6 extends Alpha6
{
   public function Bar()
   {
      var alpha6: Alpha6 = new Alpha6();
   }
}

class Alpha6
{
   public function Foo()
   {
      var beta6: Beta6 = new Beta6();
   }
}


// -----------------------------------------------------------------------------
expando class Funky
{
}


// -----------------------------------------------------------------------------
package Group8_3
{
}


// -----------------------------------------------------------------------------
var expectedError9, actualError9: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha9               " +
         "  {                          " +
         "     function Bar (value)    " +
         "     {                       " +
         "        return value;        " +
         "     }                       " +
         "  }                          " +
         "  function Bar (value)       " +
         "  {                          " +
         "     return value;           " +
         "  }                          " +
         "  var alpha9: Alpha9 = new Alpha9();  " +
         "  var actual;                " +
         "  actual = alpha9.Bar();     " +
         "  actual = Bar();            ";
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

actualError9 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError9 = "test.js(1,398) : warning JS1204: Not all required arguments have been supplied" + Environment.NewLine + "" +
                 "test.js(1,427) : warning JS1204: Not all required arguments have been supplied" + Environment.NewLine + "";
@else
expectedError9 = "test.js(1,398) : warning JS1204: Not all required arguments have been supplied" + Environment.NewLine + "" +
                 "test.js(1,427) : warning JS1204: Not all required arguments have been supplied" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
public class Alpha10
{
   public var value = 0;
   public static var staticField = 1;

   public function Alpha10 (blah) 
   { 
      value = blah;
   }
}

public class Beta10 extends Alpha10
{
   public function Beta10()
   {
      super (Alpha10.staticField);
   }
}


// -----------------------------------------------------------------------------
class Alpha11
{
   public var value: String = "";
	public function Alpha11 (x) 
	{ 
	   value = x;
	}
}

class Beta11 extends Alpha11
{
	public function Beta11()
	{
		super ("eleven");
	}
}


// -----------------------------------------------------------------------------
var exceptionThrown12 = false;
var actualError12 = "";
var expectedError12 = "SyntaxError: Expected ';'";

try
{
   eval ("  expando class Form1                                         " +
         "  {                                                           " +
         "     private function InitializeComponent()                   " +
         "     {                                                        " +
         "        this.Button1 = new System.Windows.Forms.Button()      " +
         "        this.Controls.AddRange(new System.Windows.Forms.Control() {this.Button1})  " +
         "        this.ResuthisLayout(False)                            " +
         "     }                                                        " +
         "  }                                                           ", "unsafe");
}
catch (error)
{
   exceptionThrown12 = true;
   actualError12 = error;
}         


// -----------------------------------------------------------------------------
class Alpha13
{
   protected var x: boolean;
   
   function get name() : boolean
   {
      return x;
   }
   
   function set name (f:boolean)
   {
      x=f;
   }
}

class Beta13 extends Alpha13
{
   override function get name() : boolean
   {
      return x;
   }
}


// -----------------------------------------------------------------------------
var expectedError14, actualError14: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  package GroupA14              " +
         "  {                             " +
         "     internal class Alpha14 {}  " +
         "  }                             " +
         "  var alpha14: GroupA14.Alpha14() = new GroupA14.Alpha14();   ";
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

actualError14 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError14 = "test.js(1,160) : error JS1004: Expected ';'" + Environment.NewLine + "" +    
                  "test.js(1,144) : error JS1123: 'GroupA14.Alpha14' is not accessible from this scope" + Environment.NewLine + "";
@else
expectedError14 = "test.js(1,160) : error JS1004: Expected ';'" + Environment.NewLine + "" +    
                  "test.js(1,144) : error JS1123: 'GroupA14.Alpha14' is not accessible from this scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
package GroupA15 
{
    public interface InterfaceA15 {
        function Bar (a : int) : int;
    }

    public interface InterfaceB15 {
        function Bar (a : int) : int;
    }
    
    public class Alpha15 {
        public static function Foo (i : int) : int {
            var beta15 : Beta15 = new Beta15();
            var interfaceA15 : InterfaceA15 = InterfaceA15 (beta15);
            var interfaceB15 : InterfaceB15 = InterfaceB15 (beta15);
            return (interfaceA15.Bar(i) - interfaceB15.Bar(i));
        }
    }
    
    public class Beta15 extends Object implements InterfaceA15, InterfaceB15 {
        public function InterfaceA15.Bar (a : int) : int {
            return a;
        }
        
        public function InterfaceB15.Bar (a : int) : int {
            return (5 * a);
        }
    }
}


// -----------------------------------------------------------------------------
class Alpha16
{
   protected var value: int;
   
   public function get Price(): int
   {
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta16 extends Alpha16
{
   public function get Price(): int
   {
      return value + 5;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice + 5;
   }
}


// -----------------------------------------------------------------------------
import System.Collections;

class Alpha17 extends Queue
{
	public function Bar(): int
	{
	   var I: int;
		var queue: Queue = new Queue();
		var temp: int = 0;
		
		for (I = 1; I <= 10; I++)
		   queue.Enqueue(I);
		while (queue.Count != 0)
		   temp = queue.Dequeue();
      return temp;	
	}
}


// -----------------------------------------------------------------------------
var expectedError18, actualError18: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha18 { /* ... */ };                     " +
         "  function Alpha18.prototype.Bar() { /* ... */ };  " +
         "  var alpha18 = new Alpha18();                     " +
         "  Alpha18.Bar();                                   ";
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


actualError18 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError18 = "test.js(1,71) : error JS1246: Type 'Alpha18' does not have such a static member" + Environment.NewLine + "" +
                  "test.js(1,164) : error JS1246: Type 'Alpha18' does not have such a static member" + Environment.NewLine + "";
@else
expectedError18 = "test.js(1,71) : error JS1246: Type 'Alpha18' does not have such a static member" + Environment.NewLine + "" +
                  "test.js(1,164) : error JS1246: Type 'Alpha18' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
var expectedError19, actualError19: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha19 { /* ... */ };                       " +
         "  Alpha19.prototype.Bar = function() { /* ... */ };  " +
         "  var alpha19 = new Alpha19();                       " +
         "  alpha19.Bar();                                     ";
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

actualError19 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError19 = "test.js(1,64) : error JS1246: Type 'Alpha19' does not have such a static member" + Environment.NewLine + "";
@else
expectedError19 = "test.js(1,64) : error JS1246: Type 'Alpha19' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
class Alpha20
{
}


// -----------------------------------------------------------------------------
var expectedError21, actualError21: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  public class Outer21       " +
         "  {                          " +
         "     public class Inner21 { }" +
         "  }                          " +
         "  var inner21: Outer21.Inner21 = new Outer21.Inner21(); ";
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

actualError21 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError21 = "test.js(1,162) : error JS1245: The non-static member 'Inner21' cannot be accessed with the class name" + Environment.NewLine + "";
@else
expectedError21 = "test.js(1,162) : error JS1245: The non-static member 'Inner21' cannot be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
var expectedError22, actualError22: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Person                                    " +
         "  {                                               " +
         "     protected var m_sName : String;              " +
         "     public function Person(name : String)        " +
         "     {                                            " +
         "        this.m_sName = name;                      " +
         "     }                                            " +
         "  }                                               " +
         "  class Lecturer extends Person                   " +
         "  {                                               " +
         "     public function GenerateName(sName) : String " +
         "     {                                            " +
         "        return \"Dr. \" + this.sName;             " +
         "     }                                            " +
         "     public function Lecturer(sName : String)     " +
         "     {                                            " +
         "        /* This is illegal since it calls an instance method  " +
         "           to supply a parameter to the constructor  */       " +
         "        super(this.GenerateName(sName));          " +
         "     }                                            " +
         "  }                                               " +
         "  var l : Lecturer = new Lecturer(\"Jeckle\");    ";
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

actualError22 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError22 = "test.js(1,630) : error JS1151: Objects of type 'Lecturer' do not have such a member" + Environment.NewLine + "";
@else
expectedError22 = "test.js(1,630) : error JS1151: Objects of type 'Lecturer' do not have such a member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
class Alpha23
{  
   public var value: String;
   
   public function Alpha23 (x: String)
   {
      value = x;
   }
}

class Beta23 extends Alpha23
{
   public function Beta23 (x: String)
   {
      super (x);
   }
}


// -----------------------------------------------------------------------------
package Group24
{
	public class Alpha24
	{
	   public function Foo(): String
	   {
	      return "hello";
	   }
	}

	public class Beta24
	{
		public static function Bar() : Alpha24
		{
			return new Alpha24();
		}
	}
}


// -----------------------------------------------------------------------------
class Alpha25 
{
	public function Foo()
	{
		return "Alpha25.Foo";
	}

	public function Bar()
	{
		return "Alpha25.Bar";
	}
}

class Beta25 extends Alpha25
{
	public function Foo()
	{
		return "Beta25.Foo";
	}

	public function Bar()
	{
		return "Beta25.Bar";
	}
}


// -----------------------------------------------------------------------------
package Group26
{
   class Alpha26
   {
      var value: int = 5;
   }
}


// -----------------------------------------------------------------------------
var expectedError27, actualError27: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha27           " +
         "  {                       " +
         "     public var x: int;   " +
         "     x = 5;               " +
         "  }                       ";

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

actualError27 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError27 = "test.js(1,84) : error JS1002: Syntax error" + Environment.NewLine + "";
@else
expectedError27 = "test.js(1,84) : error JS1002: Syntax error" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
var expectedError28, actualError28: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  package 1 { }; ";
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

actualError28 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError28 = "test.js(1,11) : error JS1010: Expected identifier" + Environment.NewLine + "";
@else
expectedError28 = "test.js(1,11) : error JS1010: Expected identifier" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
var expectedError29, actualError29: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  var f = false        " +
         "  if (true) ; {};      " +
         "  if (true) ; else ;   " +
         "  if (true) ; else {}  " +
         "  if (true) ; else {}; ";
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

actualError29 = process.StandardOutput.ReadToEnd();
@if(!@rotor)
expectedError29 = "test.js(1,26) : error JS1004: Expected ';'" + Environment.NewLine + "" +
                  "test.js(1,36) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,59) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,66) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,82) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,105) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "";
@else
expectedError29 = "test.js(1,26) : error JS1004: Expected ';'" + Environment.NewLine + "" +
                  "test.js(1,36) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,59) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,66) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,82) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "" +
                  "test.js(1,105) : warning JS1207: Did you intend to have an empty statement for this branch of the if statement?" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------------
import Alpha3;
import Beta3;
import GroupA15;
import Group24;
import Group26;

// -----------------------------------------------------------------------------
function regressiontests()
{
   apInitTest ("Regression tests.");
   
   // -----------------------------------------------------------------------------
   apInitScenario ("1. Circular class inheritance");
   
   if (exceptionThrown1 == false)
      apLogFailInfo ("No compile error in 1", "Should give a compile error", exceptionThrown1, "");
   if (actualError1 != expectedError1)
      apLogFailInfo ("Wrong compile error in 1", expectedError1, actualError1, "");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("2. regression");
   
   var alpha2: Alpha2 = new Alpha2();
   var beta2: Beta2 = new Beta2();
   
   actual = "";
   expected = "Alpha2";
   actual = alpha2.GetName();
   if (actual != expected)
      apLogFailInfo ("Regression (1)", expected, actual, "118700");
      
   actual = "";
   expected = "Beta2";
   actual = beta2.GetName();
   if (actual != expected)
      apLogFailInfo ("Regression (2)", expected, actual, "118700");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("3. checks to see if JScript 7 will throw a fatal error"); 
   // "Fatal errors" can't be caught by try-catch
   
   var apple: Apple = new Apple();

   try
   {
      eval ("var banana: Banana;");
   }
   catch (error)
   {
   }   
   

   // -----------------------------------------------------------------------------
   apInitScenario ("4. throws an exception when declaring an object inside eval in a function");
   
   exceptionThrown = false;
   
   try
   {
      eval ("var alpha4: Alpha4;");
   }
   catch (error)
   {
      exceptionThrown = true;
   }
   
   if (exceptionThrown == true)
      apLogFailInfo ("JS7 throws an exception when declaring an object inside eval in a function","Shouldn't throw an exception", exceptionThrown, "130847");

                     
   // -----------------------------------------------------------------------------
   apInitScenario ("5. does not give a compile error when the function parameter's type is different from that of the actual parameter's");
   
   var alpha5: Alpha5 = new Alpha5();
   var beta5: Beta5 = new Beta5();
   
   expected = "Alpha5";
   actual = "";
   actual = Bar5 (alpha5);
   if (actual != expected)
      apLogFailInfo ("Error (1)", expected, actual, "128840");
      
   exceptionThrown = false;
   actual = "";
   
   try
   {
      eval ("actual = Bar5 (beta5);");
   }
   catch (error)
   {
      exceptionThrown = true;
   }   
   
   if (exceptionThrown == false)
      apLogFailInfo ("compile error when the function parameter's type is different from that of the actual parameter's", "An error should be thrown", exceptionThrown, "128840");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("6. internal compiler error when the superclass is declared below the subclass and the subclass instantiates the superclass in one of its methods");
   
   var alpha6: Alpha6 = new Alpha6();
   var beta6: Beta6 = new Beta6();
   var mixed6: Alpha6 = new Beta6();
   
   alpha6.Foo();
   
   beta6.Foo();
   beta6.Bar();
   
   mixed6.Foo();
   
                        
   // -----------------------------------------------------------------------------
   apInitScenario ("7. function added as expando property to a class");
   
   var funky: Funky = new Funky();
   funky["foo"] = function() : int { return 123; }
   
   expected = "function() : int { return 123; }";
   actual = "";
   actual = funky["foo"];
   if (actual != expected)
      apLogFailInfo ("Error with expando (1)", expected, actual, "190901, 147135");
      
   expected = 123;
   actual = 0;
   actual = eval ("funky[\"foo\"]()");
   if (actual != expected)
      apLogFailInfo ("Error with expando (2)", expected, actual, "190901, 147135");
      
   expected = 123;
   actual = 0;
   actual = funky["foo"]();
   if (actual != expected)
      apLogFailInfo ("Error with expando (3)", expected, actual, "190901, 147135");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("8.1 Restrict eval so that it can only do what its calling context can do -- defining classes");
                   
   exceptionThrown = false;
   actualError = "";
   expectedError = "SyntaxError: Class definition not allowed in this context";

   try
   {
      eval (" class Alpha { } ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }         
   
   if (exceptionThrown == false)
      apLogFailInfo ("Defining a class inside eval in a function does not cause any errors","Should give an error", exceptionThrown, "152136");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "152136");
   
      
   // -----------------------------------------------------------------------------
   apInitScenario ("8.2 Restrict eval so that it can only do what its calling context can do -- defining packages");
                   
   exceptionThrown = false;
   actualError = "";
   expectedError = "SyntaxError: Package declaration not allowed in this context";
   
   try
   {
      eval (" package Alpha { } ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("Defining a package inside eval in a function does not cause any errors","Should give an error", exceptionThrown, "152136");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "172884, 152136");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("8.3 Restrict eval so that it can only do what its calling context can do -- using import");
                   
   exceptionThrown = false;
   actualError = "";
   expectedError = "Error: The import statement is not valid in this context";
   
   try
   {
      eval (" import Group8_3; ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("Importing a package inside eval in a function does not cause any errors","Should give an error", exceptionThrown, "152136");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "172884, 152136");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("9. call global methods with missing trailing arguments");
   
   if (actualError9 != expectedError9)
      apLogFailInfo ("Error in 9", expectedError9, actualError9, "152290");      
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("10. nested peer classes don't see each other's statics");
      
   var beta10: Beta10 = new Beta10();
   
   expected = 1;
   actual = 0;
   actual = beta10.value;
   if (actual != expected)
      apLogFailInfo ("Error in 10", expected, actual, "169814");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("11. 'super' constructor call doesn't pass arguments");

   var beta11 = new Beta11;
   
   expected = "eleven";
   actual = "";
   actual = beta11.value;
   if (actual != expected)
      apLogFailInfo ("Error in 11", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("12. internal compiler error compiling bogus code");
   
   if (exceptionThrown12 == false)
      apLogFailInfo ("No compile error in 12", "Should give a compile error", exceptionThrown12, "");
   if (actualError12 != expectedError12)
      apLogFailInfo ("Wrong compile error in 12", expectedError12, actualError12, "");   
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("13. In fast mode eval does not see property setter in base class when only the property getter is overriden in the derived class");
      
   var beta13: Beta13 = new Beta13();
   
   expected = false;
   actual = "";
   actual = beta13.name;
   if (actual != expected)
      apLogFailInfo ("Error in 13(1)", expected, actual, "");
      
   eval ("beta13.name = true");
   
   expected = true;
   actual = "";
   actual = beta13.name;
   if (actual != expected)
      apLogFailInfo ("Error in 13(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("14. ICE when trying to acces internal class inside of package with type annotation.");

   if (actualError14 != expectedError14)
      apLogFailInfo ("Error in 14", expectedError14, actualError14, "258817");      
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("15. Invalid cast exception when testing if an object can be cast to an inherited interface.");
      
   expected = -40
   actual = 0;
   actual = Alpha15.Foo(10);
   if (actual != expected)
      apLogFailInfo ("Erro rin 15.", expected, actual, "252675");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("16. ICE when trying to override property in derived class");
   
   var alpha16: Alpha16;
   var beta16: Beta16;
   
   alpha16 = new Alpha16();
   actual = 0;
   expected = 5;
   alpha16.Price = 5;
   actual = alpha16.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 16(1)", expected, actual, "148075");
      
   alpha16 = new Beta16();
   actual = 0;
   expected = 15;
   alpha16.Price = 5;
   actual = alpha16.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 16(2)", expected, actual, "148075");
      
   beta16 = new Beta16();
   actual = 0;
   expected = 20;
   beta16.Price = 10;
   actual = beta16.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 16(3)", expected, actual, "148075");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("17. Can't create an instance of a class inside a class that extends from that class");
      
   var alpha17: Alpha17 = new Alpha17();
   
   expected = 10;
   actual = 0;
   actual = alpha17.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 17.", expected, actual, "125744");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("18. DCR: Enable <class>.prototype to point to <class>");
   
   if (actualError18 != expectedError18)
      apLogFailInfo ("Error in 18", expectedError18, actualError18, "115991");      
   
   
   // -----------------------------------------------------------------------------
   apInitScenario ("19. DCR: Enable <class>.prototype to point to <class>");
   
   if (actualError19 != expectedError19)
      apLogFailInfo ("Error in 19", expectedError19, actualError19, "115991");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("20. <class>.GetType() gives an 'Invalid procedure call or argument' error");
        
   expected = "System.RuntimeType";
   actual = "";
   actual = Alpha20.GetType();
   if (actual != expected)
      apLogFailInfo ("Error in 20(1)", expected, actual, "111164");

   expected = "System.RuntimeType";
   actual = "";
   actual = Alpha20.GetType().GetType();
   if (actual != expected)
      apLogFailInfo ("Error in 20(2)", expected, actual, "111164");
   
   
   // -----------------------------------------------------------------------------
   apInitScenario ("21. Nested classes cannot be constructed");
   
   if (actualError21 != expectedError21)
      apLogFailInfo ("Error in 21", expectedError21, actualError21, "109732");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("22. Calling super with an instance member doesn't give a compiler error");
      
   if (actualError22 != expectedError22)
      apLogFailInfo ("Error in 22", expectedError22, actualError22, "101461");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("23. Calling base class constructor with super cannot use passed-in parameters");
   
   var beta23: Beta23 = new Beta23 ("hello");
   
   expected = "hello";
   actual = "";
   actual = beta23.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.", expected, actual, "101456");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("24. public static function returning a class gives fatal error");
      
   var alpha24 = Group24.Beta24.Bar();
   
   expected = "hello";
   actual = "";
   actual = alpha24.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 24.", expected, actual, "101365");
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("25. Fatal error when class overrides two base class methods");
      
   var alpha25: Alpha25;
   var beta25: Beta25;
   
   // ------------------
   alpha25 = new Alpha25();
   
   expected = "Alpha25.Foo";
   actual = "";
   actual = alpha25.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 25(1)", expected, actual, "92477");   
      
   expected = "Alpha25.Bar";
   actual = "";
   actual = alpha25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25(2)", expected, actual, "92477");

   // ------------------
   alpha25 = new Beta25();
   
   expected = "Beta25.Foo";
   actual = "";
   actual = alpha25.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 25(3)", expected, actual, "92477");   
      
   expected = "Beta25.Bar";
   actual = "";
   actual = alpha25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25(4)", expected, actual, "92477");   
   
   // ------------------
   beta25 = new Beta25();
   
   expected = "Beta25.Foo";
   actual = "";
   actual = beta25.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 25(5)", expected, actual, "92477");   
      
   expected = "Beta25.Bar";
   actual = "";
   actual = beta25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25(6)", expected, actual, "92477"); 
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("26. using a class initiation inside a function scope without assigning it cause a crash in fast mode");
   
   expected = 5;
   actual = 0;
   actual = new Alpha26().value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.", expected, actual, "89836");  
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("27. initializing variables to a value inside a class results in Null Reference Exceptions");
          
   if (actualError27 != expectedError27)
      apLogFailInfo ("Error in 27", expectedError27, actualError27, "89202");      
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("28. Creating a package with an invalid name causes a null reference exception");
          
   if (actualError28 != expectedError28)
      apLogFailInfo ("Error in 28", expectedError28, actualError28, "88436");      
      
      
   // -----------------------------------------------------------------------------
   apInitScenario ("29. IF with empty terminated statements return a runtime error");
          
   if (actualError29 != expectedError29)
      apLogFailInfo ("Error in 29", expectedError29, actualError29, "33128"); 
      
      
   
   apEndTest();      
}



regressiontests();


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
