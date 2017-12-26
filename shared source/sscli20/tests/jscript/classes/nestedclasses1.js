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


var iTestID = 173437;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
class Outer1
{
   class Alpha1
   {
      public var value: String = "one";
   }
}


// -----------------------------------------------------------------------
class Outer2
{
   class Alpha2
   {
      public function Bar()
      {
         return "two";         
      }
   }
}


// -----------------------------------------------------------------------
class Outer3
{
   static class Alpha3
   {
      public var value: String = "three";
   }
}


// -----------------------------------------------------------------------
class Outer4
{
   static class Alpha4
   {
      public function Bar()
      {
         return "four";
      }  
   }
}


// -----------------------------------------------------------------------
var exceptionThrown5 = false;
var actualError5 = "";
var expectedError5 = "ReferenceError: The non-static member 'Alpha5' cannot be accessed with the class name";

try
{
   eval ("  class Outer5            " +
         "  {                       " +
         "     class Alpha5         " +
         "     { }                  " +
         "  }                       " +
         "  var alpha5: Outer5.Alpha5 = new Outer5.Alpha5();");
}
catch (error)
{
   exceptionThrown5 = true;
   actualError5 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown6 = false;
var actualError6 = "";
var expectedError6;

// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
         expectedError6 = "ReferenceError: Variable 'outer6' has not been declared";
     @else
         expectedError6 = "ReferenceError: 'outer6' is undefined";
    
@end       

try
{
   eval ("  class Outer6            " +
         "  {                       " +
         "     class Alpha6         " +
         "     { }                  " +
         "  }                       " +
         "  var alpha6: Outer6.Alpha6 = new outer6.alpha6();");
}
catch (error)
{
   exceptionThrown6 = true;
   actualError6 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown7 = false;
var actualError7 = "";
var expectedError7;

// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
         expectedError7 = "ReferenceError: Variable 'outer7' has not been declared";
     @else
         expectedError7 = "ReferenceError: 'outer7' is undefined";
    
@end       

try
{
   eval ("  class Outer7            " +
         "  {                       " +
         "     class Alpha7         " +
         "     { }                  " +
         "  }                       " +
         "  var alpha7: Outer7.Alpha7 = new outer7.Alpha7();");
}
catch (error)
{
   exceptionThrown7 = true;
   actualError7 = error;
}  


// -----------------------------------------------------------------------
class Outer8
{
   class Alpha8
   {
      public var valueA: String = "Outer8.Alpha8";
      public function Bar(): String
      {
         return "Outer8.Alpha8";
      }
   }
   
   class Beta8 extends Alpha8
   {
      public var valueB: String = "Outer8.Beta8";
      public function Bar(): String
      {
         return "Outer8.Beta8";
      }
   }
   
   class Charlie8 extends Beta8
   {
      public var valueC: String = "Outer8.Charlie8";
      public function Bar(): String
      {
         return "Outer8.Charlie8";
      }
   }
}   


// -----------------------------------------------------------------------
class Outer9
{
   static class Alpha9
   {
      public var valueA: String = "Outer9.Alpha9";
      public function Bar(): String
      {
         return "Outer9.Alpha9";
      }
   }
   
   static class Beta9 extends Alpha9
   {
      public var valueB: String = "Outer9.Beta9";
      public function Bar(): String
      {
         return "Outer9.Beta9";
      }
   }
   
   static class Charlie9 extends Beta9
   {
      public var valueC: String = "Outer9.Charlie9";
      public function Bar(): String
      {
         return "Outer9.Charlie9";
      }
   }
}  


// -----------------------------------------------------------------------
var exceptionThrown10 = false;
var actualError10 = "";
var expectedError10 = "ReferenceError: A non-static nested type can only be extended by non-static type nested in the same class";

try
{
   eval ("  class Outer10                          " +
         "  {                                      " +
         "     class Alpha10                       " +
         "     {  }                                " +
         "     static class Beta10 extends Alpha10 " +
         "     {  }                                " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown10 = true;
   actualError10 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown11 = false;
var actualError11 = "";
var expectedError11 = "ReferenceError: A non-static nested type can only be extended by non-static type nested in the same class";

try
{
   eval ("  class Outer11                             " +
         "  {                                         " +
         "     class Alpha11                          " +
         "     {  }                                   " +
         "     class Beta11 extends Alpha11           " +
         "     {  }                                   " +
         "     static class Charlie11 extends Beta11  " +
         "     {  }                                   " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown11 = true;
   actualError11 = error;
}    


// -----------------------------------------------------------------------
class Outer12
{
   static class Alpha12
   {
      public var valueA: String = "Outer12.Alpha12";
      
      public function Bar(): String
      {
         return "Outer12.Alpha12";
      }
      
      protected function Zoo(): String
      {
         return "Outer12.Alpha12.Zoo()";
      }
   }
   
   class Beta12 extends Alpha12
   {
      public var valueB: String = "Outer12.Beta12";
      
      public function Bar(): String
      {
         return "Outer12.Beta12";
      }   
      
      public function Boo(): String
      {
         var temp: String = "";
         temp = Zoo();
         return temp;
      }
   }
}   


// -----------------------------------------------------------------------
class Outer13
{
   static class Alpha13
   {
      public var valueA: String = "Outer13.Alpha13.valueA";
      
      public function Foo()
      {
         return "Outer13.Alpha13.Foo()";
      }
   }
   
   static class Beta13 extends Alpha13
   {
      public var valueB: String = "Outer13.Alpha13.valueB";
      
      public function Zoo()
      {
         return "Outer13.Beta13.Zoo()";
      }
   }
   
   class Charlie13 extends Beta13
   {
      public var valueC: String = "Outer13.Charlie13";

      public function Bar()
      {
         return "Outer13.Charlie13.Bar()";
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown14 = false;
var actualError14 = "";
var expectedError14 = "ReferenceError: A non-static nested type can only be extended by non-static type nested in the same class";

try
{
   eval ("  class Outer14                          " +
         "  {                                      " +
         "     class Alpha14                       " +
         "     {  }                                " +
         "  }                                      " +
         "  class Beta14 extends Outer14.Alpha14   " +
         "  {  }                                   ");
}
catch (error)
{
   exceptionThrown14 = true;
   actualError14 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown15 = false;
var actualError15 = "";
var expectedError15 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  class Outer15                                " +
         "  {                                            " +
         "     class Alpha15                             " +
         "     {  }                                      " +
         "  }                                            " +
         "  static class Beta15 extends Outer15.Alpha15  " +
         "  {  }                                         ");
}
catch (error)
{
   exceptionThrown15 = true;
   actualError15 = error;
}  


// -----------------------------------------------------------------------
class Outer16
{
   static class Alpha16
   {
      public var valueA: String = "Outer16.Alpha16.valueA";
      public function BarA()
      {
         return "Outer16.Alpha16.BarA()";
      }
   }
}

class Beta16 extends Outer16.Alpha16
{
   public var valueB: String = "Beta16.valueB";
   public function BarB()
   {
      return "Beta16.BarB()";
   }
}

class Charlie16 extends Beta16
{
   public var valueC: String = "Charlie16.valueC";
   public function BarC()
   {
      return "Charlie16.BarC()";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown17 = false;
var actualError17 = "";
var expectedError17 = "SyntaxError: Only valid inside a class definition";


try
{
   eval ("  class Outer17                                " +
         "  {                                            " +
         "     static class Alpha17                      " +
         "     {  }                                      " +
         "  }                                            " +
         "  static class Beta17 extends Outer17.Alpha17  " +
         "  {  }                                         ");
}
catch (error)
{
   exceptionThrown17 = true;
   actualError17 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown18 = false;
var actualError18 = "";
var expectedError18 = "ReferenceError: A non-static nested type can only be extended by non-static type nested in the same class";

try
{
   eval ("  class A                          " +
         "  {                                " +
         "     class Alpha18                 " +
         "     {  }                          " +
         "  }                                " +
         "  class B                          " +
         "  {                                " +
         "     class Beta extends A.Alpha18  " +
         "     {  }                          " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown18 = true;
   actualError18 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown19 = false;
var actualError19 = "";
var expectedError19 = "ReferenceError: A non-static nested type can only be extended by non-static type nested in the same class";

try
{
   eval ("  class A19                              " +
         "  {                                      " +
         "     class Alpha19                       " +
         "     {  }                                " +
         "  }                                      " +
         "  class B19                              " +
         "  {                                      " +
         "     static class Beta extends A19.Alpha19  " +
         "     {  }                                " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown19 = true;
   actualError19 = error;
}


// -----------------------------------------------------------------------
class A20
{
   static class Alpha20
   {
      public var valueA: String = "A20.Alpha20.valueA";
      public function BarA()
      {
         return "A20.Alpha20.BarA()";
      }
   }   
}

class B20
{
   class Beta20 extends A20.Alpha20
   {
      public var valueB: String = "B20.Beta20.valueB";
      public function BarB()
      {
         return "B20.Beta20.BarB()";
      }
   }
   
   class Charlie20 extends Beta20
   {
      public var valueC: String = "B20.Charlie20.valueC";
      public function BarC()
      {
         return "B20.Charlie20.BarC()";
      }
   }
}


// -----------------------------------------------------------------------
class A21
{
   static class Alpha21
   {
      public var valueA: String = "A21.Alpha21.valueA";
      public function BarA(): String
      {
         return "A21.Alpha21.BarA()";
      }
   }
}

class B21
{
   static class Beta21 extends A21.Alpha21
   {
      public var valueB: String = "B21.Beta21.valueB";
      public function BarB(): String
      {
         return "B21.Beta21.BarB()";
      }
   }
   
   class Charlie21 extends Beta21
   {
      public var valueC: String = "B21.Charlie21.valueC";
      public function BarC(): String
      {
         return "B21.Charlie21.BarC()";
      }
   }
}


// -----------------------------------------------------------------------
class Alpha22
{
   public var valueA: String = "Alpha22.valueA";
   public function BarA()
   {
      return "Alpha22.BarA()";
   }
}

class B22
{
   class Beta22 extends Alpha22
   {
      public var valueB: String = "Beta22.valueB";
      public function BarB()
      {
         return "Beta22.BarB()";
      }
   }
   
   class Charlie22 extends Beta22
   {
      public var valueC: String = "Beta22.valueC";
      public function BarC()
      {
         return "Beta22.BarC()";
      }
   }
}


// -----------------------------------------------------------------------
class Alpha23
{
   public var valueA: String = "Alpha23.valueA";
   public function BarA()
   {
      return "Alpha23.BarA()";
   }
}

class B23
{
   static class Beta23 extends Alpha23
   {
      public var valueB: String = "Beta23.valueB";
      public function BarB()
      {
         return "Beta23.BarB()";
      }
   }
   
   class Charlie23 extends Beta23
   {
      public var valueC: String = "Beta23.valueC";
      public function BarC()
      {
         return "Beta23.BarC()";
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown24 = false;
var actualError24 = "";
var expectedError24 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  static class Alpha24                   " +
         "  {  }                                   " +
         "  class B24                              " +
         "  {                                      " +
         "     class Beta24 extends Alpha24        " +
         "     {  }                                " +
         "     class Charlie24 extends Beta24      " +
         "     {  }                                " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown24 = true;
   actualError24 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown25 = false;
var actualError25 = "";
var expectedError25 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("  static class Alpha25                   " +
         "  {  }                                   " +
         "  class B25                              " +
         "  {                                      " +
         "     static class Beta25 extends Alpha25 " +
         "     {  }                                " +
         "     class Charlie25 extends Beta25      " +
         "     {  }                                " +
         "  }                                      ");
}
catch (error)
{
   exceptionThrown25 = true;
   actualError25 = error;
}


// -----------------------------------------------------------------------
class A26
{
   class Alpha26
   {
      public var valueA: String = "A26.Alpha26.valueA";
      public function BarA(): String
      {
         return "A26.Alpha26.BarA()";
      }
   }
   
   public function Bar1(): String
   {
      var alpha26: Alpha26 = new Alpha26();
      var temp: String = "";
      
      temp = alpha26.valueA;
      return temp;
   }
   
   public function Bar2(): String
   {
      var alpha26: Alpha26 = new Alpha26();
      var temp: String = "";
      
      temp = alpha26.BarA();
      return temp;
   }
}


// -----------------------------------------------------------------------
class A27
{
   static class Alpha27
   {
      public var valueA: String = "A27.Alpha27.valueA";
      public function BarA(): String
      {
         return "A27.Alpha27.BarA()";
      }
   }
   
   public function Bar1(): String
   {
      var alpha27: Alpha27 = new Alpha27();
      var temp: String = "";
      
      temp = alpha27.valueA;
      return temp;
   }
   
   public function Bar2(): String
   {
      var alpha27: Alpha27 = new Alpha27();
      var temp: String = "";
      
      temp = alpha27.BarA();
      return temp;
   }
}


// -----------------------------------------------------------------------
class A28
{  
   class Alpha28
   {
      public var valueA: String = "A28.Alpha28.valueA";
      public function BarA(): String
      {
         return "A28.Alpha28.BarA()";
      }
   }
}

class B28 extends A28
{
   public function Bar1(): String
   {
      var alpha28: Alpha28 = new Alpha28();
      var temp: String = "";
      
      temp = alpha28.valueA;
      return temp;
   }
   
   public function Bar2(): String
   {
      var alpha28: Alpha28 = new Alpha28();
      var temp: String = "";
      
      temp = alpha28.BarA();
      return temp;
   }
}


// -----------------------------------------------------------------------
class A29
{  
   static class Alpha29
   {
      public var valueA: String = "A29.Alpha29.valueA";
      public function BarA(): String
      {
         return "A29.Alpha29.BarA()";
      }
   }
}

class B29 extends A29
{
   public function Bar1(): String
   {
      var alpha29: Alpha29 = new Alpha29();
      var temp: String = "";
      
      temp = alpha29.valueA;
      return temp;
   }
   
   public function Bar2(): String
   {
      var alpha29: Alpha29 = new Alpha29();
      var temp: String = "";
      
      temp = alpha29.BarA();
      return temp;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown30 = false;
var actualError30 = "";
var expectedError30 = "ReferenceError: 'Alpha30' is not accessible from this scope";

try
{
   eval ("  class A30                                    " +
         "  {                                            " +
         "     private class Alpha30                     " +
         "     {  }                                      " +
         "  }                                            " +
         "  class B30 extends A30                        " +
         "  {                                            " +
         "     public function Bar()                     " +
         "     {                                         " +
         "        var alpha: Alpha30 = new Alpha30();    " +
         "     }                                         " +
         "  }                                            ", "unsafe");
}
catch (error)
{
   exceptionThrown30 = true;
   actualError30 = error;
}


// -----------------------------------------------------------------------
class A31
{
   protected class Alpha
   {
      public var value: String = "A31.Alpha.value";
      public function Bar(): String
      {
         return "A31.Alpha.Bar()";
      }
   }
}

class B31 extends A31
{
   public function Bar1(): String
   {
      var alpha: Alpha = new Alpha();
      return alpha.value;
   }
   
   public function Bar2(): String
   {
      var alpha: Alpha = new Alpha();
      return alpha.Bar();
   }
}


// -----------------------------------------------------------------------
class A32
{
   public class Alpha32
   {
      public var value: String = "A32.Alpha32.value";
      public function Bar(): String
      {
         return "A32.Alpha32.Bar()";
      }
   }
}

class B32 extends A32
{
   public function Bar1(): String
   {
      var alpha32: Alpha32 = new Alpha32();
      return alpha32.value;
   }
   
   public function Bar2(): String
   {
      var alpha32: Alpha32 = new Alpha32();
      return alpha32.Bar();
   }
}


// -----------------------------------------------------------------------
class A33
{
   internal class Alpha33
   {
      public var value: String = "A33.Alpha33.value";
      public function Bar(): String
      {
         return "A33.Alpha33.Bar()";
      }
   }
}

class B33 extends A33
{
   public function Bar1(): String
   {
      var alpha33: Alpha33 = new Alpha33();
      return alpha33.value;
   }
   
   public function Bar2(): String
   {
      var alpha33: Alpha33 = new Alpha33();
      return alpha33.Bar();
   }
}


// -----------------------------------------------------------------------
package Group34
{
   class A34
   {
      internal class Alpha34
      {
         public var value: String = "A34.Alpha34.value";
         public function Bar(): String
         {
            return "A34.Alpha34.Bar()";
         }
      }
   }

   class B34 extends A34
   {
      public function Bar1(): String
      {
         var alpha34: Alpha34 = new Alpha34();
         return alpha34.value;
      }
      
      public function Bar2(): String
      {
         var alpha34: Alpha34 = new Alpha34();
         return alpha34.Bar();
      }
   }
}




import Group34;



// -----------------------------------------------------------------------
function nestedclasses1()
{
   apInitTest ("Nested Classes");
   
   // -----------------------------------------------------------------------
   apInitScenario ("1. Access a public field of a non-static nested class");
   
   var outer1: Outer1 = new Outer1();
   var alpha1: Outer1.Alpha1 = new outer1.Alpha1();
   
   expected = "one";
   actual = "";
   actual = alpha1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 1.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2. Access a public method of a non-static nested class");
      
   var outer2: Outer2 = new Outer2();
   var alpha2: Outer2.Alpha2 = new outer2.Alpha2();
   
   expected = "two";
   actual = "";
   actual = alpha2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 2.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Access a public field of a static nested class");
      
   var alpha3: Outer3.Alpha3 = new Outer3.Alpha3();
   
   expected = "three";
   actual = "";
   actual = alpha3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 3.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("4. Access a public method of a static nested class");
      
   var alpha4: Outer4.Alpha4 = new Outer4.Alpha4();
   
   expected = "four";
   actual = "";
   actual = alpha4.Bar();   
   if (actual != expected)
      apLogFailInfo ("Error in 4.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("5. Instantiate a non-static nested class using var alpha: Outer.Alpha = new Outer.Alpha();");

   if (exceptionThrown5 == false)
      apLogFailInfo ("No compile error in 5", "Should give a compile error", exceptionThrown5, "");
   if (actualError5 != expectedError5)
      apLogFailInfo ("Wrong compile error in 5", expectedError5, actualError5, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("6. Instantiate a non-static nested class using var alpha = Outer.Alpha = new outer.alpha();");
      
   if (exceptionThrown6 == false)
      apLogFailInfo ("No compile error in 6", "Should give a compile error", exceptionThrown6, "");
   if (actualError6 != expectedError6)
      apLogFailInfo ("Wrong compile error in 6", expectedError6, actualError6, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7. Instantiate a non-static nested class using var alpha = Outer.Alpha = new outer.Alpha();");
      
   if (exceptionThrown7 == false)
      apLogFailInfo ("No compile error in 7", "Should give a compile error", exceptionThrown7, "");
   if (actualError7 != expectedError7)
      apLogFailInfo ("Wrong compile error in 7", expectedError7, actualError7, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.1 Non-static Alpha, non-static Beta, and non-static Charlie nested in Outer; Instantiate Alpha; access Alpha's field and method");
      
   var outer8_1: Outer8 = new Outer8();
   var alpha8_1: Outer8.Alpha8 = new outer8_1.Alpha8();
   
   expected = "Outer8.Alpha8";
   actual = "";
   actual = alpha8_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 8.1(1)", expected, actual, "");
      
   expected = "Outer8.Alpha8";
   actual = "";
   actual = alpha8_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8.1(2)", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("8.2 Non-static Alpha, non-static Beta, and non-static Charlie nested in Outer; Instantiate Beta; access Beta's field and method");
      
   var outer8_2: Outer8 = new Outer8();
   var beta8_2: Outer8.Beta8 = new outer8_2.Beta8();
   
   expected = "Outer8.Beta8";
   actual = "";
   actual = beta8_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 8.2(1)", expected, actual, "");
      
   expected = "Outer8.Beta8";
   actual = "";
   actual = beta8_2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8.2(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.3 Non-static Alpha, non-static Beta, and non-static Charlie nested in Outer; Instantiate Charlie; access Charlie's field and method");
      
   var outer8_3: Outer8 = new Outer8();
   var charlie8_3: Outer8.Charlie8 = new outer8_3.Charlie8();
   
   expected = "Outer8.Charlie8";
   actual = "";
   actual = charlie8_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 8.3(1)", expected, actual, "");
      
   expected = "Outer8.Charlie8";
   actual = "";
   actual = charlie8_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8.3(2)", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9.1 Static Alpha, static Beta, and static Charlie nested in Outer; Instantiate Alpha; access Alpha's field and method");
      
   var alpha9_1: Outer9.Alpha9 = new Outer9.Alpha9();
   
   expected = "Outer9.Alpha9";
   actual = "";
   actual = alpha9_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 9.1(1)", expected, actual, "");      
      
   expected = "Outer9.Alpha9";
   actual = "";
   actual = alpha9_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 9.1(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9.2 Static Alpha, static Beta, and static Charlie nested in Outer; Instantiate Beta; access Beta's field and method");
      
   var beta9_2: Outer9.Beta9 = new Outer9.Beta9();
   
   expected = "Outer9.Beta9";
   actual = "";
   actual = beta9_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 9.2(1)", expected, actual, "");      
      
   expected = "Outer9.Beta9";
   actual = "";
   actual = beta9_2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 9.2(2)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9.3 Static Alpha, static Beta, and static Charlie nested in Outer; Instantiate Charlie; access Charlie's field and method");
      
   var charlie9_3: Outer9.Charlie9 = new Outer9.Charlie9();
   
   expected = "Outer9.Charlie9";
   actual = "";
   actual = charlie9_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 9.3(1)", expected, actual, "");      
      
   expected = "Outer9.Charlie9";
   actual = "";
   actual = charlie9_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 9.3(2)", expected, actual, "");        
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. Non-static Alpha & static Beta nested in Outer");
          
   if (exceptionThrown10 == false)
      apLogFailInfo ("No compile error in 10", "Should give a compile error", exceptionThrown10, "");
   if (actualError10 != expectedError10)
      apLogFailInfo ("Wrong compile error in 10", expectedError10, actualError10, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. Non-static Alpha, non-static Beta, & static Charlie nested in Outer");
      
   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11", "Should give a compile error", exceptionThrown11, "");
   if (actualError11 != expectedError11)
      apLogFailInfo ("Wrong compile error in 11", expectedError11, actualError11, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12.1 Static Alpha & non-static Beta nested in Outer; Instantiate Alpha; access Alpha's field and method");
      
   var alpha12_1: Outer12.Alpha12 = new Outer12.Alpha12();
   
   expected = "Outer12.Alpha12";
   actual = "";
   actual = alpha12_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 12.1(1)", expected, actual, "");        
      
   expected = "Outer12.Alpha12";
   actual = "";
   actual = alpha12_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 12.1(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12.2 Static Alpha & non-static Beta nested in Outer; Instantiate Beta; access Beta's field and method");
           
   var outer12_2: Outer12 = new Outer12();
   var beta12_2: Outer12.Beta12 = new outer12_2.Beta12();
   
   expected = "Outer12.Beta12";
   actual = "";
   actual = beta12_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 12.2(1)", expected, actual, "");   
      
   expected = "Outer12.Beta12";
   actual = "";
   actual = beta12_2.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 12.2(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12.3 Static Alpha & non-static Beta nested in Outer; Beta calls a protected method in Alpha");
        
   var outer12_3: Outer12 = new Outer12();
   var beta12_3: Outer12.Beta12 = new outer12_3.Beta12();
   
   expected = "Outer12.Alpha12.Zoo()";
   actual = "";
   actual = beta12_3.Boo();
   if (actual != expected)
      apLogFailInfo ("Error in 12.3", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13.1 Static Alpha, static Beta, non-static Charlie nested in Outer; Instantiate Charlie; access Charlie's field and method");
   
   var outer13_1: Outer13 = new Outer13();
   var charlie13_1: Outer13.Charlie13 = new outer13_1.Charlie13();
   
   expected = "Outer13.Charlie13";
   actual = "";
   actual = charlie13_1.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 13.1(1)", expected, actual, ""); 
   
   expected = "Outer13.Charlie13.Bar()";
   actual = "";
   actual = charlie13_1.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 13.1(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13.2 Static Alpha, static Beta, non-static Charlie nested in Outer; Charlie calls a protected field and method in Alpha");
      
   var outer13_2: Outer13 = new Outer13();
   var charlie13_2: Outer13.Charlie13 = new outer13_2.Charlie13();
   
   expected = "Outer13.Alpha13.valueA";
   actual = "";
   actual = charlie13_2.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 13.2(1)", expected, actual, ""); 
      
   expected = "Outer13.Alpha13.Foo()";
   actual = "";
   actual = charlie13_2.Foo();
   if (actual != expected)
      apLogFailInfo ("Error in 13.2(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13.3 Static Alpha, static Beta, non-static Charlie nested in Outer; Charlie calls an inherited field and method in Beta");
      
   var outer13_3: Outer13 = new Outer13();
   var charlie13_3: Outer13.Charlie13 = new outer13_3.Charlie13();
   
   expected = "Outer13.Alpha13.valueB";
   actual = "";
   actual = charlie13_3.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 13.3(1)", expected, actual, ""); 
      
   expected = "Outer13.Beta13.Zoo()";
   actual = "";
   actual = charlie13_3.Zoo();
   if (actual != expected)
      apLogFailInfo ("Error in 13.3(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("14. Non-static Alpha nested in Outer; non-static Beta not nested in Outer");
      
   if (exceptionThrown14 == false)
      apLogFailInfo ("No compile error in 14", "Should give a compile error", exceptionThrown14, "");
   if (actualError14 != expectedError14)
      apLogFailInfo ("Wrong compile error in 14", expectedError14, actualError14, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("15. Non-static Alpha nested in Outer; static Beta not nested in Outer");
      
   if (exceptionThrown15 == false)
      apLogFailInfo ("No compile error in 15", "Should give a compile error", exceptionThrown15, "");
   if (actualError15 != expectedError15)
      apLogFailInfo ("Wrong compile error in 15", expectedError15, actualError15, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16.1 Static Alpha nested in Outer; non-static Beta not nested in Outer; non-static Charlie not nested in Outer; Instantiate Alpha; access Alpha's field and method");
      
   var alpha16_1: Outer16.Alpha16 = new Outer16.Alpha16();
   
   expected = "Outer16.Alpha16.valueA";
   actual = "";
   actual = alpha16_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 16.1(1)", expected, actual, ""); 

   expected = "Outer16.Alpha16.BarA()";
   actual = "";
   actual = alpha16_1.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 16.1(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16.2 Static Alpha nested in Outer; non-static Beta not nested in Outer; non-static Charlie not nested in Outer; Instantiate Beta; access Beta's field and method");
   
   var beta16_2: Beta16 = new Beta16();
   
   expected = "Beta16.valueB";
   actual = "";
   actual = beta16_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 16.2(1)", expected, actual, ""); 
      
   expected = "Beta16.BarB()";
   actual = "";
   actual = beta16_2.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 16.2(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16.3 Static Alpha nested in Outer; non-static Beta not nested in Outer; non-static Charlie not nested in Outer;  Instantiate Charlie; access Charlie's field and method");
   
   var charlie16_3: Charlie16 = new Charlie16();
   
   expected = "Charlie16.valueC";
   actual = "";
   actual = charlie16_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 16.3(1)", expected, actual, ""); 
      
   expected = "Charlie16.BarC()";
   actual = "";
   actual = charlie16_3.BarC();
   if (actual != expected)
      apLogFailInfo ("Error in 16.3(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16.4 Static Alpha in Outer; non-static Beta not in Outer; non-static Charlie not in Outer; Charlie calls an inherited field and method in Alpha");
      
   var charlie16_4: Charlie16 = new Charlie16();
   
   expected = "Outer16.Alpha16.valueA";   
   actual = "";
   actual = charlie16_4.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 16.4(1)", expected, actual, ""); 
   
   expected = "Outer16.Alpha16.BarA()";  
   actual = "";
   actual = charlie16_4.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 16.4(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("16.5 Static Alpha in Outer; non-static Beta not in Outer; non-static Charlie not in Outer;  Charlie calls an inherited field and method in Beta");
      
   var charlie16_5: Charlie16 = new Charlie16();
   
   expected = "Beta16.valueB";
   actual = "";
   actual = charlie16_5.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 16.5(1)", expected, actual, ""); 
    
   expected = "Beta16.BarB()";
   actual = "";
   actual = charlie16_5.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 16.5(2)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("17. Static Alpha nested in Outer; static Beta not nested in Outer");
      
   if (exceptionThrown17 == false)
      apLogFailInfo ("No compile error in 17", "Should give a compile error", exceptionThrown17, "");
   if (actualError17 != expectedError17)
      apLogFailInfo ("Wrong compile error in 17", expectedError17, actualError17, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("18. Non-static Alpha nested in A; non-static Beta nested in B");
   
   if (exceptionThrown18 == false)
      apLogFailInfo ("No compile error in 18", "Should give a compile error", exceptionThrown18, "");
   if (actualError18 != expectedError18)
      apLogFailInfo ("Wrong compile error in 18", expectedError18, actualError18, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19. Non-static Alpha nested in A; static Beta nested in B");
      
   if (exceptionThrown19 == false)
      apLogFailInfo ("No compile error in 19", "Should give a compile error", exceptionThrown19, "");
   if (actualError19 != expectedError19)
      apLogFailInfo ("Wrong compile error in 19", expectedError19, actualError19, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.1 Static Alpha nested in A; non-static Beta & Charlie nested in B; Instantiate Alpha; access Alpha's field and method");
      
   var alpha20_1: A20.Alpha20 = new A20.Alpha20();
   
   expected = "A20.Alpha20.valueA";
   actual = "";
   actual = alpha20_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 20.1(1)", expected, actual, "");
      
   expected = "A20.Alpha20.BarA()";
   actual = "";
   actual = alpha20_1.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 20.1(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.2 Static Alpha nested in A; non-static Beta & Charlie nested in B; Instantiate Beta; access Beta's field and method");
   
   var b20_2: B20 = new B20();   
   var beta20_2: B20.Beta20 = new b20_2.Beta20();   
      
   expected = "B20.Beta20.valueB";
   actual = "";
   actual = beta20_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 20.2(1)", expected, actual, "");
      
   expected = "B20.Beta20.BarB()";
   actual = "";
   actual = beta20_2.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 20.2(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.3 Static Alpha nested in A; non-static Beta & Charlie nested in B; Instantiate Charlie; access Charlie's field and method");
                        
   var b20_3: B20 = new B20();
   var charlie20_3: B20.Charlie20 = new b20_3.Charlie20();
   
   expected = "B20.Charlie20.valueC";
   actual = "";
   actual = charlie20_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 20.3(1)", expected, actual, "");
   
   expected = "B20.Charlie20.BarC()";
   actual = "";
   actual = charlie20_3.BarC();
   if (actual != expected)
      apLogFailInfo ("Error in 20.3(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.4 Static Alpha nested in A; non-static Beta & Charlie nested in B; Charlie calls an inherited field and method in Alpha");
      
   var b20_4: B20 = new B20();
   var charlie20_4: B20.Charlie20 = new b20_4.Charlie20();
   
   expected = "A20.Alpha20.valueA";
   actual = "";
   actual = charlie20_4.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 20.4(1)", expected, actual, "");
      
   expected = "A20.Alpha20.BarA()";
   actual = "";
   actual = charlie20_4.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 20.4(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.5 Static Alpha nested in A; non-static Beta & Charlie nested in B; Charlie calls an inherited field and method in Beta");
      
   var b20_5: B20 = new B20();
   var charlie20_5: B20.Charlie20 = new b20_5.Charlie20();
   
   expected = "B20.Beta20.valueB";
   actual = "";
   actual = charlie20_5.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 20.5(1)", expected, actual, "");
      
   expected = "B20.Beta20.BarB()";
   actual = "";
   actual = charlie20_5.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 20.5(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.1 Static Alpha nested in A; static Beta & non-static Charlie nested in B; Instantiate Alpha; access Alpha's field and method");
      
   var alpha21_1: A21.Alpha21 = new A21.Alpha21();
   
   expected = "A21.Alpha21.valueA";
   actual = "";
   actual = alpha21_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 21.1(1)", expected, actual, "");
   
   expected = "A21.Alpha21.BarA()";
   actual = "";
   actual = alpha21_1.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 21.1(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.2 Static Alpha nested in A; static Beta & non-static Charlie nested in B; Instantiate Beta; access Beta's field and method");
      
   var beta21_2: B21.Beta21 = new B21.Beta21();
   
   expected = "B21.Beta21.valueB";
   actual = "";
   actual = beta21_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 21.2(1)", expected, actual, "");
   
   expected = "B21.Beta21.BarB()";
   actual = "";
   actual = beta21_2.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 21.2(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.3 Static Alpha nested in A; static Beta & non-static Charlie nested in B; Instantiate Charlie; access Charlie's field and method");
   
   var b21_3: B21 = new B21();   
   var charlie21_3: B21.Charlie21 = new b21_3.Charlie21();
   
   expected = "B21.Charlie21.valueC";
   actual = "";
   actual = charlie21_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 21.3(1)", expected, actual, "");
   
   expected = "B21.Charlie21.BarC()";
   actual = "";
   actual = charlie21_3.BarC();
   if (actual != expected)
      apLogFailInfo ("Error in 21.3(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.4 Static Alpha nested in A; static Beta & non-static Charlie nested in B; Charlie calls an inherited field and method in Alpha");
   
   var b21_4: B21 = new B21();   
   var charlie21_4: B21.Charlie21 = new b21_4.Charlie21();
   
   expected = "A21.Alpha21.valueA";
   actual = "";
   actual = charlie21_4.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 21.4(1)", expected, actual, "");
   
   expected = "A21.Alpha21.BarA()";
   actual = "";
   actual = charlie21_4.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 21.4(2)", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.5 Static Alpha nested in A; static Beta & non-static Charlie nested in B; Charlie calls an inherited field and method in Beta");
   
   var b21_5: B21 = new B21();   
   var charlie21_5: B21.Charlie21 = new b21_5.Charlie21();
   
   expected = "B21.Beta21.valueB";
   actual = "";
   actual = charlie21_5.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 21.5(1)", expected, actual, "");
   
   expected = "B21.Beta21.BarB()";
   actual = "";
   actual = charlie21_5.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 21.5(2)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.1 Non-static Alpha not nested; non-static Beta nested in B; Instantiate Alpha; access Alpha's field and method");
                     
   var alpha22_1: Alpha22 = new Alpha22();
   
   expected = "Alpha22.valueA";
   actual = "";
   actual = alpha22_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 22.1(1)", expected, actual, "");
      
   expected = "Alpha22.BarA()";  
   actual = "";
   actual = alpha22_1.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 22.1(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.2 Non-static Alpha not nested; non-static Beta nested in B; Instantiate Beta; access Beta's field and method");

   var b22_2: B22 = new B22();
   var beta22_2: B22.Beta22 = new b22_2.Beta22();

   expected = "Beta22.valueB";
   actual = "";
   actual = beta22_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 22.2(1)", expected, actual, "");
      
   expected = "Beta22.BarB()";  
   actual = "";
   actual = beta22_2.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 22.2(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.3 Non-static Alpha not nested; non-static Beta nested in B; Instantiate Charlie; access Charlie's field and method");
                        
   var b22_3: B22 = new B22();
   var charlie22_3: B22.Charlie22 = new b22_3.Charlie22();
   
   expected = "Beta22.valueC";
   actual = "";
   actual = charlie22_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 22.3(1)", expected, actual, "");
      
   expected = "Beta22.BarC()";
   actual = "";
   actual = charlie22_3.BarC();
   if (actual != expected)
      apLogFailInfo ("Error in 22.3(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.4 Non-static Alpha not nested; non-static Beta nested in B; Charlie calls an inherited field and method in Alpha");
      
   var b22_4: B22 = new B22();
   var charlie22_4: B22.Charlie22 = new b22_4.Charlie22();
   
   expected = "Alpha22.valueA";
   actual = "";
   actual = charlie22_4.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 22.4(1)", expected, actual, "");
      
   expected = "Alpha22.BarA()";
   actual = "";
   actual = charlie22_4.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 22.4(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.5 Non-static Alpha not nested; non-static Beta nested in B; Charlie calls an inherited field and method in Beta");
                        
   var b22_5: B22 = new B22();
   var charlie22_5: B22.Charlie22 = new b22_5.Charlie22();
                        
   expected = "Beta22.valueB";
   actual = "";
   actual = charlie22_5.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 22.5(1)", expected, actual, "");

   expected = "Beta22.BarB()";
   actual = "";
   actual = charlie22_5.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 22.5(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.1 Non-static Alpha not nested; static Beta nested in B; Instantiate Alpha; access Alpha's field and method");
      
   var alpha23_1: Alpha23 = new Alpha23();
   
   expected = "Alpha23.valueA";
   actual = "";
   actual = alpha23_1.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 23.1(1)", expected, actual, "");
      
   expected = "Alpha23.BarA()";  
   actual = "";
   actual = alpha23_1.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 23.1(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.2 Non-static Alpha not nested; static Beta nested in B; Instantiate Beta; access Beta's field and method");
      
   var beta23_2: B23.Beta23 = new B23.Beta23();
   
   expected = "Beta23.valueB";
   actual = "";
   actual = beta23_2.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 23.2(1)", expected, actual, "");
      
   expected = "Beta23.BarB()";
   actual = beta23_2.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 23.2(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.3 Non-static Alpha not nested; static Beta nested in B; Instantiate Charlie; access Charlie's field and method");
                        
   var b23_3: B23 = new B23();
   var charlie23_3: B23.Charlie23 = new b23_3.Charlie23();
   
   expected = "Beta23.valueC";
   actual = "";
   actual = charlie23_3.valueC;
   if (actual != expected)
      apLogFailInfo ("Error in 23.3(1)", expected, actual, "");
      
   expected = "Beta23.BarC()";
   actual = "";
   actual = charlie23_3.BarC();
   if (actual != expected)
      apLogFailInfo ("Error in 23.3(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.4 Non-static Alpha not nested; static Beta nested in B; Charlie calls an inherited field and method in Alpha");
                        
   var b23_4: B23 = new B23();
   var charlie23_4: B23.Charlie23 = new b23_4.Charlie23();
   
   expected = "Alpha23.valueA";
   actual = "";
   actual = charlie23_4.valueA;
   if (actual != expected)
      apLogFailInfo ("Error in 23.4(1)", expected, actual, "");
      
   expected = "Alpha23.BarA()";
   actual = "";
   actual = charlie23_4.BarA();
   if (actual != expected)
      apLogFailInfo ("Error in 23.4(2)", expected, actual, "");       
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.5 Non-static Alpha not nested; static Beta nested in B; Charlie calls an inherited field and method in Beta");
                          
   var b23_5: B23 = new B23();
   var charlie23_5: B23.Charlie23 = new b23_5.Charlie23();
                        
   expected = "Beta23.valueB";
   actual = "";
   actual = charlie23_5.valueB;
   if (actual != expected)
      apLogFailInfo ("Error in 23.5(1)", expected, actual, "");

   expected = "Beta23.BarB()";
   actual = "";
   actual = charlie23_5.BarB();
   if (actual != expected)
      apLogFailInfo ("Error in 23.5(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24. Static Alpha not nested; non-static Beta & Charlie nested in B");
      
   if (exceptionThrown24 == false)
      apLogFailInfo ("No compile error in 24", "Should give a compile error", exceptionThrown24, "");
   if (actualError24 != expectedError24)
      apLogFailInfo ("Wrong compile error in 24", expectedError24, actualError24, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("25. Static Alpha not nested; static Beta & non-static Charlie nested in B");
      
   if (exceptionThrown25 == false)
      apLogFailInfo ("No compile error in 25", "Should give a compile error", exceptionThrown25, "");
   if (actualError25 != expectedError25)
      apLogFailInfo ("Wrong compile error in 25", expectedError25, actualError25, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26. A.Bar() instantiates non-static Alpha; calls Alpha's field and method");
      
   var a26: A26 = new A26();
   
   expected = "A26.Alpha26.valueA";
   actual = "";
   actual = a26.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 26(1)", expected, actual, "");
      
   expected = "A26.Alpha26.BarA()";
   actual = "";
   actual = a26.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 26(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27. A.Bar() instantiates static Alpha; calls Alpha's field and method");
      
   var a27: A27 = new A27();
   
   expected = "A27.Alpha27.valueA";
   actual = "";
   actual = a27.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 27(1)", expected, actual, "");
      
   expected = "A27.Alpha27.BarA()";
   actual = "";
   actual = a27.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 27(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("28. B.Bar() instantiates non-static Alpha; calls Alpha's field and method");
      
   var b28: B28 = new B28();
   
   expected = "A28.Alpha28.valueA";
   actual = "";
   actual = b28.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 28(1)", expected, actual, "");
   
   expected = "A28.Alpha28.BarA()";
   actual = "";
   actual = b28.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 28(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. B.Bar() instantiates static Alpha; calls Alpha's field and method");
      
   var b29: B29 = new B29();
   
   expected = "A29.Alpha29.valueA";
   actual = "";
   actual = b29.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 29(1)", expected, actual, "");
   
   expected = "A29.Alpha29.BarA()";
   actual = "";
   actual = b29.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 29(2)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30. B.Bar() instantiates private class Alpha contained in A");
        
   if (exceptionThrown30 == false)
      apLogFailInfo ("No compile error in 30", "Should give a compile error", exceptionThrown30, "236975");
   if (actualError30 != expectedError30)
      apLogFailInfo ("Wrong compile error in 30", expectedError30, actualError30, "236975");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31. B.Bar() instantiates protected class Alpha contained in A");

   var b31: B31 = new B31();
   
   expected = "A31.Alpha.value";
   actual = "";
   actual = b31.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 31(1)", expected, actual, "238090");
      
   expected = "A31.Alpha.Bar()";
   actual = "";
   actual = b31.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 31(2)", expected, actual, "238090");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("32. B.Bar() instantiates public class Alpha contained in A");
      
   var b32: B32 = new B32();
   
   expected = "A32.Alpha32.value";
   actual = "";
   actual = b32.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 32(1)", expected, actual, "");
      
   expected = "A32.Alpha32.Bar()";
   actual = "";
   actual = b32.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 32(2)", expected, actual, "");      
      
   var a32: A32 = new A32();
   var alpha32: A32.Alpha32 = new a32.Alpha32();
   
   expected = "A32.Alpha32.value";
   actual = "";
   actual = alpha32.value;
   if (actual != expected)
      apLogFailInfo ("Error in 32(3)", expected, actual, "");      

   expected = "A32.Alpha32.Bar()";
   actual = "";
   actual = alpha32.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 32(4)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("33. B.Bar() instantiates internal class Alpha contained in A");

   var b33: B33 = new B33();
   
   expected = "A33.Alpha33.value";
   actual = "";
   actual = b33.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 33(1)", expected, actual, "238109");
      
   expected = "A33.Alpha33.Bar()";
   actual = "";
   actual = b33.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 33(2)", expected, actual, "238109");      
      
   var a33: A33 = new A33();
   var alpha33: A33.Alpha33 = new a33.Alpha33();
   
   expected = "A33.Alpha33.value";
   actual = "";
   actual = alpha33.value;
   if (actual != expected)
      apLogFailInfo ("Error in 33(3)", expected, actual, "238109");      

   expected = "A33.Alpha33.Bar()";
   actual = "";
   actual = alpha33.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 33(4)", expected, actual, "238109");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("34. B.Bar() instantiates internal class Alpha contained in A -- inside a package");
        
   var b34: B34 = new B34();
   
   expected = "A34.Alpha34.value";
   actual = "";
   actual = b34.Bar1();
   if (actual != expected)
      apLogFailInfo ("Error in 34(1)", expected, actual, "");
      
   expected = "A34.Alpha34.Bar()";
   actual = "";
   actual = b34.Bar2();
   if (actual != expected)
      apLogFailInfo ("Error in 34(2)", expected, actual, "");  
      
   
   apEndTest();
}



nestedclasses1();


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
