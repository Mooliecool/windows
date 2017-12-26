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


var iTestID = 197616;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown1 = false;
var actualError1 = "";
var expectedError1 = "SyntaxError: Abstract function cannot have body"

try
{
   eval ("  interface Interface1          " +
         "  {                             " +
         "     public function Bar()      " +
         "     {                          " +
         "        var x;                  " +
         "        x = 10;                 " +
         "     }                          " +
         "  }                             ", "unsafe");
}
catch (error)
{
   exceptionThrown1 = true;
   actualError1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown2 = false;
var actualError2 = "";
var expectedError2 = "SyntaxError: Variables cannot be declared in an interface"

try
{
   eval ("  interface Interface2       " +
         "  {                          " +
         "     const x: int = 20;      " +
         "  }                          ");
}
catch (error)
{
   exceptionThrown2 = true;
   actualError2 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown3 = false;
var actualError3 = "";
var expectedError3 = "SyntaxError: Variables cannot be declared in an interface"

try
{
   eval ("  interface Interface3       " +
         "  {                          " +
         "     var x: int = 30;      " +
         "  }                          ");
}
catch (error)
{
   exceptionThrown3 = true;
   actualError3 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown4 = false;
var actualError4 = "";
var expectedError4 = "Error: No implementation provided for 'Interface4.Foo()'"

try
{
   eval ("  interface Interface4                   " +
         "  {                                      " +
         "     public function Bar();              " +
         "     public function Foo();              " +
         "     public function Zoo();              " +
         "  }                                      " +
         "  class Alpha4 implements Interface4     " +
         "  {                                      " +
         "     public function Bar()               " +
         "     {  }                                " +
         "     public function Zoo()               " +
         "     {  }                                " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown4 = true;
   actualError4 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown5 = false;
var actualError5 = "";
var expectedError5 = "Error: No implementation provided for 'Interface5.Foo()'"

try
{
   eval ("  interface Interface5          " +
         "  {  }                          " +
         "  var interface5: Interface5;   ");
}
catch (error)
{
   exceptionThrown5 = true;
   actualError5 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown6_1 = false;
var actualError6_1 = "";
var expectedError6_1 = "TypeError: No such constructor"

try
{
   eval ("  interface Interface6_1                                " +
         "  {  }                                                  " +
         "  var interface6_1: Interface6_1 = new Interface6_1();  ");
}
catch (error)
{
   exceptionThrown6_1 = true;
   actualError6_1 = error;
}


// -----------------------------------------------------------------------
interface Interface6_2
{
   public function Foo(): String;
}

class Alpha6_2 implements Interface6_2
{
   public function Foo(): String
   {
      var temp: String;
      temp = "";
      temp = "Ten";
      return temp;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown7 = false;
var actualError7 = "";
var expectedError7 = "TypeError: Method 'Bar' is already defined in a base class with a different return type"

try
{
   eval ("  interface Interface7                " +
         "  {                                   " +
         "     public function Bar(): int;      " +
         "  }                                   " +
         "                                      " +
         "  class Alpha7 implements Interface7  " +
         "  {                                   " +
         "     public function Bar(): String    " +
         "     {  }                             " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown7 = true;
   actualError7 = error;
}


// -----------------------------------------------------------------------
interface Interface8A
{
   public function Bar(): String;
}

interface Interface8B
{
   public function Bar(): String;
}

interface Interface8C
{
   public function Bar(): String;
}

class Alpha8 implements Interface8A, Interface8B
{
   public function Bar(): String
   {
      return "Alpha8.Bar()";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown9 = false;
var actualError9 = "";
var expectedError9 = "Error: No implementation provided for 'Interface9B.Bar() : System.String'"

try
{
   eval ("  interface Interface9A                                 " +
         "  {                                                     " +
         "     public function Bar(): String;                     " +
         "  }                                                     " +
         "                                                        " +
         "  interface Interface9B                                 " +
         "  {                                                     " +
         "     public function Bar(): String;                     " +
         "  }                                                     " +
         "                                                        " +
         "  interface Interface9C                                 " +
         "  {                                                     " +
         "     public function Bar(): String;                     " +
         "  }                                                     " +
         "                                                        " +
         "  class Alpha9 implements Interface9A, Interface9B      " +
         "  {                                                     " +
         "     public function Interface9A.Bar(): String          " +
         "     {                                                  " +
         "        return \"Alpha9.Interface9A.Bar()\";            " +
         "     }                                                  " +
         "  }                                                     ", "unsafe");
}
catch (error)
{
   exceptionThrown9 = true;
   actualError9 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown10 = false;
var actualError10 = "";
var expectedError10 = "Error: No implementation provided for 'Interface10C.Bar() : System.String'"

try
{
   eval ("  interface Interface10A                                               " +
         "  {                                                                    " +
         "     public function Bar(): String;                                    " +
         "  }                                                                    " +
         "                                                                       " +
         "  interface Interface10B                                               " +
         "  {                                                                    " +
         "     public function Bar(): String;                                    " +
         "  }                                                                    " +
         "                                                                       " +
         "  interface Interface10C                                               " +
         "  {                                                                    " +
         "     public function Bar(): String;                                    " +
         "  }                                                                    " +
         "                                                                       " +
         "  class Alpha10 implements Interface10A, Interface10B, Interface10C    " +
         "  {                                                                    " +
         "     public function Interface10A.Bar(): String                        " +
         "     {                                                                 " +
         "     }                                                                 " +
         "     public function Interface10B.Bar(): String                        " +
         "     {                                                                 " +
         "     }                                                                 " +
         "  }                                                                    ", "unsafe");
}
catch (error)
{
   exceptionThrown10 = true;
   actualError10 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown11 = false;
var actualError11 = "";
var expectedError11 = "ReferenceError: Type: 'InterfaceA11' cannot be extended"

try
{
   eval ("  interface InterfaceA11              " +
         "  { }                                 " +
         "  class Alpha extends InterfaceA11    " +
         "  {  }                                ");
}
catch (error)
{
   exceptionThrown11 = true;
   actualError11 = error;
}


// -----------------------------------------------------------------------
interface InterfaceA12
{
   public function Bar(): String;
}

class Alpha12 implements InterfaceA12
{
   public function Bar(): String
   {
      return "hello";
   }
}

class Beta12 extends Alpha12
{
}


// -----------------------------------------------------------------------
interface InterfaceA13
{
   public function Bar(): int;
}

class Alpha13 implements InterfaceA13
{
   public function Bar(): int
   {
      return 23;
   }
}

class Beta13
{
   public function Foo (interfaceA: InterfaceA13): int
   {
      var x: int;
      x = interfaceA.Bar();
      x = x + 10;
      return x;
   }
}


// -----------------------------------------------------------------------
interface InterfaceA14
{
   public function Bar(): int;
}

class Alpha14 
{
   public function Bar(): int
   {
      return 23;
   }
}

class Beta14
{
   public function Foo (interfaceA: InterfaceA14): int
   {
      var x: int;
      x = interfaceA.Bar();
      x = x + 10;
      return x;
   }
}


// -----------------------------------------------------------------------
interface InterfaceA15
{
   public function Bar(): int;
}

class Alpha15 implements InterfaceA15
{
   public function Bar(): int
   {
      return 35;
   }
}

function Foo15 (interfaceA15: InterfaceA15): int
{
   var x: int;
   x = interfaceA15.Bar();
   x = x + 20;
   return x;
}


// -----------------------------------------------------------------------
interface InterfaceA16
{
   public function Bar(): int;
}

class Alpha16 
{
   public function Bar(): int
   {
      return 35;
   }
}

function Foo16 (interfaceA16: InterfaceA16): int
{
   var x: int;
   x = interfaceA16.Bar();
   x = x + 20;
   return x;
}


// -----------------------------------------------------------------------
interface InterfaceA17
{
   public function Bar(): String;
}

class Alpha17 implements InterfaceA17
{
   public function Bar()
   {
      return "hello";
   }
}

class Beta17 extends Alpha17
{
}

class Charlie17
{
   public function Foo (interfaceA: InterfaceA17): String
   {
      var x;
      x = interfaceA.Bar();
      x = x + "!";
      return x;
   }
}


// -----------------------------------------------------------------------
interface InterfaceA18
{
   public function Bar(): String;
}

class Alpha18 
{
   public function Bar()
   {
      return "hello";
   }
}

class Beta18 extends Alpha18
{
}

class Charlie18
{
   public function Foo (interfaceA: InterfaceA18): String
   {
      var x;
      x = interfaceA.Bar();
      x = x + "!";
      return x;
   }
}


// -----------------------------------------------------------------------
interface InterfaceA19
{
   public function Bar(): String;
}

class Alpha19 implements InterfaceA19
{
   public function Bar(): String
   {
      return ("hello world");
   }
}

class Beta19 extends Alpha19
{
   public function Foo()
   {
      var x: String;
      x = super.Bar();
      x = x + "!";
      return x;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown20 = false;
var actualError20 = "";
var expectedError20 = "TypeError: Interface name expected"

try
{
   eval ("  class Alpha20                    " +
         "  {  }                             " +
         "  class Beta20 implements Alpha20  " +
         "  {  }                             ");
}
catch (error)
{
   exceptionThrown20 = true;
   actualError20 = error;
}


// -----------------------------------------------------------------------
interface InterfaceA21
{
   public function Bar(): String;
}

class Alpha21 implements InterfaceA21
{
   public function Bar(): String
   {
      return "One"
   }
}

class Beta21 extends Alpha21
{
   public function Bar(): String
   {
      return "Two";
   }
}


// -----------------------------------------------------------------------
interface InterfaceA22
{
   public function Bar(): String;
}

class Alpha22 implements InterfaceA22
{
   public function Bar(): String
   {
      return "One"
   }
}

class Beta22 extends Alpha22
{
   public function Bar(): String
   {
      return "Two";
   }
}

class Charlie22 extends Beta22
{
   public function Bar(): String
   {
      return "Three";
   }
}

class Delta22
{
   public function Foo (interfaceA: InterfaceA22): String
   {
      var temp;
      temp = interfaceA.Bar();
      temp = temp + "!";
      return temp;
   }
}

function Zoo22 (interfaceA: InterfaceA22): String
{
   var temp;
   temp = interfaceA.Bar();
   temp = temp + "?";
   return temp;
}


// -----------------------------------------------------------------------
interface InterfaceA23
{
   public function Bar(): String;
}

class Alpha23 implements InterfaceA23
{
   public function Bar(): String
   {
      return "One";
   }
}

class Beta23 extends Alpha23
{
   public function Bar(): String
   {
      var temp: String;
      temp = super.Bar();
      temp = temp + " " + "Two";
      return temp;
   }
}

class Charlie23 extends Beta23
{
   public function Bar(): String
   {
      var temp: String;
      temp = super.Bar();
      temp = temp + " " + "Three";
      return temp;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown24 = false;
var actualError24 = "";
var expectedError24 = "SyntaxError: This modifier cannot be used on an interface member"

try
{
   eval ("  interface InterfaceA24              " +
         "  {                                   " +
         "     public static function Bar();    " +
         "  }                                   ");
}
catch (error)
{
   exceptionThrown24 = true;
   actualError24 = error;
}


// -----------------------------------------------------------------------
package Group25
{
   interface InterfaceA25
   {
      public function Bar(): String;
   }
}

import Group25;

class Alpha25 implements InterfaceA25
{
   public function Bar(): String
   {
      return "One";
   }
}




// -----------------------------------------------------------------------
function interface01()
{
   apInitTest ("Interface01");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Interface implements its methods");

   if (exceptionThrown1 == false)
      apLogFailInfo ("No compile error in 1.", "Should give a compile error", exceptionThrown1, "");
   if (actualError1 != expectedError1)
      apLogFailInfo ("Wrong compile error in 1.", expectedError1, actualError1, "273429");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2. Interface contains constants");
      
   if (exceptionThrown2 == false)
      apLogFailInfo ("No compile error in 2.", "Should give a compile error", exceptionThrown2, "");
   if (actualError2 != expectedError2)
      apLogFailInfo ("Wrong compile error in 2.", expectedError2, actualError2, "273434");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Interface contains variables");
   
   if (exceptionThrown3 == false)
      apLogFailInfo ("No compile error in 3.", "Should give a compile error", exceptionThrown3, "");
   if (actualError3 != expectedError3)
      apLogFailInfo ("Wrong compile error in 3.", expectedError3, actualError3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4. A class does not implement all the methods in the interface");
      
   if (exceptionThrown4 == false)
      apLogFailInfo ("No compile error in 4.", "Should give a compile error", exceptionThrown4, "");
   if (actualError4 != expectedError4)
      apLogFailInfo ("Wrong compile error in 4.", expectedError4, actualError4, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5. Can interfaces be used as a type?");
      
   if (exceptionThrown5 == true)
      apLogFailInfo ("Declaring an interface causes compile error", "Shouldn't give a compile error", actualError5, "");
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("6.1 Can interfaces be instantiated?");
                     
   if (exceptionThrown6_1 == false)
      apLogFailInfo ("No compile error in 6.1", "Should give a compile error", exceptionThrown6_1, "");
   if (actualError6_1 != expectedError6_1)
      apLogFailInfo ("Wrong compile error in 6.1", expectedError6_1, actualError6_1, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("6.2 Can interfaces be instantiated?");
   
   var alpha6_2: Interface6_2 = new Alpha6_2();
      
   expected = "Ten";
   actual = "";
   actual = alpha6_2.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 6.2", expected, actual, "299019");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7. Class declares a different return type from the interface it implements");
                
   if (exceptionThrown7 == false)
      apLogFailInfo ("No compile error in 7.", "Should give a compile error", exceptionThrown7, "284969");
   if (actualError7 != expectedError7)
      apLogFailInfo ("Wrong compile error in 7.", expectedError7, actualError7, "284969");   
                     
      
   // -----------------------------------------------------------------------
   apInitScenario ("8. Class implements multiple interfaces; common implementation");
   
   var alpha8: Alpha8 = new Alpha8();
   
   actual = "";
   expected = "Alpha8.Bar()";
   actual = alpha8.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9. Class implements multiple interfaces; only implements InterfaceA's method");
      
   if (exceptionThrown9 == false)
      apLogFailInfo ("No compile error in 9.", "Should give a compile error", exceptionThrown9, "");
   if (actualError9 != expectedError9)
      apLogFailInfo ("Wrong compile error in 9.", expectedError9, actualError9, "");  
 

   // -----------------------------------------------------------------------
   apInitScenario ("10. Class implements multiple interfaces: only implements InterfaceA & InterfaceB's method");

   if (exceptionThrown10 == false)
      apLogFailInfo ("No compile error in 10.", "Should give a compile error", exceptionThrown10, "");
   if (actualError10 != expectedError10)
      apLogFailInfo ("Wrong compile error in 10.", expectedError10, actualError10, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. Interface is part of a class hierarchy");
      
   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11.", "Should give a compile error", exceptionThrown11, "");
   if (actualError11 != expectedError11)
      apLogFailInfo ("Wrong compile error in 11.", expectedError11, actualError11, "283329"); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12. Alpha implements InterfaceA; Alpha implements InterfaceA.Bar(); Beta inherits Alpha; instantiate Beta; call method beta.Bar()");
       
   var beta12: Beta12 = new Beta12();
   
   expected = "hello";
   actual = "";
   actual = beta12.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 12.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13. A class implements an interface. It is passed to a method that has the interface as its parameter.");
      
   var alpha13: Alpha13 = new Alpha13();
   var beta13: Beta13 = new Beta13();
   
   expected = 33;
   actual = 0;
   actual = beta13.Foo (alpha13);
   if (actual != expected)
      apLogFailInfo ("Error in 13.", expected, actual, "299019");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14. A class does not implement an interface. It is passed to a method that has the interface as its parameter.");
      
   var alpha14: Alpha14 = new Alpha14();
   var beta14: Beta14 = new Beta14();
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: Type mismatch";
   
   try
   {
      eval ("actual = beta14.Foo (alpha14);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 11.", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 11.", expectedError, actualError, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("15. A class implements an interface. It is passed to a function that has the interface as its parameter.");      
   
   var alpha15: Alpha15 = new Alpha15();
   
   expected = 55;
   actual = 0;
   actual = Foo15 (alpha15);
   if (actual != expected)
      apLogFailInfo ("Error in 15.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16. A class does not implement an interface. It is passed to a function that has the interface as its parameter");      
      
   var alpha16: Alpha16 = new Alpha16();

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: Type mismatch";
   
   try
   {   
      eval ("actual = Foo16 (alpha16);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 16.", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 16.", expectedError, actualError, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17. A class' superclass implements an interface. It is passed to a method that has the interface as its parameter");      
      
   var beta17: Beta17 = new Beta17();
   var charlie17: Charlie17 = new Charlie17();
   
   expected = "hello!";
   actual = "";
   actual = charlie17.Foo (beta17);
   if (actual != expected) 
      apLogFailInfo ("Error in 17.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18. A class' superclass does not implement an interface. It is passed to a method that has the interface as its parameter");      
                       
   var beta18: Beta18 = new Beta18();
   var charlie18: Charlie18 = new Charlie18();
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: Type mismatch";
   
   try
   {
      eval ("actual = charlie18.Foo (beta18);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 18.", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 18.", expectedError, actualError, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19. A class' superclass implements an interface. The subclass calls the implemented method using super.");      
                       
   var beta19: Beta19 = new Beta19();
   
   expected = "hello world!";
   actual = "";
   actual = beta19.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 19.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20. A class implements another class");      
      
   if (exceptionThrown20 == false)
      apLogFailInfo ("No compile error in 20.", "Should give a compile error", exceptionThrown20, "284961");
   if (actualError20 != expectedError20)
      apLogFailInfo ("Wrong compile error in 20.", expectedError20, actualError20, "284961"); 
                          
      
   // -----------------------------------------------------------------------
   apInitScenario ("21. A class' superclass implements an interface. The subclass overrides the implemented method.");         
                       
   var beta21: Beta21 = new Beta21();
   var alpha21: Alpha21;
   
   expected = "Two";
   actual = "";
   actual = beta21.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 21(1)", expected, actual, "");
      
   alpha21 = new Beta21();
   expected = "Two";
   actual = "";
   actual = alpha21.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 21(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22. A class' superclass implements an interface. " +
                       "The subclass overrides the implemented method. " +
                       "Pass an instance of the subclass to a method that has the interface as a parameter");         
      
   var alpha22: Alpha22;
   var beta22: Beta22;
   var charlie22: Charlie22;
   var delta22: Delta22 = new Delta22();
   
   // ------------------------------------
   alpha22 = new Alpha22();      
   
   expected = "One!";
   actual = "";
   actual = delta22.Foo (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(1)", expected, actual, "");
      
   expected = "One?";
   actual = "";
   actual = Zoo22 (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(2)", expected, actual, "");
    
   // ------------------------------------
   alpha22 = new Beta22();
   
   expected = "Two!";
   actual = "";
   actual = delta22.Foo (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(3)", expected, actual, "");

   expected = "Two?";
   actual = "";
   actual = Zoo22 (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(4)", expected, actual, "");
      
   // ------------------------------------
   alpha22 = new Charlie22();
   
   expected = "Three!";
   actual = "";
   actual = delta22.Foo (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(5)", expected, actual, "");
      
   expected = "Three?";
   actual = "";
   actual = Zoo22 (alpha22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(6)", expected, actual, "");
      
   // ------------------------------------
   beta22 = new Beta22();
   
   expected = "Two!";
   actual = "";
   actual = delta22.Foo (beta22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(7)", expected, actual, "");   

   expected = "Two?";
   actual = "";
   actual = Zoo22 (beta22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(8)", expected, actual, "");   
      
   // ------------------------------------
   charlie22 = new Charlie22();
   
   expected = "Three!";
   actual = "";
   actual = delta22.Foo (charlie22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(9)", expected, actual, "");   
      
   expected = "Three?";

   actual = "";
   actual = Zoo22 (charlie22);
   if (actual != expected)
      apLogFailInfo ("Error in 22(10)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23. A class' superclass implements an interface. The subclass overrides the implemented method and calls the implemented method using super.");         
      
   var alpha23: Alpha23;
   var beta23: Beta23;
   var charlie23: Charlie23;
   
   alpha23 = new Alpha23();
   expected = "One"
   actual = "";
   actual = alpha23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(1)", expected, actual, "");
      
   alpha23 = new Beta23();
   expected = "One Two"
   actual = "";
   actual = alpha23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(2)", expected, actual, "");
      
   alpha23 = new Charlie23();
   expected = "One Two Three"
   actual = "";
   actual = alpha23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(3)", expected, actual, "");
      
   beta23 = new Beta23();
   expected = "One Two"
   actual = "";
   actual = beta23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(4)", expected, actual, "");
   
   charlie23 = new Charlie23();
   expected = "One Two Three"
   actual = "";
   actual = charlie23.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23(5)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24. Static method inside an interface");         
      
   if (exceptionThrown24 == false)
      apLogFailInfo ("No compile error in 24.", "Should give a compile error", exceptionThrown24, "");
   if (actualError24 != expectedError24)
      apLogFailInfo ("Wrong compile error in 24.", expectedError24, actualError24, ""); 
   
            
   // -----------------------------------------------------------------------
   apInitScenario ("25. Interface inside a package; Alpha implements a method of the interface");         
      
   var alpha25: Alpha25 = new Alpha25();
   expected = "One";
   actual = "";
   actual = alpha25.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 25.", expected, actual, "");   
      
      
   apEndTest();
}


interface01();


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
