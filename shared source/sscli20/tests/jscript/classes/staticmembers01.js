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


var iTestID = 136501;


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


// -----------------------------------------------------------------------
class Alpha1
{
   public static var counter = 10;
   
   public function ModifyCounter (x: int)
   {
      counter = x;
   }
}

class Beta1 extends Alpha1
{
}

class Charlie1 extends Beta1
{
}


// -----------------------------------------------------------------------
var expectedError1_2, actualError1_2: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha1_2                                  " +
         "  {                                               " +
         "     public static var counter = 20;              " +
         "     public function ModifyCounter (x: int)       " +
         "     {                                            " +
         "        counter = x;                              " +
         "     }                                            " +
         "  }                                               " +
         "  class Beta1_2 extends Alpha1_2 { }              " +
         "  class Charlie1_2 extends Beta1_2 { }            " +
         "  var x = Beta1_2.counter;                        " +
         "  var y = Charlie1_2.counter;                     ";


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

actualError1_2 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError1_2 = "test.js(1,519) : error JS1246: Type 'Beta1_2' does not have such a static member" + Environment.NewLine + "" +
                   "test.js(1,572) : error JS1246: Type 'Charlie1_2' does not have such a static member" + Environment.NewLine + "";
@else
expectedError1_2 = "test.js(1,519) : error JS1246: Type 'Beta1_2' does not have such a static member" + Environment.NewLine + "" +
                   "test.js(1,572) : error JS1246: Type 'Charlie1_2' does not have such a static member" + Environment.NewLine + "";
@end
                   
// -----------------------------------------------------------------------
// This class declaration is used by scenarios 2.x (2.1, 2.2, ...)
class Alpha2
{
   public var instanceField = 1;
   public static var classField = 2;
   
   public function InstanceMethod()
   {
      return 3;
   }
   
   public static function ClassMethod()
   {
      return 4;
   }
}

class Beta2 extends Alpha2
{
}


// -----------------------------------------------------------------------
var expectedError2_1, actualError2_1: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha2_1                               " +
         "  {                                            " +
         "     public var instanceField = 1;             " +
         "     public static var classField = 2;         " +
         "     public function InstanceMethod()          " +
         "     {                                         " +
         "        return 3;                              " +
         "     }                                         " +
         "     public static function ClassMethod()      " +
         "     {                                         " +
         "        return 4;                              " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta2_1 extends Alpha2_1               " +
         "  {                                            " +
         "  }                                            " +
         "  var x;                                       " +
         "  x = Alpha2_1.instanceField;                  " +
         "  x = Alpha2_1.InstanceMethod();               " +
         "  x = Beta2_1.instanceField;                   " +
         "  x = Beta2_1.classField;                      " +
         "  x = Beta2_1.InstanceMethod();                " +
         "  x = Beta2_1.ClassMethod();                   ";
         

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
expectedError2_1 = "test.js(1,815) : error JS1245: The non-static member 'instanceField' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,862) : error JS1245: The non-static member 'InstanceMethod' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,908) : error JS1245: The non-static member 'instanceField' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,955) : error JS1246: Type 'Beta2_1' does not have such a static member" + Environment.NewLine + "" +
                   "test.js(1,1002) : error JS1245: The non-static member 'InstanceMethod' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,1049) : error JS1246: Type 'Beta2_1' does not have such a static member" + Environment.NewLine + "";
@else
expectedError2_1 = "test.js(1,815) : error JS1245: The non-static member 'instanceField' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,862) : error JS1245: The non-static member 'InstanceMethod' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,908) : error JS1245: The non-static member 'instanceField' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,955) : error JS1246: Type 'Beta2_1' does not have such a static member" + Environment.NewLine + "" +
                   "test.js(1,1002) : error JS1245: The non-static member 'InstanceMethod' cannot be accessed with the class name" + Environment.NewLine + "" +
                   "test.js(1,1049) : error JS1246: Type 'Beta2_1' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var exceptionThrown2_7 = false;


var actualError2_7 = "";
var expectedError2_7 = "ReferenceError: The non-static member 'Bar' cannot be accessed with the class name";

try
{
   eval ("  class Alpha2_7                   " +
         "  {                                " +
         "     public function Bar()         " +
         "     {  }                          " +
         "  }                                " +
         "  class Beta2_7 extends Alpha2_7   " +
         "  {  }                             " +
         "                                   " +
         "  var temp = Beta2_7.Bar();        ", "unsafe");
}
catch (error)
{
   exceptionThrown2_7 = true;
   actualError2_7 = error;

}


// -----------------------------------------------------------------------
class Alpha2_17
{
   private static var value: int = 10;
   
   public static function get Price()
   {
      return value;
   }
   
   public static function set Price (newPrice)
   {
      value = newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown2_18_1 = false;
var actualError2_18_1 = "";
var expectedError2_18_1 = "ReferenceError: Type 'Beta2_18_1' does not have such a static member";

try
{
   eval ("  class Alpha2_18_1                                  " +
         "  {                                                  " +
         "     private static var value: int = 10;             " +
         "     public static function get Price(): int         " +
         "     {                                               " +
         "        return value;                                " +
         "     }                                               " +
         "     public static function set Price (newPrice: int)" +
         "     {                                               " +
         "        value = newPrice;                            " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta2_18_1 extends Alpha2_18_1               " +
         "  {  }                                               " +
         "  var x = Beta2_18_1.Price;                          ", "unsafe");
}
catch (error)
{
   exceptionThrown2_18_1 = true;
   actualError2_18_1 = error;
}

var exceptionThrown2_18_2 = false;
var actualError2_18_2 = "";
var expectedError2_18_2 = "ReferenceError: Type 'Beta2_18_2' does not have such a static member";

try
{
   eval ("  class Alpha2_18_2                                  " +
         "  {                                                  " +
         "     private static var value: int = 10;             " +
         "     public static function get Price(): int         " +
         "     {                                               " +
         "        return value;                                " +
         "     }                                               " +
         "     public static function set Price (newPrice: int)" +
         "     {                                               " +
         "        value = newPrice;                            " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta2_18_2 extends Alpha2_18_2               " +
         "  {  }                                               " +
         "  Beta2_18_2.Price = 20;                             ", "unsafe");
}
catch (error)
{
   exceptionThrown2_18_2 = true;
   actualError2_18_2 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown2_19_1 = false;
var actualError2_19_1 = "";
var expectedError2_19_1 = "ReferenceError: The static member 'Price' must be accessed with the class name";

try
{
   eval ("  class Alpha2_19_1                              " +
         "  {                                            " +
         "     private static var value: int = 10;       " +
         "     public static function get Price()        " +
         "     {                                         " +
         "        return value;                          " +
         "     }                                         " +
         "     public static function set Price (newPrice)  " +
         "     {                                         " +
         "        value = newPrice;                      " +
         "     }                                         " +
         "  }                                            " +
         "  var alpha2_19_1: Alpha2_19_1 = new Alpha2_19_1();  " +
         "  var x;                                       " +
         "  x = alpha2_19_1.Price;                         ", "unsafe");
}
catch (error)
{
   exceptionThrown2_19_1 = true;
   actualError2_19_1 = error;
}

var exceptionThrown2_19_2 = false;
var actualError2_19_2 = "";
var expectedError2_19_2 = "ReferenceError: The static member 'Price' must be accessed with the class name";

try
{
   eval ("  class Alpha2_19_2                            " +
         "  {                                            " +
         "     private static var value: int = 10;       " +
         "     public static function get Price()        " +
         "     {                                         " +
         "        return value;                          " +
         "     }                                         " +
         "     public static function set Price (newPrice)  " +
         "     {                                         " +
         "        value = newPrice;                      " +
         "     }                                         " +
         "  }                                            " +
         "  var alpha2_19_2: Alpha2_19_2 = new Alpha2_19_2();  " +
         "  alpha2_19_2.Price = 20;                      ", "unsafe");
}
catch (error)
{
   exceptionThrown2_19_2 = true;
   actualError2_19_2 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown2_20_1 = false;
var actualError2_20_1 = "";
var expectedError2_20_1 = "ReferenceError: The static member 'Price' must be accessed with the class name";

try
{
   eval ("  class Alpha2_20_1                            " +
         "  {                                            " +
         "     private static var value: int = 10;       " +
         "     public static function get Price()        " +
         "     {                                         " +
         "        return value;                          " +
         "     }                                         " +
         "     public static function set Price (newPrice)  " +
         "     {                                         " +
         "        value = newPrice;                      " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta2_20_1 extends Alpha2_20_1         " + 
         "  {  }                                         " +
         "  var beta2_20_1: Beta2_20_1 = new Beta2_20_1();  " +
         "  var x;                                       " +
         "  x = beta2_20_1.Price;                        ", "unsafe");
}
catch (error)
{
   exceptionThrown2_20_1 = true;
   actualError2_20_1 = error;
}

var exceptionThrown2_20_2 = false;
var actualError2_20_2 = "";
var expectedError2_20_2 = "ReferenceError: The static member 'Price' must be accessed with the class name";

try
{
   eval ("  class Alpha2_20_2                            " +
         "  {                                            " +
         "     private static var value: int = 10;       " +
         "     public static function get Price()        " +
         "     {                                         " +
         "        return value;                          " +
         "     }                                         " +
         "     public static function set Price (newPrice)  " +
         "     {                                         " +
         "        value = newPrice;                      " +
         "     }                                         " +
         "  }                                            " +
         "  class Beta2_20_2 extends Alpha2_20_2         " + 
         "  {  }                                         " +
         "  var beta2_20_2: Beta2_20_2 = new Beta2_20_2();  " +
         "  beta2_20_2.Price = 20;                       ", "unsafe");
}
catch (error)
{
   exceptionThrown2_20_2 = true;
   actualError2_20_2 = error;
}


// -----------------------------------------------------------------------
class Alpha3
{
   private var instanceField = 1;
   private static var classField = 2;
   
   public function InstanceMethodAccessingInstanceField()
   {
      var x = 0;
      x = instanceField;
      return x;
   }
   
   public function InstanceMethodAccessingClassField()
   {
      var x = 0;
      x = classField;
      return x;
   }
/*   
   public static function ClassMethodAccessingInstanceField()
   {
      var x = 0;
      actual = 0;
      exceptionThrown = false;
      actualError = "";
      expectedError = "ReferenceError: Variable 'instanceField' has not been declared";
      
      try
      {
         eval ("x = instanceField;");
      }
      catch (error)
      {
         exceptionThrown = true;
         actualError = error;
      }
      
      if (exceptionThrown == false)
         apLogFailInfo ("No compile error in 3.3", "Should give an error", exceptionThrown, "");
      if (actualError != expectedError)
         apLogFailInfo ("Wrong compile error in 3.3", expectedError, actualError, "200532");
      if (x == 1)
         apLogFailInfo ("Class methods can access instance fields", "Shouldn't be accessible", x, "");
   }
*/   
   public static function ClassMethodAccessingClassField()
   {
      var x = 0;
      x = classField;
      return x;
   }
}


// -----------------------------------------------------------------------
var expectedError3_3, actualError3_3: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha3_1                         " +
         "  {                                      " +
         "     private var instanceField = 1;      " +
         "     public static function Bar()        " +
         "     {                                   " +
         "        var x = 0;                       " +
         "        x = instanceField;               " +
         "     }                                   " +
         "  }                                      ";


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

actualError3_3 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError3_3 = "test.js(1,259) : error JS1243: Non-static member 'instanceField' is not accessible from a static scope" + Environment.NewLine + "";
@else
expectedError3_3 = "test.js(1,259) : error JS1243: Non-static member 'instanceField' is not accessible from a static scope" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha3_5
{
   private static var value: int = 10;
   
   public static function get Price(): int
   {
      return value;
   }
   
   public static function set Price (newPrice: int)
   {
      value = newPrice;
   }
   
   public function BarGet(): int
   {
      var x: int;
      x = Price;
      return x;
   }
   
   public function BarSet (newPrice)
   {
      Price = newPrice;
   }
}


// -----------------------------------------------------------------------
class Alpha3_6
{
   private static var value: int = 10;
   
   public static function get Price(): int
   {
      return value;
   }
   
   public static function set Price (newPrice: int)
   {
      value = newPrice;
   }
   
   public static function BarGet(): int
   {
      var x: int;
      x = Price;
      return x;
   }
   
   public static function BarSet (newPrice)
   {
      Price = newPrice;
   }
}




// -----------------------------------------------------------------------
class Alpha4_1                                  
{                                               
   public static var classField: String = "One";    
}                                               

class Beta4_1 extends Alpha4_1                  
{                                               
   public function Bar(): String                        
   {                                            
      var temp;                                 
      temp = classField;        
      return temp;                
   }                                            
}                                              


// -----------------------------------------------------------------------
class Alpha4_2
{
   public static var classField: String = "Two";
}

class Beta4_2 extends Alpha4_2
{
   public function Bar(): String
   {
      var temp;
      temp = Alpha4_2.classField;
      return temp;
   }
}


// -----------------------------------------------------------------------
class Alpha4_3                                  
{                                               
   public static function Foo(): String         
   {                                            
      return "Three";                         
   }                                            
}                                               
class Beta4_3 extends Alpha4_3                  
{                                               
   public function Bar(): String                
   {                                            
      var temp: String = "";                  
      temp = Foo();                             
      return temp;                              
   }                                            
}                                              


// -----------------------------------------------------------------------
class Alpha4_4
{
   public static function Foo(): String
   {  
      return "Four";
   }    
}

class Beta4_4 extends Alpha4_4
{
   public function Bar(): String
   {
      var temp: String = "";
      temp = Alpha4_4.Foo();
      return temp;
   }
}


// -----------------------------------------------------------------------
class Alpha4_5                                  
{                                               
   protected static var value: String = "Five";  
}                                               
class Beta4_5 extends Alpha4_5                  
{                                               
   public static function Bar(): String         
   {                                            
      var temp: String = "";                  
      temp = value;                             
      return temp;                              
   }                                            
}                                              


// -----------------------------------------------------------------------
class Alpha4_6
{
   public static var value: String = "Six";
}

class Beta4_6 extends Alpha4_6
{
   public static function Bar(): String
   {
      var temp: String = "";
      temp = Alpha4_6.value;
      return temp;
   }
}


// -----------------------------------------------------------------------
class Alpha4_7
{
   public static function Foo(): String
   {
      return "Seven";
   }
}

class Beta4_7 extends Alpha4_7
{
   public static function Bar(): String
   {
      var temp: String = "";
      temp = Foo();
      return temp;
   }
}


// -----------------------------------------------------------------------
class Alpha4_8
{
   public static function Foo(): String
   {
      return "Eight";
   }
}

class Beta4_8 extends Alpha4_8
{
   public static function Bar(): String
   {
      var temp: String = "";
      temp = Alpha4_8.Foo();
      return temp;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown5_1 = false;
var actualError5_1 = "";
var expectedError5_1 = "ReferenceError: The static member 'value' must be accessed with the class name";

try
{
   eval ("  class Alpha5_1                                  " +
         "  {                                               " +
         "     public static var value: String = \"One\";   " +
         "  }                                               " +
         "  class Beta5_1 extends Alpha5_1                  " +
         "  {                                               " +
         "     public function Bar()                        " +
         "     {                                            " +
         "        var temp: String = \"\";                  " +
         "        temp = super.value;                       " +
         "     }                                            " +
         "  }                                               ", "unsafe");
}
catch (error)
{
   exceptionThrown5_1 = true;
   actualError5_1 = error;
}


// -----------------------------------------------------------------------
function staticmembers01()
{
   apInitTest ("StaticMembers01");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Single copy of static fields?");
   
   var redAlpha1: Alpha1 = new Alpha1();
   var blueAlpha1: Alpha1 = new Alpha1();

   var redBeta1: Beta1 = new Beta1();
   var blueBeta1: Beta1 = new Beta1();
   
   var redCharlie1: Charlie1 = new Charlie1();
   var blueCharlie1: Charlie1 = new Charlie1();

   /*
      *  Check if static fields accessed through the following have the same values:
         o  base class
         o  subclass 
            - Removed because of change in design.  Now gives a compile error.  Will be checked below.         
   */
   
   expected = 10;
   
   // base class
   actual = 0;
   actual = Alpha1.counter;
   if (actual != expected)
      apLogFailInfo ("Error in 1(1)", expected, actual, "");

   /*
      *  Modify the static field through the base class.  
         Check if static fields accessed through the following still have the same values:
         o  base class 
         o  subclass 
            - Removed because of change in design.  Now gives a compile error.  Already checked above
   */
   
   Alpha1.counter = 15;
   expected = 15;
   
   // base class
   actual = 0;
   actual = Alpha1.counter;
   if (actual != expected)
      apLogFailInfo ("Modified the static field through the base class, Error in accessing static fields through the base class itself", expected, actual, "");
                     
   /*
      *  Modify the static field through an instance method.  
         Check if static fields accessed through the following still have the same values:
         o  base class 
         o  subclass
               - Removed because of change in design.  Now gives a compile error.  Already checked above
   */                              
  
   redAlpha1.ModifyCounter (40);
   expected = 40;
   
   // base class 
   actual = 0;
   actual = Alpha1.counter;
   if (actual != expected)
      apLogFailInfo ("Modified the static field through an instance method, Error in accessing static fields through the base class itself", expected, actual, "");
      
   /*                     
   // subclass
      - Removed because of change in design.  Now gives a compile error.  Already checked above
   
   actual = 0;
   actual = Beta1.counter;
   if (actual != expected)
      apLogFailInfo ("Modified the static field through an instance method, Error in accessing static fields through the subclass itself -- Beta1", expected, actual, "");

   actual = 0;
   actual = Charlie1.counter;
   if (actual != expected)
      apLogFailInfo ("Modified the static field through an instance method, Error in accessing static fields through the subclass itself -- Charlie1", expected, actual, "");   
   */        
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("1.2 Access a static field through subclasses");
      
   if (actualError1_2 != expectedError1_2)
      apLogFailInfo ("Error in 1.2", expectedError1_2, actualError1_2, "");
                     
                     
   // -----------------------------------------------------------------------
   // These instantiations are used by 2.2, 2.4
   var alpha2: Alpha2 = new Alpha2();
   var beta2: Beta2 = new Beta2();

   
   // -----------------------------------------------------------------------
   apInitScenario ("2.1 base class access an instance field; base class accesses an instance method; subclass accesses an instance field; subclass accesses a class field");
   // base class -> instance field
   // base class -> instance method
   // subclass -> instance field
   // subclass -> class field
   // subclass -> instance method
   // subclass -> class method
   
   
   
   if (actualError2_1 != expectedError2_1)
      apLogFailInfo ("Error in 2.1", expectedError2_1, actualError2_1, "");

                     
   // -----------------------------------------------------------------------
   apInitScenario ("2.2 Base class accesses a class field");
   // base class -> class field
   expected = 2;
   actual = 0;
   actual = Alpha2.classField;
   if (actual != expected)
      apLogFailInfo ("Error in 2.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.4 Base class accesses a class method");         
   // base class -> class method
   expected = 4;
   actual = 0;
   actual = Alpha2.ClassMethod();
   if (actual != expected)
      apLogFailInfo ("Error in 2.4", expected, actual, "");
      


   // -----------------------------------------------------------------------
   apInitScenario ("2.8 Subclass accesses a class method");   
   // subclass -> class method

/*   
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Type 'Beta2' does not have such a static member"   
   try
   {
      eval ("actual = Beta2.ClassMethod();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error
   }  
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 2.8", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 2.8", expectedError, actualError, "");    
*/      
   

   // -----------------------------------------------------------------------
   apInitScenario ("2.9 Base class object accesses an instance field");
   // base class object -> instance field
   alpha2 = new Alpha2();
   actual = 0;
   expected = 1;
   actual = alpha2.instanceField;
   if (actual != expected)
      apLogFailInfo ("Error in 2.9", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("2.10 Base class object accesses a class field");
   // base class object -> class field
   alpha2 = new Alpha2();
   actual = 0;
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Alpha2' do not have such a member";
   
   try
   {
      eval ("actual = alpha2.classField;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (actual == 2)
      apLogFailInfo ("Instances can access class/static fields through eval", "Shouldn't be accessible", actual, "179249, 161976");
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 2.10", "Should give a compile error", exceptionThrown, "179249, 161976");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 2.10", expectedError, actualError, "179249, 161976");


   // -----------------------------------------------------------------------
   apInitScenario ("2.11 Base class object accesses an instance method");
   // base class object -> instance method
   alpha2 = new Alpha2();
   actual = 0;
   expected = 3;
   actual = alpha2.InstanceMethod();
   if (actual != expected)
      apLogFailInfo ("Error in 2.11", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.12 Base class object accesses a class method");
   // base class object -> class method
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Alpha2' do not have such a member";
   alpha2 = new Alpha2();
   
   try
   {
      eval ("actual = alpha2.ClassMethod();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 2.12", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 2.12", expectedError, actualError, "");
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.13 Subclass object accesses an instance field");
   // subclass object -> instance field
   beta2 = new Beta2();
   actual = 0;
   expected = 1;
   actual = beta2.instanceField;
   if (actual != expected)
      apLogFailInfo ("Error in 2.13", expected, actual, "");
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.14 Subclass object accesses a class field");
   // subclass object -> class field
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Beta2' do not have such a member";
   beta2 = new Beta2();
   
   try
   {
      eval ("actual = beta2.classField;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 2.14", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 2.14", expectedError, actualError, "");      

   // -----------------------------------------------------------------------
   apInitScenario ("2.15 Subclass object accesses an instance method");
   // subclass object -> instance method
   beta2 = new Beta2();
   actual = 0;
   expected = 3;
   actual = beta2.InstanceMethod();
   if (actual != expected)
      apLogFailInfo ("Error in 2.15", expected, actual, "");
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.16 Subclass object accesses a class method");
   // subclass object -> class method
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Beta2' do not have such a member";
   beta2 = new Beta2();
   
   try
   {
      eval ("actual = beta2.ClassMethod();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 2.16", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 2.16", expectedError, actualError, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.17 Base class accessing properties");
   // base class -> properties
   
   expected = 10;
   actual = 0;
   actual = Alpha2_17.Price;
   if (actual != expected) 
      apLogFailInfo ("Error in 2.17(1)", expected, actual, "299605");
      
   expected = 20;
   actual = 0;
   Alpha2_17.Price = 20;
   actual = Alpha2_17.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 2.18(2)", expected, actual, "299605");


   // -----------------------------------------------------------------------
   apInitScenario ("2.18 Subclass accessing properties");
   // subclass -> properties
   
   if (exceptionThrown2_18_1 == false)
      apLogFailInfo ("No compile error in 2.18(1)", "Should give a compile error", exceptionThrown2_18_1, "");
   if (actualError2_18_1 != expectedError2_18_1)
      apLogFailInfo ("Wrong compile error in 2.18(1)", expectedError2_18_1, actualError2_18_1, "");
      
   if (exceptionThrown2_18_2 == false)
      apLogFailInfo ("No compile error in 2.18(2)", "Should give a compile error", exceptionThrown2_18_2, "");
   if (actualError2_18_2 != expectedError2_18_2)
      apLogFailInfo ("Wrong compile error in 2.18(2)", expectedError2_18_2, actualError2_18_2, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.19 Base class object accessing properties");
   // Base class object -> properties
           
   if (exceptionThrown2_19_1 == false)
      apLogFailInfo ("No compile error in 2.19(1)", "Should give a compile error", exceptionThrown2_19_1, "");
   if (actualError2_19_1 != expectedError2_19_1)
      apLogFailInfo ("Wrong compile error in 2.19(1)", expectedError2_19_1, actualError2_19_1, "");
      
   if (exceptionThrown2_19_2 == false)
      apLogFailInfo ("No compile error in 2.19(2)", "Should give a compile error", exceptionThrown2_19_2, "");
   if (actualError2_19_2 != expectedError2_19_2)
      apLogFailInfo ("Wrong compile error in 2.19(2)", expectedError2_19_2, actualError2_19_2, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2.20 Subclass object accessing properties");
   // Subclass object -> properties
      
   if (exceptionThrown2_20_1 == false)
      apLogFailInfo ("No compile error in 2.20(1)", "Should give a compile error", exceptionThrown2_20_1, "");
   if (actualError2_20_1 != expectedError2_20_1)
      apLogFailInfo ("Wrong compile error in 2.20(1)", expectedError2_20_1, actualError2_20_1, "");
      
   if (exceptionThrown2_20_2 == false)
      apLogFailInfo ("No compile error in 2.20(2)", "Should give a compile error", exceptionThrown2_20_2, "");
   if (actualError2_20_2 != expectedError2_20_2)
      apLogFailInfo ("Wrong compile error in 2.20(2)", expectedError2_20_2, actualError2_20_2, ""); 
                   

   // -----------------------------------------------------------------------
   // This instantiation is used by 3.x (3.1, 3.2, ...)   
   var alpha3: Alpha3;
   
   // -----------------------------------------------------------------------
   apInitScenario ("3.1 Instance method accesses an instance field");
   // instance method -> instance field
   alpha3 = new Alpha3();
   actual = 0;
   expected = 1;
   actual = alpha3.InstanceMethodAccessingInstanceField();
   if (actual != expected) 
      apLogFailInfo ("Error in 3.1", expected, actual, "");
   
   // -----------------------------------------------------------------------
   apInitScenario ("3.2 Instance method accesses a class field");   
   // instance method -> class field
   alpha3 = new Alpha3();
   actual = 0;
   expected = 2;
   actual = alpha3.InstanceMethodAccessingClassField();
   if (actual != expected)
      apLogFailInfo ("Error in 3.2", expected, actual, "");
   
   // -----------------------------------------------------------------------
   apInitScenario ("3.3 Class method accesses an instance field");
   // class method -> instance field
   // compile error
   if (actualError3_3 != expectedError3_3)
      apLogFailInfo ("Error in 3.3", expectedError3_3, actualError3_3, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("3.4 Class method accesses a class field");
   // class method -> class field
   actual = 0;
   expected = 2;
   actual = Alpha3.ClassMethodAccessingClassField();
   if (actual != expected) 
      apLogFailInfo ("Error in 3.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.5 Instance method accesses properties");
   // instance method -> properties
   
   var alpha3_5: Alpha3_5 = new Alpha3_5();
   
   expected = 10;
   actual = 0;
   actual = alpha3_5.BarGet();
   if (actual != expected)
      apLogFailInfo ("Error in 3.5(1)", expected, actual, "");
      
   expected = 20;
   actual = 0;
   alpha3_5.BarSet(20);
   actual = Alpha3_5.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 3.5(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3.6 Class method accesses properties");
   // class method -> properties
   
   expected = 10;
   actual = 0;
   actual = Alpha3_6.BarGet();
   if (actual != expected)
      apLogFailInfo ("Error in 3.6(1)", expected, actual, "");
      
   expected = 20;
   actual = 0;
   Alpha3_6.BarSet(20);
   actual = Alpha3_6.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 3.6(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.1 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' instance method -> Base class' class field w/o classname");

   var beta4_1: Beta4_1 = new Beta4_1();
   
   expected = "One";
   actual = "";
   actual = beta4_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.1", expected, actual, "297427");                    
                          

   // -----------------------------------------------------------------------
   apInitScenario ("4.2 Check subclass' instance and class methods accessing base class' class fields and methods: Subclass' instance method -> Base class' class field w/ classname");
      
   var beta4_2: Beta4_2 = new Beta4_2();
   
   expected = "Two";
   actual = "";
   actual = beta4_2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.3 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' instance method -> Base class' class method w/o classname");
   
   var beta4_3: Beta4_3 = new Beta4_3();
   
   expected = "Three";
   actual = "";
   actual = beta4_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.3", expected, actual, "297427");      

      
   // -----------------------------------------------------------------------
   apInitScenario ("4.4 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' instance method -> Base class' class method w/ classname");
      
   var beta4_4: Beta4_4 = new Beta4_4();
   
   expected = "Four";
   actual = "";
   actual = beta4_4.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.4", expected, actual, "297427");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.5 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' class method -> Base class' class field w/o classname");
                       
   expected = "Five";
   actual = "";
   actual = Beta4_5.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.5", expected, actual, "297427");

      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.6 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' class method -> Base class' class field w/ classname");
      
   expected = "Six";
   actual = "";
   actual = Beta4_6.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.6", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.7 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' class method -> Base class' class method w/o classname");
         
   expected = "Seven";
   actual = "";
   actual = Beta4_7.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.7", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4.8 Check subclass' instance and class methods accessing; base class' class fields and methods: Subclass' class method -> Base class' class method w/ classname");
         
   expected = "Eight";
   actual = "";
   actual = Beta4_8.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 4.8", expected, actual, "");        
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5.1 Access a static member using super: Static field");
   
   if (exceptionThrown5_1 == false)
      apLogFailInfo ("No compile error in 5.1", "Should give a compile error", exceptionThrown5_1, "");
   if (actualError5_1 != expectedError5_1)
      apLogFailInfo ("Wrong compile error in 5.1", expectedError5_1, actualError5_1, "");
      
         
   apEndTest();
}


staticmembers01();


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
