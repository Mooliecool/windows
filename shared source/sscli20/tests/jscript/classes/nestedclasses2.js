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


var iTestID = 173438;


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
var exceptionThrown35 = false;
var actualError35 = "";
var expectedError35 = "ReferenceError: The non-static member 'Alpha35' cannot be accessed with the class name";
var version = Number(@_jscript_version).ToString("0.0")

try
{
   eval ("  class A35                     " +
         "  {                             " +
         "     private class Alpha35      " +
         "     {  }                       " +
         "  }                             " +
         "  var a35: A35 = new A35();     " +
         "  var alpha35: A35.Alpha35 = new a35.Alpha35();   ");
}
catch (error)
{
   exceptionThrown35 = true;
   actualError35 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown36 = false;
var actualError36 = "";
var expectedError36 = "ReferenceError: The non-static member 'Alpha36' cannot be accessed with the class name";

try
{
   eval ("  class A36                     " +
         "  {                             " +
         "     private class Alpha36      " +
         "     {  }                       " +
         "  }                             " +
         "  class B36 extends A36         " +
         "  {  }                          " +
         "  var b36: B36 = new B36();     " +
         "  var alpha36: B36.Alpha36 = new b36.Alpha36();   ");
}
catch (error)
{
   exceptionThrown36 = true;
   actualError36 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown37 = false;
var actualError37 = "";
var expectedError37 = "ReferenceError: The non-static member 'Alpha37' cannot be accessed with the class name";

try
{
   eval ("  class A37                     " +
         "  {                             " +
         "     private class Alpha37      " +
         "     {  }                       " +
         "  }                             " +
         "  class B37 extends A37         " +
         "  {  }                          " +
         "  class C37 extends B37         " +
         "  {  }                          " +
         "  var c37: C37 = new C37();     " +
         "  var alpha37: C37.Alpha37 = new c37.Alpha37();   ");
}
catch (error)
{
   exceptionThrown37 = true;
   actualError37 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown38 = false;
var actualError38 = "";
var expectedError38 = "ReferenceError: The non-static member 'Alpha38' cannot be accessed with the class name";

try
{
   eval ("  class A38                                       " +
         "  {                                               " +
         "     protected class Alpha38                      " +
         "     {  }                                         " +
         "  }                                               " +
         "  var a38: A38 = new A38();                       " +
         "  var alpha38: A38.Alpha38 = new a38.Alpha38();   ");
}
catch (error)
{
   exceptionThrown38 = true;
   actualError38 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown39 = false;
var actualError39 = "";
var expectedError39 = "ReferenceError: The non-static member 'Alpha39' cannot be accessed with the class name";

try
{
   eval ("  class A39                     " +
         "  {                             " +
         "     protected class Alpha39    " +
         "     {  }                       " +
         "  }                             " +
         "  class B39 extends A39         " +
         "  {  }                          " +
         "  var b39: B39 = new B39();     " +
         "  var alpha39: B39.Alpha39 = new b39.Alpha39();   ");
}
catch (error)
{
   exceptionThrown39 = true;
   actualError39 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown40 = false;
var actualError40 = "";
var expectedError40 = "ReferenceError: The non-static member 'Alpha40' cannot be accessed with the class name";

try
{
   eval ("  class A40                     " +
         "  {                             " +
         "     protected class Alpha40    " +
         "     {  }                       " +
         "  }                             " +
         "  class B40 extends A40         " +
         "  {  }                          " +
         "  class C40 extends B40         " +
         "  {  }                          " +
         "  var c40: C40 = new C40();     " +
         "  var alpha40: C40.Alpha40 = new c40.Alpha40();   ");
}
catch (error)
{
   exceptionThrown40 = true;
   actualError40 = error;   
}


// -----------------------------------------------------------------------
class A41
{
   public class Alpha41
   {
      public var value: String = "ten";
      public function Bar()
      {
         return "five";
      }
   }
}


// -----------------------------------------------------------------------
class A42
{
   public class Alpha42
   {
      public var value: String = "one";
      public function Bar()
      {
         return "two";
      }
   }
}

class B42 extends A42
{
}


// -----------------------------------------------------------------------
class A43
{
   public class Alpha43
   {
      public var value: String = "three";
      public function Bar()
      {
         return "four";
      }
   }
}

class B43 extends A43
{
}

class C43 extends B43
{
}


// -----------------------------------------------------------------------
package Group44
{
   class A44
   {
      public class Alpha44
      {
         public var value: String = "ten";
         public function Bar()
         {
            return "five";
         }
      }
   }
}


// -----------------------------------------------------------------------
package Group45
{
   class A45
   {
      public class Alpha45
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }

   class B45 extends A45
   {
   }
}


// -----------------------------------------------------------------------
package Group46
{
   class A46
   {
      public class Alpha46
      {
         public var value: String = "seven";
         public function Bar()
         {
            return "eight";
         }
      }
   }

   class B46 extends A46
   { }
   
   class C46 extends B46
   { }
}


// -----------------------------------------------------------------------
class A47
{
   internal class Alpha47
   {
      public var value: String = "one";
      public function Bar()
      {
         return "two";
      }
   }
}


// -----------------------------------------------------------------------
class A48
{
   internal class Alpha48
   {
      public var value: String = "three";
      public function Bar()
      {
         return "four";
      }
   }
}

class B48 extends A48
{
}


// -----------------------------------------------------------------------
class A49
{
   internal class Alpha49
   {
      public var value: String = "five";
      public function Bar()
      {
         return "six";
      }
   }
}

class B49 extends A49
{
}

class C49 extends B49
{
}


// -----------------------------------------------------------------------
var exceptionThrown50 = false;
var actualError50 = "";
var expectedError50 = "";

@cc_on 
   @if (@_fast)  
      if ((version == "7.0")||(version == "8.0"))
         expectedError50 = "ReferenceError: 'A50.Alpha50' is not accessible from this scope";
      else 
         expectedError50 = "ReferenceError: Type 'Group50.A50' does not have such a static member";
   @else
      expectedError50 = "ReferenceError: Expression must be a compile time constant";
    
@end      

package Group50
{
   class A50
   {
      internal class Alpha50
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
}

import Group50;

try
{
   eval (" var a50: A50 = new A50();   " +
         " var alpha50: A50.Alpha50;   ");
}
catch (error)
{
   exceptionThrown50 = true;
   actualError50 = error;   
}        


// -----------------------------------------------------------------------
var exceptionThrown51 = false;
var actualError51 = "";
var expectedError51 = ""

@cc_on 
   @if (@_fast)  
         expectedError51 = "ReferenceError: Type 'Group51.B51' does not have such a static member";
     @else
         expectedError51 = "ReferenceError: Expression must be a compile time constant";
    
@end 

package Group51
{
   class A51
   {
      internal class Alpha51
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
   
   class B51 extends A51
   {
   }
}

import Group51;

try
{
   eval (" var b51: B51 = new B51();   " +
         " var alpha51: B51.Alpha51;   ");
}
catch (error)
{
   exceptionThrown51 = true;
   actualError51 = error;   
}  


// -----------------------------------------------------------------------
var exceptionThrown52 = false;
var actualError52 = "";
var expectedError52 = "";

@cc_on 
   @if (@_fast)  
         expectedError52 = "ReferenceError: Type 'Group52.C52' does not have such a static member";
     @else
         expectedError52 = "ReferenceError: Expression must be a compile time constant";
    
@end 

package Group52
{
   class A52
   {
      internal class Alpha52
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
   
   class B52 extends A52
   {
   }
   
   class C52 extends B52
   {
   }
}

import Group52;

try
{
   eval (" var c52: C52 = new C52();   " +
         " var alpha52: C52.Alpha52;   ");
}
catch (error)
{
   exceptionThrown52 = true;
   actualError52 = error;   
}  


// -----------------------------------------------------------------------
var exceptionThrown53_1 = false;
var actualError53_1 = "";
var expectedError53_1 = "ReferenceError: 'A53_1.Alpha53_1' is not accessible from this scope";

try
{
   eval ("  class A53_1                         " +
         "  {                                   " +
         "     private static class Alpha53_1   " +
         "     {  }                             " +
         "  }                                   " +
         "  var alpha: A53_1.Alpha53_1 = new A53_1.Alpha53_1();");
}
catch (error)
{
   exceptionThrown53_1 = true;
   actualError53_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown53_2 = false;
var actualError53_2 = "";
var expectedError53_2 = "ReferenceError: 'A53_2.Alpha53_2' is not accessible from this scope";

try
{
   eval ("  class A53_2                         " +
         "  {                                   " +
         "     private static class Alpha53_2   " +
         "     {  }                             " +
         "  }                                   " +
         "  var a53_2: A53_2 = new A53_2();     " +
         "  var alpha53_2: A53_2.Alpha53_2 = new a53_2.Alpha53_2(); ");
}
catch (error)
{
   exceptionThrown53_2 = true;
   actualError53_2 = error;   
}    


// -----------------------------------------------------------------------
var exceptionThrown54_1 = false;
var actualError54_1 = "";
var expectedError54_1 = "ReferenceError: Type 'B54_1' does not have such a static member";

try
{
   eval ("  class A54_1                         " +
         "  {                                   " +
         "     private static class Alpha54_1   " +
         "     {  }                             " +
         "  }                                   " +
         "  class B54_1 extends A54_1           " +
         "  {  }                                " +
         "  var alpha54_1: B54_1.Alpha54_1 = new B54_1.Alpha54_1();");
}
catch (error)
{
   exceptionThrown54_1 = true;
   actualError54_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown54_2 = false;
var actualError54_2 = "";
var expectedError54_2 = "ReferenceError: Type 'B54_2' does not have such a static member";

try
{
   eval ("  class A54_2                         " +
         "  {                                   " +
         "     private static class Alpha54_2   " +
         "     {  }                             " +
         "  }                                   " +
         "  class B54_2 extends A54_2           " +
         "  {  }                                " +
         "  var b54_2: B54_2 = new B54_2();     " +
         "  var alpha54_2: B54_2.Alpha54_2 = new b54_2.Alpha54_2(); ");
}
catch (error)
{
   exceptionThrown54_2 = true;
   actualError54_2 = error;   
}    


// -----------------------------------------------------------------------
var exceptionThrown55_1 = false;
var actualError55_1 = "";
var expectedError55_1 = "ReferenceError: Type 'C55_1' does not have such a static member";

try
{
   eval ("  class A55_1                         " +
         "  {                                   " +
         "     private static class Alpha55_1   " +
         "     {  }                             " +
         "  }                                   " +
         "  class B55_1 extends A55_1           " +
         "  {  }                                " +
         "  class C55_1 extends B55_1           " +
         "  {  }                                " +
         "  var alpha55_1: C55_1.Alpha55_1 = new C55_1.Alpha55_1();");
}
catch (error)
{
   exceptionThrown55_1 = true;
   actualError55_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown55_2 = false;
var actualError55_2 = "";
var expectedError55_2 = "ReferenceError: Type 'C55_2' does not have such a static member";

try
{
   eval ("  class A55_2                         " +
         "  {                                   " +
         "     private static class Alpha55_2   " +
         "     {  }                             " +
         "  }                                   " +
         "  class B55_2 extends A55_2           " +
         "  {  }                                " +
         "  class C55_2 extends B55_2           " +
         "  {  }                                " +
         "  var c55_2: C55_2 = new C55_2();     " +
         "  var alpha55_2: C55_2.Alpha55_2 = new c55_2.Alpha55_2(); ");
}
catch (error)
{
   exceptionThrown55_2 = true;
   actualError55_2 = error;   
}    


// -----------------------------------------------------------------------
var exceptionThrown56_1 = false;
var actualError56_1 = "";
var expectedError56_1 = "ReferenceError: 'A56_1.Alpha56_1' is not accessible from this scope";

try
{
   eval ("  class A56_1                         " +
         "  {                                   " +
         "     protected static class Alpha56_1 " +
         "     {  }                             " +
         "  }                                   " +
         "  var alpha56_1: A56_1.Alpha56_1 = new A56_1.Alpha56_1();");
}
catch (error)
{
   exceptionThrown56_1 = true;
   actualError56_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown56_2 = false;
var actualError56_2 = "";
var expectedError56_2 = "ReferenceError: 'A56_2.Alpha56_2' is not accessible from this scope";

try
{
   eval ("  class A56_2                         " +
         "  {                                   " +
         "     protected static class Alpha56_2 " +
         "     {  }                             " +
         "  }                                   " +
         "  var a56_2: A56_2 = new A56_2();     " +
         "  var alpha56_2: A56_2.Alpha56_2 = new a56_2.Alpha56_2(); ");
}
catch (error)
{
   exceptionThrown56_2 = true;
   actualError56_2 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown57_1 = false;
var actualError57_1 = "";
var expectedError57_1 = "ReferenceError: Type 'B57_1' does not have such a static member";

try
{
   eval ("  class A57_1                         " +
         "  {                                   " +
         "     protected static class Alpha57_1 " +
         "     {  }                             " +
         "  }                                   " +
         "  class B57_1 extends A57_1           " +
         "  {  }                                " +
         "  var alpha57_1: B57_1.Alpha57_1 = new B57_1.Alpha57_1();");
}
catch (error)
{
   exceptionThrown57_1 = true;
   actualError57_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown57_2 = false;
var actualError57_2 = "";
var expectedError57_2 = "ReferenceError: Type 'B57_2' does not have such a static member";

try
{
   eval ("  class A57_2                         " +
         "  {                                   " +
         "     protected static class Alpha57_2 " +
         "     {  }                             " +
         "  }                                   " +
         "  class B57_2 extends A57_2           " +
         "  {  }                                " +
         "  var b57_2: B57_2 = new B57_2();     " +
         "  var alpha57_2: B57_2.Alpha57_2 = new b57_2.Alpha57_2(); ");
}
catch (error)
{
   exceptionThrown57_2 = true;
   actualError57_2 = error;   
}   


// -----------------------------------------------------------------------
var exceptionThrown58_1 = false;
var actualError58_1 = "";
var expectedError58_1 = "ReferenceError: Type 'C58_1' does not have such a static member";

try
{
   eval ("  class A58_1                         " +
         "  {                                   " +
         "     protected static class Alpha58_1 " +
         "     {  }                             " +
         "  }                                   " +
         "  class B58_1 extends A58_1           " +
         "  {  }                                " +
         "  class C58_1 extends B58_1           " +
         "  {  }                                " +
         "  var alpha58_1: C58_1.Alpha58_1 = new C58_1.Alpha58_1();");
}
catch (error)
{
   exceptionThrown58_1 = true;
   actualError58_1 = error;   
}     


// -----------------------------------------------------------------------
var exceptionThrown58_2 = false;
var actualError58_2 = "";
var expectedError58_2 = "ReferenceError: Type 'C58_2' does not have such a static member";

try
{
   eval ("  class A58_2                         " +
         "  {                                   " +
         "     protected static class Alpha58_2 " +
         "     {  }                             " +
         "  }                                   " +
         "  class B58_2 extends A58_2           " +
         "  {  }                                " +
         "  class C58_2 extends B58_2           " +
         "  {  }                                " +
         "  var c58_2: C58_2 = new C58_2();     " +
         "  var alpha58_2: C58_2.Alpha58_2 = new c58_2.Alpha58_2(); ");
}
catch (error)
{
   exceptionThrown58_2 = true;
   actualError58_2 = error;   
}      


// -----------------------------------------------------------------------
class A59
{
   public static class Alpha59
   {
      public var value: String = "ten";
      public function Bar()
      {
         return "five";
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown60 = false;
var actualError60 = "";
var expectedError60 = "ReferenceError: Type 'B60' does not have such a static member";

try
{
   eval ("  class A60                           " +
         "  {                                   " +
         "     public static class Alpha60      " +
         "     {  }                             " +
         "  }                                   " +
         "  class B60 extends A60               " +
         "  {  }                                " +
         "  var alpha60: B60.Alpha60 = new B60.Alpha60();   ");
}
catch (error)
{
   exceptionThrown60 = true;
   actualError60 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown61 = false;
var actualError61 = "";
var expectedError61 = "ReferenceError: Type 'C61' does not have such a static member";

try
{
   eval ("  class A61                           " +
         "  {                                   " +
         "     public static class Alpha61      " +
         "     {  }                             " +
         "  }                                   " +
         "  class B61 extends A61               " +
         "  {  }                                " +
         "  class C61 extends B61               " +
         "  {  }                                " +
         "  var alpha61: C61.Alpha61 = new C61.Alpha61();   ");
}
catch (error)
{   exceptionThrown61 = true;
   actualError61 = error;   
}



// -----------------------------------------------------------------------
package Group62
{
   class A62
   {
      public static class Alpha62
      {
         public var value: String = "ten";
         public function Bar()
         {
            return "five";
         }
      }
   }
}


// -----------------------------------------------------------------------
package Group63
{
   class A63
   {
      public static class Alpha63
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }

   class B63 extends A63
   {
   }
}

/*
// -----------------------------------------------------------------------
package Group64
{
   class A64
   {
      public static class Alpha64
      {
         public var value: String = "seven";
         public function Bar()
         {
            return "eight";
         }
      }
   }

   class B64 extends A64
   { }
   
   class C64 extends B64
   { }
}


// -----------------------------------------------------------------------
class A65
{
   internal static class Alpha65
   {
      public var value: String = "one";
      public function Bar()
      {
         return "two";
      }
   }
}
*/


// -----------------------------------------------------------------------
var exceptionThrown66 = false;
var actualError66 = "";
var expectedError66 = "ReferenceError: Type 'B66' does not have such a static member";

try
{
   eval ("  class A66                              " +
         "  {                                      " +
         "     internal static class Alpha66       " +
         "     {  }                                " +
         "  }                                      " +
         "  class B66 extends A66                  " +
         "  {  }                                   " +
         "  var alpha66: B66.Alpha66 = new B66.Alpha66();   ");
}
catch (error)
{
   exceptionThrown66 = true;
   actualError66 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown67 = false;
var actualError67 = "";
var expectedError67 = "ReferenceError: Type 'C67' does not have such a static member";

try
{
   eval ("  class A67                                 " +
         "  {                                         " +
         "     internal static class Alpha67          " +
         "     {  }                                   " +
         "  }                                         " +
         "  class B67 extends A67                     " +
         "  {  }                                      " +
         "  class C67 extends B67                     " +
         "  {  }                                      " +
         "  var alpha67: C67.Alpha67 = new C67.Alpha67();   ");
}
catch (error)
{
   exceptionThrown67 = true;
   actualError67 = error;   
}   


// -----------------------------------------------------------------------
var exceptionThrown68 = false;
var actualError68 = "";
var expectedError68 = "";

@cc_on 
   @if (@_fast)  
      if ((version == "7.0")||(version == "8.0"))
         expectedError68 = "ReferenceError: 'A68.Alpha68' is not accessible from this scope";
      else
         expectedError68 = "ReferenceError: Type 'Group68.A68' does not have such a static member";
   @else
         expectedError68 = "ReferenceError: Expression must be a compile time constant";
    
@end      

package Group68
{
   class A68
   {
      internal static class Alpha68
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
}

import Group68;

try
{
   eval (" var alpha68: A68.Alpha68;   ");
}
catch (error)
{
   exceptionThrown68 = true;
   actualError68 = error;   
}  


// -----------------------------------------------------------------------
var exceptionThrown69 = false;
var actualError69 = "";
var expectedError69 = "";

@cc_on 
   @if (@_fast)  
         expectedError69 = "ReferenceError: Type 'Group69.B69' does not have such a static member";
     @else
         expectedError69 = "ReferenceError: Expression must be a compile time constant";
    
@end      

package Group69
{
   class A69
   {
      internal static class Alpha69
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
   
   class B69 extends A69
   {
   }
}

import Group69;

try
{
   eval (" var alpha69: B69.Alpha69;   ");
}
catch (error)
{
   exceptionThrown69 = true;
   actualError69 = error;   
}  


// -----------------------------------------------------------------------
var exceptionThrown70 = false;
var actualError70 = "";
var expectedError70 = "";

@cc_on 
   @if (@_fast)  
         expectedError70 = "ReferenceError: Type 'Group70.C70' does not have such a static member";
     @else
         expectedError70 = "ReferenceError: Expression must be a compile time constant";
    
@end      

package Group70
{
   class A70
   {
      internal static class Alpha70
      {
         public var value: String = "five";
         public function Bar()
         {
            return "six";
         }
      }
   }
   
   class B70 extends A70
   {
   }
   
   class C70 extends B70
   {
   }
}

import Group70;

try
{
   eval (" var alpha70: C70.Alpha70;   ");
}
catch (error)
{
   exceptionThrown70 = true;
   actualError70 = error;   
}  


// -----------------------------------------------------------------------
class A71
{
   static private var value: String = "one";
   
   static class Alpha71
   {
      public function Bar(): String
      {
         var temp: String = "";
         temp = value;
         return temp;
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown72 = false;
var actualError72 = "";
var expectedError72 = "";

@cc_on 
   @if (@_fast)  
         expectedError72 = "ReferenceError: Variable 'value' has not been declared";
     @else
         expectedError72 = "ReferenceError: 'value' is undefined";
    
@end 

class A72
{
   private var value: String = "two";
   
   static class Alpha72
   {
      public function Bar()
      {
         var temp: String = "";
         try
         {         
            eval ("  temp = value;  ");
         }
         catch (error)
         {
            exceptionThrown72 = true;
            actualError72 = error;   
         }
      }
   }
}


// -----------------------------------------------------------------------
class A73
{
   static private var value: String = "three";
   
   class Alpha73
   {
      public function Bar(): String
      {
         var temp: String = "";
         temp = value;
         return temp;
      }
   }
}


// -----------------------------------------------------------------------
class A74
{
   private var value: String = "four";
   
   class Alpha74
   {
      public function Bar(): String
      {
         var temp: String = "";
         temp = value;
         return temp;
      }
   }
}


// -----------------------------------------------------------------------
class A75
{
   abstract class Alpha75
   {
      abstract public function Bar(): String;
   }
   
   class Beta75 extends Alpha75
   {
      public function Bar(): String
      {
         return "One";
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown76 = false;
var actualError76 = "";
var expectedError76 = "Error: No implementation provided for 'A76+Alpha76.Bar() : System.String'";

try
{
   eval ("  class A76                                    " +
         "  {                                            " +
         "     abstract class Alpha76                    " +
         "     {                                         " +
         "        abstract public function Bar(): String;   " +
         "     }                                         " +
         "     class Beta76 extends Alpha76              " +
         "     {  }                                      " +
         "  }                                            ", "unsafe");
}
catch (error)
{
   exceptionThrown76 = true;
   actualError76 = error;   
}


// -----------------------------------------------------------------------
class A77
{
   abstract static class Alpha77
   {
      abstract public function Bar(): String;
   }
}

class B77
{
   class Beta77 extends A77.Alpha77
   {
      public function Bar(): String
      {
         return "Two";
      }
   }
}


// -----------------------------------------------------------------------
var exceptionThrown78 = false;
var actualError78 = "";
var expectedError78 = "Error: No implementation provided for 'A78+Alpha78.Bar() : System.String'";

try
{
   eval ("  class A78                                       " +
         "  {                                               " +
         "     abstract static class Alpha78                " +
         "     {                                            " +
         "        abstract public function Bar(): String;   " +
         "     }                                            " +
         "  }                                               " +
         "  class B78                                       " +
         "  {                                               " +
         "     class Beta78 extends A78.Alpha78             " +
         "     {  }                                         " +
         "  }                                               ", "unsafe");
}
catch (error)
{
   exceptionThrown78 = true;
   actualError78 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown79 = false;
var actualError79 = "";
var expectedError79 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class A79                        " +
         "  {                                " +
         "     abstract class Alpha79        " +
         "     {  }                          " +
         "  }                                " +
         "  var a79: A79 = new A79();        " +
         "  var alpha79: A79.Alpha79 = new a79.Alpha79(); ");
}
catch (error)
{
   exceptionThrown79 = true;
   actualError79 = error;   
}  


// -----------------------------------------------------------------------
var exceptionThrown80 = false;
var actualError80 = "";
var expectedError80 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class A80                        " +
         "  {                                " +
         "     static abstract class Alpha80 " +
         "     {  }                          " +
         "  }                                " +
         "  var alpha80: A80.Alpha80 = new A80.Alpha80(); ");
}
catch (error)
{
   exceptionThrown80 = true;
   actualError80 = error;   
} 


// -----------------------------------------------------------------------
var exceptionThrown81 = false;
var actualError81 = "";
var expectedError81 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  abstract class A81         " +
         "  {                          " +
         "     class Alpha81 { }       " +
         "  }                          " +   
         "  var a81: A81 = new A81();  ");         
}
catch (error)
{
   exceptionThrown81 = true;
   actualError81 = error;   
}


// -----------------------------------------------------------------------
abstract class A82             
{                              
   static class Alpha82 
   { 
      public var value: String = "three";
   }    
}  


// -----------------------------------------------------------------------
var exceptionThrown83 = false;
var actualError83 = "";
var expectedError83 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class A83                              " +
         "  {                                      " +
         "     abstract class Alpha83              " +
         "     {  }                                " +
         "  }                                      " +
         "  var a83: A83 = new A83();              " +
         "  var alpha83: A83.Alpha83 = new a83.Alpha83();   ");
}
catch (error)
{
   exceptionThrown83 = true;
   actualError83 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown84 = false;
var actualError84 = "";
var expectedError84 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  class A84                                 " +
         "  {                                         " +
         "     abstract static class Alpha84          " +
         "     {  }                                   " +
         "  }                                         " +
         "  var alpha84: A84.Alpha84 = new A84.Alpha84();   ");
}
catch (error)
{
   exceptionThrown84 = true;
   actualError84 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown85 = false;
var actualError85 = "";
var expectedError85 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  abstract class A85            " +
         "  {                             " +
         "     abstract class Alpha85     " +
         "     {  }                       " +
         "  }                             " +
         "  var a85: A85 = new A85()      ");
}
catch (error)
{
   exceptionThrown85 = true;
   actualError85 = error;   
}


// -----------------------------------------------------------------------
var exceptionThrown86 = false;
var actualError86 = "";
var expectedError86 = "TypeError: It is not possible to construct an instance of an abstract class";

try
{
   eval ("  abstract class A86                        " +
         "  {                                         " +
         "     abstract static class Alpha86          " +
         "     {  }                                   " +
         "  }                                         " +
         "  var a86: A86.Alpha86 = new A86.Alpha86()  ");
}
catch (error)
{
   exceptionThrown86 = true;
   actualError86 = error;   
}


// -----------------------------------------------------------------------
class A87
{
   final class Alpha87
   {
      public var value: String = "One";
   }
}


// -----------------------------------------------------------------------
class A88
{
   final static class Alpha88
   {
      public var value: String = "Two";
   }
}


// -----------------------------------------------------------------------
class A89
{
   class B89
   {
      class C89
      {
         class D89
         {
            class E89
            {
               public function Bar(): String
               {  
                  return "E89.Bar()"; 
               }
            }
            public function Bar(): String
            {
               return "D89.Bar()"; 
            }
         }  // class D89
         public function Bar(): String
         {
            return "C89.Bar()";
         }
      }  // class C89
   }  // class B89
}  // class A89


// -----------------------------------------------------------------------
class Alpha90
{
   class Beta90
   {
      public function Zoo(): String
      {
         return "Alpha90.Beta90.Zoo()";
      }
   }
   
   function Bar()
   {
      function Foo()
      {
         return (new Beta90());
      }
      return (Foo());
   }
}


// -----------------------------------------------------------------------
/*
Waiting for Bug 319735

class Alpha91 extends Alpha91.Beta91
{
   static class Beta91
   {
      public function Bar()
      {
         return 25;
      }
   }
}
*/


// -----------------------------------------------------------------------
class Alpha92 extends Charlie92
{
   class Beta92 extends Charlie92
   {
   }
}

class Charlie92 extends Delta92
{
}

class Delta92
{
   public function Bar(): int
   {
      return 30;
   }
}


// -----------------------------------------------------------------------
class Alpha93 extends Charlie93
{
   static class Beta93 extends Charlie93
   {
   }
}

class Charlie93 extends Delta93
{
}

class Delta93
{
   public function Bar(): int
   {
      return 30;
   }
}


// -----------------------------------------------------------------------
var expectedError94, actualError94: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha94                       " +
         "  {                                   " +
         "     class Beta94 extends Charlie94   " +
         "     { }                              " +
         "     class Charlie94 extends Beta94   " +
         "     { }                              " +
         "  }                                   ";
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
actualError94 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError94 = "test.js(1,103) : error JS1120: Circular definition" + Environment.NewLine + "";
@else
expectedError94 = "test.js(1,103) : error JS1120: Circular definition" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError95, actualError95: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha95                             " +
         "  {                                         " +
         "     static class Beta95 extends Charlie95  " +
         "     { }                                    " +
         "     static class Charlie95 extends Beta95  " +
         "     { }                                    " +
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
actualError95 = process.StandardOutput.ReadToEnd();
@if(@rotor)
expectedError95 = "test.js(1,122) : error JS1120: Circular definition" + Environment.NewLine + "";
@else
expectedError95 = "test.js(1,122) : error JS1120: Circular definition" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
/*
Waiting for Bug 319735.

class Charlie96 extends Alpha96.Beta96
{
}

class Alpha96
{
   static class Beta96
   {
      public function Bar(): String
      {
         return "ten";
      }
   }
}
*/

   

import Group44;
import Group45;
import Group46;
import Group62;
import Group63;
// import Group64;



function nestedclasses2()
{
   apInitTest ("NestedClasses2");
   
   // -----------------------------------------------------------------------
   apInitScenario ("35. Instantiate private non-static Alpha nested in A");
   
   if (exceptionThrown35 == false)
      apLogFailInfo ("No compile error in 35", "Should give a compile error", exceptionThrown35, "");
   if (actualError35 != expectedError35)
      apLogFailInfo ("Wrong compile error in 35", expectedError35, actualError35, "244978");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Instantiate private non-static Alpha nested in A using B");

   if (exceptionThrown36 == false)
      apLogFailInfo ("No compile error in 36", "Should give a compile error", exceptionThrown36, "");
   if (actualError36 != expectedError36)
      apLogFailInfo ("Wrong compile error in 36", expectedError36, actualError36, "244978");


   // -----------------------------------------------------------------------
   apInitScenario ("37. Instantiate private non-static Alpha nested in A using C");
   
   if (exceptionThrown37 == false)
      apLogFailInfo ("No compile error in 37", "Should give a compile error", exceptionThrown37, "");
   if (actualError37 != expectedError37)
      apLogFailInfo ("Wrong compile error in 37", expectedError37, actualError37, "244978");
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("38. Instantiate protected non-static Alpha nested in A");
   
   if (exceptionThrown38 == false)
      apLogFailInfo ("No compile error in 38", "Should give a compile error", exceptionThrown38, "");
   if (actualError38 != expectedError38)
      apLogFailInfo ("Wrong compile error in 38", expectedError38, actualError38, "244978");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("39. Instantiate protected non-static Alpha nested in A using B");
      
   if (exceptionThrown39 == false)
      apLogFailInfo ("No compile error in 39", "Should give a compile error", exceptionThrown39, "");
   if (actualError39 != expectedError39)
      apLogFailInfo ("Wrong compile error in 39", expectedError39, actualError39, "244978"); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40. Instantiate protected non-static Alpha nested in A using C");
      
   if (exceptionThrown40 == false)
      apLogFailInfo ("No compile error in 40", "Should give a compile error", exceptionThrown40, "");
   if (actualError40 != expectedError40)
      apLogFailInfo ("Wrong compile error in 40", expectedError40, actualError40, "244978"); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("41. Instantiate public non-static Alpha nested in A");
      
   var a41: A41 = new A41();
   var alpha41: A41.Alpha41 = new a41.Alpha41();
   
   expected = "ten";
   actual = "";
   actual = alpha41.value;
   if (actual != expected)
      apLogFailInfo ("Error in 41(1)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = alpha41.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 41(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("42. Instantiate public non-static Alpha nested in A using B");
      
   var b42: B42 = new B42();
   var alpha42: B42.Alpha42 = new b42.Alpha42();
   
   expected = "one";
   actual = "";
   actual = alpha42.value;
   if (actual != expected)
      apLogFailInfo ("Error in 42(1)", expected, actual, "");
      
   expected = "two";
   actual = "";
   actual = alpha42.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 42(2)", expected, actual, "");        
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("43. Instantiate public non-static Alpha nested in A using C");
   
   var c43: C43 = new C43();
   var alpha43: C43.Alpha43 = new c43.Alpha43();
   
   expected = "three";
   actual = "";
   actual = alpha43.value;
   if (actual != expected)
      apLogFailInfo ("Error in 43(1)", expected, actual, "");
      
   expected = "four";
   actual = "";
   actual = alpha43.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 43(2)", expected, actual, "");        
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("44. Instantiate public non-static Alpha nested in A – inside a package");
   
   var a44: A44 = new A44();
   var alpha44: A44.Alpha44 = new a44.Alpha44();
   
   expected = "ten";
   actual = "";
   actual = alpha44.value;
   if (actual != expected)
      apLogFailInfo ("Error in 44(1)", expected, actual, "266995");
      
   expected = "five";
   actual = "";
   actual = alpha44.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 44(2)", expected, actual, "266995");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("45. Instantiate public non-static Alpha nested in A using B – inside a package");
   
   var b45: B45 = new B45();
   var alpha45: B45.Alpha45 = new b45.Alpha45();
   
   expected = "five";
   actual = "";
   actual = alpha45.value;
   if (actual != expected)
      apLogFailInfo ("Error in 45(1)", expected, actual, "");
      
   expected = "six";
   actual = "";
   actual = alpha45.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 45(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("46. Instantiate public non-static Alpha nested in A using C – inside a package");
      
   var c46: C46 = new C46();
   var alpha46: C46.Alpha46 = new c46.Alpha46();
   
   expected = "seven";
   actual = "";
   actual = alpha46.value;
   if (actual != expected)
      apLogFailInfo ("Error in 46(1)", expected, actual, "");
      
   expected = "eight";
   actual = "";
   actual = alpha46.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 46(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("47. Instantiate internal non-static Alpha nested in A");
      
   var a47: A47 = new A47();
   var alpha47: A47.Alpha47 = new a47.Alpha47();
   
   expected = "one";
   actual = "";
   actual = alpha47.value;
   if (actual != expected)
      apLogFailInfo ("Error in 47(1)", expected, actual, "");
      
   expected = "two";
   actual = "";
   actual = alpha47.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 47(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("48. Instantiate internal non-static Alpha nested in A using B");
      
   var b48: B48 = new B48();
   var alpha48: B48.Alpha48 = new b48.Alpha48();
   
   expected = "three";
   actual = "";
   actual = alpha48.value;
   if (actual != expected)
      apLogFailInfo ("Error in 48(1)", expected, actual, "");
      
   expected = "four";
   actual = "";
   actual = alpha48.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 48(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("49. Instantiate internal non-static Alpha nested in A using C");
      
   var c49: C49 = new C49();
   var alpha49: C49.Alpha49 = new c49.Alpha49();
   
   expected = "five";
   actual = "";
   actual = alpha49.value;
   if (actual != expected)
      apLogFailInfo ("Error in 49(1)", expected, actual, "");
      
   expected = "six";
   actual = "";
   actual = alpha49.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 49(2)", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("50. Instantiate internal non-static Alpha nested in A – inside a package");
      
   if (exceptionThrown50 == false)
      apLogFailInfo ("No compile error in 50", "Should give a compile error", exceptionThrown50, "");
   if (actualError50 != expectedError50)
      apLogFailInfo ("Wrong compile error in 50", expectedError50, actualError50, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("51. Instantiate internal non-static Alpha nested in A using B – inside a package");
      
   if (exceptionThrown51 == false)
      apLogFailInfo ("No compile error in 51", "Should give a compile error", exceptionThrown51, "");
   if (actualError51 != expectedError51)
      apLogFailInfo ("Wrong compile error in 51", expectedError51, actualError51, ""); 
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("52. Instantiate internal non-static Alpha nested in A using C – inside a package");
   
   if (exceptionThrown52 == false)
      apLogFailInfo ("No compile error in 52", "Should give a compile error", exceptionThrown52, "");
   if (actualError52 != expectedError52)
      apLogFailInfo ("Wrong compile error in 52", expectedError52, actualError52, ""); 
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("53. Instantiate private static Alpha nested in A");

   if (exceptionThrown53_1 == false)
      apLogFailInfo ("No compile error in 53(1)", "Should give a compile error", exceptionThrown53_1, "");
   if (actualError53_1 != expectedError53_1)
      apLogFailInfo ("Wrong compile error in 53(1)", expectedError53_1, actualError53_1, "");

   if (exceptionThrown53_2 == false)
      apLogFailInfo ("No compile error in 53(2)", "Should give a compile error", exceptionThrown53_2, "");
   if (actualError53_2 != expectedError53_2)
      apLogFailInfo ("Wrong compile error in 53(2)", expectedError53_2, actualError53_2, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("54. Instantiate private static Alpha nested in A using B");

   if (exceptionThrown54_1 == false)
      apLogFailInfo ("No compile error in 54(1)", "Should give a compile error", exceptionThrown54_1, "");
   if (actualError54_1 != expectedError54_1)
      apLogFailInfo ("Wrong compile error in 54(1)", expectedError54_1, actualError54_1, "");

   if (exceptionThrown54_2 == false)
      apLogFailInfo ("No compile error in 54(2)", "Should give a compile error", exceptionThrown54_2, "");
   if (actualError54_2 != expectedError54_2)
      apLogFailInfo ("Wrong compile error in 54(2)", expectedError54_2, actualError54_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("55. Instantiate private static Alpha nested in A using C");
      
   if (exceptionThrown55_1 == false)
      apLogFailInfo ("No compile error in 55(1)", "Should give a compile error", exceptionThrown55_1, "");
   if (actualError55_1 != expectedError55_1)
      apLogFailInfo ("Wrong compile error in 55(1)", expectedError55_1, actualError55_1, "");

   if (exceptionThrown55_2 == false)
      apLogFailInfo ("No compile error in 55(2)", "Should give a compile error", exceptionThrown55_2, "");
   if (actualError55_2 != expectedError55_2)
      apLogFailInfo ("Wrong compile error in 55(2)", expectedError55_2, actualError55_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("56. Instantiate protected static Alpha nested in A");
      
   if (exceptionThrown56_1 == false)
      apLogFailInfo ("No compile error in 56(1)", "Should give a compile error", exceptionThrown56_1, "");
   if (actualError56_1 != expectedError56_1)
      apLogFailInfo ("Wrong compile error in 56(1)", expectedError56_1, actualError56_1, "");

   if (exceptionThrown56_2 == false)
      apLogFailInfo ("No compile error in 56(2)", "Should give a compile error", exceptionThrown56_2, "");
   if (actualError56_2 != expectedError56_2)
      apLogFailInfo ("Wrong compile error in 56(2)", expectedError56_2, actualError56_2, "");
      
   
   // -----------------------------------------------------------------------
   apInitScenario ("57. Instantiate protected non-static Alpha nested in A using B");
      
   if (exceptionThrown57_1 == false)
      apLogFailInfo ("No compile error in 57(1)", "Should give a compile error", exceptionThrown57_1, "");
   if (actualError57_1 != expectedError57_1)
      apLogFailInfo ("Wrong compile error in 57(1)", expectedError57_1, actualError57_1, "244978");

   if (exceptionThrown57_2 == false)
      apLogFailInfo ("No compile error in 57(2)", "Should give a compile error", exceptionThrown57_2, "");
   if (actualError57_2 != expectedError57_2)
      apLogFailInfo ("Wrong compile error in 57(2)", expectedError57_2, actualError57_2, "244978");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("58. Instantiate protected non-static Alpha nested in A using C");
      
   if (exceptionThrown58_1 == false)
      apLogFailInfo ("No compile error in 58(1)", "Should give a compile error", exceptionThrown58_1, "");
   if (actualError58_1 != expectedError58_1)
      apLogFailInfo ("Wrong compile error in 58(1)", expectedError58_1, actualError58_1, "244978");

   if (exceptionThrown58_2 == false)
      apLogFailInfo ("No compile error in 58(2)", "Should give a compile error", exceptionThrown58_2, "");
   if (actualError58_2 != expectedError58_2)
      apLogFailInfo ("Wrong compile error in 58(2)", expectedError58_2, actualError58_2, "244978");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("59. Instantiate public static Alpha nested in A");
      
   var alpha59: A59.Alpha59 = new A59.Alpha59();
   
   expected = "ten";
   actual = "";
   actual = alpha59.value;
   if (actual != expected)
      apLogFailInfo ("Error in 59(1)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = alpha59.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 59(2)", expected, actual, "");   
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("60. Instantiate public static Alpha nested in A using B");
      
   if (exceptionThrown60 == false)
      apLogFailInfo ("No compile error in 60.", "Should give a compile error", exceptionThrown60, "");
   if (actualError60 != expectedError60)
      apLogFailInfo ("Wrong compile error in 60.", expectedError60, actualError60, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("61. Instantiate public static Alpha nested in A using C");
   
   if (exceptionThrown61 == false)
      apLogFailInfo ("No compile error in 61.", "Should give a compile error", exceptionThrown61, "");
   if (actualError61 != expectedError61)
      apLogFailInfo ("Wrong compile error in 61.", expectedError61, actualError61, "");
   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("62. Instantiate public static Alpha nested in A – inside a package");
   
   var alpha62: A62.Alpha62 = new A62.Alpha62();
   
   expected = "ten";
   actual = "";
   actual = alpha62.value;
   if (actual != expected)
      apLogFailInfo ("Error in 62(1)", expected, actual, "");
      
   expected = "five";
   actual = "";
   actual = alpha62.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 62(2)", expected, actual, "");      
      
         // -----------------------------------------------------------------------
   apInitScenario ("63. Instantiate public static Alpha nested in A using B – inside a package");
   
   exceptionThrown = false;
   actualError = "";
   expectedError = "";
   
@cc_on 
   @if (@_fast)  
         expectedError = "ReferenceError: Type 'Group63.B63' does not have such a static member";
     @else
         expectedError = "ReferenceError: Expression must be a compile time constant";
    
@end      
   
   try
   {
      eval ("  var alpha63: B63.Alpha63 = new B63.Alpha63();  ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;      
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 63.", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 63.", expectedError, actualError, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("64. Instantiate public static Alpha nested in A using C – inside a package");

/*
   exceptionThrown = false;
   actualError = "";
   expectedError = "ReferenceError: Type 'Group64.C64' does not have such a static member";
   
   try
   {
      eval ("  var alpha64: C64.Alpha64 = new C64.Alpha64();  ");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;      
   }
   
   if (exceptionThrown == false)
      apLogFailInfo ("No compile error in 64.", "Should give a compile error", exceptionThrown, "");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong compile error in 64.", expectedError, actualError, "");
*/      
      
   // -----------------------------------------------------------------------
   apInitScenario ("65. Instantiate internal static Alpha nested in A");
      
/*
   var alpha65: A65.Alpha65 = new A65.Alpha65();
   
   expected = "one";
   actual = "";
   actual = alpha65.value;
   if (actual != expected)
      apLogFailInfo ("Error in 65(1)", expected, actual, "");
      
   expected = "two";
   actual = "";
   actual = alpha65.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 65(2)", expected, actual, "");
*/      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("66. Instantiate internal static Alpha nested in A using B");
      
   if (exceptionThrown66 == false)
      apLogFailInfo ("No compile error in 66.", "Should give a compile error", exceptionThrown66, "");
   if (actualError66 != expectedError66)
      apLogFailInfo ("Wrong compile error in 66.", expectedError66, actualError66, "");

      
   // -----------------------------------------------------------------------
   apInitScenario ("67. Instantiate internal static Alpha nested in A using C");
      
   if (exceptionThrown67 == false)
      apLogFailInfo ("No compile error in 67.", "Should give a compile error", exceptionThrown67, "");
   if (actualError67 != expectedError67)
      apLogFailInfo ("Wrong compile error in 67.", expectedError67, actualError67, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("68. Instantiate internal non-static Alpha nested in A – inside a package");
      
   if (exceptionThrown68 == false)
      apLogFailInfo ("No compile error in 68", "Should give a compile error", exceptionThrown68, "");
   if (actualError68 != expectedError68)
      apLogFailInfo ("Wrong compile error in 68", expectedError68, actualError68, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("69. Instantiate internal static Alpha nested in A using B – inside a package");
      
   if (exceptionThrown69 == false)
      apLogFailInfo ("No compile error in 69", "Should give a compile error", exceptionThrown69, "");
   if (actualError69 != expectedError69)
      apLogFailInfo ("Wrong compile error in 69", expectedError69, actualError69, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("70. Instantiate internal static Alpha nested in A using C – inside a package");
   
   if (exceptionThrown70 == false)
      apLogFailInfo ("No compile error in 70", "Should give a compile error", exceptionThrown70, "");
   if (actualError70 != expectedError70)
      apLogFailInfo ("Wrong compile error in 70", expectedError70, actualError70, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("71. Static nested Alpha accesses static members of A");
      
   var alpha71: A71.Alpha71 = new A71.Alpha71();
   
   expected = "one";
   actual = "";
   actual = alpha71.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 71", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("72. Static nested Alpha accesses non-static members of A");
      
   var alpha72: A72.Alpha72 = new A72.Alpha72();
   
   alpha72.Bar();
         
   if (exceptionThrown72 == false)
      apLogFailInfo ("No compile error in 72", "Should give a compile error", exceptionThrown72, "");
   if (actualError72 != expectedError72)
      apLogFailInfo ("Wrong compile error in 72", expectedError72, actualError72, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("73. Non-static nested Alpha accesses static members of A");
      
   var a73: A73 = new A73();
   var alpha73: A73.Alpha73 = new a73.Alpha73();
   
   expected = "three";
   actual = "";
   actual = alpha73.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 73", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("74. Non-static nested Alpha accesses non-static members of A");
      
   var a74: A74 = new A74();
   var alpha74: A74.Alpha74 = new a74.Alpha74();
   
   expected = "four";
   actual = "";
   actual = alpha74.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 74", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("75. Alpha and Beta nested in A; Alpha is abstract; Beta extends Alpha; Beta implements abstract method Alpha.Bar()");
      
   var a75: A75 = new A75();
   var beta75: A75.Beta75 = new a75.Beta75();
   
   expected = "One";
   actual = "";
   actual = beta75.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 75", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("76. Alpha and Beta nested in A; Alpha is abstract; Beta extends Alpha; Beta does not implement abstract method Alpha.Bar()");
                       
   if (exceptionThrown76 == false)
      apLogFailInfo ("No compile error in 76", "Should give a compile error", exceptionThrown76, "");
   if (actualError76 != expectedError76)
      apLogFailInfo ("Wrong compile error in 76", expectedError76, actualError76, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("77. Alpha nested in A; Beta nested in B; Alpha is abstract and static; Beta extends Alpha; Beta implements abstract method Alpha.Bar()");
                       
   var b77: B77 = new B77();
   var beta77: B77.Beta77 = new b77.Beta77();
   
   expected = "Two";
   actual = "";
   actual = beta77.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 77", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("78. Alpha nested in A; Beta nested in B; Alpha is abstract and static; Beta extends Alpha; Beta does not implement abstract method Alpha.Bar()");

   if (exceptionThrown78 == false)
      apLogFailInfo ("No compile error in 78", "Should give a compile error", exceptionThrown78, "");
   if (actualError78 != expectedError78)
      apLogFailInfo ("Wrong compile error in 78", expectedError78, actualError78, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("79. Alpha nested in A; Alpha is non-static & abstract; instantiate Alpha");
      
   if (exceptionThrown79 == false)
      apLogFailInfo ("No compile error in 79", "Should give a compile error", exceptionThrown79, "");
   if (actualError79 != expectedError79)
      apLogFailInfo ("Wrong compile error in 79", expectedError79, actualError79, "");    
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("80. Alpha nested in A; Alpha is static & abstract; instantiate Alpha");
      
   if (exceptionThrown80 == false)
      apLogFailInfo ("No compile error in 80", "Should give a compile error", exceptionThrown80, "");
   if (actualError80 != expectedError80)
      apLogFailInfo ("Wrong compile error in 80", expectedError80, actualError80, "");    
      
           
   // -----------------------------------------------------------------------
   apInitScenario ("81. Alpha nested in A; A is abstract; Alpha is not abstract; Alpha is non-static; instantiate Alpha");
   
   if (exceptionThrown81 == false)
      apLogFailInfo ("No compile error in 81", "Should give a compile error", exceptionThrown81, "");
   if (actualError81 != expectedError81)
      apLogFailInfo ("Wrong compile error in 81", expectedError81, actualError81, "");    
      
   // -----------------------------------------------------------------------
   apInitScenario ("82. Alpha nested in A; A is abstract; Alpha is not abstract; Alpha is static; instantiate Alpha");
   
   var alpha82: A82.Alpha82 = new A82.Alpha82();
   
   expected = "three";
   actual = "";
   actual = alpha82.value;
   if (actual != expected)
      apLogFailInfo ("Error in 82", expected, actual, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("83. Alpha nested in A; A is not abstract; Alpha is abstract;  Alpha is non-static; instantiate Alpha");
      
   if (exceptionThrown83 == false)
      apLogFailInfo ("No compile error in 83", "Should give a compile error", exceptionThrown83, "");
   if (actualError83 != expectedError83)
      apLogFailInfo ("Wrong compile error in 83", expectedError83, actualError83, "");    

      
   // -----------------------------------------------------------------------
   apInitScenario ("84. Alpha nested in A; A is not abstract; Alpha is abstract; Alpha is static; instantiate Alpha");
      
   if (exceptionThrown84 == false)
      apLogFailInfo ("No compile error in 84", "Should give a compile error", exceptionThrown84, "");
   if (actualError84 != expectedError84)
      apLogFailInfo ("Wrong compile error in 84", expectedError84, actualError84, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("85. Alpha nested in A; A is abstract; Alpha is abstract; Alpha is non-static; instantiate Alpha");
      
   if (exceptionThrown85 == false)
      apLogFailInfo ("No compile error in 85", "Should give a compile error", exceptionThrown85, "");
   if (actualError85 != expectedError85)
      apLogFailInfo ("Wrong compile error in 85", expectedError85, actualError85, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("86. Alpha nested in A; A is abstract; Alpha is abstract;  Alpha is static; instantiate Alpha");
            
   if (exceptionThrown86 == false)
      apLogFailInfo ("No compile error in 86", "Should give a compile error", exceptionThrown86, "");
   if (actualError86 != expectedError86)
      apLogFailInfo ("Wrong compile error in 86", expectedError86, actualError86, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("87. Alpha nested in A; Alpha is final; Alpha is non-static; instantiate Alpha");                          
                       
   var a87: A87 = new A87();
   var alpha87: A87.Alpha87 = new a87.Alpha87();
   
   expected = "One";
   actual = "";
   actual = alpha87.value;
   if (actual != expected)
      apLogFailInfo ("Error in 87", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("88. Alpha nested in A; Alpha is final; Alpha is static; instantiate Alpha");                          

   var alpha88: A88.Alpha88 = new A88.Alpha88();
   
   expected = "Two";
   actual = "";
   actual = alpha88.value;
   if (actual != expected)
      apLogFailInfo ("Error in 88", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("89. Multiple nested classes");                          
         
   var a89: A89 = new A89();
   var b89: A89.B89 = new a89.B89();
   var c89: A89.B89.C89 = new b89.C89();
   var d89: A89.B89.C89.D89 = new c89.D89();

   expected = "C89.Bar()";
   actual = "";
   actual = c89.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 89(1)", expected, actual, "");
      
   expected = "D89.Bar()";
   actual = "";
   actual = d89.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 89(2)", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("90. Return an instance of a nested class from a nested function");
   
   var alpha90: Alpha90 = new Alpha90();
   var beta90: Alpha90.Beta90;

   beta90 = alpha90.Bar();
   expected = "Alpha90.Beta90.Zoo()";
   actual = "";
   actual = beta90.Zoo();
   if (actual != expected)
      apLogFailInfo ("Error in 90.", expected, actual, "308544");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("91. A class extends its own static nested class.");

/*
Waiting for Bug 319735
   
   var alpha: Alpha = new Alpha();
   
   expected = 25;
   actual = 0;
   actual = alpha.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 91.", expected, actual, "319735");
*/  


   // -----------------------------------------------------------------------
   apInitScenario ("92. Outer and inner classes extend the same base class -- non-static nested class");
   
   var alpha92: Alpha92 = new Alpha92();
   var beta92: Alpha92.Beta92 = new alpha92.Beta92();
   
   expected = 30;
   actual = 0;
   actual = beta92.Bar();
   if (actual != expected) 
      apLogFailInfo ("Error in 92.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("93. Outer and inner classes extend the same base class -- static nested class");
      
   var beta93: Alpha93.Beta93 = new Alpha93.Beta93();
   
   expected = 30;
   actual = 0;
   actual = beta93.Bar();
   if (actual != expected) 
      apLogFailInfo ("Error in 93.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("94. Circular inheritance in non-static nested classes");
      
   if (actualError94 != expectedError94)
      apLogFailInfo ("Error in 94", expectedError94, actualError94, "");     
      
    
   // -----------------------------------------------------------------------
   apInitScenario ("95. Circular inheritance in static nested classes");

   if (actualError95 != expectedError95)
      apLogFailInfo ("Error in 95", expectedError95, actualError95, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("96. Charlie inherits a static nested class using forward reference.");

/*
Waiting for Bug 319735.   
       
   var charlie96: Charlie96 = new Charlie96();
   
   expected = "ten";
   actual = "";
   actual = charlie96.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 96.", expected, actual, "");      
*/      
      
     
   
   apEndTest();
}



nestedclasses2();


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
