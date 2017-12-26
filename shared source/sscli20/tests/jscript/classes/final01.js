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


var iTestID = 169559;



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
final class Alpha1
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown2 = false;
var actualError2 = "";
var expectedError2 = "ReferenceError: Type: 'Alpha2' cannot be extended";

try
{
   eval ("  final class Alpha2 { }         " +
         "  class Beta extends Alpha2 { }  ");
}
catch (error)
{
   exceptionThrown2 = true;
   actualError2 = error;
}


// -----------------------------------------------------------------------
class Alpha3
{
   public var value: String = "none";
   public function Bar()
   {
      value = "three";
   }
}

final class Beta3 extends Alpha3
{
}


// -----------------------------------------------------------------------
class Alpha4
{
   public var value: String = "none";
   public function Bar()
   {
      value = "four";
   }
}

class Beta4 extends Alpha4
{
}

final class Charlie4 extends Beta4
{
}


// -----------------------------------------------------------------------
var exceptionThrown5 = false;
var actualError5 = "";
var expectedError5 = "ReferenceError: Type: 'Beta5' cannot be extended";

try
{
   eval ("  class Alpha5                        " +
         "  { }                                 " +
         "  final class Beta5 extends Alpha5    " +
         "  { }                                 " +
         "  class Charlie5 extends Beta5        " +
         "  { }                                 ");
}
catch (error)
{
   exceptionThrown5 = true;
   actualError5 = error;
}


// -----------------------------------------------------------------------
final public class Alpha6
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "six";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown7 = false;
var actualError7 = "";
var expectedError7 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  final private class Alpha7                " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     public function Bar()                  " +
         "     {                                      " +
         "        value = \"seven\";                  " +
         "     }                                      " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown7 = true;
   actualError7 = error;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
var exceptionThrown8 = false;
var actualError8 = "";
var expectedError8 = "SyntaxError: Only valid inside a class definition";

try
{  
   eval ("  final protected class Alpha8     " +
         "  {                                " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown8 = true;
   actualError8 = error;
}


// -----------------------------------------------------------------------
final internal class Alpha9
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "nine";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown10 = false;
var actualError10 = "";
var expectedError10 = "Error: final and abstract cannot be used together";

try
{
   eval ("  abstract final class Alpha10  " +
         "  {                             " +
         "  }                             ");
}
catch (error)
{
   exceptionThrown10 = true;
   actualError10 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown11 = false;
var actualError11 = "";
var expectedError11 = "SyntaxError: Syntax error";

try
{
   eval ("  final final class Alpha11  " +
         "  {                          " +
         "  }                          ");
}
catch (error)
{
   exceptionThrown11 = true;
   actualError11 = error;
}


// -----------------------------------------------------------------------
final expando class Alpha12
{
   public var value: String = "none";
   
   public function Bar()
   {
      value = "twelve";   
   }
}  


// -----------------------------------------------------------------------
var exceptionThrown13 = false;
var actualError13 = "";
var expectedError13 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  final static class Alpha13    " +
         "  {                             " +
         "  }                             ");
}
catch (error)
{
   exceptionThrown13 = true;
   actualError13 = error;
}


// -----------------------------------------------------------------------
class Nesting14_1
{
   static final class Alpha14_1
   {
      public var value: String = "none";
   
      public function Bar()
      {
         value = "one";   
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown14_2 = false;
var actualError14_2 = "";
var expectedError14_2 = "ReferenceError: Type: 'Alpha14_2' cannot be extended";

try
{
   eval ("  class Nesting14_2                            " +
         "  {                                            " +
         "     final static class Alpha14_2              " +
         "     {  }                                      " +
         "     static class Beta14_2 extends Alpha14_2   " +
         "     {  }                                      " +
         "  }                                            ");
}
catch (error)
{
   exceptionThrown14_2 = true;
   actualError14_2 = error;
}


// -----------------------------------------------------------------------
class Nesting14_3
{
   static class Alpha14_3
   {
      public var value: String = "none";
   
      public function Bar()
      {
         value = "three";   
      }
   }
   
   final static class Beta14_3 extends Alpha14_3
   {
   }
}


// -----------------------------------------------------------------------
class Nesting14_4
{
   static class Alpha14_4
   {
      public var value: String = "none";
      public function Bar()
      {
         value = "four";
      }
   }

   static class Beta14_4 extends Alpha14_4
   {
   }

   static final class Charlie14_4 extends Beta14_4
   {
   }
}


// -----------------------------------------------------------------------
var exceptionThrown14_5 = false;
var actualError14_5 = "";
var expectedError14_5 = "ReferenceError: Type: 'Beta14_5' cannot be extended";

try
{
   eval ("  class Nesting14_5                            " +
         "  {                                            " +
         "     class Alpha14_5                           " +
         "     { }                                       " +
         "     final class Beta14_5 extends Alpha14_5    " +
         "     { }                                       " +
         "     static class Charlie14_5 extends Beta14_5 " +
         "     { }                                       " +
         "  }                                            ");
}
catch (error)
{
   exceptionThrown14_5 = true;
   actualError14_5 = error;
}


// -----------------------------------------------------------------------
class Nesting15_1
{
   final class Alpha15_1
   {
      public var value: String = "none";
   
      public function Bar()
      {
         value = "one";   
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown15_2 = false;
var actualError15_2 = "";
var expectedError15_2 = "ReferenceError: Type: 'Alpha15_2' cannot be extended";

try
{
   eval ("  class Nesting15_2                            " +
         "  {                                            " +
         "     final static class Alpha15_2              " +
         "     {  }                                      " +
         "     class Beta15_2 extends Alpha15_2          " +
         "     {  }                                      " +
         "  }                                            ");
}
catch (error)
{
   exceptionThrown15_2 = true;
   actualError15_2 = error;
}


// -----------------------------------------------------------------------
class Nesting15_3
{
   class Alpha15_3
   {
      public var value: String = "none";
   
      public function Bar()
      {
         value = "three";   
      }
   }
   
   final class Beta15_3 extends Alpha15_3
   {
   }
}


// -----------------------------------------------------------------------
class Nesting15_4
{
   class Alpha15_4
   {
      public var value: String = "none";
      public function Bar()
      {
         value = "four";
      }
   }

   class Beta15_4 extends Alpha15_4
   {
   }

   final class Charlie15_4 extends Beta15_4
   {
   }
}


// -----------------------------------------------------------------------
var exceptionThrown15_5 = false;
var actualError15_5 = "";
var expectedError15_5 = "ReferenceError: Type: 'Beta15_5' cannot be extended";

try
{
   eval ("  class Nesting15_5                            " +
         "  {                                            " +
         "     class Alpha15_5                           " +
         "     { }                                       " +
         "     final class Beta15_5 extends Alpha15_5    " +
         "     { }                                       " +
         "     class Charlie15_5 extends Beta15_5        " +
         "     { }                                       " +
         "  }                                            ");
}
catch (error)
{
   exceptionThrown15_5 = true;
   actualError15_5 = error;
}


// -----------------------------------------------------------------------
class Alpha16
{
   private var value: String = "ten";
   
   final public function SetValue (newValue)
   {
      value = newValue;
   }
   
   final public function GetValue()
   {
      return value;
   }
}


// -----------------------------------------------------------------------
class Alpha17
{
   protected var value: int;
   
   final public function SetValue (newValue: int)
   {
      value = newValue;
   }
   
   final public function GetValue(): int
   {
      return value;
   }
}

class Beta17 extends Alpha17
{
   final public function SetValue (newValue: int)
   {
      value = newValue + 2;
   }
   
   final public function GetValue(): int
   {
      return value + 2;
   }
}


// -----------------------------------------------------------------------
class Alpha18
{
   protected var value: int;
   
   final public function SetValue (newValue: int)
   {
      value = newValue;
   }
   
   final public function GetValue(): int
   {
      return value;
   }
}

class Beta18 extends Alpha18
{
   final public function SetValue (newValue: int)
   {
      value = newValue + 2;
   }
   
   final public function GetValue(): int
   {
      return value + 2;
   }
}

class Charlie18 extends Beta18
{
   final public function SetValue (newValue: int)
   {
      value = newValue + 10;
   }
   
   final public function GetValue(): int
   {
      return value + 10;
   }
}


// -----------------------------------------------------------------------
class Alpha19
{
   public var value: String = "none";
   public final function Alpha19()
   {
      value = "one";
   }
}

class Beta19 extends Alpha19
{
   public function Alpha19()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha20
{
   public var value: String = "none";
   final public function Bar()
   {
      value = "three";
   }
}


// -----------------------------------------------------------------------
class Alpha21
{
   public var value: String = "none";
   
   final private function Foo()
   {
      value = "four";
   }
   final public function Bar()
   {
      Foo();
   }
}


// -----------------------------------------------------------------------
class Alpha22
{
   public var value: String = "none";
   
   final protected function Foo()
   {
      value = "five";
   }
   final public function Bar()
   {
      Foo();
   }
}


// -----------------------------------------------------------------------
class Alpha23
{
   public var value: String = "none";
   
   final internal function Foo()
   {
      value = "six";
   }
   final public function Bar()
   {
      Foo();
   }
}


// -----------------------------------------------------------------------
class Alpha24
{
   public var value: String = "none";
   final public function Bar()
   {
      value = "Alpha24.Bar()";
   }
}

class Beta24 extends Alpha24
{
   final public function Bar()
   {
      value = "Beta24.Bar()";
   }
}


// -----------------------------------------------------------------------
class Alpha25
{
   public var value: String = "none";
   
   final private function Foo()
   {  
      value = "Alpha25.Foo()";  
   }
   
   public function Bar()
   {  
      Foo();   
   }
   
   public function Zoo()
   {  
      Foo();   
   }
}

class Beta25 extends Alpha25
{
   final private function Foo()
   {  
      value = "Beta25.Foo()";   
   }

   public function Bar()
   {  
      Foo();   
   }   
}


// -----------------------------------------------------------------------
class Alpha26
{
   public var value: String = "none";
   
   final protected function Foo()
   {  
      value = "Alpha26.Foo()";  
   }
   
   public function Bar()
   {  
      Foo();   
   }
   
   public function Zoo()
   {  
      Foo();   
   }
}

class Beta26 extends Alpha26
{
   final protected function Foo()
   {  
      value = "Beta26.Foo()";   
   }

   public function Bar()
   {  
      Foo();   
   }   
}


// -----------------------------------------------------------------------
class Alpha27
{
   public var value: String = "none";
   
   final internal function Foo()
   {  
      value = "Alpha27.Foo()";  
   }
   
   public function Bar()
   {  
      Foo();   
   }
   
   public function Zoo()
   {  
      Foo();   
   }
}

class Beta27 extends Alpha27
{
   final internal function Foo()
   {  
      value = "Beta27.Foo()";   
   }

   public function Bar()
   {  
      Foo();   
   }   
}


// -----------------------------------------------------------------------
package Group28
{
   class Alpha28
   {
      public var value: String = "none";
      
      final internal function Foo()
      {
         value = "Group28.Alpha28.Foo()";
      }
      
      public function Bar()
      {
         Foo();
      }
   }
   
   class Beta28
   {
      public var temp: String = "none";
      
      public function Zoo1()
      {
         temp = "none";
         var alpha28: Alpha28 = new Alpha28();
         alpha28.Foo();
         temp = alpha28.value;
      }
      
      public function Zoo2()
      {
         temp = "none";
         var alpha28: Alpha28 = new Alpha28();
         alpha28.Bar();
         temp = alpha28.value;
      }
   }
}


// -----------------------------------------------------------------------
package Group29
{
   class Alpha29
   {
      public var value: String = "none";
      
      final internal function Foo()
      {  
         value = "Group29.Alpha29.Foo()";  
      }
      
      public function Bar()
      {  
         Foo();   
      }
      
      public function Zoo()
      {  
         Foo();   
      }
   }

   class Beta29 extends Alpha29
   {
      final internal function Foo()
      {  
         value = "Group29.Beta29.Foo()";   
      }

      public function Bar()
      {  
         Foo();   
      }   
   }
}


// -----------------------------------------------------------------------
var exceptionThrown30 = false;
var actualError30 = "";
var expectedError30 = "Error: final and abstract cannot be used together";

try
{
   eval ("  class Alpha30                       " +
         "  {                                   " +
         "     final abstract function Bar()    " +
         "  }                                   ");
}
catch (error)
{
   exceptionThrown30 = true;
   actualError30 = error
}     


// -----------------------------------------------------------------------
var exceptionThrown31 = false;
var actualError31 = "";
var expectedError31 = "SyntaxError: Syntax error";

try
{
   eval ("  class Alpha31                 " +
         "  {                             " +
         "     final final function Bar() " +
         "     {  }                       " +
         "  }                             ");
}
catch (error)
{
   exceptionThrown31 = true;
   actualError31 = error
}         


// -----------------------------------------------------------------------
class Alpha32
{
   public var value: String = "none";
   
   expando function Bar (x: String)
   {
      this.value = x;
      return "hello";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown33 = false;
var actualError33 = "";
var expectedError33 = "Error: Static methods cannot be final";

try
{
   eval ("  class Alpha33                          " +
         "  {                                      " +
         "     final static public function Bar()  " +
         "     {  }                                " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown33 = true;
   actualError33 = error
}


// -----------------------------------------------------------------------
class Alpha34
{  
   public var value: String = "none";
   
   final public function Bar()
   {
      value = "Alpha34.Bar()";
   }
}

class Beta34 extends Alpha34
{
   final public function Bar()
   {
      value = "Beta34.Bar()";
   }
}


// -----------------------------------------------------------------------
class Alpha35
{
   public var value: String = "none";
   final public function Bar()
   {
      value = "Alpha35.Bar()";
   }
}

abstract class Beta35 extends Alpha35
{
   abstract public function Bar();
}

class Charlie35 extends Beta35
{
   public function Bar()
   {
      value = "Charlie35.Bar()";
   }
}


// -----------------------------------------------------------------------
var expectedError35_1, actualError35_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha35_1                                 " +
         "  {                                               " +
         "     public var value: String = \"none\";         " +
         "     final public function Bar()                  " +
         "     {                                            " +
         "        value = \"Alpha35_1.Bar()\";              " +
         "     }                                            " +
         "  }                                               " +
         "  abstract class Beta35_1 extends Alpha35_1       " +
         "  {                                               " +
         "     abstract public function Bar();              " +
         "  }                                               " +
         "  class Charlie35_1 extends Beta35_1              " +
         "  {                                               " +
         "     public function Bar()                        " +
         "     {                                            " +
         "        value = \"Charlie35_1.Bar()\";            " +
         "     }                                            " +
         "  }                                               " +
         "  var alpha35_1: Alpha35_1;                       " +
         "  var beta35_1: Beta35_1;                         " +
         "  alpha35_1 = new Beta35_1();                     " +
         "  beta35_1 = new Beta35_1();                      ";


//fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
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
actualError35_1 = process.StandardOutput.ReadToEnd();


@if(!@rotor)
expectedError35_1 = "test.js(1,1063) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine + "" +
                    "test.js(1,1112) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine + "";
@else
expectedError35_1 = "test.js(1,1063) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine + "" +
                    "test.js(1,1112) : error JS1214: It is not possible to construct an instance of an abstract class" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha36
{
   static public var value: int = 0;
   final public function Bar()
   {
      value++;
   }
}

class Beta36 extends Alpha36
{
   static public function Bar()
   {
      value = value + 2;
   }
}


// -----------------------------------------------------------------------
class Alpha37
{
   class Beta37
   {
      public var value: String = "none";
      final public function Bar()
      {
         value = "one";
      }
   }
   
   class Charlie37 extends Beta37
   {
      public function Bar()
      {
         value = "two";
      }
   }
}


// -----------------------------------------------------------------------
class Alpha38
{
   static class Beta38
   {
      public var value: String = "none";
      final public function Bar()
      {
         value = "one"
      }
   }
   
   static class Charlie38 extends Beta38
   {
      public function Bar()
      {
         value = "two";
      }
   }
}


// -----------------------------------------------------------------------
class Alpha39
{
   static class Beta39
   {
      public var value: String = "none";
      final public function Bar()
      {
         value = "one";
      }
   }
}

class Charlie39 extends Alpha39.Beta39
{
   public function Bar()
   {
      value = "two";
   }
}

class Delta39 extends Charlie39
{
   public function Bar()
   {
      value = "three";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown40 = false;
var actualError40 = "";
var expectedError40 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  final function Bar() { }   ", "unsafe");
}
catch (error)
{
   exceptionThrown40 = true;
   actualError40 = error;
}


// -----------------------------------------------------------------------
class Alpha41
{
   public var value: int = 0;
   
   final public function get Price(): int
   {
      value = value + 5;
      return value;
   }
   
   final public function set Price (newPrice: int)
   {
      value = newPrice;
      value++;
   }
}


// -----------------------------------------------------------------------
class Alpha42
{
   public var value: int = 0;
   
   final public function get Price(): int
   {
      value = value + 2;
      return value;
   }
   
   final public function set Price (newPrice: int)
   {
      value = newPrice;
      value++;
   }   
}

class Beta42 extends Alpha42
{
   public function get Price(): int
   {
      value = value + 10;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value + 20;
   }   
}

class Charlie42 extends Beta42
{
   public function get Price(): int
   {
      value = value - 1;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value - 2;
   }   
}


// -----------------------------------------------------------------------
class Alpha43
{
   public var value: int = 0;
   
   final public function get Price(): int
   {
      value = value + 2;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value++;
   }   
}

class Beta43 extends Alpha43
{
   public function get Price(): int
   {
      value = value + 10;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value + 20;
   }   
}

class Charlie43 extends Beta43
{
   public function get Price(): int
   {
      value = value - 1;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value - 2;
   }   
}


// -----------------------------------------------------------------------
class Alpha44
{
   public var value: int = 0;
   
   public function get Price(): int
   {
      value = value + 2;
      return value;
   }
   
   final public function set Price (newPrice: int)
   {
      value = newPrice;
      value++;
   }   
}

class Beta44 extends Alpha44
{
   public function get Price(): int
   {
      value = value + 10;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value + 20;
   }   
}

class Charlie44 extends Beta44
{
   public function get Price(): int
   {
      value = value - 1;
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
      value = value - 2;
   }   
}


// -----------------------------------------------------------------------
class Nesting45
{
   class Alpha45
   {
      public var value: int = 0;
      
      final public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      final public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   class Beta45 extends Alpha45
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   class Charlie45 extends Beta45
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
class Nesting46
{
   class Alpha46
   {
      public var value: int = 0;
      
      final public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   class Beta46 extends Alpha46
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   class Charlie46 extends Beta46
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
class Nesting47
{
   class Alpha47
   {
      public var value: int = 0;
      
      public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      final public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   class Beta47 extends Alpha47
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   class Charlie47 extends Beta47
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
class Nesting48
{
   static class Alpha48
   {
      public var value: int = 0;
      
      final public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      final public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   static class Beta48 extends Alpha48
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   static class Charlie48 extends Beta48
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
class Nesting49
{
   static class Alpha49
   {
      public var value: int = 0;
      
      final public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   static class Beta49 extends Alpha49
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   static class Charlie49 extends Beta49
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
class Nesting50
{
   static class Alpha50
   {
      public var value: int = 0;
      
      public function get Price(): int
      {
         value = value + 2;
         return value;
      }
      
      final public function set Price (newPrice: int)
      {
         value = newPrice;
         value++;
      }   
   }

   static class Beta50 extends Alpha50
   {
      public function get Price(): int
      {
         value = value + 10;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value + 20;
      }   
   }

   static class Charlie50 extends Beta50
   {
      public function get Price(): int
      {
         value = value - 1;
         return value;
      }
      
      public function set Price (newPrice: int)
      {
         value = newPrice;
         value = value - 2;
      }   
   }
}


// -----------------------------------------------------------------------
var exceptionThrown51 = false;
var actualError51 = "";
var expectedError51 = "Error: Invalid visibility modifier";

try
{
   eval ("  class Alpha51                 " +
         "  {                             " +
         "     final var value: String;   " +
         "  }                             ");
   
}
catch (error)
{
   exceptionThrown51 = true;
   actualError51 = error;
}



import Group28;
import Group29;



// -----------------------------------------------------------------------
function final01()
{
   apInitTest ("Final01");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Instantiate a final class");
   
   var alpha1: Alpha1 = new Alpha1();
   
   actual = "";
   expected = "one";
   alpha1.Bar();
   actual = alpha1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 1.", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2. Inherit from a final class");
   
   if (exceptionThrown2 == false)
      apLogFailInfo ("No compile error in 2.", "Should give a compile error", exceptionThrown2, "");
   if (actualError2 != expectedError2)
      apLogFailInfo ("Wrong compile error in 2.", expectedError2, actualError2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Beta is a final class");
   
   var beta3: Beta3 = new Beta3();
   
   actual = "";
   expected = "three";
   beta3.Bar();
   actual = beta3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 3.", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4. Charlie is a final class");
   
   var charlie4: Charlie4 = new Charlie4();
   
   actual = "";
   expected = "four";
   charlie4.Bar();
   actual = charlie4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 4.", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5. Alpha not final; Beta is final; Charlie not final");
   
   if (exceptionThrown5 == false)
      apLogFailInfo ("No compile error in 5.", "Should give a compile error", exceptionThrown5, "");
   if (actualError5 != expectedError5)
      apLogFailInfo ("Wrong compile error in 5.", expectedError5, actualError5, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("6. Final public class");
   
   var alpha6: Alpha6 = new Alpha6();
   
   actual = "";
   expected = "six";
   alpha6.Bar();
   actual = alpha6.value;
   if (actual != expected)
      apLogFailInfo ("Error in 6.", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7. Final private class");
   
   if (exceptionThrown7 == false)
      apLogFailInfo ("No compile error in 7.", "Should give a compile error", exceptionThrown7, "");
   if (actualError7 != expectedError7)
      apLogFailInfo ("Wrong compile error in 7.", expectedError7, actualError7, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8. Final protected class");
   
   if (exceptionThrown8 == false)
      apLogFailInfo ("No compile error in 8.", "Should give a compile error", exceptionThrown8, "");
   if (actualError8 != expectedError8)
      apLogFailInfo ("Wrong compile error in 8.", expectedError8, actualError8, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9. Final internal class");
      
   var alpha9: Alpha9 = new Alpha9();
   
   actual = "";
   expected = "nine";
   alpha9.Bar();
   actual = alpha9.value;
   if (actual != expected)
      apLogFailInfo ("Error in 9.", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. Final abstract class");
        
   if (exceptionThrown10 == false)
      apLogFailInfo ("No compile error in 10.", "Should give a compile error", exceptionThrown10, "");
   if (actualError10 != expectedError10)
      apLogFailInfo ("Wrong compile error in 10.", expectedError10, actualError10, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. Final final class");
   
   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11.", "Should give a compile error", exceptionThrown11, "");
   if (actualError11 != expectedError11)
      apLogFailInfo ("Wrong compile error in 11.", expectedError11, actualError11, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12. Final expando class");
      
   var alpha12: Alpha12 = new Alpha12();
   
   actual = "";
   expected = "twelve";
   alpha12.Bar();
   actual = alpha12.value;
   if (actual != expected)
      apLogFailInfo ("Error in 12.", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13. Final static class");
      
   if (exceptionThrown13 == false)
      apLogFailInfo ("No compile error in 13.", "Should give a compile error", exceptionThrown13, "");
   if (actualError13 != expectedError13)
      apLogFailInfo ("Wrong compile error in 13.", expectedError13, actualError13, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14.1 Nested class (static); Instantiate a final class");
   
   var alpha14_1: Nesting14_1.Alpha14_1 = new Nesting14_1.Alpha14_1();
   
   actual = "";
   expected = "one";
   alpha14_1.Bar();
   actual = alpha14_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 14.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14.2 Nested class (static); Inherit from a final class");
      
   if (exceptionThrown14_2 == false)
      apLogFailInfo ("No compile error in 14.2", "Should give a compile error", exceptionThrown14_2, "");
   if (actualError14_2 != expectedError14_2)
      apLogFailInfo ("Wrong compile error in 14.2", expectedError14_2, actualError14_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14.3 Nested class (static); Beta is a final class");
         
   var beta14_3: Nesting14_3.Beta14_3 = new Nesting14_3.Beta14_3();
   
   actual = "";
   expected = "three";
   beta14_3.Bar();
   actual = beta14_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 14.3", expected, actual, "226203");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14.4 Nested class (static); Charlie is a final class");
   
   var charlie14_4: Nesting14_4.Charlie14_4 = new Nesting14_4.Charlie14_4();
   
   actual = "";
   expected = "four";
   charlie14_4.Bar();
   actual = charlie14_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 14.4", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14.5 Nested class (static); Alpha not final; Beta is final; Charlie not final");
        
   if (exceptionThrown14_5 == false)
      apLogFailInfo ("No compile error in 14.5", "Should give a compile error", exceptionThrown14_5, "");
   if (actualError14_5 != expectedError14_5)
      apLogFailInfo ("Wrong compile error in 14.5", expectedError14_5, actualError14_5, "");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("15.1 Nested class (non-static); Instantiate a final class");
   
   var nesting15_1: Nesting15_1 = new Nesting15_1();
   var alpha15_1: Nesting15_1.Alpha15_1 = new nesting15_1.Alpha15_1();   
   
   actual = "";
   expected = "one";
   alpha15_1.Bar();
   actual = alpha15_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 15.1", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("15.2 Nested class (non-static); Inherit from a final class");
      
   if (exceptionThrown15_2 == false)
      apLogFailInfo ("No compile error in 15.2", "Should give a compile error", exceptionThrown15_2, "");
   if (actualError15_2 != expectedError15_2)
      apLogFailInfo ("Wrong compile error in 15.2", expectedError15_2, actualError15_2, "");


   // -----------------------------------------------------------------------
   apInitScenario ("15.3 Nested class (non-static); Beta is a final class");

   var nesting15_3: Nesting15_3 = new Nesting15_3();
   var beta15_3: Nesting15_3.Beta15_3 = new nesting15_3.Beta15_3();
   
   actual = "";
   expected = "three";
   beta15_3.Bar();
   actual = beta15_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 15.3", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("15.4 Nested class (non-static); Charlie is a final class");
  
   var nesting15_4: Nesting15_4 = new Nesting15_4();
   var charlie15_4: Nesting15_4.Charlie15_4 = new nesting15_4.Charlie15_4();
   
   actual = "";
   expected = "four";
   charlie15_4.Bar();
   actual = charlie15_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 15.4", expected, actual, ""); 


   // -----------------------------------------------------------------------
   apInitScenario ("15.5 Nested class (non-static); Alpha not final; Beta is final; Charlie not final");
        
   if (exceptionThrown15_5 == false)
      apLogFailInfo ("No compile error in 15.5", "Should give a compile error", exceptionThrown15_5, "");
   if (actualError15_5 != expectedError15_5)
      apLogFailInfo ("Wrong compile error in 15.5", expectedError15_5, actualError15_5, "");


   // -----------------------------------------------------------------------
   apInitScenario ("16. Call a final method");

   var alpha16: Alpha16 = new Alpha16();
   
   expected = "ten";
   actual = "";
   alpha16.SetValue ("ten");
   actual = alpha16.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 16.", expected, actual, "");   
               
   
   // -----------------------------------------------------------------------
   apInitScenario ("17. Override a final method; 2 levels of inheritance");
      
   // -----------------------------------
   var alpha17_1: Alpha17 = new Alpha17();
   
   expected = 2;
   actual = 0;
   alpha17_1.SetValue (2);
   actual = alpha17_1.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 17 (1)", expected, actual, "");

   // -----------------------------------
   var beta17_2: Beta17 = new Beta17();      
   
   expected = 6;
   actual = 0;
   beta17_2.SetValue (2);
   actual = beta17_2.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 17 (2)", expected, actual, "");
      
   // -----------------------------------
   var alpha17_3: Alpha17;
   alpha17_3 = new Beta17();
   
   expected = 4;
   actual = 0;
   alpha17_3.SetValue (4);
   actual = alpha17_3.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 17 (3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18. Override a final method; 3 levels of inheritance");
      
   // -----------------------------------
   var alpha18_1: Alpha18 = new Alpha18();
   
   expected = 2;
   actual = 0;
   alpha18_1.SetValue (2);
   actual = alpha18_1.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 18 (1)", expected, actual, "");

   // -----------------------------------
   var beta18_2: Beta18 = new Beta18();      
   
   expected = 6;
   actual = 0;
   beta18_2.SetValue (2);
   actual = beta18_2.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 18 (2)", expected, actual, "");
      
   // -----------------------------------
   var alpha18_3: Alpha18;
   alpha18_3 = new Beta18();
   
   expected = 4;
   actual = 0;
   alpha18_3.SetValue (4);
   actual = alpha18_3.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 18 (3)", expected, actual, "");      
      
   // -----------------------------------
   var charlie18_4: Charlie18 = new Charlie18();      
   
   expected = 22;
   actual = 0;
   charlie18_4.SetValue (2);
   actual = charlie18_4.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 18 (4)", expected, actual, "");
      
   // -----------------------------------
   var alpha18_5: Alpha18;
   alpha18_5 = new Charlie18();
   
   expected = 4;
   actual = 0;
   alpha18_5.SetValue (4);
   actual = alpha18_5.GetValue();
   if (actual != expected)
      apLogFailInfo ("Error in 18 (5)", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19. Final constructors");
      
   var alpha19: Alpha19 = new Alpha19();
   var beta19: Beta19 = new Beta19();
   
   // -----------------------------------------
   expected = "one";
   actual = "";
   actual = alpha19.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19(1)", expected, actual, "");

   // -----------------------------------------
   expectedError = "ReferenceError: Objects of type 'Alpha19' do not have such a member";
   actualError = "";
   exceptionThrown = false;
         
   try
   {
      eval ("alpha19.Alpha19();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 19(2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 19(2)", expectedError, actualError, "");
      
   // -----------------------------------------
   expected = "one";
   actual = "";
   actual = beta19.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19(3)", expected, actual, "");
   
   // -----------------------------------------
   expected = "two";
   actual = "";
   beta19.Alpha19();
   actual = beta19.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19(4)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20. Final public method");
   
   var alpha20: Alpha20 = new Alpha20();
   
   expected = "three";
   actual = "";
   alpha20.Bar();
   actual = alpha20.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21. Final private method");
            
   var alpha21: Alpha21 = new Alpha21();
   
   expected = "four";
   actual = "";
   alpha21.Bar();
   actual = alpha21.value;
   if (actual != expected)
      apLogFailInfo ("Error in 21.", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22. Final protected method");
            
   var alpha22: Alpha22 = new Alpha22();
   
   expected = "five";
   actual = "";
   alpha22.Bar();
   actual = alpha22.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22.", expected, actual, "");           
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23. Final internal method");
            
   var alpha23: Alpha23 = new Alpha23();
   
   expected = "six";
   actual = "";
   alpha23.Bar();
   actual = alpha23.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24. Final public method with subclass");
      
   var alpha24: Alpha24;
   var beta24: Beta24;
   
   // -----------------------------------------------
   alpha24 = new Alpha24();

   expected = "Alpha24.Bar()";
   actual = "";
   alpha24.Bar();
   actual = alpha24.value;
   if (actual != expected)
      apLogFailInfo ("Error in 24 (1)", expected, actual, "");
   
   // -----------------------------------------------
   beta24 = new Beta24();

   expected = "Beta24.Bar()";
   actual = "";
   beta24.Bar();
   actual = beta24.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 24 (2)", expected, actual, "");
   
   // -----------------------------------------------
   alpha24 = new Beta24();
   
   expected = "Alpha24.Bar()";
   actual = "";
   alpha24.Bar();
   actual = alpha24.value;
   if (actual != expected)
      apLogFailInfo ("Error in 24 (3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("25. Final private method with subclass");
      
   var alpha25: Alpha25;
   var beta25: Beta25;
   
   // -----------------------------------------------
   alpha25 = new Alpha25();
   
   // call Bar()
   expected = "Alpha25.Foo()";
   actual = "";
   alpha25.Bar();
   actual = alpha25.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 25(1)", expected, actual, "");
      
   // call Zoo()
   expected = "Alpha25.Foo()";
   actual = "";
   alpha25.Zoo();
   actual = alpha25.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 25(2)", expected, actual, "");
   
   // -----------------------------------------------
   beta25 = new Beta25();
   
   // call Bar()
   expected = "Beta25.Foo()";
   actual = "";
   beta25.Bar();
   actual = beta25.value;
   if (actual != expected)
      apLogFailInfo ("Error in 25(3)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha25.Foo()";
   actual = "";
   beta25.Zoo();
   actual = beta25.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 25(4)", expected, actual, "");
   
   // -----------------------------------------------
   alpha25 = new Beta25();
   
   // call Bar()
   expected = "Beta25.Foo()";
   actual = "";
   alpha25.Bar();
   actual = alpha25.value;
   if (actual != expected)
      apLogFailInfo ("Error in 25(5)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha25.Foo()";
   actual = "";
   alpha25.Zoo();
   actual = alpha25.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 25(6)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26. Final protected method with subclass");
      
   var alpha26: Alpha26;
   var beta26: Beta26;
   
   // -----------------------------------------------
   alpha26 = new Alpha26();
   
   // call Bar()
   expected = "Alpha26.Foo()";
   actual = "";
   alpha26.Bar();
   actual = alpha26.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 26(1)", expected, actual, "");
      
   // call Zoo()
   expected = "Alpha26.Foo()";
   actual = "";
   alpha26.Zoo();
   actual = alpha26.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 26(2)", expected, actual, "");
   
   // -----------------------------------------------
   beta26 = new Beta26();
   
   // call Bar()
   expected = "Beta26.Foo()";
   actual = "";
   beta26.Bar();
   actual = beta26.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26(3)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha26.Foo()";
   actual = "";
   beta26.Zoo();
   actual = beta26.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 26(4)", expected, actual, "");
   
   // -----------------------------------------------
   alpha26 = new Beta26();
   
   // call Bar()
   expected = "Beta26.Foo()";
   actual = "";
   alpha26.Bar();
   actual = alpha26.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26(5)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha26.Foo()";
   actual = "";
   alpha26.Zoo();
   actual = alpha26.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 26(6)", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27. Final internal method with subclass");
      
   var alpha27: Alpha27;
   var beta27: Beta27;
   
   // -----------------------------------------------
   alpha27 = new Alpha27();
   
   // call Bar()
   expected = "Alpha27.Foo()";
   actual = "";
   alpha27.Bar();
   actual = alpha27.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 27(1)", expected, actual, "");
      
   // call Zoo()
   expected = "Alpha27.Foo()";
   actual = "";
   alpha27.Zoo();
   actual = alpha27.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 27(2)", expected, actual, "");
   
   // -----------------------------------------------
   beta27 = new Beta27();
   
   // call Bar()
   expected = "Beta27.Foo()";
   actual = "";
   beta27.Bar();
   actual = beta27.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27(3)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha27.Foo()";
   actual = "";
   beta27.Zoo();
   actual = beta27.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 27(4)", expected, actual, "");
   
   // -----------------------------------------------
   alpha27 = new Beta27();
   
   // call Bar()
   expected = "Beta27.Foo()";
   actual = "";
   alpha27.Bar();
   actual = alpha27.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27(5)", expected, actual, "");
   
   // call Zoo()
   expected = "Alpha27.Foo()";
   actual = "";
   alpha27.Zoo();
   actual = alpha27.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 27(6)", expected, actual, "");            
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("28. Final internal method with package");
   
   var alpha28: Alpha28 = new Alpha28();
   var beta28: Beta28 = new Beta28();

   // -----------------------------------
   expected = "Group28.Alpha28.Foo()";
   actual = "";
   alpha28.Bar();
   actual = alpha28.value;
   if (actual != expected)
      apLogFailInfo ("Error in 28(1)", expected, actual, "");
      
   // -----------------------------------
   // call Zoo1()
   expected = "Group28.Alpha28.Foo()";
   actual = "";
   beta28.Zoo1();
   actual = beta28.temp;
   if (actual != expected)
      apLogFailInfo ("Error in 28(2)", expected, actual, "");
      
   // -----------------------------------
   // call Zoo2()
   expected = "Group28.Alpha28.Foo()";
   actual = "";
   beta28.Zoo2();
   actual = beta28.temp;
   if (actual != expected)
      apLogFailInfo ("Error in 28(3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. Final internal method with package and subclass");
      
   var alpha29: Alpha29;
   var beta29: Beta29;
   
   // -----------------------------------------------
   alpha29 = new Alpha29();
   
   // call Bar()
   expected = "Group29.Alpha29.Foo()";
   actual = "";
   alpha29.Bar();
   actual = alpha29.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 29(1)", expected, actual, "");
      
   // call Zoo()
   expected = "Group29.Alpha29.Foo()";
   actual = "";
   alpha29.Zoo();
   actual = alpha29.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 29(2)", expected, actual, "");
   
   // -----------------------------------------------
   beta29 = new Beta29();
   
   // call Bar()
   expected = "Group29.Beta29.Foo()";
   actual = "";
   beta29.Bar();
   actual = beta29.value;
   if (actual != expected)
      apLogFailInfo ("Error in 29(3)", expected, actual, "");
   
   // call Zoo()
   expected = "Group29.Alpha29.Foo()";
   actual = "";
   beta29.Zoo();
   actual = beta29.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 29(4)", expected, actual, "");
   
   // -----------------------------------------------
   alpha29 = new Beta29();
   
   // call Bar()
   expected = "Group29.Beta29.Foo()";
   actual = "";
   alpha29.Bar();
   actual = alpha29.value;
   if (actual != expected)
      apLogFailInfo ("Error in 29(5)", expected, actual, "");
   
   // call Zoo()
   expected = "Group29.Alpha29.Foo()";
   actual = "";
   alpha29.Zoo();
   actual = alpha29.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 29(6)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30. Final abstract method");
   
   if (exceptionThrown30 == false)
      apLogFailInfo ("No compile error in 30", "Should give a compile error", exceptionThrown30, "");
   if (actualError30 != expectedError30)
      apLogFailInfo ("Wrong compile error in 30", expectedError30, actualError30, "");
   
        
   // -----------------------------------------------------------------------
   apInitScenario ("31. Final final method");
        
   if (exceptionThrown31 == false)
      apLogFailInfo ("No compile error in 31", "Should give a compile error", exceptionThrown31, "");
   if (actualError31 != expectedError31)
      apLogFailInfo ("Wrong compile error in 31", expectedError31, actualError31, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("32. Final expando method");   
   
   var alpha32: Alpha32 = new Alpha32();

   actual = "";
   expected = "hello";
   actual = alpha32.Bar("Boo");
   if (actual != expected)
      apLogFailInfo ("Error in 32(1)", expected, actual, "228345");
      
   actual = "";
   expected = "Boo";
   actual = alpha32.value;
   if (actual != expected)
      apLogFailInfo ("Error in 32(2)", expected, actual, "228345");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("33. Final static method");   

   if (exceptionThrown33 == false)
      apLogFailInfo ("No compile error in 33", "Should give a compile error", exceptionThrown33, "");
   if (actualError33 != expectedError33)
      apLogFailInfo ("Wrong compile error in 33", expectedError33, actualError33, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("34. Final Bar() to final Bar()");   
      
   var alpha34: Alpha34;
   var beta34: Beta34; 
   
   // ------------------------------------
   alpha34 = new Alpha34();
   
   expected = "none";
   actual = "";
   actual = alpha34.value;
   if (actual != expected)
      apLogFailInfo ("Error in 34(1)", expected, actual, "");
      
   expected = "Alpha34.Bar()";
   actual = "";
   alpha34.Bar();
   actual = alpha34.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 34(2)", expected, actual, "");

   // ------------------------------------      
   beta34 = new Beta34();
      
   expected = "none";
   actual = "";
   actual = beta34.value;
   if (actual != expected)
      apLogFailInfo ("Error in 34(3)", expected, actual, "");
      
   expected = "Beta34.Bar()";
   actual = "";
   beta34.Bar();
   actual = beta34.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 34(4)", expected, actual, "");   
      
   // ------------------------------------      
   alpha34 = new Beta34();
   
   expected = "none";
   actual = "";
   actual = alpha34.value;
   if (actual != expected)
      apLogFailInfo ("Error in 34(5)", expected, actual, "");   
      
   expected = "Alpha34.Bar()";
   actual = "";
   alpha34.Bar();
   actual = alpha34.value;
   if (actual != expected)
      apLogFailInfo ("Error in 34(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35. Final Bar() to abstract Bar()");   
      
   var alpha35: Alpha35;
   var beta35: Beta35;
   var charlie35: Charlie35;
   
   // ------------------------------------
   alpha35 = new Alpha35();
   
   expected = "none";
   actual = "";
   actual = alpha35.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(1)", expected, actual, "");
      
   expected = "Alpha35.Bar()";
   actual = "";
   alpha35.Bar();
   actual = alpha35.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 35(2)", expected, actual, "");
   
   // ------------------------------------ 
/*
This test is moved down to 35.1
     
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   exceptionThrown = false;
   
   try
   {
      eval ("beta35 = new Beta35();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 35(3)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 35(4)", expectedError, actualError, "");
*/      
      
   // ------------------------------------   
   charlie35 = new Charlie35();
   
   expected = "none";
   actual = "";
   actual = charlie35.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(5)", expected, actual, "");
      
   expected = "Charlie35.Bar()";
   actual = "";
   charlie35.Bar();
   actual = charlie35.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 35(6)", expected, actual, "");

      
   // ------------------------------------   
/* 
This test is moved down to 35.1
   
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   exceptionThrown = false;
   
   try
   {
      eval ("alpha35 = new Beta35();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 35(7)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 35(8)", expectedError, actualError, ""); 
*/        


   // ------------------------------------   
   alpha35 = new Charlie35();
   
   expected = "none";
   actual = "";
   actual = alpha35.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(9)", expected, actual, "");
      
   expected = "Alpha35.Bar()";
   actual = "";
   alpha35.Bar();
   actual = alpha35.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 35(10)", expected, actual, "");
      
   // ------------------------------------   
   beta35 = new Charlie35();
   
   expected = "none";
   actual = "";
   actual = beta35.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(11)", expected, actual, "");
   
   expected = "Charlie35.Bar()";
   actual = "";
   beta35.Bar();           
   actual = beta35.value;
   if (actual != expected)
      apLogFailInfo ("Error in 35(12)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35.1 Final Bar() to abstract Bar()"); 
   
   if (actualError35_1 != expectedError35_1)
      apLogFailInfo ("Error in 35.1", expectedError35_1, actualError35_1, "");     
      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Final Bar() to static Bar()");   
      
   var alpha36: Alpha36;
   var beta36: Beta36;  
   
   //------------------------------------
   alpha36 = new Alpha36();    
   
   expected = 0;
   actual = -1;
   actual = Alpha36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(1)", expected, actual, "");
      
   expected = 1;
   actual = -1;   
   alpha36.Bar();
   actual = Alpha36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(2)", expected, actual, "");
      
/*      
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'System.Type' do not have such a member";
   try
   {
      eval ("Alpha36.Bar();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 36(3)", "Should give a compile error", exceptionThrown, "294947");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 36(4)", expectedError, actualError, "294947");
*/      
   
   //------------------------------------
/*
Commented out due to change in behavior.
Peter:
   You can access a static member: 
      *  Via the name of the class that declares the static, but not via any of it's subclasses 

   beta36 = new Beta36();
   
   expected = 1;
   actual = -1;
   actual = Beta36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(5)", expected, actual, "");
      
   expected = 3;
   actual = -1;      
   Beta36.Bar();
   actual = Beta36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(6)", expected, actual, "");
      
   expected = 4;
   actual = -1;      
   beta36.Bar();
   actual = Beta36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(7)", expected, actual, "");
*/      
   
   //------------------------------------
   alpha36 = new Beta36();
   
   expected = 1;
   actual = -1;
   actual = Alpha36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(8)", expected, actual, "");
   
   expected = 2;
   actual = -1;      
   alpha36.Bar();
   actual = Alpha36.value;
   if (actual != expected)
      apLogFailInfo ("Error in 36(9)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("37. Final method inside a non-static nested class");   
   
   var alpha37: Alpha37 = new Alpha37();
   var beta37: Alpha37.Beta37;
   var charlie37: Alpha37.Charlie37;
   
   // --------------------------------------
   beta37 = new alpha37.Beta37();
   
   expected = "none";
   actual = "";
   actual = beta37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(1)", expected, actual, "");
   
   expected = "one";
   actual = "";
   beta37.Bar();
   actual = beta37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(2)", expected, actual, "");
      
   // --------------------------------------
   charlie37 = new alpha37.Charlie37();
   
   expected = "none";
   actual = "";
   actual = charlie37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(3)", expected, actual, "");
      
   expected = "two";
   actual = "";
   charlie37.Bar();
   actual = charlie37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(4)", expected, actual, "");
      
   // --------------------------------------
   beta37 = new alpha37.Charlie37();
   
   expected = "none";
   actual = "";
   actual = beta37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(5)", expected, actual, "");

   expected = "one";
   actual = "";
   beta37.Bar();
   actual = beta37.value;
   if (actual != expected)
      apLogFailInfo ("Error in 37(6)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("38. Final method inside a static nested class");   
      
   var alpha38: Alpha38 = new Alpha38
   var beta38: Alpha38.Beta38;
   var charlie38: Alpha38.Charlie38;
   
   // --------------------------------------
   beta38 = new Alpha38.Beta38();
   
   expected = "none";
   actual = "";
   actual = beta38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(1)", expected, actual, "");
   
   expected = "one";
   actual = "";
   beta38.Bar();
   actual = beta38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(2)", expected, actual, "");
      
   // --------------------------------------
   charlie38 = new Alpha38.Charlie38();
   
   expected = "none";
   actual = "";
   actual = charlie38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(3)", expected, actual, "");
      
   expected = "two";
   actual = "";
   charlie38.Bar();
   actual = charlie38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(4)", expected, actual, "");
      
   // --------------------------------------
   beta38 = new Alpha38.Charlie38();
   
   expected = "none";
   actual = "";
   actual = beta38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(5)", expected, actual, "");

   expected = "one";
   actual = "";
   beta38.Bar();
   actual = beta38.value;
   if (actual != expected)
      apLogFailInfo ("Error in 38(6)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("39. Final method inside a static nested class; " +
                       "non-static subclass outside the nesting class; " +
                       "non-static sub-subclass outside the nesting class");  
   
   var beta39: Alpha39.Beta39;
   var charlie39: Charlie39;
   var delta39: Delta39;
      
   // -------------------------------
   beta39 = new Alpha39.Beta39();
   
   expected = "none";
   actual = "";
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(1)", expected, actual, "");   
      
   expected = "one";
   actual = "";
   beta39.Bar();
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(2)", expected, actual, "");   
      
   // -------------------------------
   charlie39 = new Charlie39();
   
   expected = "none";
   actual = "";
   actual = charlie39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(3)", expected, actual, "");   
      
   expected = "two";
   actual = "";
   charlie39.Bar();
   actual = charlie39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(4)", expected, actual, "");   
         
   // -------------------------------
   beta39 = new Charlie39();
         
   expected = "none";
   actual = "";
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(5)", expected, actual, "");   
      
   expected = "one";
   actual = "";
   beta39.Bar();
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(6)", expected, actual, "");   
      
   // -------------------------------
   delta39 = new Delta39();
   
   expected = "none";
   actual = "";
   actual = delta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(7)", expected, actual, "");   
      
   expected = "three";
   actual = "";
   delta39.Bar();
   actual = delta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(8)", expected, actual, "");         
      
   // -------------------------------
   beta39 = new Delta39();   
         
   expected = "none";
   actual = "";
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(9)", expected, actual, "");   
      
   expected = "one";
   actual = "";
   beta39.Bar();
   actual = beta39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(10)", expected, actual, "");
      
   // -------------------------------
   charlie39 = new Delta39();   
         
   expected = "none";
   actual = "";
   actual = charlie39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(11)", expected, actual, "");   
      
   expected = "three";
   actual = "";
   charlie39.Bar();
   actual = charlie39.value;
   if (actual != expected)
      apLogFailInfo ("Error in 39(12)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40. Final functions outside a class");
   
   if (exceptionThrown40 == false)
      apLogFailInfo ("No compile error in 40", "Should give a compile error", exceptionThrown40, "");
   if (actualError40 != expectedError40)
      apLogFailInfo ("Wrong compile error in 40", expectedError40, actualError40, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("41. Final getter and setter");
   
   var alpha41: Alpha41 = new Alpha41();
   
   // ---------------------------------
   expected = 11;
   actual = 0;
   
   alpha41.Price = 10;       // setter
   actual = alpha41.value;
   if (actual != expected)
      apLogFailInfo ("Error in 41(1)", expected, actual, "");   
      
   // ---------------------------------
   expected = 25;
   actual = 0;
   
   alpha41.value = 20;
   actual = alpha41.Price;   // getter
   if (actual != expected) 
      apLogFailInfo ("Error in 41(2)", expected, actual, "");        
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("42. Final getter and setter; subclasses (Beta & Charlie) implement getter and setter");
   
   var alpha42: Alpha42;
   var beta42: Beta42;
   var charlie42: Charlie42;
   
   // ------------------------------------------
   alpha42 = new Alpha42();
   
   expected = 12;
   actual = 0;
   alpha42.value = 10;
   actual = alpha42.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 42(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha42.Price = 20;          // setter
   actual = alpha42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(2)", expected, actual, "");
         
   // ------------------------------------------
   beta42 = new Beta42();
   
   expected = 40;
   actual = 0;
   beta42.value = 30;
   actual = beta42.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 42(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta42.Price = 15;           // setter
   actual = beta42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha42 = new Beta42();
   
   expected = 27;
   actual = 0;
   alpha42.value = 25;
   actual = alpha42.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 42(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha42.Price = 35;          // setter
   actual = alpha42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie42 = new Charlie42();
   
   expected = 9;
   actual = 0;
   charlie42.value = 10;
   actual = charlie42.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 42(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie42.Price = 20;        // setter     
   actual = charlie42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha42 = new Charlie42();
   
   expected = 12;
   actual = 0;
   alpha42.value = 10;
   actual = alpha42.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 42(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha42.Price = 20;        // setter
   actual = alpha42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(10)", expected, actual, "");
   
   // ------------------------------------------
   beta42 = new Charlie42();
         
   expected = 19;
   actual = 0;
   beta42.value = 20;  
   actual = beta42.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 42(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta42.Price = 50;        // setter
   actual = beta42.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 42(12)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("43. Getter is final; setter not final; subclasses (Beta & Charlie) implement getter and setter");
            
   var alpha43: Alpha43;
   var beta43: Beta43;
   var charlie43: Charlie43;
   
   // ------------------------------------------
   alpha43 = new Alpha43();
   
   expected = 12;
   actual = 0;
   alpha43.value = 10;
   actual = alpha43.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 43(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha43.Price = 20;          // setter
   actual = alpha43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(2)", expected, actual, "");
         
   // ------------------------------------------
   beta43 = new Beta43();
   
   expected = 40;
   actual = 0;
   beta43.value = 30;
   actual = beta43.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 43(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta43.Price = 15;           // setter
   actual = beta43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha43 = new Beta43();
   
   expected = 27;
   actual = 0;
   alpha43.value = 25;
   actual = alpha43.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 43(5)", expected, actual, "");
      
   expected = 55;
   actual = 0;
   alpha43.Price = 35;          // setter
   actual = alpha43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie43 = new Charlie43();
   
   expected = 9;
   actual = 0;
   charlie43.value = 10;
   actual = charlie43.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 43(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie43.Price = 20;        // setter     
   actual = charlie43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha43 = new Charlie43();
   
   expected = 12;
   actual = 0;
   alpha43.value = 10;
   actual = alpha43.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 43(9)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   alpha43.Price = 20;        // setter
   actual = alpha43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(10)", expected, actual, "");
   
   // ------------------------------------------
   beta43 = new Charlie43();
         
   expected = 19;
   actual = 0;
   beta43.value = 20;  
   actual = beta43.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 43(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta43.Price = 50;        // setter
   actual = beta43.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 43(12)", expected, actual, "");            
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("44. Getter not final; setter is final; subclasses (Beta & Charlie) implement getter and setter");
         
   var alpha44: Alpha44;
   var beta44: Beta44;
   var charlie44: Charlie44;
   
   // ------------------------------------------
   alpha44 = new Alpha44();
   
   expected = 12;
   actual = 0;
   alpha44.value = 10;
   actual = alpha44.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 44(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha44.Price = 20;          // setter
   actual = alpha44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(2)", expected, actual, "");
         
   // ------------------------------------------
   beta44 = new Beta44();
   
   expected = 40;
   actual = 0;
   beta44.value = 30;
   actual = beta44.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 44(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta44.Price = 15;           // setter
   actual = beta44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha44 = new Beta44();
   
   expected = 35;
   actual = 0;
   alpha44.value = 25;
   actual = alpha44.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 44(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha44.Price = 35;          // setter
   actual = alpha44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie44 = new Charlie44();
   
   expected = 9;
   actual = 0;
   charlie44.value = 10;
   actual = charlie44.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 44(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie44.Price = 20;        // setter     
   actual = charlie44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha44 = new Charlie44();
   
   expected = 9;
   actual = 0;
   alpha44.value = 10;
   actual = alpha44.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 44(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha44.Price = 20;        // setter
   actual = alpha44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(10)", expected, actual, "");
   
   // ------------------------------------------
   beta44 = new Charlie44();
         
   expected = 19;
   actual = 0;
   beta44.value = 20;  
   actual = beta44.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 44(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta44.Price = 50;        // setter
   actual = beta44.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 44(12)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("45. Nested class; non-static classes; final getter and setter; subclasses (Beta & Charlie) implement getter and setter;");
                       
   var nesting45: Nesting45 = new Nesting45();
   var alpha45: Nesting45.Alpha45;
   var beta45: Nesting45.Beta45;
   var charlie45: Nesting45.Charlie45;
   
   // ------------------------------------------
   alpha45 = new nesting45.Alpha45();
   
   expected = 12;
   actual = 0;
   alpha45.value = 10;
   actual = alpha45.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 45(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha45.Price = 20;          // setter
   actual = alpha45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(2)", expected, actual, "");
         
   // ------------------------------------------
   beta45 = new nesting45.Beta45();
   
   expected = 40;
   actual = 0;
   beta45.value = 30;
   actual = beta45.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 45(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta45.Price = 15;           // setter
   actual = beta45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha45 = new nesting45.Beta45();
   
   expected = 27;
   actual = 0;
   alpha45.value = 25;
   actual = alpha45.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 45(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha45.Price = 35;          // setter
   actual = alpha45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie45 = new nesting45.Charlie45();
   
   expected = 9;
   actual = 0;
   charlie45.value = 10;
   actual = charlie45.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 45(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie45.Price = 20;        // setter     
   actual = charlie45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha45 = new nesting45.Charlie45();
   
   expected = 12;
   actual = 0;
   alpha45.value = 10;
   actual = alpha45.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 45(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha45.Price = 20;        // setter
   actual = alpha45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(10)", expected, actual, "");
   
   // ------------------------------------------
   beta45 = new nesting45.Charlie45();
         
   expected = 19;
   actual = 0;
   beta45.value = 20;  
   actual = beta45.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 45(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta45.Price = 50;        // setter
   actual = beta45.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 45(12)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("46. Nested class; non-static classes; getter is final; setter not final; subclasses (Beta & Charlie) implement getter and setter");
               
   var nesting46: Nesting46 = new Nesting46();
   var alpha46: Nesting46.Alpha46;
   var beta46: Nesting46.Beta46;
   var charlie46: Nesting46.Charlie46;
   
   // ------------------------------------------
   alpha46 = new nesting46.Alpha46();
   
   expected = 12;
   actual = 0;
   alpha46.value = 10;
   actual = alpha46.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 46(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha46.Price = 20;          // setter
   actual = alpha46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(2)", expected, actual, "");
         
   // ------------------------------------------
   beta46 = new nesting46.Beta46();
   
   expected = 40;
   actual = 0;
   beta46.value = 30;
   actual = beta46.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 46(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta46.Price = 15;           // setter
   actual = beta46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha46 = new nesting46.Beta46();
   
   expected = 27;
   actual = 0;
   alpha46.value = 25;
   actual = alpha46.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 46(5)", expected, actual, "");
      
   expected = 55;
   actual = 0;
   alpha46.Price = 35;          // setter
   actual = alpha46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie46 = new nesting46.Charlie46();
   
   expected = 9;
   actual = 0;
   charlie46.value = 10;
   actual = charlie46.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 46(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie46.Price = 20;        // setter     
   actual = charlie46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha46 = new nesting46.Charlie46();
   
   expected = 12;
   actual = 0;
   alpha46.value = 10;
   actual = alpha46.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 46(9)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   alpha46.Price = 20;        // setter
   actual = alpha46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(10)", expected, actual, "");
   
   // ------------------------------------------
   beta46 = new nesting46.Charlie46();
         
   expected = 19;
   actual = 0;
   beta46.value = 20;  
   actual = beta46.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 46(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta46.Price = 50;        // setter
   actual = beta46.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 46(12)", expected, actual, "");       
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("47. Nested class; non-static classes; getter not final; setter is final; subclasses (Beta & Charlie) implement getter and setter");
            
   var nesting47: Nesting47 = new Nesting47();
   var alpha47: Nesting47.Alpha47;
   var beta47: Nesting47.Beta47;
   var charlie47: Nesting47.Charlie47;
   
   // ------------------------------------------
   alpha47 = new nesting47.Alpha47();
   
   expected = 12;
   actual = 0;
   alpha47.value = 10;
   actual = alpha47.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 47(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha47.Price = 20;          // setter
   actual = alpha47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(2)", expected, actual, "");
         
   // ------------------------------------------
   beta47 = new nesting47.Beta47();
   
   expected = 40;
   actual = 0;
   beta47.value = 30;
   actual = beta47.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 47(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta47.Price = 15;           // setter
   actual = beta47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha47 = new nesting47.Beta47();
   
   expected = 35;
   actual = 0;
   alpha47.value = 25;
   actual = alpha47.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 47(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha47.Price = 35;          // setter
   actual = alpha47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie47 = new nesting47.Charlie47();
   
   expected = 9;
   actual = 0;
   charlie47.value = 10;
   actual = charlie47.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 47(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie47.Price = 20;        // setter     
   actual = charlie47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha47 = new nesting47.Charlie47();
   
   expected = 9;
   actual = 0;
   alpha47.value = 10;
   actual = alpha47.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 47(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha47.Price = 20;        // setter
   actual = alpha47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(10)", expected, actual, "");
   
   // ------------------------------------------
   beta47 = new nesting47.Charlie47();
         
   expected = 19;
   actual = 0;
   beta47.value = 20;  
   actual = beta47.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 47(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta47.Price = 50;        // setter
   actual = beta47.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 47(12)", expected, actual, "");          
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("48. Nested class; static classes; final getter and setter; subclasses (Beta & Charlie) implement getter and setter;");
       
   var alpha48: Nesting48.Alpha48;
   var beta48: Nesting48.Beta48;
   var charlie48: Nesting48.Charlie48;

   // ------------------------------------------
   alpha48 = new Nesting48.Alpha48();
   
   expected = 12;
   actual = 0;
   alpha48.value = 10;
   actual = alpha48.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 48(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha48.Price = 20;          // setter
   actual = alpha48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(2)", expected, actual, "");
         
   // ------------------------------------------
   beta48 = new Nesting48.Beta48();
   
   expected = 40;
   actual = 0;
   beta48.value = 30;
   actual = beta48.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 48(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta48.Price = 15;           // setter
   actual = beta48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha48 = new Nesting48.Beta48();
   
   expected = 27;
   actual = 0;
   alpha48.value = 25;
   actual = alpha48.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 48(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha48.Price = 35;          // setter
   actual = alpha48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie48 = new Nesting48.Charlie48();
   
   expected = 9;
   actual = 0;
   charlie48.value = 10;
   actual = charlie48.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 48(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie48.Price = 20;        // setter     
   actual = charlie48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha48 = new Nesting48.Charlie48();
   
   expected = 12;
   actual = 0;
   alpha48.value = 10;
   actual = alpha48.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 48(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha48.Price = 20;        // setter
   actual = alpha48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(10)", expected, actual, "");
   
   // ------------------------------------------
   beta48 = new Nesting48.Charlie48();
         
   expected = 19;
   actual = 0;
   beta48.value = 20;  
   actual = beta48.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 48(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta48.Price = 50;        // setter
   actual = beta48.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 48(12)", expected, actual, "");       
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("49. Nested class; static classes; getter is final; setter not final; subclasses (Beta & Charlie) implement getter and setter");
          
   var alpha49: Nesting49.Alpha49;
   var beta49: Nesting49.Beta49;
   var charlie49: Nesting49.Charlie49;
   
   // ------------------------------------------
   alpha49 = new Nesting49.Alpha49();
   
   expected = 12;
   actual = 0;
   alpha49.value = 10;
   actual = alpha49.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 49(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha49.Price = 20;          // setter
   actual = alpha49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(2)", expected, actual, "");
         
   // ------------------------------------------
   beta49 = new Nesting49.Beta49();
   
   expected = 40;
   actual = 0;
   beta49.value = 30;
   actual = beta49.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 49(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta49.Price = 15;           // setter
   actual = beta49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha49 = new Nesting49.Beta49();
   
   expected = 27;
   actual = 0;
   alpha49.value = 25;
   actual = alpha49.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 49(5)", expected, actual, "");
      
   expected = 55;
   actual = 0;
   alpha49.Price = 35;          // setter
   actual = alpha49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie49 = new Nesting49.Charlie49();
   
   expected = 9;
   actual = 0;
   charlie49.value = 10;
   actual = charlie49.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 49(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie49.Price = 20;        // setter     
   actual = charlie49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha49 = new Nesting49.Charlie49();
   
   expected = 12;
   actual = 0;
   alpha49.value = 10;
   actual = alpha49.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 49(9)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   alpha49.Price = 20;        // setter
   actual = alpha49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(10)", expected, actual, "");
   
   // ------------------------------------------
   beta49 = new Nesting49.Charlie49();
         
   expected = 19;
   actual = 0;
   beta49.value = 20;  
   actual = beta49.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 49(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta49.Price = 50;        // setter
   actual = beta49.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 49(12)", expected, actual, "");      
         
         
   // -----------------------------------------------------------------------
   apInitScenario ("50. Nested class; static classes; getter not final; setter is final; subclasses (Beta & Charlie) implement getter and setter");
              
   var alpha50: Nesting50.Alpha50;
   var beta50: Nesting50.Beta50;
   var charlie50: Nesting50.Charlie50;
   
   // ------------------------------------------
   alpha50 = new Nesting50.Alpha50();
   
   expected = 12;
   actual = 0;
   alpha50.value = 10;
   actual = alpha50.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 50(1)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha50.Price = 20;          // setter
   actual = alpha50.value;
   if (actual != expected)
      apLogFailInfo ("Error in 50(2)", expected, actual, "");
         
   // ------------------------------------------
   beta50 = new Nesting50.Beta50();
   
   expected = 40;
   actual = 0;
   beta50.value = 30;
   actual = beta50.Price;       // getter
   if (actual != expected)
      apLogFailInfo ("Error in 50(3)", expected, actual, "");
      
   expected = 35;
   actual = 0;
   beta50.Price = 15;           // setter
   actual = beta50.value;
   if (actual != expected)
      apLogFailInfo ("Error in 50(4)", expected, actual, "");
   
   // ------------------------------------------
   alpha50 = new Nesting50.Beta50();
   
   expected = 35;
   actual = 0;
   alpha50.value = 25;
   actual = alpha50.Price;      // getter
   if (actual != expected)
      apLogFailInfo ("Error in 50(5)", expected, actual, "");
      
   expected = 36;
   actual = 0;
   alpha50.Price = 35;          // setter
   actual = alpha50.value;
   if (actual != expected)
      apLogFailInfo ("Error in 50(6)", expected, actual, "");
      
   // ------------------------------------------
   charlie50 = new Nesting50.Charlie50();
   
   expected = 9;
   actual = 0;
   charlie50.value = 10;
   actual = charlie50.Price;    // getter      
   if (actual != expected)
      apLogFailInfo ("Error in 50(7)", expected, actual, "");
      
   expected = 18;
   actual = 0;
   charlie50.Price = 20;        // setter     
   actual = charlie50.value;
   if (actual != expected)
      apLogFailInfo ("Error in 50(8)", expected, actual, "");
   
   // ------------------------------------------
   alpha50 = new Nesting50.Charlie50();
   
   expected = 9;
   actual = 0;
   alpha50.value = 10;
   actual = alpha50.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 50(9)", expected, actual, "");
      
   expected = 21;
   actual = 0;
   alpha50.Price = 20;        // setter
   actual = alpha50.value;
   if (actual != expected)
      apLogFailInfo ("Error in 50(10)", expected, actual, "");
   
   // ------------------------------------------
   beta50 = new Nesting50.Charlie50();
         
   expected = 19;
   actual = 0;
   beta50.value = 20;  
   actual = beta50.Price;    // getter
   if (actual != expected)
      apLogFailInfo ("Error in 50(11)", expected, actual, "");
   
   expected = 48;
   actual = 0;
   beta50.Price = 50;        // setter
   actual = beta50.value;   
   if (actual != expected)
      apLogFailInfo ("Error in 50(12)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51. Final fields");
      
   if (exceptionThrown51 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown51, "");
   if (actualError51 != expectedError51)
      apLogFailInfo ("Wrong compile error message", expectedError51, actualError51, "");  
 
   apEndTest();
}



final01();


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
