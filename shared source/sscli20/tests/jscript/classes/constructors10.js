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


var iTestID = 193764;




// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown19_1 = false;
var actualError19_1 = "";
var expectedError19_1 = "ReferenceError: 'Alpha19_1' is not accessible from this scope";

try
{
   eval ("  class Alpha19_1                              " +
         "  {                                            " +
         "     protected function Alpha19_1()            " +
         "     {  }                                      " +
         "  }                                            " +
         "  var alpha19_1: Alpha19_1 = new Alpha19_1();  ", "unsafe");
}
catch (error)
{
   exceptionThrown19_1 = true;
   actualError19_1 = error;
}



// -----------------------------------------------------------------------
class Alpha19_2
{
   public var value: String = "none";
   
   protected function Alpha19_2()
   {
      value = "one";
   }
}

class Beta19_2 extends Alpha19_2
{
}


// -----------------------------------------------------------------------
var exceptionThrown19_3 = false;
var actualError19_3 = "";
var expectedError19_3 = "ReferenceError: 'Beta19_3' is not accessible from this scope";

try
{
   eval ("  class Alpha19_3                           " +
         "  {                                         " +
         "  }                                         " +
         "  class Beta19_3 extends Alpha19_3          " +
         "  {                                         " +
         "     protected function Beta19_3()          " +
         "     {                                      " +
         "     }                                      " +
         "  }                                         " +
         "  var beta19_3: Beta19_3 = new Beta19_3();  ", "unsafe");
}
catch (error)
{
   exceptionThrown19_3 = true;
   actualError19_3 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown19_4 = false;
var actualError19_4 = "";
var expectedError19_4 = "ReferenceError: 'Charlie19_4' is not accessible from this scope";

try
{
   eval ("  class Alpha19_4                                    " +
         "  {  }                                               " +
         "  class Beta19_4 extends Alpha19_4                   " +
         "  {  }                                               " +
         "  class Charlie19_4 extends Beta19_4                 " +
         "  {                                                  " +
         "     protected function Charlie19_4()                " +
         "     {  }                                            " +
         "  }                                                  " +
         "  var charlie19_4: Charlie19_4 = new Charlie19_4();  ", "unsafe");
}
catch (error)
{
   exceptionThrown19_4 = true;
   actualError19_4 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown19_5 = false;
var actualError19_5 = "";
var expectedError19_5 = "ReferenceError: 'Beta19_5' is not accessible from this scope";

try
{
   eval ("  class Alpha19_5                              " +
         "  {                                            " +
         "     protected function Alpha19_5()            " +
         "     {  }                                      " +
         "  }                                            " +
         "  class Beta19_5 extends Alpha19_5             " +
         "  {                                            " +
         "     protected function Beta19_5()             " +
         "     {  }                                      " +
         "  }                                            " +
         "  var beta19_5: Beta19_5 = new Beta19_5();     ", "unsafe");
}
catch (error)
{
   exceptionThrown19_5 = true;
   actualError19_5 = error;
}


// -----------------------------------------------------------------------
class Alpha19_6
{
   protected function Alpha19_6 (x: boolean, y: String)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha19_7
{
   public var value: String = "none";
   
   protected function Alpha19_7 (x: int, y: boolean)
   {
      value = "five";
   }
}

class Beta19_7 extends Alpha19_7
{
}


// -----------------------------------------------------------------------
class Alpha19_8
{
}

class Beta19_8 extends Alpha19_8
{
   protected function Beta19_8 (x: boolean, y: double)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha19_9
{
}

class Beta19_9 extends Alpha19_9
{
}

class Charlie19_9 extends Beta19_9
{
   protected function Charlie19_9 (x: double, y: boolean, z: String)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha19_10
{
   protected function Alpha19_10 (x: String, y: boolean, z: int)
   {
   }
}

class Beta19_10 extends Alpha19_10
{
   protected function Beta19_10 (x: int, y: String, z: Boolean)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha19_11_1
{
   public var value: String = "none";
   
   protected function Alpha19_11_1()
   {
      value = "one";
   }
   
   protected function Alpha19_11_1 (x: int, y: boolean)
   {
      value = "two";
   }
}

class Beta19_11_1 extends Alpha19_11_1
{
   public function Beta19_11_1 (x: int, y: boolean)
   {
      super (x, y);
   }
}


// -----------------------------------------------------------------------
class Alpha19_11_2
{
   public var value: String = "none";
   
   protected function Alpha19_11_2()
   {
      value = "one";
   }
   
   protected function Alpha19_11_2 (x: int, y: boolean)
   {
      value = "two";
   }
}

class Beta19_11_2 extends Alpha19_11_2
{
   public function Beta19_11_2 (x: int, y: boolean)
   {
   }
}




// -----------------------------------------------------------------------
class Alpha20_1
{  
   public var value: String = "none";
   
   internal function Alpha20_1()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
class Alpha20_2
{
   public var value: String = "";
   
   internal function Alpha20_2()
   {
      value = "six";
   }
}

class Beta20_2 extends Alpha20_2
{
}


// -----------------------------------------------------------------------
class Alpha20_3
{
   public var value: String = "none";
}

class Beta20_3 extends Alpha20_3
{
   internal function Beta20_3()
   {
      value = "nine";
   }
}


// -----------------------------------------------------------------------
class Alpha20_4
{
   public var value: String = "none";
}

class Beta20_4 extends Alpha20_4
{
}

class Charlie20_4 extends Beta20_4
{
   internal function Charlie20_4()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha20_5
{
   public var value: String = "none";
   
   internal function Alpha20_5()
   {
      value = "five";
   }
}

class Beta20_5 extends Alpha20_5
{
   internal function Beta20_5()
   {
   }
}



// -----------------------------------------------------------------------
class Alpha20_6
{
   public var value: String = "";
   
   internal function Alpha20_6 (x: boolean, y: String)
   {
      value = "seven";
   }
}


// -----------------------------------------------------------------------
class Alpha20_7
{
   public var value: String = "none";
   
   internal function Alpha20_7 (x: int, y: boolean)
   {
      value = "ten";   
   }
}

class Beta20_7 extends Alpha20_7
{
}


// -----------------------------------------------------------------------
class Alpha20_8
{
   public var value: String = "none";
}

class Beta20_8 extends Alpha20_8
{
   internal function Beta20_8 (x: boolean, y: double)
   {
      value = "six";
   }
}


// -----------------------------------------------------------------------
class Alpha20_9
{
   public var value: String = "none";
}

class Beta20_9 extends Alpha20_9
{
}

class Charlie20_9 extends Beta20_9
{
   internal function Charlie20_9 (x: double, y: boolean, z: String)
   {
      value = "eight";
   }
}


// -----------------------------------------------------------------------
class Alpha20_10
{
   public var value: String = "none";
   
   internal function Alpha20_10 (x: String, y: boolean, z: int)   
   {
      value = "three";
   }
}

class Beta20_10 extends Alpha20_10
{
   internal function Beta20_10 (x: int, y: String, z: Boolean)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha20_11_1
{
   public var value: String = "none";
   
   internal function Alpha20_11_1()
   {
      value = "one";
   }
   
   internal function Alpha20_11_1 (x: int, y: boolean)
   {
      value = "two";
   }
}

class Beta20_11_1 extends Alpha20_11_1
{
   public function Beta20_11_1 (x: int, y: boolean)
   {
      super (x, y);
   }
}


// -----------------------------------------------------------------------
class Alpha20_11_2
{
   public var value: String = "none";
   
   internal function Alpha20_11_2()
   {
      value = "one";
   }
   
   internal function Alpha20_11_2 (x: int, y: boolean)
   {
      value = "two";
   }
}

class Beta20_11_2 extends Alpha20_11_2
{
   public function Beta20_11_2 (x: int, y: boolean)
   {
   }
}


// -----------------------------------------------------------------------
class Alpha21_1
{
   public var value: String = "none";
   
   private function Alpha21_1()
   {
      value = "one";
   }
   
   public function Alpha21_1 (x: String)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha21_2
{
   private function Alpha21_2()
   {
   }
   
   static public var value: String = "three";
}


// -----------------------------------------------------------------------
class Alpha21_3
{
   private function Alpha21_3()
   {
   }
   
   static public function Bar(): String
   {
      return "seven";
   }
}


// -----------------------------------------------------------------------
class Alpha21_4
{
   public var value: String = "none";
   
   private function Alpha21_4()
   {  
      value = "one";
   }
   
   public function Alpha21_4 (x: String)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha21_5
{
   public var value: String = "none";
   
   private function Alpha21_5()
   {
      value = "one";
   }
   
   public function Alpha21_5 (x: String)
   {
      value = "two";
   }
   
   public function Bar()
   {
      return value;
   }
}


// -----------------------------------------------------------------------
class Alpha22_1
{
   public var value: String = "none";
   
   protected function Alpha22_1()
   {
      value = "one";
   }
   
   public function Alpha22_1 (x: String)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha22_2
{
   protected function Alpha22_2()
   {
   }
   
   static public var value: String = "three";
}


// -----------------------------------------------------------------------
class Alpha22_3
{
   protected function Alpha22_3()
   {
   }
   
   static public function Bar(): String
   {
      return "seven";
   }
}


// -----------------------------------------------------------------------
class Alpha22_4
{
   public var value: String = "none";
   
   protected function Alpha22_4()
   {
      value = "one";
   }
   
   public function Alpha22_4 (x: String)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha22_5
{
   public var value: String = "none";
   
   protected function Alpha22_5()
   {
      value = "one";
   }
   
   public function Alpha22_5 (x: String)
   {
      value = "two";
   }
   
   public function Bar(): String
   {
      value = "three";
      return value;
   }
}


// -----------------------------------------------------------------------
class Alpha23_1
{
   public var value: String = "none";
   
   internal function Alpha23_1()
   {
      value = "one";
   }
   
   public function Alpha23_1 (x: String)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha23_2
{
   internal function Alpha23_2()
   {
   }
   
   static public var value: String = "ten";
}


// -----------------------------------------------------------------------
class Alpha23_3
{
   internal function Alpha23_3()
   {
   }
   
   static public function Bar(): String
   {
      return "twelve";
   }
}


// -----------------------------------------------------------------------
class Alpha23_4
{
   public var value: String = "none";
   
   internal function Alpha23_4()
   {
      value = "five";
   }
   
   public function Alpha23_4 (x: String)
   {
      value = "six";
   }
}


// -----------------------------------------------------------------------
class Alpha23_5
{
   public var value: String = "none";
   
   internal function Alpha23_5()
   {
      value = "ten";
   }
   
   public function Alpha23_5 (x: String)
   {
      value = "eleven";
   }
   
   public function Bar(): String
   {
      value = "twelve";
      return value;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown24_1 = false;
var actualError24_1 = "";
var expectedError24_1 = "Error: Not valid for a constructor";

try
{
   eval ("  class Alpha24_1                        " +
         "  {                                      " +
         "     static public function Alpha24_1()  " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown24_1 = true;
   actualError24_1 = error
}


// -----------------------------------------------------------------------
var exceptionThrown24_2 = false;
var actualError24_2 = "";
var expectedError24_2 = "Error: Not valid for a constructor";

try
{
   eval ("  class Alpha24_2                                 " +
         "  {                                               " +
         "     static public function Alpha24_2 (x: String) " +
         "     { }                                          " +
         "  }                                               ", "unsafe");
}
catch (error)
{
   exceptionThrown24_2 = true;
   actualError24_2 = error
}


// -----------------------------------------------------------------------
var exceptionThrown25_1 = false;
var actualError25_1 = "";
var expectedError25_1 = "Error: Not valid for a constructor";

try
{
   eval ("  abstract class Alpha25_1                  " +
         "  {                                         " +
         "     abstract public function Alpha25_1();  " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown25_1 = true;
   actualError25_1 = error
}


// -----------------------------------------------------------------------
var exceptionThrown25_2 = false;
var actualError25_2 = "";
var expectedError25_2 = "Error: Not valid for a constructor";

try
{
   eval ("  abstract class Alpha25_2                           " +
         "  {                                                  " +
         "     abstract public function Alpha25_2 (x: String); " +
         "  }                                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown25_2 = true;
   actualError25_2 = error
}


// -----------------------------------------------------------------------
class Alpha26_1
{
   public var value: String = "none";
   
   public function Alpha26_1()
   {
      value = "one";
   }
}

class Beta26_1 extends Alpha26_1
{
   public function Alpha26_1()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha26_2
{
   public var value: String = "none";
   
   public function Alpha26_2()
   {
      value = "three";
   }
}

class Beta26_2 extends Alpha26_2
{
   public function Alpha26_2()
   {
      value = "four";
   }
}

class Charlie26_2 extends Beta26_2
{
   public function Alpha26_2()
   {
      value = "five";
   }
}


// -----------------------------------------------------------------------
class Alpha26_3
{
   public var value: String = "none";
   
   public function Alpha26_3 (x: double)
   {
      value = "ten";
   }
}

class Beta26_3 extends Alpha26_3
{
   public function Alpha26_3 (x: double)
   {
      value = "eleven";
   }
}


// -----------------------------------------------------------------------
class Alpha26_4
{
   public var value: String = "none";
   
   public function Alpha26_4 (x: double)
   {
      value = "five";
   }
}

class Beta26_4 extends Alpha26_4
{
   public function Alpha26_4 (x: double)
   {
      value = "six";
   }
}

class Charlie26_4 extends Beta26_4
{
   public function Alpha26_4 (x: double)
   {
      value = "seven";
   }
}


// -----------------------------------------------------------------------
class Alpha27_1
{
   public var value: String = "zero";
   
   public function Alpha27_1 (x: int, y: double)
   {
      value = "three";
   }
}

class Beta27_1 extends Alpha27_1
{
   public function Alpha27_1 (x: boolean, y: String)
   {
      value = "four";
   }
}


// -----------------------------------------------------------------------
class Alpha27_2
{
   public var value: String = "zero";
   
   public function Alpha27_2 (x: int, y: double)
   {
      value = "five";
   }
}

class Beta27_2 extends Alpha27_2
{
   public function Alpha27_2 (x: boolean, y: String)
   {
      value = "six";
   }
}

class Charlie27_2 extends Beta27_2
{
   public function Alpha27_2 (x: String, y: int)
   {
      value = "seven";
   }
}


// -----------------------------------------------------------------------
class Alpha28_1
{
   public var value: String = "none";
   
   public function Alpha28_1()
   {
      value = "one";
   }
   
   public function Alpha28_1 (x: int, y: boolean)
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
class Alpha28_2
{
   public var value: String = "none";
   
   public function Alpha28_2()
   {
      value = "one";
   }
   
   public function Alpha28_2 (x: int, y: boolean)
   {
      value = "two";
   }
}

class Beta28_2 extends Alpha28_2
{
}


// -----------------------------------------------------------------------
var exceptionThrown29 = false;
var actualError29 = "";
var expectedError29 = "ReferenceError: More than one constructor matches this argument list";

try
{
   eval ("  class Alpha29                          " +
         "  {                                      " +
         "     public function Alpha29 (x: int)    " +
         "     {                                   " +
         "     }                                   " +
         "     public function Alpha29 (x: boolean)" +
         "     {                                   " +
         "     }                                   " +
         "  }                                      " +
         "  class Beta29 extends Alpha29           " +
         "  {                                      " +
         "     public function Beta29 ()           " +
         "     {                                   " +
         "     }                                   " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown29 = true;
   actualError29 = error;
}


// -----------------------------------------------------------------------
class Alpha30_1
{
   public var value: String = "none";
   
   public function Alpha30_1 ()
   {
      this (false);
   }
   
   public function Alpha30_1 (x: boolean)
   {
      value = "three";
   }
}


// -----------------------------------------------------------------------
class Alpha30_2
{
   public var value: String = "none";
   
   public function Alpha30_2 (x: String)
   {
      value = "eleven";
   }
   
   public function Alpha30_2 (x: int)
   {
      this ("hello");
   }
}


// -----------------------------------------------------------------------
class Alpha30_3
{
   public var value: String = "none";
   
   public function Alpha30_3 (x: String)
   {
      this (23, false);
   }
   
   public function Alpha30_3 (x: int, y: boolean)
   {
      value = "twelve";
   }
}



// -----------------------------------------------------------------------
function constructors10()
{
   apInitTest ("Constructors10");


   // -----------------------------------------------------------------------
   apInitScenario ("19.1 Protected constructor -- Simple class with protected default constructor");
   
   if (exceptionThrown19_1 == false)
      apLogFailInfo ("No compile error in 19.1", "Should give a compile error", exceptionThrown19_1, "");
   if (actualError19_1 != expectedError19_1)
      apLogFailInfo ("Wrong compile error in 19.1", expectedError19_1, actualError19_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.2 Protected constructor -- Superclass with protected default constructor");
   
   var beta19_2: Beta19_2 = new Beta19_2();
   
   expected = "one";
   actual = "";
   actual = beta19_2.value;
   if (actual != expected)
      apLogFailInfo ("Error", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.3 Protected constructor -- Subclass with protected default constructor");

   if (exceptionThrown19_3 == false)
      apLogFailInfo ("No compile error in 19.3", "Should give a compile error", exceptionThrown19_3, "");
   if (actualError19_3 != expectedError19_3)
      apLogFailInfo ("Wrong compile error in 19.3", expectedError19_3, actualError19_3, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("19.4 Protected constructor -- Sub-subclass with protected default constructor");

   if (exceptionThrown19_4 == false)
      apLogFailInfo ("No compile error in 19.4", "Should give a compile error", exceptionThrown19_4, "");
   if (actualError19_4 != expectedError19_4)
      apLogFailInfo ("Wrong compile error in 19.4", expectedError19_4, actualError19_4, "");


   // -----------------------------------------------------------------------
   apInitScenario ("19.5 Protected constructor -- Superclass and subclass with protected default constructors");
   
   if (exceptionThrown19_5 == false)
      apLogFailInfo ("No compile error in 19.5", "Should give a compile error", exceptionThrown19_5, "");
   if (actualError19_5 != expectedError19_5)
      apLogFailInfo ("Wrong compile error in 19.5", expectedError19_5, actualError19_5, "");
   

   // -----------------------------------------------------------------------
   apInitScenario ("19.6 Protected constructor -- Simple class with protected constructor w/ parameters");
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: No such constructor";
   
   try
   {
      eval ("var alpha19_6: Alpha19_6 = new Alpha19_6 (true, \"hello\");");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.7 Protected constructor -- Superclass with protected constructor w/ parameters");
   
   var beta19_7: Beta19_7 = new Beta19_7();
   
   expected = "five";
   actual = "";
   actual = beta19_7.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19.7", expected, actual, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("19.8 Protected constructor -- Subclass with protected constructor w/ parameters");
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: No such constructor";
   
   try
   {
      eval ("var beta19_8: Beta19_8 = new Beta19_8();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;   
   }

   if (exceptionThrown == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.9 Protected constructor -- Sub-subclass with protected constructor w/ parameters");
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: No such constructor";
   
   try
   {
      eval ("var charlie19_9: Charlie19_9 = new Charlie19_9 (20, false, \"Microsoft\");");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.10 Protected constructor -- Superclass and subclass with protected constructor w/ parameters");

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: No such constructor";
   
   try
   {
      eval ("var beta19_10: Beta19_10 = new Beta19_10 (5, \"five\", true);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.11.1 Protected constructor -- \n " +
                   "        Superclass with protected default constructor and protected constructor w/ parameters \n" +
                   "        Subclass constructor calls superclass' constructor w/ parameters using super (…)");
   
   var beta19_11_1: Beta19_11_1 = new Beta19_11_1 (20, false);
   
   expected = "two";
   actual = "";
   actual = beta19_11_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19.11.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("19.11.2 Protected constructor -- \n " +
                   "        Superclass with protected default constructor and protected constructor w/ parameters \n" +
                   "        Subclass constructor does not call any superclass constructor");
                   
   var beta19_11_2: Beta19_11_2 = new Beta19_11_2 (10, true);
   
   expected = "one";
   actual = "";
   actual = beta19_11_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 19.11.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.1 Internal constructor -- Simple class with internal default constructor");
   
   var alpha20_1: Alpha20_1 = new Alpha20_1();
   
   expected = "one";
   actual = "";
   actual = alpha20_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.2 Internal constructor -- Superclass with internal default constructor");
   
   var beta20_2: Beta20_2 = new Beta20_2();
   
   expected = "six";
   actual = "";
   actual = beta20_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.3 Internal constructor -- Subclass with internal default constructor");
   
   var beta20_3: Beta20_3 = new Beta20_3();
   
   expected = "nine";
   actual = "";
   actual = beta20_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.3", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.4 Internal constructor -- Sub-subclass with internal default constructor");
   
   var charlie20_4: Charlie20_4 = new Charlie20_4();
   
   expected = "two";
   actual = "";
   actual = charlie20_4.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 20.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.5 Internal constructor -- Superclass and subclass with internal default constructors");

   var beta20_5: Beta20_5 = new Beta20_5();
   
   expected = "five";
   actual = "";
   actual = beta20_5.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.5", expected, actual, "");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.6 Internal constructor -- Simple class with internal constructor w/ parameters");
   
   var alpha20_6: Alpha20_6 = new Alpha20_6 (false, "something");
   
   expected = "seven";
   actual = "";
   actual = alpha20_6.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.6", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.7 Internal constructor -- Superclass with internal constructor w/ parameters");
   
   var beta20_7: Beta20_7 = new Beta20_7();
   
   expected = "ten";
   actual = "";
   actual = beta20_7.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.7", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("20.8 Internal constructor -- Subclass with internal constructor w/ parameters");
   
   var beta20_8: Beta20_8 = new Beta20_8 (false, 23);
   
   expected = "six";
   actual = "";
   actual = beta20_8.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.8", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.9 Internal constructor -- Sub-subclass with internal constructor w/ parameters");
   
   var charlie20_9: Charlie20_9 = new Charlie20_9 (20, false, "something");
   
   expected = "eight";
   actual = "";
   actual = charlie20_9.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.9", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.10 Internal constructor -- Superclass and subclass with internal constructor w/ parameters");

   var beta20_10: Beta20_10 = new Beta20_10 (5, "five", false);
   
   expected = "three";
   actual = "";
   actual = beta20_10.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.10", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.11.1 Internal constructor -- \n " +
                   "        Superclass with internal default constructor and internal constructor w/ parameters \n" +
                   "        Subclass constructor calls superclass' constructor w/ parameters using super (…)");
      
   var beta20_11_1: Beta20_11_1 = new Beta20_11_1 (25, true);
   
   expected = "two";
   actual = "";
   actual = beta20_11_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.11.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("20.11.2 Internal constructor -- \n " +
                   "        Superclass with internal default constructor and internal constructor w/ parameters \n" +
                   "        Subclass constructor does not call any superclass constructor");
   
   var beta20_11_2: Beta20_11_2 = new Beta20_11_2 (21, true);
   
   expected = "one";
   actual = "";
   actual = beta20_11_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 20.11.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.1 Private default constructor and public non-default constructor");

   var alpha21_1: Alpha21_1 = new Alpha21_1 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha21_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 21.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.2 Private default constructor and static field");
      
   expected = "three";
   actual = "";
   actual = Alpha21_2.value;
   if (actual != expected)
      apLogFailInfo ("Error", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.3 Private default constructor and static method");
      
   expected = "seven";
   actual = "";
   actual = Alpha21_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("21.4 Private default constructor and non-static field");
   
   var alpha21_4: Alpha21_4 = new Alpha21_4 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha21_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 21.4", expected, actual, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("21.5 Private default constructor and non-static method");
   
   var alpha21_5: Alpha21_5 = new Alpha21_5 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha21_5.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 21.5", expected, actual, "");

      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.1 Protected default constructor and public non-default constructor");
   
   var alpha22_1: Alpha22_1 = new Alpha22_1 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha22_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.2 Protected default constructor and static field");
      
   expected = "three";
   actual = "";
   actual = Alpha22_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.3 Protected default constructor and static method");
      
   expected = "seven";
   actual = "";
   actual = Alpha22_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 22.3", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.4 Protected default constructor and non-static field");
      
   var alpha22_4: Alpha22_4 = new Alpha22_4 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha22_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 22.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("22.5 Protected default constructor and non-static method");
   
   var alpha22_5: Alpha22_5 = new Alpha22_5 ("something");
   
   expected = "three";
   actual = "";
   actual = alpha22_5.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 22.5", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.1 Internal default constructor and public non-default constructor");
   
   var alpha23_1: Alpha23_1 = new Alpha23_1 ("something");
   
   expected = "two";
   actual = "";
   actual = alpha23_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.2 Internal default constructor and static field");
      
   expected = "ten";
   actual = "";
   actual = Alpha23_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.3 Internal default constructor and static method");
      
   expected = "twelve";
   actual = "";
   actual = Alpha23_3.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23.3", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.4 Internal default constructor and non-static field");
      
   var alpha23_4: Alpha23_4 = new Alpha23_4 ("something");
   
   expected = "six";
   actual = "";
   actual = alpha23_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 23.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("23.5 Internal default constructor and non-static method");
   
   var alpha23_5: Alpha23_5 = new Alpha23_5 ("something");
   
   expected = "twelve";
   actual = "";
   actual = alpha23_5.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 23.5", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24.1 Static default constructor");
   // compile error
   
   if (exceptionThrown24_1 == false)
      apLogFailInfo ("No compile error in 24.1", "Should give a compile error", exceptionThrown24_1, "");
   if (actualError24_1 != expectedError24_1)
      apLogFailInfo ("Wrong compile error in 24.1", expectedError24_1, actualError24_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("24.2 Static non-default constructor");
   // compile error
   
   if (exceptionThrown24_2 == false)
      apLogFailInfo ("No compile error in 24.2", "Should give a compile error", exceptionThrown24_2, "");
   if (actualError24_2 != expectedError24_2)
      apLogFailInfo ("Wrong compile error in 24.2", expectedError24_2, actualError24_2, "");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("25.1 Abstract default constructor");
   // compile error
   
   if (exceptionThrown25_1 == false)
      apLogFailInfo ("No compile error in 25.1", "Should give a compile error", exceptionThrown25_1, "");
   if (actualError25_1 != expectedError25_1)
      apLogFailInfo ("Wrong compile error in 25.1", expectedError25_1, actualError25_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("25.2 Abstract non-default constructor");
   // compile error
   
   if (exceptionThrown25_2 == false)
      apLogFailInfo ("No compile error in 25.2", "Should give a compile error", exceptionThrown25_2, "");
   if (actualError25_2 != expectedError25_2)
      apLogFailInfo ("Wrong compile error in 25.2", expectedError25_2, actualError25_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26.1 Overriding the constructor of a base class -- default constructor; 2 levels of inheritance");
         
   var beta26_1: Beta26_1 = new Beta26_1();
   
   expected = "one";
   actual = "";
   actual = beta26_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.1 (1)", expected, actual, "");
   
   expected = "two";
   actual = "";
   beta26_1.Alpha26_1();
   actual = beta26_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.1 (2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26.2 Overriding the constructor of a base class -- default constructor; 3 levels of inheritance");
         
   var charlie26_2: Charlie26_2 = new Charlie26_2();
   
   expected = "three";
   actual = "";
   actual = charlie26_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.2 (1)", expected, actual, "");
   
   expected = "five";
   actual = "";
   charlie26_2.Alpha26_2();
   actual = charlie26_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.2 (2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26.3 Overriding the constructor of a base class -- non-default constructor; 2 levels of inheritance");
   
   var beta26_3: Beta26_3 = new Beta26_3();
   
   expected = "ten";
   actual = "";
   actual = beta26_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.3 (1)", expected, actual, "");
   
   expected = "eleven";
   actual = "";
   beta26_3.Alpha26_3 (25);
   actual = beta26_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.3 (2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("26.4 Overriding the constructor of a base class -- non-default constructor; 3 levels of inheritance");
   
   var charlie26_4: Charlie26_4 = new Charlie26_4();
   
   expected = "five";
   actual = "";
   actual = charlie26_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.4 (1)", expected, actual, "");
   
   expected = "seven";
   actual = "";
   charlie26_4.Alpha26_4 (25);
   actual = charlie26_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 26.4 (2)", expected, actual, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("27.1 Overloading the constructor of a base class -- non-default constructor; 2 levels of inheritance");
   
   var beta27_1: Beta27_1 = new Beta27_1();
   
   expected = "three";
   actual = "";
   actual = beta27_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27.1 (1)", expected, actual, "");
      
   expected = "four";
   actual = "";
   beta27_1.Alpha27_1 (true, "hello");
   actual = beta27_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27.1 (2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("27.2 Overloading the constructor of a base class -- non-default constructor; 3 levels of inheritance");
   
   var charlie27_2: Charlie27_2 = new Charlie27_2();
   
   expected = "five";
   actual = "";
   actual = charlie27_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27.2 (1)", expected, actual, "");
      
   expected = "seven";
   actual = "";
   charlie27_2.Alpha27_2 ("hello", 25);
   actual = charlie27_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 27.2 (2)", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("28.1 Can the constructor be explicitly called? -- from the base class instance");
   
   var alpha28_1: Alpha28_1 = new Alpha28_1();
   
   exceptionThrown = false;
   expectedError = "ReferenceError: Objects of type 'Alpha28_1' do not have such a member";
   actualError = "";
   
   try
   {
      eval ("alpha28_1.Alpha();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 28.1 (1)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 28.1 (1)", expectedError, actualError, ""); 
      
   exceptionThrown = false;
   expectedError = "ReferenceError: Objects of type 'Alpha28_1' do not have such a member";
   actualError = "";
   
   try
   {
      eval ("alpha28_1.Alpha (23, false);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 28.1 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 28.1 (2)", expectedError, actualError, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("28.2 Can the constructor be explicitly called? -- from the subclass instance");
      
   var beta28_2: Beta28_2 = new Beta28_2();
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Beta28_2' do not have such a member";
   
   try
   {
      eval ("beta28_2.Alpha28_2();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 28.2 (1)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 28.2 (1)", expectedError, actualError, "");
      
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Objects of type 'Beta28_2' do not have such a member";
   
   try
   {
      eval ("beta28_2.Alpha28_2 (21, false);");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 28.2 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 28.2 (2)", expectedError, actualError, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("29. Base class doesn't have a default constructor and" +
                   " has 2 non-default constructors that conflict.  Subclass doesn't make an explicit call");
      
   if (exceptionThrown29 == false)
      apLogFailInfo ("No compile error in 29", "Should give a compile error", exceptionThrown29, "");
   if (actualError29 != expectedError29)
      apLogFailInfo ("Wrong compile error in 29", expectedError29, actualError29, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30.1 Calling other constructors using 'this'. " +
                        "Default constructor calls non-default constructor with boolean parameter");
   
   var alpha30_1: Alpha30_1 = new Alpha30_1();
   
   expected = "three";
   actual = "";
   actual = alpha30_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 30.1", expected, actual, "");
  

   // -----------------------------------------------------------------------
   apInitScenario ("30.2 Calling other constructors using 'this'. " +
                        "Alpha (int) calls Alpha (String) using 'this'");

   var alpha30_2: Alpha30_2 = new Alpha30_2 (25);
   
   expected = "eleven";
   actual = "";
   actual = alpha30_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 30.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30.3 Calling other constructors using 'this'. " +
                        "Alpha (String) calls Alpha (int, boolean) using 'this'");
                        
   var alpha30_3: Alpha30_3 = new Alpha30_3 ("hello");
   
   expected = "twelve";
   actual = "";
   actual = alpha30_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 30.3", expected, actual, "");
      
      
   
   apEndTest();
}



constructors10();


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
