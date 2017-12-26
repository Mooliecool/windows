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


var iTestID = 175013;



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
class Alpha8
{
   public function Bar (x: int)
   {  
      return "One";  
   }
   
   public function Bar ()
   {  
      return "Two";  
   }
   
   public function Bar (x: String)
   {  
      return "Three";   
   }

   public function Bar (x: boolean)
   {  
      return "Four";   
   }

   public function Bar (x: boolean, y: int)
   {
      return "Five";
   }
   
   public function Bar (x: boolean, y: short)
   {
      return "Six";
   }
   
   public function Bar (x: boolean, y: int, z: String)
   {
      return "Seven";
   }
   
   public function Bar (x: boolean, y: int, z: boolean)
   {
      return "Eight";
   }
}


// -----------------------------------------------------------------------
class Alpha9
{
   public function Bar (x: int, y: boolean, z: String)
   {
      return "One";
   }
   
   public function Bar (x: boolean, y: String, z: int)
   {
      return "Two";
   }
}


// -----------------------------------------------------------------------
class Alpha10
{
   public function Bar()
   {
      return "Alpha10.Bar()";
   }
   
   public function Foo (x: Alpha10)
   {
      return x.Bar();
   }
}

class Beta10 extends Alpha10
{
   public function Bar()
   {
      return "Beta10.Bar()";
   }
}

class Charlie10 extends Beta10
{
   public function Bar()
   {
      return "Charlie10.Bar()";
   }
}

function Foo (x: Alpha10)
{
   return x.Bar();
}


// -----------------------------------------------------------------------
final class Alpha11
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown12 = false;
var actualError12 = "";
var expectedError12 = "ReferenceError: Type: 'Alpha12' cannot be extended";

try
{
   eval ("  final class Alpha12 { }         " +
         "  class Beta extends Alpha12 { }  ");
}
catch (error)
{
   exceptionThrown12 = true;
   actualError12 = error;
}


// -----------------------------------------------------------------------
class Alpha13
{
   public var value: String = "none";
   public function Bar()
   {
      value = "three";
   }
}

final class Beta13 extends Alpha13
{
}


// -----------------------------------------------------------------------
class Alpha14
{
   public var value: String = "none";
   public function Bar()
   {
      value = "four";
   }
}

class Beta14 extends Alpha14
{
}

final class Charlie14 extends Beta14
{
}


// -----------------------------------------------------------------------
class Alpha15
{
   final public function Bar()
   {
      return "five"
   }
}

class Beta15 extends Alpha15
{
   public function Bar()
   {
      return "six";
   }
}

class Charlie15 extends Beta15
{
   public function Bar()
   {
      return "seven";
   }
}

function Foo15 (x: Alpha15)
{
   return x.Bar();
}

class Blah15
{
   public function Foo (x: Alpha15)
   {
      return x.Bar();
   }
}


// -----------------------------------------------------------------------
class Alpha16
{
   private function Zoo()
   {
      return "one"
   }
   
   public function Bar()
   {
      return Zoo();
   }
}

class Beta16 extends Alpha16
{
   private function Zoo()
   {
      return "two";
   }
}

function Foo16 (x: Alpha16)
{
   return x.Bar();
}

class Blah16
{
   public function Foo (x: Alpha16)
   {
      return x.Bar();
   }
}


// -----------------------------------------------------------------------
class Alpha17_1
{
   static public function Bar()
   {
      return "ten";
   }
}

class Beta17_1 extends Alpha17_1
{
   public function Bar()
   {
      return "eleven";
   }
}


// -----------------------------------------------------------------------
var expectedError17_2, actualError17_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha17_2                              " +
         "  {                                            " +
         "     static public function Bar()              " +
         "     {                                         " +
         "        return \"ten\";                        " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta17_2 extends Alpha17_2             " +
         "  {                                            " +
         "     public function Bar()                     " +
         "     {                                         " +
         "        return \"eleven\";                     " +
         "     }                                         " +
         "  }                                            " +
         "  var alpha17_2: Alpha17_2 = new Alpha17_2();  " +
         "  alpha17_2.Bar();                             ";


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
actualError17_2 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError17_2 = "test.js(1,714) : error JS1244: The static member 'Bar' must be accessed with the class name" + Environment.NewLine + "";
@else
expectedError17_2 = "test.js(1,714) : error JS1244: The static member 'Bar' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError17_3, actualError17_3: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha17_3                              " +
         "  {                                            " +
         "     static public function Bar()              " +
         "     {                                         " +
         "        return \"ten\";                        " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta17_3 extends Alpha17_3             " +
         "  {                                            " +
         "     public function Bar()                     " +
         "     {                                         " +
         "        return \"eleven\";                     " +
         "     }                                         " +
         "  }                                            " +
         "  var x: String = \"\";                        " +
         "  x = Beta17_3.Bar();                          ";


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
actualError17_3 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError17_3 = "test.js(1,715) : error JS1245: The non-static member 'Bar' cannot be accessed with the class name" + Environment.NewLine + "";
@else
expectedError17_3 = "test.js(1,715) : error JS1245: The non-static member 'Bar' cannot be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha17_4
{
   static public function Bar()
   {
      return "ten";
   }
}

class Beta17_4 extends Alpha17_4
{
   public function Bar()
   {
      return "eleven";
   }
}


// -----------------------------------------------------------------------
class Alpha18
{
   public function Bar()
   {
      return "one";
   }
}

class Beta18 extends Alpha18
{
   public function Bar()
   {
      return "two";
   }
}

class Charlie18 extends Beta18
{
   public function Bar()
   {
      return "three";
   }
}

function Foo18 (x: Alpha18)
{
   return x.Bar();
}

class Blah18
{
   public function Foo (x: Alpha18)
   {
      return x.Bar();
   }
}


// -----------------------------------------------------------------------
expando class Alpha19
{
}


// -----------------------------------------------------------------------
expando class Alpha20
{
   public var value = 20;
}


// -----------------------------------------------------------------------
class Alpha21
{
   static public var value = "One";

   static public function Bar()
   {
      return "Two";
   }

   static protected var x = "Three";
   
   public function Foo()
   {
      var temp;
      temp = x;
      return temp;
   }
   
   static public function Zoo()
   {
      var temp;
      temp = x;
      return temp;
   }
}

class Beta21 extends Alpha21
{
}

class Charlie21 extends Beta21
{
   public function Foo()
   {
      var temp;
      temp = x;
      temp = temp + "xyz";
      return temp;
   }
   
   static public function Zoo()
   {
      var temp;
      temp = x + "123";
      return temp;   
   }
}


// -----------------------------------------------------------------------
var expectedError21_1, actualError21_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha21_1                        " +
         "  {                                      " +
         "     static public var value = \"One\";  " +
         "     static public function Bar()        " +
         "     {                                   " +
         "        return \"Two\";                  " +
         "     }                                   " +
         "  }                                      " +
         "  class Beta21_1 extends Alpha21_1       " +
         "  {  }                                   " +
         "  class Charlie21_1 extends Beta21_1     " +
         "  {  }                                   " +
         "  var x = Charlie21_1.value;             " +
         "  var y = Charlie21_1.Bar();             ";


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
actualError21_1 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError21_1 = "test.js(1,511) : error JS1246: Type 'Charlie21_1' does not have such a static member" + Environment.NewLine + "" +
                    "test.js(1,552) : error JS1246: Type 'Charlie21_1' does not have such a static member" + Environment.NewLine + "";
@else
expectedError21_1 = "test.js(1,511) : error JS1246: Type 'Charlie21_1' does not have such a static member" + Environment.NewLine + "" +
                    "test.js(1,552) : error JS1246: Type 'Charlie21_1' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class A22
{
   class Alpha22
   {
      public var value = "One";
      
      public function Bar()
      {
         return "Two";
      }
      
      public function Foo()
      {
         return "Three";
      }
   }
   
   class Beta22 extends Alpha22
   {
      public function Foo()
      {
         return "Four";
      }
   }
}


// -----------------------------------------------------------------------
class A23
{
   static class Alpha23
   {
      public var value = "One";
      
      public function Bar()
      {
         return "Two";
      }
      
      public function Foo()
      {
         return "Three";
      }
   }
   
   static class Beta23 extends Alpha23
   {
      public function Foo()
      {
         return "Four";
      }
   }
}


// -----------------------------------------------------------------------
interface A25
{
   public function Bar(): String;
}

class Alpha25 implements A25
{
   public function Bar(): String
   {
      return "Alpha25.Bar()";
   }
}

class Beta25 implements A25
{
   public function Bar(): String
   {
      return "Beta25.Bar()";
   }
}


// -----------------------------------------------------------------------
interface A26
{
   public function Bar(): String;
}

class Alpha26 implements A26
{
   public function Bar()
   {
      return ("Alpha26.Bar()");
   }
}

class Beta26 extends Alpha26
{
}


// -----------------------------------------------------------------------
interface A27
{
   public function Bar(): String;
}

interface B27
{
   public function Foo(): String;
}

class Alpha27 implements A27, B27
{
   public function Bar(): String
   {
      return "Alpha27.Bar()";
   }
   
   public function Foo(): String
   {
      return "Alpha27.Foo()";
   }
}

class Beta27 extends Alpha27
{
   public function Foo(): String
   {
      return "Beta27.Foo()";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown28 = false;
var actualError28 = "";
var expectedError28 = "ReferenceError: 'alpha28.value' is read-only";

try
{
   eval ("  class Alpha28                                " +
         "  {                                            " +
         "     public const value: String = \"One\";     " +
         "  }                                            " +
         "  var alpha28: Alpha28 = new Alpha28();        " +
         "  alpha28.value = \"Two\";                     ");
}
catch (error)
{
   exceptionThrown28 = true;
   actualError28 = error;
}


// -----------------------------------------------------------------------
class Alpha29
{
   public const value1: String = "Hello";
   public const value2: int = 23;
   
   public function Bar(): String
   {
      return value1;
   }
   
   public function Foo(): int
   {
      return value2;
   }
}

class Beta29 extends Alpha29
{
}


// -----------------------------------------------------------------------
class Alpha30
{
   public function Bar1(): Boolean
   {
      return true;
   }
   
   public function Bar2(): boolean
   {
      return false;
   }
   
   public function Bar3(): Number
   {
      return 1;
   }
   
   public function Bar4(): double
   {
      return 2;
   }

   public function Bar5(): String
   {
      return 2;
   }   
   
   public function Bar6(): Object
   {  
      var x = new Object()
      return x;
   }   
   
   public function Bar7(): Date
   {
      var x = new Date();
      return x;
   }   

   public function Bar8(): Function
   {
      var x = new Function();
      return x;
   }   

   public function Bar9(): RegExp
   {
      var x = new RegExp();
      return x;
   }   
   
   public function Bar10(): Error
   {
      var x = new Error();
      return x;
   }   
   
   public function Bar11(): byte
   {
      var x = 1;
      return x;
   }   

   public function Bar12(): char
   {
      var x: char = 'a';
      return x;
   }   
   
   public function Bar13(): short
   {
      var x: short = 1;
      return x;
   }         
   
   public function Bar14(): int
   {
      var x: int = 1;
      return x;
   }  
   
   public function Bar15(): long
   {
      var x: long = 1;
      return x;
   }         
     
   public function Bar16(): float
   {
      var x: float = 1;
      return x;
   }         
          
   
}





// -----------------------------------------------------------------------
function class00()
{
   apInitTest ("Class00");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Operator overloading with System.DateTime -- Minus");

   var sep: DateTime = new DateTime (1980, 9, 9);
   var aug: DateTime = new DateTime (1975, 8, 19);
   var result1: TimeSpan;
   
   result1 = new TimeSpan(0);
   result1 = sep - aug;
   if (result1.TotalSeconds != 159667200)
      apLogFailInfo ("Error in 1", 159667200, result1.TotalSeconds, "");


   // -----------------------------------------------------------------------
   apInitScenario ("2. Operator overloading with System.DateTime -- Plus");

   var apr: DateTime = new DateTime (1975, 4, 26);
   var timeSpan: TimeSpan = new TimeSpan (5555999999999923);
   var result2: DateTime = new DateTime (0);

   result2 = apr + timeSpan;


   // -----------------------------------------------------------------------
   apInitScenario ("3. Operator overloading with System.DateTime -- Equality");

   var jun20: DateTime = new DateTime (1945, 6, 20);
   var jul29: DateTime = new DateTime (1972, 7, 29);
   var jun20x: DateTime = new DateTime (1945, 6, 20);
   var jun21: DateTime = new DateTime (1945, 6, 21);
   
   actual = "";
   expected = false;
   actual = (jun20 == jul29);
   if (actual != expected)
      apLogFailInfo ("Error in 3(1)", expected, actual, "");

   actual = "";
   expected = true;
   actual = (jun20 == jun20x);
   if (actual != expected)
      apLogFailInfo ("Error in 3(2)", expected, actual, "");

   actual = "";
   expected = false;
   actual = (jun20 == jun21);
   if (actual != expected)
      apLogFailInfo ("Error in 3(3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4. Operator overloading with System.DateTime  Greater than, greater than or equal, less than, less than or equal, equal");
      
   var feb14: DateTime = new DateTime (1945, 2, 14);
   var mar1: DateTime = new DateTime (1945, 3, 1);
   var feb14x: DateTime = new DateTime (1946, 2, 14);
   var mar30: DateTime = new DateTime (1912, 3, 30);
   
   // feb14 (2/14/1945) and mar1 (3/1/1945)
   actual = "";
   expected = false;
   actual = (feb14 > mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(1)", expected, actual, "");

   actual = "";
   expected = false;
   actual = (feb14 >= mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(2)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 < mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(3)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 <= mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(4)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (feb14 == mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(5)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 != mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(6)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (feb14 != feb14)
   if (actual != expected)
      apLogFailInfo ("Error in 4(7)", expected, actual, "248260");

   // feb14 (2/14/1945) and feb14x (2/14/1946)
   actual = "";
   expected = false;
   actual = (feb14 > feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(8)", expected, actual, "");

   actual = "";
   expected = false;
   actual = (feb14 >= feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(9)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 < feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(10)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 <= feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(11)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (feb14 == feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(12)", expected, actual, "248260");

   actual = "";
   expected = true;
   actual = (feb14 != feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(13)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (feb14x != feb14x)
   if (actual != expected)
      apLogFailInfo ("Error in 4(14)", expected, actual, "248260");
      
   // mar1 (3/1/1945) and mar30 (3/30/1912)
      
   actual = "";
   expected = true;
   actual = (mar1 > mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(15)", expected, actual, "");

   actual = "";
   expected = true;
   actual = (mar1 >= mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(16)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (mar1 < mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(17)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (mar1 <= mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(18)", expected, actual, "248260");

   actual = "";
   expected = false;
   actual = (mar1 == mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(19)", expected, actual, "248260");      
      
   actual = "";
   expected = true;
   actual = (mar1 != mar30)
   if (actual != expected)
      apLogFailInfo ("Error in 4(20)", expected, actual, "248260");      

   actual = "";
   expected = false;
   actual = (mar1 != mar1)
   if (actual != expected)
      apLogFailInfo ("Error in 4(21)", expected, actual, "248260");      

      
   // -----------------------------------------------------------------------
   if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
   apInitScenario ("5. Operator overloading with System.DateTime -- Plus");
      
   var apr26: DateTime = new DateTime (1975, 1, 15);
   var timeSpan5: TimeSpan = new TimeSpan (45, 4, 4, 4);
   var dateTime5: DateTime;
      
   expected = "Sat Mar 1 04:04:04 PST 1975";
   dateTime5 = apr26 + timeSpan5;
   if (dateTime5 != expected)
      apLogFailInfo ("Error in 5", expected, dateTime5, "252198, 252207");
   }

   
   // -----------------------------------------------------------------------
   if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
   apInitScenario ("6. Operator overloading with System.DateTime -- Minus (DateTime - TimeSpan)");   

   var nov3: DateTime = new DateTime (1975, 3, 15);
   var timeSpan6: TimeSpan = new TimeSpan (20, 0, 0, 0);
   var dateTime6: DateTime;
   
   expected = "Sun Feb 23 00:00:00 PST 1975";
   dateTime6 = nov3 - timeSpan6;
   if (dateTime6 != expected)
      apLogFailInfo ("Error in 6", expected, dateTime6, "252198, 252207");
   }
   

   // -----------------------------------------------------------------------
   apInitScenario ("7. Operator overloading with System.DateTime -- Minus (DateTime - DateTime)");   
   
   var nov4: DateTime = new DateTime (2000, 11, 4);
   var sep19: DateTime = new DateTime (2000, 9, 19);
   var fortySix: TimeSpan = new TimeSpan (46,0,0,0);
   var timeSpan7: TimeSpan;
   
   timeSpan7 = nov4 - sep19;
   if (timeSpan7 != fortySix)
      apLogFailInfo ("Error in 7", fortySix, timeSpan7, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8. Overloading");   
      
   var alpha8: Alpha8 = new Alpha8();

   expected = "Two";
   actual = "";
   actual = alpha8.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8(1)", expected, actual, "");
      
   expected = "One";
   actual = "";
   actual = alpha8.Bar(1);
   if (actual != expected)
      apLogFailInfo ("Error in 8(2)", expected, actual, "");

   expected = "Three";
   actual = "";
   actual = alpha8.Bar("hello");
   if (actual != expected)
      apLogFailInfo ("Error in 8(3)", expected, actual, "");
      
   expected = "Four";
   actual = "";
   actual = alpha8.Bar(true);
   if (actual != expected)
      apLogFailInfo ("Error in 8(4)", expected, actual, "");
      
   expected = "Five";
   actual = "";
   actual = alpha8.Bar (1,2);
   if (actual != expected)
      apLogFailInfo ("Error in 8(5)", expected, actual, "");

   expected = "Five";
   actual = "";
   actual = alpha8.Bar (true, 10);
   if (actual != expected)
      apLogFailInfo ("Error in 8(6)", expected, actual, "");

   expected = "Seven";
   actual = "";
   actual = alpha8.Bar (true, 20, "hello");
   if (actual != expected)
      apLogFailInfo ("Error in 8(8)", expected, actual, "");

   expected = "Eight";
   actual = "";
   actual = alpha8.Bar (true, 20, false);
   if (actual != expected)
      apLogFailInfo ("Error in 8(8)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9. Overloading with missing arguments");   
      
   var alpha9: Alpha9 = new Alpha9();
   
   // --------------------------------------------
   
   expectedError = "ReferenceError: More than one method or property matches this argument list";
   actualError = "";
   exceptionThrown = false;
   
   try
   {
      eval ("  actual = alpha9.Bar(false);   ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 9(1)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 9(1)", expectedError, actualError, "");
      
   // --------------------------------------------
   
   expectedError = "ReferenceError: More than one method or property matches this argument list";
   actualError = "";
   exceptionThrown = false;
   
   try
   {
      eval ("  alpha9.Bar (false, \"hello\");   ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 9(2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 9(2)", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. Overriding methods");   
   
   var alpha10: Alpha10 = new Alpha10();
   var beta10: Beta10 = new Beta10();
   var blah10: Alpha10 = new Alpha10();
   var mixed10: Alpha10;
      
   expected = "Alpha10.Bar()";
   actual = "";
   actual = alpha10.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 10(1)", expected, actual, "");
   
   expected = "Beta10.Bar()";
   actual = "";
   actual = beta10.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 10(2)", expected, actual, "");
   
   expected = "Alpha10.Bar()";
   actual = "";
   actual = Foo (alpha10);
   if (actual != expected)
      apLogFailInfo ("Error in 10(3)", expected, actual, "");

   expected = "Beta10.Bar()";
   actual = "";
   actual = Foo (beta10);
   if (actual != expected)
      apLogFailInfo ("Error in 10(4)", expected, actual, "");
      
   expected = "Alpha10.Bar()";
   actual = "";
   actual = blah10.Foo (alpha10)
   if (actual != expected)
      apLogFailInfo ("Error in 10(5)", expected, actual, "");

   expected = "Beta10.Bar()";
   actual = "";
   actual = blah10.Foo (beta10)
   if (actual != expected)
      apLogFailInfo ("Error in 10(6)", expected, actual, "");
      
   mixed10 = new Beta10();
   expected = "Beta10.Bar()";
   actual = "";
   actual = Foo (mixed10);   
   if (actual != expected)
      apLogFailInfo ("Error in 10(7)", expected, actual, "");
     
   mixed10 = new Beta10();
   expected = "Beta10.Bar()";
   actual = "";
   actual = blah10.Foo (mixed10);
   if (actual != expected)
      apLogFailInfo ("Error in 10(8)", expected, actual, "");
      
   mixed10 = new Charlie10();
   expected = "Charlie10.Bar()";
   actual = "";
   actual = Foo (mixed10);   
   if (actual != expected)
      apLogFailInfo ("Error in 10(9)", expected, actual, "");

   mixed10 = new Charlie10();
   expected = "Charlie10.Bar()";
   actual = "";
   actual = blah10.Foo (mixed10);   
   if (actual != expected)
      apLogFailInfo ("Error in 10(10)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. Instantiate a final class");
   
   var alpha11: Alpha11 = new Alpha11();
   
   actual = "";
   expected = "one";
   alpha11.Bar();
   actual = alpha11.value;
   if (actual != expected)
      apLogFailInfo ("Error in 11.", expected, actual, "");    
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("12. Inherit from a final class");
   
   if (exceptionThrown12 == false)
      apLogFailInfo ("No compile error in 12.", "Should give a compile error", exceptionThrown12, "");
   if (actualError12 != expectedError12)
      apLogFailInfo ("Wrong compile error in 12.", expectedError12, actualError12, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13. Beta is a final class");
   
   var beta13: Beta13 = new Beta13();
   
   actual = "";
   expected = "three";
   beta13.Bar();
   actual = beta13.value;
   if (actual != expected)
      apLogFailInfo ("Error in 13.", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14. Charlie is a final class");
   
   var charlie14: Charlie14 = new Charlie14();
   
   actual = "";
   expected = "four";
   charlie14.Bar();
   actual = charlie14.value;
   if (actual != expected)
      apLogFailInfo ("Error in 14.", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("15. Method hiding: Final Alpha.Bar()");
      
   var alpha15: Alpha15;
   var beta15: Beta15;
   var charlie15: Charlie15;
   var blah15: Blah15 = new Blah15();
   
   // ---------------------------------
   alpha15 = new Alpha15();
   
   expected = "five";
   actual = "";
   actual = alpha15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(1)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(2)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(3)", expected, actual, "");
      
   // ---------------------------------
   beta15 = new Beta15();
   
   expected = "six";
   actual = "";
   actual = beta15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(4)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (beta15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(5)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (beta15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(6)", expected, actual, "");
      
   // ---------------------------------
   alpha15 = new Beta15();
   
   expected = "five";
   actual = "";
   actual = alpha15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(7)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(8)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(9)", expected, actual, "");
      
   // ---------------------------------
   charlie15 = new Charlie15();
      
   expected = "seven";
   actual = "";
   actual = charlie15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(10)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (charlie15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(11)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (charlie15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(12)", expected, actual, "");
      
   // ---------------------------------
   alpha15 = new Charlie15();
      
   expected = "five";
   actual = "";
   actual = alpha15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(13)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(14)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(15)", expected, actual, "");
      
   // ---------------------------------
   beta15 = new Charlie15();
   
   expected = "seven";
   actual = "";
   actual = beta15.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 15(16)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = Foo15 (beta15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(17)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = blah15.Foo (beta15);
   if (actual != expected)
      apLogFailInfo ("Error in 15(18)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16. Method hiding: Private Alpha.Bar()");
        
   var alpha16: Alpha16;
   var beta16: Beta16;
   var blah16: Blah16 = new Blah16();
   
   // ------------------------------
   var alpha16 = new Alpha16();
   
   expected = "one";
   actual = "";
   actual = alpha16.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 16(1)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = Foo16 (alpha16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(2)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = blah16.Foo (alpha16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(3)", expected, actual, "");
      
   // ------------------------------
   var beta16 = new Beta16();
   
   expected = "one";
   actual = "";
   actual = beta16.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 16(4)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = Foo16 (beta16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(5)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = blah16.Foo (beta16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(6)", expected, actual, "");   
   
   // ------------------------------
   var alpha16 = new Beta16();
   
   expected = "one";
   actual = "";
   actual = alpha16.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 16(7)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = Foo16 (alpha16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(8)", expected, actual, "");
      
   expected = "one";
   actual = "";
   actual = blah16.Foo (alpha16);
   if (actual != expected)
      apLogFailInfo ("Error in 16(9)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17.1 Method hiding: static Alpha.Bar()");

   var alpha17_1: Alpha17_1 = new Alpha17_1();
   
   expected = "ten";
   actual = "";
   actual = Alpha17_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 17.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17.2 Method hiding: static Alpha.Bar()");
      
   if (actualError17_2 != expectedError17_2)
      apLogFailInfo ("Error in 17.2", expectedError17_2, actualError17_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17.3 Method hiding: static Alpha.Bar()");
   
   if (actualError17_3 != expectedError17_3)
      apLogFailInfo ("Error in 17.3", expectedError17_3, actualError17_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17.4 Method hiding: static Alpha.Bar()");
   
   var beta17_4: Beta17_4 = new Beta17_4();
   
   expected = "eleven";
   actual = "";
   actual = beta17_4.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 17.4", expected, actual, "");   

      
   // -----------------------------------------------------------------------
   apInitScenario ("18. Method overriding");
      
   var alpha18 = new Beta18();
   var blah18 = new Blah18()
   
   expected = "two";
   actual = "";
   actual = alpha18.Bar();
   if (actual != expected) 
      apLogFailInfo ("Error in 18(1)", expected, actual, "");
      
   expected = "two";
   actual = "";
   actual = Foo18 (alpha18);
   if (actual != expected) 
      apLogFailInfo ("Error in 18(2)", expected, actual, "");

   expected = "two";
   actual = "";
   actual = blah18.Foo (alpha18);
   if (actual != expected) 
      apLogFailInfo ("Error in 18(3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19. Expando: add/remove expandos to/from expando classes");
      
   var alpha19: Alpha19 = new Alpha19();
   var I;
   
   alpha19["x"] = "ten";
   expected = "ten";
   actual = "";
   actual = alpha19["x"];
   if (actual != expected)
      apLogFailInfo ("Error in 19", expected, actual, "");

   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Alpha19' do not have such a member";
   
   delete alpha19["x"];
   try
   {
      eval ("actual = alpha19.x");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 19", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 19", expectedError, actualError, "");


   // -----------------------------------------------------------------------
   apInitScenario ("20. Expando: for ... in over an expando class");

   var x, y, foo, bar;
   var alpha20: Alpha20 = new Alpha20();

   alpha20["x"] = "ten";
   alpha20["y"] = "eleven";
   alpha20["foo"] = 10;
   alpha20["bar"] = 11;
   
   for (I in alpha20)
   {
      switch (I)
      {
         case "x":
            x = alpha20[I];
            break;
         case "y":
            y = alpha20[I];
            break;
         case "foo":
            foo = alpha20[I];
            break;
         case "bar":
            bar = alpha20[I];
            break;
      }
   }

   expected = "ten";
   actual = x;
   if (actual != expected)
      apLogFailInfo ("Error in 20(1)", expected, actual, "");

   expected = "eleven";
   actual = y;
   if (actual != expected)
      apLogFailInfo ("Error in 20(2)", expected, actual, "");
   
   expected = 10;
   actual = foo;
   if (actual != expected)
      apLogFailInfo ("Error in 20(3)", expected, actual, "");

   expected = 11;
   actual = bar;
   if (actual != expected)
      apLogFailInfo ("Error in 20(4)", expected, actual, "");
         
      
   // -----------------------------------------------------------------------
   apInitScenario ("21. Static");
   
   var alpha21: Alpha21 = new Alpha21();
   var charlie21: Charlie21 = new Charlie21();
   
   expected = "One";
   actual = "";
   actual = Alpha21.value;
   if (actual != expected)
      apLogFailInfo ("Error in 21(1)", expected, actual, "");
      
   expected = "Two";
   actual = "";
   actual = Alpha21.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 21(2)", expected, actual, "");
      
   expected = "Three";
   actual = "";
   Alpha21.value = "Three";
   actual = Alpha21.value;
   if (actual != expected)
      apLogFailInfo ("Error in 21(3)", expected, actual, "");
      
      
/*      
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Type 'Charlie21' does not have such a static member"   
   try
   {
      eval ("actual = Charlie21.value;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error
   }
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 21(4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 21(4)", expectedError, actualError, ""); 

   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Type 'Charlie21' does not have such a static member"   
   try
   {
      eval ("actual = Charlie21.Bar();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error
   }
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 21(5)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 21(5)", expectedError, actualError, "");
*/       
      
      

   expected = "Three";
   actual = "";
   actual = alpha21.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 21(4)", expected, actual, "");
      
   expected = "Threexyz";
   actual = "";
   actual = charlie21.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 21(5)", expected, actual, "");
      
   expected = "Three";
   actual = "";
   actual = Alpha21.Zoo();
   if (actual != expected)
      apLogFailInfo ("Error in 21(6)", expected, actual, "");
      
   expected = "Three123";
   actual = "";
   actual = Charlie21.Zoo();
   if (actual != expected)
      apLogFailInfo ("Error in 21(7)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.1 Compile errors with static");
   
   if (actualError21_1 != expectedError21_1)
      apLogFailInfo ("Error in 21.1", expectedError21_1, actualError21_1, "");
   
      
     
   
   
   
   
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("22. Nested classes (non-static)");
   
   var a22: A22 = new A22();
   var alpha22: A22.Alpha22 = new a22.Alpha22();
   var beta22: A22.Beta22 = new a22.Beta22();
   
   expected = "One";
   actual = "";
   actual = alpha22.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22(1)", expected, actual, "");
   
   expected = "Two";
   actual = "";
   actual = alpha22.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 22(2)", expected, actual, "");
      
   expected = "Three";
   actual = "";
   actual = alpha22.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 22(3)", expected, actual, "");
      
   expected = "One";
   actual = "";
   actual = beta22.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22(4)", expected, actual, "");
      
   expected = "Two";
   actual = "";
   actual = beta22.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 22(5)", expected, actual, "");
      
   expected = "Four";
   actual = "";
   actual = beta22.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 22(6)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23. Nested classes (static)");
      
   var alpha23: A23.Alpha23 = new A23.Alpha23();
   var beta23: A23.Beta23 = new A23.Beta23();
   
   expected = "One";
   actual = "";
   actual = alpha23.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23(1)", expected, actual, "");
   
   expected = "Two";
   actual = "";
   actual = alpha23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(2)", expected, actual, "");
      
   expected = "Three";
   actual = "";
   actual = alpha23.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 23(3)", expected, actual, "");
      
   expected = "One";
   actual = "";
   actual = beta23.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23(4)", expected, actual, "");
      
   expected = "Two";
   actual = "";
   actual = beta23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(5)", expected, actual, "");
      
   expected = "Four";
   actual = "";
   actual = beta23.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 23(6)", expected, actual, "");   
   
   
   // -----------------------------------------------------------------------
   // apInitScenario ("24. Closures");
   // Waiting for Peter's sample on closures
   

   // -----------------------------------------------------------------------
   apInitScenario ("25. Interfaces");
   
   var alpha25: Alpha25 = new Alpha25();
   var beta25: Beta25 = new Beta25();
   
   expected = "Alpha25.Bar()";
   actual = "";
   actual = alpha25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25(1)", expected, actual, "");
      
   expected = "Beta25.Bar()";
   actual = "";
   actual = beta25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26. Interfaces w/ inheritance in the class");

   var alpha26: Alpha26;
   var beta26: Beta26;

   alpha26 = new Alpha26();
   expected = "Alpha26.Bar()";
   actual = "";
   actual = alpha26.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 26(1)", expected, actual, "");
   
   beta26 = new Beta26();
   expected = "Alpha26.Bar()";
   actual = "";
   actual = beta26.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 26(2)", expected, actual, "");

   alpha26 = new Beta26();      
   expected = "Alpha26.Bar()";
   actual = "";
   actual = alpha26.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 26(3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27. Implementing multiple interfaces; inheritance in the class");
      
   var alpha27: Alpha27;
   var beta27: Beta27;
   
   alpha27 = new Alpha27();

   expected = "Alpha27.Bar()";
   actual = "";
   actual = alpha27.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 27(1)", expected, actual, "");
      
   expected = "Alpha27.Foo()";
   actual = "";
   actual = alpha27.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 27(2)", expected, actual, "");
   
   beta27 = new Beta27();
   
   expected = "Alpha27.Bar()";
   actual = "";
   actual = beta27.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 27(3)", expected, actual, "");
      
   expected = "Beta27.Foo()";
   actual = "";
   actual = beta27.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 27(4)", expected, actual, "");
      
   alpha27 = new Beta27();
   
   expected = "Alpha27.Bar()";
   actual = "";
   actual = alpha27.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 27(5)", expected, actual, "");

   expected = "Beta27.Foo()";
   actual = "";
   actual = alpha27.Foo();   
   if (actual != expected)
      apLogFailInfo ("Error in 27(6)", expected, actual, "");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("28. Constants: compile error");
       
   if (exceptionThrown28 != true)
      apLogFailInfo ("No error in 28.", "Should give an error", exceptionThrown28, "");
   if (actualError28 != expectedError28)
      apLogFailInfo ("Wrong compile error in 28.", expectedError28, actualError28, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. Constants");
      
   var alpha29: Alpha29;
   var beta29: Beta29;
   
   alpha29 = new Alpha29();
   
   expected = "Hello";
   actual = "";
   actual = alpha29.value1;
   if (actual != expected)
      apLogFailInfo ("Error in 29(1)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = alpha29.value2;
   if (actual != expected)
      apLogFailInfo ("Error in 29(2)", expected, actual, "");
      
   expected = "Hello";
   actual = "";
   actual = alpha29.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 29(3)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = alpha29.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 29(4)", expected, actual, "");
      
   beta29 = new Beta29();
   
   expected = "Hello";
   actual = "";
   actual = beta29.value1;
   if (actual != expected)
      apLogFailInfo ("Error in 29(5)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = beta29.value2;
   if (actual != expected)
      apLogFailInfo ("Error in 29(6)", expected, actual, "");
      
   expected = "Hello";
   actual = "";
   actual = beta29.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 29(7)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = beta29.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 29(8)", expected, actual, "");   
   
   alpha29 = new Beta29();
      
   expected = "Hello";
   actual = "";
   actual = alpha29.value1;
   if (actual != expected)
      apLogFailInfo ("Error in 29(9)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = alpha29.value2;
   if (actual != expected)
      apLogFailInfo ("Error in 29(10)", expected, actual, "");
      
   expected = "Hello";
   actual = "";
   actual = alpha29.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 29(11)", expected, actual, "");
      
   expected = 23;
   actual = 0;
   actual = alpha29.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 29(12)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30. Method return type");
        
   var alpha30: Alpha30 = new Alpha30();
   
   expected = "boolean";
   actual = "";
   actual = typeof (alpha30.Bar1());
   if (actual != expected)
      apLogFailInfo ("Error in 30(1)", expected, actual, "");
      
   expected = "boolean";
   actual = "";
   actual = typeof (alpha30.Bar2());
   if (actual != expected)
      apLogFailInfo ("Error in 30(2)", expected, actual, "");
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar3());
   if (actual != expected)
      apLogFailInfo ("Error in 30(3)", expected, actual, "");
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar4());
   if (actual != expected)
      apLogFailInfo ("Error in 30(4)", expected, actual, "");      
      
   expected = "string";
   actual = "";
   actual = typeof (alpha30.Bar5());
   if (actual != expected)
      apLogFailInfo ("Error in 30(5)", expected, actual, "");     
      
   expected = "object";
   actual = "";
   actual = typeof (alpha30.Bar6());
   if (actual != expected)
      apLogFailInfo ("Error in 30(6)", expected, actual, "");     
      
   expected = "object";
   actual = "";
   actual = typeof (alpha30.Bar7());
   if (actual != expected)
      apLogFailInfo ("Error in 30(7)", expected, actual, "");     
   
   expected = "function";
   actual = "";
   actual = typeof (alpha30.Bar8());
   if (actual != expected)
      apLogFailInfo ("Error in 30(8)", expected, actual, "");     

   expected = "object";
   actual = "";
   actual = typeof (alpha30.Bar9());
   if (actual != expected)
      apLogFailInfo ("Error in 30(9)", expected, actual, "");     
      
   expected = "object";
   actual = "";
   actual = typeof (alpha30.Bar10());
   if (actual != expected)
      apLogFailInfo ("Error in 30(10)", expected, actual, "");     
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar11());
   if (actual != expected)
      apLogFailInfo ("Error in 30(11)", expected, actual, "");         
        
   expected = "string";
   actual = "";
   actual = typeof (alpha30.Bar12());
   if (actual != expected)
      apLogFailInfo ("Error in 30(12)", expected, actual, "");         
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar13());
   if (actual != expected)
      apLogFailInfo ("Error in 30(13)", expected, actual, "");         
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar14());
   if (actual != expected)
      apLogFailInfo ("Error in 30(14)", expected, actual, "");         
      
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar15());
   if (actual != expected)
      apLogFailInfo ("Error in 30(15)", expected, actual, "");         
          
   expected = "number";
   actual = "";
   actual = typeof (alpha30.Bar16());
   if (actual != expected)
      apLogFailInfo ("Error in 30(16)", expected, actual, "");             
            
   apEndTest();
}


class00();


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
