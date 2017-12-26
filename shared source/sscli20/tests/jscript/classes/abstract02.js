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


var iTestID = 172298;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
var exceptionThrown30 = false;
var actualError30 = "";
var expectedError30 = "Error: No implementation provided for 'Charlie30.Bar()'";

try
{
   eval ("  abstract class Alpha30                    " +
         "  {                                         " +
         "     public var value: String = \"\";       " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  class Beta30 extends Alpha30              " +
         "  {                                         " +
         "     public function Bar()                  " +
         "     {                                      " +
         "        value = \"one\";                    " +
         "     }                                      " +
         "  }                                         " +
         "  abstract class Charlie30 extends Beta30   " +
         "  {                                         " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  class Delta30 extends Charlie30           " +
         "  { }                                       ", "unsafe");
}
catch (error)
{
   exceptionThrown30 = true;
   actualError30 = error;
} 


// -----------------------------------------------------------------------
abstract class Alpha31
{
   public var value: String = "none";
   abstract public function Bar();
}        

class Beta31 extends Alpha31
{
   public function Bar()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown32 = false;
var actualError32 = "";
var expectedError32 = "Error: An abstract method cannot be private";

try
{
   eval ("  abstract class Alpha32              " +
         "  {                                   " +
         "     abstract private function Bar(); " +
         "  }                                   ");
}
catch (error)
{
   exceptionThrown32 = true;
   actualError32 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha33
{
   public var value: String = "none";
   abstract protected function Bar();
}        

class Beta33 extends Alpha33
{
   protected function Bar()
   {
      value = "three";
   }
   
   public function Foo()
   {
      Bar();
   }
}


// -----------------------------------------------------------------------
abstract class Alpha34
{
   public var value: String = "none";
   abstract internal function Bar();
}        

class Beta34 extends Alpha34
{
   internal function Bar()
   {
      value = "five";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown35 = false;
var actualError35 = "";
var expectedError35 = "SyntaxError: Syntax error";

try
{
   eval ("  abstract class Alpha                   " +
         "  {                                      " +
         "     abstract abstract function Bar();   " +
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
var expectedError36 = "Error: final and abstract cannot be used together";

try
{
   eval ("  abstract class Alpha36                    " +
         "  {                                         " +
         "     abstract final public function Bar();  " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown36 = true;
   actualError36 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown37 = false;
var actualError37 = "";
var expectedError37 = "Error: Expando methods cannot be abstract";

try
{
   eval ("  abstract class Alpha37                       " +
         "  {                                            " +
         "     abstract expando public function Bar();   " +
         "  }                                            ", "unsafe");
         
}
catch (error)
{
   exceptionThrown37 = true;
   actualError37 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown38 = false;
var actualError38 = "";
var expectedError38 = "Error: Static methods cannot be abstract";

try
{
   eval ("  abstract class Alpha38                    " +
         "  {                                         " +
         "     abstract static public function Bar(); " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown38 = true;
   actualError38 = error;
}


// -----------------------------------------------------------------------
package Group39
{
   abstract class Alpha39
   {
      public var value: String = "none";
      abstract public function Bar();
   }
}

import Group39;

class Beta39 extends Alpha39
{
   public function Bar()
   {
      value = "nine";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown40 = false;
var actualError40 = "";
var expectedError40 = "Error: No implementation provided for 'Group40.Alpha40.Bar()'";

package Group40
{
   abstract class Alpha40
   {
      abstract public function Bar();
   }
}

import Group40;

try
{
   eval ("  class Beta40 extends Alpha40 { }    ");
}
catch (error)
{
   exceptionThrown40 = true;
   actualError40 = error;
}


// -----------------------------------------------------------------------
package GroupA41
{
   abstract class Alpha41
   {
      public var value: String = "none";     
      abstract public function Bar();
   }
}

import GroupA41;

package GroupB41
{
   class Beta41 extends Alpha41
   {
      public function Bar()
      {
         value = "five";
      }
   }
}

import GroupB41;


/*
Eval cannot catch this compile error.
// -----------------------------------------------------------------------
var exceptionThrown42 = false;
var actualError42 = "";
var expectedError42 = "blah";

try
{
   eval ("  package GroupA42                          " +
         "  {                                         " +
         "     abstract class Alpha42                 " +
         "     {                                      " +
         "        abstract public function Bar();     " +
         "     }                                      " +
         "  }                                         " +
         "  import GroupA42;                          " +
         "  package GroupB42                          " +
         "  {                                         " +
         "     class Beta42 extends Alpha42           " +
         "     {  }                                   " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown42 = true;
   actualError42 = error;
}
*/


// -----------------------------------------------------------------------
package Group43
{
   abstract class Alpha43
   {
      public var value: String = "none";
      abstract public function Bar();
   }
   
   class Beta43 extends Alpha43
   {
      public function Bar()
      {
         value = "one";
      }
   }
}

import Group43;


/*
Eval cannot catch this compile error.
// -----------------------------------------------------------------------
var exceptionThrown44 = false;
var actualError44 = "";
var expectedError44 = "blah";

try
{
   eval ("  package Group44                                 " +
         "  {                                               " +
         "     abstract class Alpha44                       " +
         "     {                                            " +
         "         abstract public function Bar();          " +
         "     }                                            " +
         "     class Beta44 extends Alpha44                 " +
         "     {  }                                         " +
         "  }                                               ");
}
catch (error)
{
   exceptionThrown44 = true;
   actualError44 = error;
}
*/


// -----------------------------------------------------------------------
var exceptionThrown45 = false;
var actualError45 = "";
var expectedError45 = "SyntaxError: Abstract function cannot have body";

try
{
   eval ("  abstract class Alpha45                 " +
         "  {                                      " +
         "     abstract public function Bar();     " +
         "  }                                      " +
         "  class Beta45 extends Alpha45           " +
         "  {                                      " +
         "     abstract public function Bar()      " +
         "     {  }                                " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown45 = true;
   actualError45 = error;
}    


// -----------------------------------------------------------------------
abstract class Alpha46
{
   public var value: String = "none";
   abstract public function Bar();
}

class Beta46 extends Alpha46
{
   final public function Bar()
   {
      value = "five";
   }
}  


// -----------------------------------------------------------------------
abstract class Alpha47
{
   public var value: String = "none";
   abstract public function Bar();
}

class Beta47 extends Alpha47
{
   public function Bar()
   {
      value = "seven";
   }
}     

class Charlie47 extends Beta47
{
   final public function Bar()
   {
      value = "eight";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown48 = false;
var actualError48 = "";
var expectedError48 = "Error: Base class member 'Alpha48.Bar' is hidden by this declaration";

try
{
   eval ("  abstract class Alpha48                 " +
         "  {                                      " +
         "     abstract public function Bar();     " +
         "  }                                      " +
         "  class Beta48 extends Alpha48           " +
         "  {                                      " +
         "     expando public function Bar()       " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}  
catch (error)
{
   exceptionThrown48 = true;
   actualError48 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown49 = false;
var actualError49 = "";
var expectedError49 = "Error: No implementation provided for 'Alpha49.Bar()'";

try
{
   eval ("  abstract class Alpha49                 " +
         "  {                                      " +
         "     abstract public function Bar();     " +
         "  }                                      " +
         "  class Beta49 extends Alpha49           " +
         "  {                                      " +
         "     static public function Bar()        " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}  
catch (error)
{
   exceptionThrown49 = true;
   actualError49 = error;
}


// -----------------------------------------------------------------------
class Alpha50_1
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta50_1 extends Alpha50_1
{
}


// -----------------------------------------------------------------------
class Alpha50_2
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta50_2 extends Alpha50_2
{
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
class Alpha50_3
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta50_3 extends Alpha50_3
{
   public function get Price (): int
   {
      return ++value;
   }
}


// -----------------------------------------------------------------------
class Alpha50_4
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta50_4 extends Alpha50_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown51_1 = false;
var actualError51_1 = "";
var expectedError51_1 = "Error: No implementation provided for 'Alpha51_1.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha51_1                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     public function get Price(): int                      " +
         "     {                                                     " +
         "        return value;                                      " +
         "     }                                                     " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Beta51_1 extends Alpha51_1                         " +
         "  {  }                                                     ", "unsafe");
}
catch (error)
{
   exceptionThrown51_1 = true;
   actualError51_1 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha51_2
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   abstract public function set Price (newPrice: int);
}

class Beta51_2 extends Alpha51_2
{
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown51_3 = false;
var actualError51_3 = "";
var expectedError51_3 = "Error: No implementation provided for 'Alpha51_3.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha51_3                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     public function get Price(): int                      " +
         "     {                                                     " +
         "        return value;                                      " +
         "     }                                                     " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Beta51_3 extends Alpha51_3                         " +
         "  {                                                        " +
         "     public function get Price (): int                     " +
         "     {                                                     " +
         "        return ++value;                                    " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
}
catch (error)
{
   exceptionThrown51_3 = true;
   actualError51_3 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha51_4
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   abstract function set Price (newPrice: int);
}

class Beta51_4 extends Alpha51_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown52_1 = false;
var actualError52_1 = "";
var expectedError52_1 = "Error: No implementation provided for 'Alpha52_1.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha52_1                        " +
         "  {                                               " +
         "     protected var value: int = 0;                " +
         "     abstract public function get Price(): int;   " +
         "     public function set Price (newPrice: int)    " +
         "     {                                            " +
         "        value = newPrice;                         " +
         "     }                                            " +
         "  }                                               " +
         "  class Beta52_1 extends Alpha52_1                " +
         "  {  }                                            ", "unsafe");
}
catch (error)
{
   exceptionThrown52_1 = true;
   actualError52_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown52_2 = false;
var actualError52_2 = "";
var expectedError52_2 = "Error: No implementation provided for 'Alpha52_2.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha52_2                           " +
         "  {                                                  " +
         "     protected var value: int = 0;                   " +
         "     abstract public function get Price(): int;      " +
         "     public function set Price (newPrice: int)       " +
         "     {                                               " +
         "        value = newPrice;                            " +
         "     }                                               " +
         "  }                                                  " +
         "  class Beta52_2 extends Alpha52_2                   " +
         "  {                                                  " +
         "     public function set Price (newPrice: int)       " +
         "     {                                               " +
         "        value = ++newPrice;                          " +
         "     }                                               " +
         "  }                                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown52_2 = true;
   actualError52_2 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha52_3
{
   protected var value: int = 0;
   
   abstract public function get Price(): int;
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta52_3 extends Alpha52_3
{
   public function get Price (): int
   {
      return ++value;
   }
}


// -----------------------------------------------------------------------
abstract class Alpha52_4
{
   protected var value: int = 0;
   
   abstract public function get Price(): int;
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

class Beta52_4 extends Alpha52_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown53_1 = false;
var actualError53_1 = "";
var expectedError53_1 = "Error: No implementation provided for 'Alpha53_1.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha53_1                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Beta53_1 extends Alpha53_1                         " +
         "  {  }                                                     ", "unsafe");
}
catch (error)
{
   exceptionThrown53_1 = true;
   actualError53_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown53_2 = false;
var actualError53_2 = "";
var expectedError53_2 = "Error: No implementation provided for 'Alpha53_2.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha53_2                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Beta53_2 extends Alpha53_2                         " +
         "  {                                                        " +
         "     public function set Price (newPrice: int)             " +
         "     {                                                     " +
         "        value = ++newPrice;                                " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
}
catch (error)
{
   exceptionThrown53_2 = true;
   actualError53_2 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown53_3 = false;
var actualError53_3 = "";
var expectedError53_3 = "Error: No implementation provided for 'Alpha53_3.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha53_3                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Beta53_3 extends Alpha53_3                         " +
         "  {                                                        " +
         "     public function get Price (): int                     " +
         "     {                                                     " +
         "        return ++value;                                    " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
}
catch (error)
{
   exceptionThrown53_3 = true;
   actualError53_3 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha53_4
{
   protected var value: int = 0;
   abstract public function get Price(): int;
   abstract public function set Price (newPrice: int);
}

class Beta53_4 extends Alpha53_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown54_1 = false;
var actualError54_1 = "";
var expectedError54_1 = "Error: No implementation provided for 'Beta54_1.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha54_1                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     public function get Price(): int                      " +
         "     {                                                     " +
         "        return value;                                      " +
         "     }                                                     " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  abstract class Beta54_1 extends Alpha54_1                " +
         "  {                                                        " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Charlie54_1 extends Beta54_1                       " +
         "  {  }                                                     ", "unsafe");
}
catch (error)
{
   exceptionThrown54_1 = true;
   actualError54_1 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha54_2
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   abstract public function set Price (newPrice: int);
}

abstract class Beta54_2 extends Alpha54_2
{
   abstract public function set Price (newPrice: int);
}

class Charlie54_2 extends Beta54_2
{
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown54_3 = false;
var actualError54_3 = "";
var expectedError54_3 = "Error: No implementation provided for 'Beta54_3.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha54_3                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     public function get Price(): int                      " +
         "     {                                                     " +
         "        return value;                                      " +
         "     }                                                     " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  abstract class Beta54_3 extends Alpha54_3                " +
         "  {                                                        " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Charlie54_3 extends Beta54_3                       " +
         "  {                                                        " +
         "     public function get Price (): int                     " +
         "     {                                                     " +
         "        return ++value;                                    " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
}
catch (error)
{
   exceptionThrown54_3 = true;
   actualError54_3 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha54_4
{
   protected var value: int = 0;
   
   public function get Price(): int
   {
      return value;
   }
   
   abstract function set Price (newPrice: int);
}

abstract class Beta54_4 extends Alpha54_4
{
   abstract function set Price (newPrice: int);
}

class Charlie54_4 extends Beta54_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown55_1 = false;
var actualError55_1 = "";
var expectedError55_1 = "Error: No implementation provided for 'Beta55_1.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha55_1                        " +
         "  {                                               " +
         "     protected var value: int = 0;                " +
         "     abstract public function get Price(): int;   " +
         "     public function set Price (newPrice: int)    " +
         "     {                                            " +
         "        value = newPrice;                         " +
         "     }                                            " +
         "  }                                               " +
         "  abstract class Beta55_1 extends Alpha55_1       " +
         "  {                                               " +
         "     abstract public function get Price(): int;   " +
         "  }                                               " +
         "  class Charlie55_1 extends Beta55_1              " +
         "  {  }                                            ", "unsafe");         
}
catch (error)
{
   exceptionThrown55_1 = true;
   actualError55_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown55_2 = false;
var actualError55_2 = "";
var expectedError55_2 = "Error: No implementation provided for 'Beta55_2.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha55_2                           " +
         "  {                                                  " +
         "     protected var value: int = 0;                   " +
         "     abstract public function get Price(): int;      " +
         "     public function set Price (newPrice: int)       " +
         "     {                                               " +
         "        value = newPrice;                            " +
         "     }                                               " +
         "  }                                                  " +
         "  abstract class Beta55_2 extends Alpha55_2          " +
         "  {                                                  " +
         "     abstract public function get Price(): int;      " +
         "  }                                                  " +
         "  class Charlie55_2 extends Beta55_2                 " +
         "  {                                                  " +
         "     public function set Price (newPrice: int)       " +
         "     {                                               " +
         "        value = ++newPrice;                          " +
         "     }                                               " +
         "  }                                                  ", "unsafe");
}
catch (error)
{
   exceptionThrown55_2 = true;
   actualError55_2 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha55_3
{
   protected var value: int = 0;
   
   abstract public function get Price(): int;
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

abstract class Beta55_3 extends Alpha55_3
{
   abstract public function get Price(): int;
}

class Charlie55_3 extends Beta55_3
{
   public function get Price (): int
   {
      return ++value;
   }
}


// -----------------------------------------------------------------------
abstract class Alpha55_4
{
   protected var value: int = 0;
   
   abstract public function get Price(): int;
   
   public function set Price (newPrice: int)
   {
      value = newPrice;
   }
}

abstract class Beta55_4 extends Alpha55_4
{
   abstract public function get Price(): int;
}

class Charlie55_4 extends Beta55_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown56_1 = false;
var actualError56_1 = "";
var expectedError56_1 = "Error: No implementation provided for 'Beta56_1.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha56_1                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  abstract class Beta56_1 extends Alpha56_1                " +
         "  {                                                        " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Charlie56_1 extends Beta56_1                       " +
         "  { }                                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown56_1 = true;
   actualError56_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown56_2 = false;
var actualError56_2 = "";
var expectedError56_2 = "Error: No implementation provided for 'Beta56_2.get_Price() : System.Int32'";

try
{
   eval ("  abstract class Alpha56_2                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  abstract class Beta56_2 extends Alpha56_2                " +
         "  {                                                        " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Charlie56_2 extends Beta56_2                       " +
         "  {                                                        " +
         "     public function set Price (newPrice: int)             " +
         "     {                                                     " +
         "        value = ++newPrice;                                " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");

}
catch (error)
{
   exceptionThrown56_2 = true;
   actualError56_2 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown56_3 = false;
var actualError56_3 = "";
var expectedError56_3 = "Error: No implementation provided for 'Beta56_3.set_Price(System.Int32)'";

try
{
   eval ("  abstract class Alpha56_3                                 " +
         "  {                                                        " +
         "     protected var value: int = 0;                         " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  abstract class Beta56_3 extends Alpha56_3                " +
         "  {                                                        " +
         "     abstract public function get Price(): int;            " +
         "     abstract public function set Price (newPrice: int);   " +
         "  }                                                        " +
         "  class Charlie56_3 extends Beta56_3                       " +
         "  {                                                        " +
         "     public function get Price (): int                     " +
         "     {                                                     " +
         "        return ++value;                                    " +
         "     }                                                     " +
         "  }                                                        ", "unsafe");
         
}
catch (error)
{
   exceptionThrown56_3 = true;
   actualError56_3 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha56_4
{
   protected var value: int = 0;
   abstract public function get Price(): int;
   abstract public function set Price (newPrice: int);
}

abstract class Beta56_4 extends Alpha56_4
{
   abstract public function get Price(): int;
   abstract public function set Price (newPrice: int);
}

class Charlie56_4 extends Beta56_4
{
   public function get Price (): int
   {
      return ++value;
   }
   
   public function set Price (newPrice: int)
   {
      value = ++newPrice;
   }
}


// -----------------------------------------------------------------------
var exceptionThrown57 = false;
var actualError57 = "";
var expectedError57 = "SyntaxError: Only valid inside a class definition";

try
{
   eval ("abstract function Foo();", "unsafe");
}
catch (error)
{
   exceptionThrown57 = true;
   actualError57 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown58 = false;
var actualError58 = "";
var expectedError58 = "Error: 'Bar' cannot be abstract unless the declaring class is marked as abstract";

try
{
   eval ("  class Alpha58                 " +
         "  {                             " +
         "     public function Bar();     " +
         "  }                             ", "unsafe");
}
catch (error)
{
   exceptionThrown58 = true;
   actualError58 = error;
}


// -----------------------------------------------------------------------
class Alpha59
{
   public var value: String = "none";
   public function Bar()
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown60 = false;
var actualError60 = "";
var expectedError60 = "SyntaxError: Syntax error";

try
{
   eval ("  class Alpha60                 " +
         "  {                             " +
         "     public function Bar();     " +
         "     { }                        " +
         "  }                             ", "unsafe");
}
catch (error)
{
   exceptionThrown60 = true;
   actualError60 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown61 = false;
var actualError61 = "";
var expectedError61 = "Error: 'Bar' cannot be abstract unless the declaring class is marked as abstract";

try
{
   eval ("  class Alpha61                       " +
         "  {                                   " +
         "     abstract public function Bar()   " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown61 = true;
   actualError61 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown62 = false;
var actualError62 = "";
var expectedError62 = "Error: 'Bar' cannot be abstract unless the declaring class is marked as abstract";

try
{
   eval ("  class Alpha62                       " +
         "  {                                   " +
         "     abstract public function Bar()   " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown62 = true;
   actualError62 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown63 = false;
var actualError63 = "";
var expectedError63 = "SyntaxError: Abstract function cannot have body";

try
{
   eval ("  class Alpha63                       " +
         "  {                                   " +
         "     abstract public function Bar()   " +
         "     { }                              " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown63 = true;
   actualError63 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown64 = false;
var actualError64 = "";
var expectedError64 = "SyntaxError: Syntax error";

try
{
   eval ("  class Alpha64                       " +
         "  {                                   " +
         "     abstract public function Bar();  " +
         "     { }                              " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown64 = true;
   actualError64 = error;
}  


// -----------------------------------------------------------------------
/*
This scenario causes JS to return a warning, not a compile error.  Eval does not catch warnings.
Detailed info in Bug 288749.
Just declare the class to check if it will cause problems in future builds.
*/

abstract class Alpha65_1
{
   public function Bar()
}


// -----------------------------------------------------------------------
var exceptionThrown65_2 = false;
var actualError65_2 = "";
var expectedError65_2 = "Error: No implementation provided for 'Alpha65_2.Bar()'";

try
{
   eval ("  abstract class Alpha65_2            " +
         "  {                                   " +
         "     public function Bar()            " +
         "  }                                   " +
         "  class Beta65_2 extends Alpha65_2    " +
         "  {  }                                ", "unsafe");
}
catch (error)
{
   exceptionThrown65_2 = true;
   actualError65_2 = error;
}  


// -----------------------------------------------------------------------
// HermanV: The draft ECMA spec allows this, so we cannot give an error.
abstract class Alpha66_1
{ 
   public static var value: String = "ten"; 
   public function Bar();
}


// -----------------------------------------------------------------------
var exceptionThrown66_2 = false;
var actualError66_2 = "";
var expectedError66_2 = "Error: No implementation provided for 'Alpha66_2.Bar()'";

try
{
   eval ("  abstract class Alpha66_2                        " +
         "  {                                               " +
         "     public static var value: String = \"five\";  " +
         "     public function Bar();                       " +
         "  }                                               " +
         "  class Beta66_2 extends Alpha66_2                " +
         "  {  }                                            ", "unsafe");
}
catch (error)
{
   exceptionThrown66_2 = true;
   actualError66_2 = error;
}


// -----------------------------------------------------------------------
abstract class Alpha66_3
{ 
   public static var value: String = "eight"; 
   public function Bar();
}

class Beta66_3 extends Alpha66_3
{
   public function Bar()
   {
   }
}


// -----------------------------------------------------------------------
abstract class Alpha67
{
   public var value: String = "none";
   public function Bar()
   {
      value = "one";
   }
}

class Beta67 extends Alpha67
{
}


// -----------------------------------------------------------------------
var exceptionThrown68 = false;
var actualError68 = "";
var expectedError68 = "SyntaxError: Syntax error";

try
{
   eval ("  abstract class Alpha68     " +
         "  {                          " +
         "     public function Bar();  " +
         "     { }                     " +
         "  }                          ", "unsafe");
}
catch (error)
{
   exceptionThrown68 = true;
   actualError68 = error;
}  


// -----------------------------------------------------------------------
abstract class Alpha69
{
   public var value: String = "none";
   abstract public function Bar()
}

class Beta69 extends Alpha69
{
   public function Bar()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
abstract class Alpha70
{
   public var value: String = "none";
   abstract public function Bar();
}

class Beta70 extends Alpha70
{
   public function Bar()
   {
      value = "two";
   }
}


// -----------------------------------------------------------------------
var exceptionThrown71 = false;
var actualError71 = "";
var expectedError71 = "SyntaxError: Abstract function cannot have body";

try
{
   eval ("  abstract class Alpha71              " +
         "  {                                   " +
         "     abstract public function Bar()   " +
         "     { }                              " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown71 = true;
   actualError71 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown72 = false;
var actualError72 = "";
var expectedError72 = "SyntaxError: Syntax error";

try
{
   eval ("  abstract class Alpha72              " +
         "  {                                   " +
         "     abstract public function Bar();  " +
         "     { }                              " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown72 = true;
   actualError72 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown73 = false;
var actualError73 = "";
var expectedError73 = "ReferenceError: Type 'Charlie73' does not have such a static member";

try
{
   eval ("  abstract class Alpha73                          " +
         "  {                                               " +
         "     abstract public function Bar();              " +
         "     static public var value: String = \"One\";   " +
         "  }                                               " +
         "  abstract class Beta73 extends Alpha73           " +
         "  {  }                                            " +
         "  abstract class Charlie73 extends Beta73         " +
         "  {  }                                            " +
         "  var temp = Charlie73.value;                     ", "unsafe");
}
catch (error)
{
   exceptionThrown73 = true;
   actualError73 = error;
}


// -----------------------------------------------------------------------
class Charlie74 extends Beta74
{
   public function Bar(): String
   {
      return "five";
   }
}

abstract class Beta74 extends Alpha74
{
}

abstract class Alpha74
{
   abstract public function Bar(): String;
}




function abstract02()
{
   apInitTest ("Abstract02");
   
   // -----------------------------------------------------------------------
   apInitScenario ("30. Alpha declares Bar() as abstract - Beta implements Bar() - Charlie declares Bar() as abstract - Delta does not implement Bar()");
                       
   if (exceptionThrown30 == false)
      apLogFailInfo ("No compile error in 30.", "Should give a compile error", exceptionThrown30, "");
   if (actualError30 != expectedError30)
      apLogFailInfo ("Wrong compile error in 30.", expectedError30, actualError30, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31. Abstract public method");
   
   var beta31: Beta31 = new Beta31();
   
   expected = "one";
   actual = "";
   beta31.Bar();
   actual = beta31.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 31.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("32. Abstract private method");
   
   if (exceptionThrown32 == false)
      apLogFailInfo ("No compile error in 32.", "Should give a compile error", exceptionThrown32, "212468");
   if (actualError32 != expectedError32)
      apLogFailInfo ("Wrong compile error in 32.", expectedError32, actualError32, "212468");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("33. Abstract protected method");
      
   var beta33: Beta33 = new Beta33();
   
   expected = "three";
   actual = "";
   beta33.Foo();
   actual = beta33.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 33.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("34. Abstract internal method");
      
   var beta34: Beta34 = new Beta34();
   
   expected = "five";
   actual = "";
   beta34.Bar();
   actual = beta34.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 34.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35. Abstract abstract method");
      
   if (exceptionThrown35 == false)
      apLogFailInfo ("No compile error in 35.", "Should give a compile error", exceptionThrown35, "");
   if (actualError35 != expectedError35)
      apLogFailInfo ("Wrong compile error in 35.", expectedError35, actualError35, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Abstract final method");
      
   if (exceptionThrown36 == false)
      apLogFailInfo ("No compile error in 36.", "Should give a compile error", exceptionThrown36, "");
   if (actualError36 != expectedError36)
      apLogFailInfo ("Wrong compile error in 36.", expectedError36, actualError36, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("37. Abstract expando method");
   
   if (exceptionThrown37 == false)
      apLogFailInfo ("No compile error in 37.", "Should give a compile error", exceptionThrown37, "212591, ");
   if (actualError37 != expectedError37)
      apLogFailInfo ("Wrong compile error in 37.", expectedError37, actualError37, "212591, ");


   // -----------------------------------------------------------------------
   apInitScenario ("38. Abstract static method");

   if (exceptionThrown38 == false)
      apLogFailInfo ("No compile error in 38.", "Should give a compile error", exceptionThrown38, "");
   if (actualError38 != expectedError38)
      apLogFailInfo ("Wrong compile error in 38.", expectedError38, actualError38, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("39. Alpha inside the package - Beta outside the package - Beta implements abstract Bar(); ");
                       
   var beta39: Beta39 = new Beta39();
   
   expected = "nine";
   actual = "";
   beta39.Bar();
   actual = beta39.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 39.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40. Alpha inside the package - Beta outside the package - Beta does not implement abstract Bar();");
                       
   if (exceptionThrown40 == false)
      apLogFailInfo ("No compile error in 40.", "Should give a compile error", exceptionThrown40, "");
   if (actualError40 != expectedError40)
      apLogFailInfo ("Wrong compile error in 40.", expectedError40, actualError40, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("41. Alpha inside GroupA; Beta inside GroupB; Beta implements Bar()");      
                       
   var beta41: Beta41 = new Beta41();
   
   expected = "five";
   actual = "";
   beta41.Bar();
   actual = beta41.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 41.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("42. Alpha inside GroupA; Beta inside GroupB; Beta does not implement Bar()"); 

/*
Eval cannot catch this compile error.
   
   if (exceptionThrown42 == false)
      apLogFailInfo ("No compile error in 42.", "Should give a compile error", exceptionThrown42, "");
   if (actualError42 != expectedError42)
      apLogFailInfo ("Wrong compile error in 42.", expectedError42, actualError42, "");
*/      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("43. Alpha and Beta inside the package; Beta implements Bar()"); 
      
   var beta43: Beta43 = new Beta43();
   
   expected = "one";
   actual = "";
   beta43.Bar();
   actual = beta43.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 43.", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("44. Alpha and Beta inside the package; Beta does not implement Bar()"); 
      
/*
Eval cannot catch this compile error.

   if (exceptionThrown44 == false)
      apLogFailInfo ("No compile error in 44.", "Should give a compile error", exceptionThrown44, "");
   if (actualError44 != expectedError44)
      apLogFailInfo ("Wrong compile error in 44.", expectedError44, actualError44, "");
*/
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("45. Abstract Bar() to abstract Bar()"); 
   
   if (exceptionThrown45 == false)
      apLogFailInfo ("No compile error in 45.", "Should give a compile error", exceptionThrown45, "");
   if (actualError45 != expectedError45)
      apLogFailInfo ("Wrong compile error in 45.", expectedError45, actualError45, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("46. Abstract Bar() to final Bar()"); 
      
   var beta46: Beta46 = new Beta46();
   
   expected = "five";
   actual = "";
   beta46.Bar();
   actual = beta46.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 46.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("47. Abstract Bar() to final Bar() in Charlie"); 
      
   var charlie47: Charlie47 = new Charlie47();
   
   expected = "eight";
   actual = "";
   charlie47.Bar();
   actual = charlie47.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 47.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("48. Abstract Bar() to expando Bar()"); 
      
   if (exceptionThrown48 == false)
      apLogFailInfo ("No compile error in 48.", "Should give a compile error", exceptionThrown48, "");
   if (actualError48 != expectedError48)
      apLogFailInfo ("Wrong compile error in 48.", expectedError48, actualError48, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("49. Abstract Bar() to static Bar()"); 
   
   if (exceptionThrown49 == false)
      apLogFailInfo ("No compile error in 49.", "Should give a compile error", exceptionThrown49, "");
   if (actualError49 != expectedError49)
      apLogFailInfo ("Wrong compile error in 49.", expectedError49, actualError49, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("50.1 Alpha's getter not abstract; setter not abstract - Beta does not implement getter; does not implement setter. "); 
      
   var beta50_1: Beta50_1 = new Beta50_1();
   
   expected = 20;
   actual = 0;
   beta50_1.Price = 20;
   actual = beta50_1.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("50.2 Alpha's getter not abstract; setter not abstract - Beta does not implement getter; implements setter. "); 
      
   var one50_2: Beta50_2 = new Beta50_2();
   
   expected = 41;
   actual = 0;
   one50_2.Price = 40;
   actual = one50_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.2 (1)", expected, actual, "");
      
   // -----------------------------------------
      
   var two50_2: Alpha50_2 = new Alpha50_2();
   
   expected = 30;
   actual = 0;
   two50_2.Price = 30;
   actual = two50_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.2 (2)", expected, actual, "");
      
   // -----------------------------------------

   var three50_2: Alpha50_2 = new Beta50_2();
   
   expected = 21;
   actual = 0;
   three50_2.Price = 20;
   actual = three50_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.2 (3)", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("50.3 Alpha's getter not abstract; setter not abstract. Beta implements getter; does not implement setter. "); 
          
   var one50_3: Beta50_3 = new Beta50_3();
   
   expected = 11;
   actual = 0;
   one50_3.Price = 10;
   actual = one50_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.3 (1)", expected, actual, "");
      
   // -----------------------------------------

   var two50_3: Alpha50_3 = new Alpha50_3();
   
   expected = 20;
   actual = 0;
   two50_3.Price = 20;
   actual = two50_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.3 (2)", expected, actual, "");
      
   // -----------------------------------------

   var three50_3: Alpha50_3 = new Beta50_3();
   
   expected = 31;
   actual = 0;
   three50_3.Price = 30;
   actual = three50_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.3 (3)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("50.4 Alpha's getter not abstract; setter not abstract. Beta implements getter; implements setter. "); 
              
   var one50_4: Beta50_4 = new Beta50_4();
   
   expected = 52;
   actual = 0;
   one50_4.Price = 50;
   actual = one50_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   var two50_4: Alpha50_4 = new Alpha50_4();
   
   expected = 60;
   actual = 0;
   two50_4.Price = 60;
   actual = two50_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.4 (2)", expected, actual, "");
      
   // -----------------------------------------

   var three50_4: Alpha50_4 = new Beta50_4();
   
   expected = 72;
   actual = 0;
   three50_4.Price = 70;
   actual = three50_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 50.4 (3)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51.1 Alpha's getter not abstract; setter abstract. Beta does not implement getter; does not implement setter. "); 
      
   if (exceptionThrown51_1 == false)
      apLogFailInfo ("No compile error in 51.1", "Should give a compile error", exceptionThrown51_1, "");
   if (actualError51_1 != expectedError51_1)
      apLogFailInfo ("Wrong compile error in 51.1", expectedError51_1, actualError51_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51.2 Alpha's getter not abstract; setter abstract. Beta does not implement getter; implements setter"); 
      
   var one51_2: Beta51_2 = new Beta51_2();
   
   expected = 41;
   actual = 0;
   one51_2.Price = 40;
   actual = one51_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 51.2 (1)", expected, actual, "");

   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {      
      eval ("var two51_2: Alpha51_2 = new Alpha51_2();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 51.2 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 51.2 (2)", expectedError, actualError, "");
   
   // -----------------------------------------

   var three51_2: Alpha51_2 = new Beta51_2();
   
   expected = 21;
   actual = 0;
   three51_2.Price = 20;
   actual = three51_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 51.2 (3)", expected, actual, "");       
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51.3 Alpha's getter not abstract; setter abstract. Beta implements getter; does not implement setter"); 
      
   if (exceptionThrown51_3 == false)
      apLogFailInfo ("No compile error in 51.3", "Should give a compile error", exceptionThrown51_3, "");
   if (actualError51_3 != expectedError51_3)
      apLogFailInfo ("Wrong compile error in 51.3", expectedError51_3, actualError51_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51.4 Alpha's getter not abstract; setter abstract. Beta implements getter; implements setter"); 
      
   var one51_4: Beta51_4 = new Beta51_4();
   
   expected = 52;
   actual = 0;
   one51_4.Price = 50;
   actual = one51_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 51.4 (1)", expected, actual, "");
      
   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two51_4: Alpha51_4 = new Alpha51_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 51.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 51.4 (2)", expectedError, actualError, "");   
   
         
   // -----------------------------------------

   var three51_4: Alpha51_4 = new Beta51_4();
   
   expected = 72;
   actual = 0;
   three51_4.Price = 70;
   actual = three51_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 51.4 (3)", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("52.1 Alpha's getter abstract; setter not abstract. Beta does not implement getter; does not implement setter"); 
      
   if (exceptionThrown52_1 == false)
      apLogFailInfo ("No compile error in 52.1", "Should give a compile error", exceptionThrown52_1, "");
   if (actualError52_1 != expectedError52_1)
      apLogFailInfo ("Wrong compile error in 52.1", expectedError52_1, actualError52_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("52.2 Alpha's getter abstract; setter not abstract. Beta does not implement getter; implements setter"); 
                        
   if (exceptionThrown52_2 == false)
      apLogFailInfo ("No compile error in 52.2", "Should give a compile error", exceptionThrown52_2, "");
   if (actualError52_2 != expectedError52_2)
      apLogFailInfo ("Wrong compile error in 52.2", expectedError52_2, actualError52_2, "");
          
          
   // -----------------------------------------------------------------------
   apInitScenario ("52.3 Alpha's getter abstract; setter not abstract. Beta implements getter; does not implement setter"); 
                        
   var one52_3: Beta52_3 = new Beta52_3();
   
   expected = 11;
   actual = 0;
   one52_3.Price = 10;
   actual = one52_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 52.3 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two52_3: Alpha52_3 = new Alpha52_3();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 52.3 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 52.3 (2)", expectedError, actualError, "");
            
   // -----------------------------------------

   var three52_3: Alpha52_3 = new Beta52_3();
   
   expected = 31;
   actual = 0;
   three52_3.Price = 30;
   actual = three52_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 52.3 (3)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("52.4 Alpha's getter abstract; setter not abstract. Beta implements getter; implements setter"); 
                        
   var one52_4: Beta52_4 = new Beta52_4();
   
   expected = 52;
   actual = 0;
   one52_4.Price = 50;
   actual = one52_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 52.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two52_4: Alpha52_4 = new Alpha52_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 52.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 52.4 (2)", expectedError, actualError, "");
      
   // -----------------------------------------

   var three52_4: Alpha52_4 = new Beta52_4();
   
   expected = 72;
   actual = 0;
   three52_4.Price = 70;
   actual = three52_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 52.4 (3)", expected, actual, "");                         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("53.1 Alpha's getter abstract; setter abstract. Beta does not implement getter; does not implement setter"); 
                        
   if (exceptionThrown53_1 == false)
      apLogFailInfo ("No compile error in 53.1", "Should give a compile error", exceptionThrown53_1, "");
   if (actualError53_1 != expectedError53_1)
      apLogFailInfo ("Wrong compile error in 53.1", expectedError53_1, actualError53_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("53.2 Alpha's getter abstract; setter abstract. Beta does not implement getter; implements setter"); 
      
   if (exceptionThrown53_2 == false)
      apLogFailInfo ("No compile error in 53.2", "Should give a compile error", exceptionThrown53_2, "");
   if (actualError53_2 != expectedError53_2)
      apLogFailInfo ("Wrong compile error in 53.2", expectedError53_2, actualError53_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("53.3 Alpha's getter abstract; setter abstract. Beta implements getter; does not implement setter");
                        
   if (exceptionThrown53_3 == false)
      apLogFailInfo ("No compile error in 53.3", "Should give a compile error", exceptionThrown53_3, "");
   if (actualError53_3 != expectedError53_3)
      apLogFailInfo ("Wrong compile error in 53.3", expectedError53_3, actualError53_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("53.4 Alpha's getter abstract; setter abstract. Beta implements getter; implements setter");    
                        
   var one53_4: Beta53_4 = new Beta53_4();
   
   expected = 52;
   actual = 0;
   one53_4.Price = 50;
   actual = one53_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 53.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two53_4: Alpha53_4 = new Alpha53_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 53.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 53.4 (2)", expectedError, actualError, "");
      
   // -----------------------------------------

   var three53_4: Alpha53_4 = new Beta53_4();
   
   expected = 72;
   actual = 0;
   three53_4.Price = 70;
   actual = three53_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 53.4 (3)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("54.1 Alpha's getter not abstract, setter abstract. Beta re-declares abstract methods as abstract. Charlie does not implement getter; does not implement setter");    
                               
   if (exceptionThrown54_1 == false)
      apLogFailInfo ("No compile error in 54.1", "Should give a compile error", exceptionThrown54_1, "");
   if (actualError54_1 != expectedError54_1)
      apLogFailInfo ("Wrong compile error in 54.1", expectedError54_1, actualError54_1, "");
                         
      
   // -----------------------------------------------------------------------
   apInitScenario ("54.2 Alpha's getter not abstract; setter abstract; Beta re-declares abstract methods as abstract. Charlie does not implement getter; implements setter");    
                        
   var one54_2: Charlie54_2 = new Charlie54_2();
   
   expected = 41;
   actual = 0;
   one54_2.Price = 40;
   actual = one54_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.2 (1)", expected, actual, "");

   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {      
      eval ("var two54_2: Alpha54_2 = new Alpha54_2();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 54.2 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 54.2 (2)", expectedError, actualError, "");
   
   // -----------------------------------------

   var three54_2: Alpha54_2 = new Charlie54_2();
   
   expected = 21;
   actual = 0;
   three54_2.Price = 20;
   actual = three54_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.2 (3)", expected, actual, "");    
      
   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {      
      eval ("var four54_2: Beta54_2 = new Beta54_2();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 54.2 (4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 54.2 (4)", expectedError, actualError, ""); 
      
   // -----------------------------------------

   var five54_2: Beta54_2 = new Charlie54_2();
   
   expected = 81;
   actual = 0;
   five54_2.Price = 80;
   actual = five54_2.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.2 (5)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("54.3 Alpha's getter not abstract; setter abstract. Beta re-declares abstract methods as abstract. Charlie implements getter; does not implement setter");    
         
   if (exceptionThrown54_3 == false)
      apLogFailInfo ("No compile error in 54.3", "Should give a compile error", exceptionThrown54_3, "");
   if (actualError54_3 != expectedError54_3)
      apLogFailInfo ("Wrong compile error in 54.3", expectedError54_3, actualError54_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("54.4 Alpha's getter not abstract; setter abstract. Beta re-declares abstract methods as abstract. Charlie implements getter; implements setter");    
      
   var one54_4: Charlie54_4 = new Charlie54_4();
   
   expected = 52;
   actual = 0;
   one54_4.Price = 50;
   actual = one54_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two54_4: Alpha54_4 = new Alpha54_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 54.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 54.4 (2)", expectedError, actualError, "");
      
            
   // -----------------------------------------

   var three54_4: Alpha54_4 = new Charlie54_4();
   
   expected = 72;
   actual = 0;
   three54_4.Price = 70;
   actual = three54_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.4 (3)", expected, actual, "");         
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var four54_4: Beta54_4 = new Beta54_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 54.4 (4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 54.4 (4)", expectedError, actualError, "");   
      
   // -----------------------------------------

   var five54_4: Beta54_4 = new Charlie54_4();
   
   expected = 82;
   actual = 0;
   five54_4.Price = 80;
   actual = five54_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 54.4 (5)", expected, actual, "");         
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55.1 Alpha's getter abstract; setter not abstract. Beta re-declares abstract methods as abstract; Charlie does not implement getter; does not implement setter");
      
   if (exceptionThrown55_1 == false)
      apLogFailInfo ("No compile error in 55.1", "Should give a compile error", exceptionThrown55_1, "");
   if (actualError55_1 != expectedError55_1)
      apLogFailInfo ("Wrong compile error in 55.1", expectedError55_1, actualError55_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55.2 Alpha's getter abstract; setter not abstract. Beta re-declares abstract methods as abstract; Charlie does not implement getter; implements setter");
      
   if (exceptionThrown55_2 == false)
      apLogFailInfo ("No compile error in 55.2", "Should give a compile error", exceptionThrown55_2, "");
   if (actualError55_2 != expectedError55_2)
      apLogFailInfo ("Wrong compile error in 55.2", expectedError55_2, actualError55_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55.3 Alpha's getter abstract; setter not abstract. Beta re-declares abstract methods as abstract; Charlie implements getter; does not implement setter");
      
   var one55_3: Charlie55_3 = new Charlie55_3();
   
   expected = 11;
   actual = 0;
   one55_3.Price = 10;
   actual = one55_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.3 (1)", expected, actual, "266671");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two55_3: Alpha55_3 = new Alpha55_3();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 55.3 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 55.3 (2)", expectedError, actualError, "");
            
   // -----------------------------------------

   var three55_3: Alpha55_3 = new Charlie55_3();
   
   expected = 31;
   actual = 0;
   three55_3.Price = 30;
   actual = three55_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.3 (3)", expected, actual, "");    
      
   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var four55_3: Beta55_3 = new Beta55_3();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 55.3 (4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 55.3 (4)", expectedError, actualError, "");
   
   // -----------------------------------------

   var five55_3: Beta55_3 = new Charlie55_3();
   
   expected = 61;
   actual = 0;
   five55_3.Price = 60;
   actual = five55_3.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.3 (5)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55.4 Alpha's getter abstract; setter not abstract. Beta re-declares abstract methods as abstract; Charlie implements getter; implements setter");
      
   var one55_4: Charlie55_4 = new Charlie55_4();
   
   expected = 52;
   actual = 0;
   one55_4.Price = 50;
   actual = one55_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two55_4: Alpha55_4 = new Alpha55_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 55.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 55.4 (2)", expectedError, actualError, "");
      
   // -----------------------------------------

   var three55_4: Alpha55_4 = new Charlie55_4();
   
   expected = 72;
   actual = 0;
   three55_4.Price = 70;
   actual = three55_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.4 (3)", expected, actual, "");      
   
   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var four55_4: Beta55_4 = new Beta55_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 55.4 (4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 55.4 (4)", expectedError, actualError, "");
     
                  
   // -----------------------------------------

   var five55_4: Beta55_4 = new Charlie55_4();   
   
   expected = 82;
   actual = 0;
   five55_4.Price = 80;
   actual = five55_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 55.4 (5)", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56.1 Alpha's getter abstract; setter abstract. Beta re-declares abstract methods as abstract; Charlie does not implement getter; does not implement setter");
        
   if (exceptionThrown56_1 == false)
      apLogFailInfo ("No compile error in 56.1", "Should give a compile error", exceptionThrown56_1, "");
   if (actualError56_1 != expectedError56_1)
      apLogFailInfo ("Wrong compile error in 56.1", expectedError56_1, actualError56_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56.2 Alpha's getter abstract; setter abstract. Beta re-declares abstract methods as abstract; Charlie does not implement getter; implements setter");
                        
   if (exceptionThrown56_2 == false)
      apLogFailInfo ("No compile error in 56.2", "Should give a compile error", exceptionThrown56_2, "");
   if (actualError56_2 != expectedError56_2)
      apLogFailInfo ("Wrong compile error in 56.2", expectedError56_2, actualError56_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56.3 Alpha's getter abstract; setter abstract. Beta re-declares abstract methods as abstract, Charlie implements getter; does not implement setter");
                        
   if (exceptionThrown56_3 == false)
      apLogFailInfo ("No compile error in 56.3", "Should give a compile error", exceptionThrown56_3, "");
   if (actualError56_3 != expectedError56_3)
      apLogFailInfo ("Wrong compile error in 56.3", expectedError56_3, actualError56_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56.4 Alpha's getter abstract; setter abstract. Beta re-declares abstract methods as abstract; Charlie implements getter; implements setter");
                        
   var one56_4: Charlie56_4 = new Charlie56_4();
   
   expected = 52;
   actual = 0;
   one56_4.Price = 50;
   actual = one56_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 56.4 (1)", expected, actual, "");
      
   // -----------------------------------------

   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var two56_4: Alpha56_4 = new Alpha56_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 56.4 (2)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 56.4 (2)", expectedError, actualError, "");
      
   // -----------------------------------------

   var three56_4: Alpha56_4 = new Charlie56_4();
   
   expected = 72;
   actual = 0;
   three56_4.Price = 70;
   actual = three56_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 56.4 (3)", expected, actual, "");                         
      
   // -----------------------------------------
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "TypeError: It is not possible to construct an instance of an abstract class";
   
   try
   {
      eval ("var four56_4: Beta56_4 = new Beta56_4();");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 56.4 (4)", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 56.4 (4)", expectedError, actualError, "");
   
   
   // -----------------------------------------
   
   var five56_4: Beta56_4 = new Charlie56_4(); 
   
   expected = 82;
   actual = 0;
   five56_4.Price = 80;
   actual = five56_4.Price;
   if (actual != expected)
      apLogFailInfo ("Error in 56.4 (5)", expected, actual, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("57. Abstract functions outside a class");
   
   if (exceptionThrown57 == false)
      apLogFailInfo ("No compile error in 57.", "Should give a compile error", exceptionThrown57, "");
   if (actualError57 != expectedError57)
      apLogFailInfo ("Wrong compile error in 57.", expectedError57, actualError57, "");
   
                          
   // -----------------------------------------------------------------------
   apInitScenario ("58. Abstract class = N; Abstract method = N; Body = N; Semi-colon = Y;");
   
   if (exceptionThrown58 == false)
      apLogFailInfo ("No compile error in 58.", "Should give a compile error", exceptionThrown58, "");
   if (actualError58 != expectedError58)
      apLogFailInfo ("Wrong compile error in 58.", expectedError58, actualError58, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("59. Abstract class = N; Abstract method = N; Body = Y; Semi-colon = N;");
      
   var alpha59: Alpha59 = new Alpha59();
   
   expected = "one";
   actual = "";
   alpha59.Bar();
   actual = alpha59.value;
   if (actual != expected)
      apLogFailInfo ("Error in 59.", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("60. Abstract class = N; Abstract method = N; Body = Y; Semi-colon = Y;");
          
   if (exceptionThrown60 == false)
      apLogFailInfo ("No compile error in 60.", "Should give a compile error", exceptionThrown60, "299061");
   if (actualError60 != expectedError60)
      apLogFailInfo ("Wrong compile error in 60.", expectedError60, actualError60, "299061");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("61. Abstract class = N; Abstract method = Y; Body = N; Semi-colon = N;");
      
   if (exceptionThrown61 == false)
      apLogFailInfo ("No compile error in 61.", "Should give a compile error", exceptionThrown61, "");
   if (actualError61 != expectedError61)
      apLogFailInfo ("Wrong compile error in 61.", expectedError61, actualError61, "");
     
   
   // -----------------------------------------------------------------------
   apInitScenario ("62. Abstract class = N; Abstract method = Y; Body = N; Semi-colon = Y;");
   
   if (exceptionThrown62 == false)
      apLogFailInfo ("No compile error in 62.", "Should give a compile error", exceptionThrown62, "");
   if (actualError62 != expectedError62)
      apLogFailInfo ("Wrong compile error in 62.", expectedError62, actualError62, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("63. Abstract class = N; Abstract method = Y; Body = Y; Semi-colon = N;");
      
   if (exceptionThrown63 == false)
      apLogFailInfo ("No compile error in 63.", "Should give a compile error", exceptionThrown63, "");
   if (actualError63 != expectedError63)
      apLogFailInfo ("Wrong compile error in 63.", expectedError63, actualError63, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("64. Abstract class = N; Abstract method = Y; Body = Y; Semi-colon = Y;");
      
   if (exceptionThrown64 == false)
      apLogFailInfo ("No compile error in 64.", "Should give a compile error", exceptionThrown64, "");
   if (actualError64 != expectedError64)
      apLogFailInfo ("Wrong compile error in 64.", expectedError64, actualError64, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("65.1 Abstract class = Y; Abstract method = N; Body = N; Semi-colon = N;");
   /*
   This scenario causes JS to return a warning, not a compile error.  Eval does not catch warnings.
   Detailed info in Bug 288749.
   Just declare the class to check if it will cause problems in future builds.
   */
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("65.2 Abstract class = Y; Abstract method = N; Body = N; Semi-colon = N;");
      
   if (exceptionThrown65_2 == false)
      apLogFailInfo ("No compile error in 65.2", "Should give a compile error", exceptionThrown65_2, "288749");
   if (actualError65_2 != expectedError65_2)
      apLogFailInfo ("Wrong compile error in 65.2", expectedError65_2, actualError65_2, "288749");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("66.1 Abstract class = Y; Abstract method = N; Body = N; Semi-colon = Y; " +
                        "No inheritance");
      
   expected = "ten";
   actual = "";
   actual = Alpha66_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 66.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("66.2 Abstract class = Y; Abstract method = N; Body = N; Semi-colon = Y; Beta does not implement Bar()");
                        
   if (exceptionThrown66_2 == false)
      apLogFailInfo ("No compile error in 66_2.", "Should give a compile error", exceptionThrown66_2, "");
   if (actualError66_2 != expectedError66_2)
      apLogFailInfo ("Wrong compile error in 66_2.", expectedError66_2, actualError66_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("66.3 Abstract class = Y; Abstract method = N; Body = N; Semi-colon = Y; Beta implements Bar()");
      
   expected = "eight";      
   actual = "";
   actual = Alpha66_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 66.3", expected, actual, "");
                 
      
   // -----------------------------------------------------------------------
   apInitScenario ("67. Abstract class = Y; Abstract method = N; Body = Y; Semi-colon = N;");
      
   var beta67: Beta67 = new Beta67();
   
   expected = "one";
   actual = "";
   beta67.Bar();
   actual = beta67.value;
   if (actual != expected)
      apLogFailInfo ("Error in 67.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("68. Abstract class = Y; Abstract method = N; Body = Y; Semi-colon = Y;");
   
   if (exceptionThrown68 == false)
      apLogFailInfo ("No compile error in 68.", "Should give a compile error", exceptionThrown68, "");
   if (actualError68 != expectedError68)
      apLogFailInfo ("Wrong compile error in 68.", expectedError68, actualError68, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("69. Abstract class = Y; Abstract method = Y; Body = N; Semi-colon = N;");
   
   var beta69: Beta69 = new Beta69();
   
   expected = "two";
   actual = "";
   beta69.Bar();
   actual = beta69.value;
   if (actual != expected)
      apLogFailInfo ("Error in 69.", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("70. Abstract class = Y; Abstract method = Y; Body = N; Semi-colon = Y;");
      
   var beta70: Beta70 = new Beta70();
   
   expected = "two";
   actual = "";
   beta70.Bar();
   actual = beta70.value;
   if (actual != expected)
      apLogFailInfo ("Error in 70.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("71. Abstract class = Y; Abstract method = Y; Body = Y; Semi-colon = N;");
   
   if (exceptionThrown71 == false)
      apLogFailInfo ("No compile error in 71.", "Should give a compile error", exceptionThrown71, "");
   if (actualError71 != expectedError71)
      apLogFailInfo ("Wrong compile error in 71.", expectedError71, actualError71, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("72. Abstract class = Y; Abstract method = Y; Body = Y; Semi-colon = Y;");
      
   if (exceptionThrown72 == false)
      apLogFailInfo ("No compile error in 72.", "Should give a compile error", exceptionThrown72, "");
   if (actualError72 != expectedError72)
      apLogFailInfo ("Wrong compile error in 72.", expectedError72, actualError72, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("73. Alpha defines an abstract method; Beta is abstract; Charlie is abstract");
   
   if (exceptionThrown73 == false)
      apLogFailInfo ("No compile error in 73.", "Should give a compile error", exceptionThrown73, "");
   if (actualError73 != expectedError73)
      apLogFailInfo ("Wrong compile error in 73.", expectedError73, actualError73, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("74. Inherit an abstract class using forward referencing");
      
   var charlie74: Charlie74 = new Charlie74();
   
   expected = "five";
   actual = "";
   actual = charlie74.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 74.", expected, actual, "");   
   

   
   apEndTest();
   
}



abstract02();


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
