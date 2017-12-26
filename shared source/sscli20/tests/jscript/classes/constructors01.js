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


var iTestID = 161145;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// -----------------------------------------------------------------------
class Alpha1
{
   public var booleanField1: Boolean;
   public var booleanField2: boolean;
   public var numberField: Number;
   public var doubleField: double;
   public var stringField: String;
   public var objectField: Object;
   public var dateField: Date;
   public var arrayField: Array;
   public var functionField: Function;
   public var regExpField: RegExp;
   public var errorField: Error;
   public var byteField: byte;
   public var charField: char;
   public var shortField: short;
   public var intField: int;
   public var longField: long;
   public var floatField: float;
   
   public function PrintFields()
   {
      print ("Boolean: " + booleanField1); 
      print ("boolean: " + booleanField2); 
      print ("Number: " + numberField); 
      print ("double: " + doubleField); 
      print ("String: " + stringField); 
      print ("Object: " + objectField); 
      print ("Date: " + dateField); 
      print ("Array: " + arrayField); 
      print ("Function: " + functionField); 
      print ("RegExp: " + regExpField); 
      print ("Error: " + errorField); 
      print ("byte: " + byteField); 
      print ("char: " + charField); 
      print ("short: " + shortField); 
      print ("int: " + intField); 
      print ("long: " + longField); 
      print ("float: " + floatField); 
   }
}


// -----------------------------------------------------------------------
class Alpha2
{
   public var booleanField1: Boolean;
   public var booleanField2: boolean;
   public var numberField: Number;
   public var doubleField: double;
   public var stringField: String;
   public var objectField: Object;
   public var dateField: Date;
   public var arrayField: Array;
   public var functionField: Function;
   public var regExpField: RegExp;
   public var errorField: Error;
   public var byteField: byte;
   public var charField: char;
   public var shortField: short;
   public var intField: int;
   public var longField: long;
   public var floatField: float;
}

class Beta2 extends Alpha2
{
}


// -----------------------------------------------------------------------
class Alpha3
{
   public var instanceField1 = globalCtr++;  // 1
   public var constructor;
   public function Alpha3()
   {
      constructor = globalCtr++; // 3
   }
   public var instanceField2 = globalCtr++;  // 2
}


// -----------------------------------------------------------------------
class Alpha4
{
   public var alphaConstructor;
   public function Alpha4()
   {
      alphaConstructor = globalCtr++;  // 3
   }   
   public var alphaInstanceField1 = globalCtr++;   // 1
   public var alphaInstanceField2 = globalCtr++;   // 2
}

class Beta4 extends Alpha4
{
   public var betaInstanceField1 = globalCtr++;    // 4
   public var betaInstanceField2 = globalCtr++;    // 5
   public var betaConstructor;
   public function Beta4()
   {
      betaConstructor = globalCtr++;   // 6
   }
}


// -----------------------------------------------------------------------
class Alpha5
{
   public var instanceField1 = globalCtr++;  // 1
   public var constructor;
   public function Alpha5()
   {
      constructor = globalCtr++  // this won't be executed
   }
   public function Alpha5 (value: int)
   {
      constructor = globalCtr++; // 3
   }
   public var instanceField2 = globalCtr++;  // 2
}


// -----------------------------------------------------------------------
class Alpha6
{
   public var alphaConstructor;
   public var alphaInstanceField1 = globalCtr++;   // 1
   public function Alpha6()
   {
      alphaConstructor = globalCtr++;  // 3
   }   
   public function Alpha6 (value: int)
   {
      alphaConstructor = globalCtr++;  // this won't be executed
   }
   public var alphaInstanceField2 = globalCtr++;   // 2
}

class Beta6 extends Alpha6
{
   public var betaConstructor;
   public function Beta6 (value: int)
   {
      betaConstructor = globalCtr++;   // 6
   }
   public function Beta6()
   {
      betaConstructor = globalCtr++;   // this won't be executed
   }
   public var betaInstanceField1 = globalCtr++;    // 4
   public var betaInstanceField2 = globalCtr++;    // 5
}


// -----------------------------------------------------------------------
var exceptionThrown7_1 = false;
var actualError7_1 = "";
var expectedError7_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha7_1                         " +
         "  {                                      " +
         "     public function Alpha7_1(): boolean " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown7_1 = true;
   actualError7_1 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown7_2 = false;
var actualError7_2 = "";
var expectedError7_2 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha7_2                         " +
         "  {                                      " +
         "     public function Alpha7_2(): double  " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown7_2 = true;
   actualError7_2 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown7_3 = false;
var actualError7_3 = "";
var expectedError7_3 = "SyntaxError: A constructor function may not have a return type";
    
try
{
   eval ("  class Alpha7_3                         " +
         "  {                                      " +
         "     public function Alpha7_3(): String  " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown7_3 = true;
   actualError7_3 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown8_1 = false;
var actualError8_1 = "";
var expectedError8_1 = "SyntaxError: A constructor function may not have a return type";
   
try
{
   eval ("  class Alpha8_1                         " +
         "  {                                      " +
         "     public function Alpha8_1(): Object  " +
         "     { }                                 " +
         "  }                                      " +
         "  class Beta8_1 extends Alpha8_1         " +
         "  {                                      " +
         "     public function Beta8_1()           " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown8_1 = true;
   actualError8_1 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown8_2 = false;
var actualError8_2 = "";
var expectedError8_2 = "SyntaxError: A constructor function may not have a return type";
     
try
{
   eval ("  class Alpha8_2                       " +
         "  {                                    " +
         "     public function Alpha8_2(): Date  " +
         "     { }                               " +
         "  }                                    " +
         "  class Beta8_2 extends Alpha8_2       " +
         "  {                                    " +
         "     public function Beta8_2()         " +
         "     { }                               " +
         "  }                                    ", "unsafe");
}
catch (error)
{
   exceptionThrown8_2 = true;
   actualError8_2 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown8_3 = false;
var actualError8_3 = "";
var expectedError8_3 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha8_3                         " +
         "  {                                      " +
         "     public function Alpha8_3(): Array   " +
         "     { }                                 " +
         "  }                                      " +
         "  class Beta8_3 extends Alpha8_3         " +
         "  {                                      " +
         "     public function Beta8_3()           " +
         "     { }                                 " +
         "  }                                      ", "unsafe");
}
catch (error)
{
   exceptionThrown8_3 = true;
   actualError8_3 = error;
}       

   
// -----------------------------------------------------------------------
var exceptionThrown9_1 = false;
var actualError9_1 = "";
var expectedError9_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha9_1                            " +
         "  {                                         " +
         "     public function Alpha9_1()             " +
         "     { }                                    " +
         "  }                                         " +
         "  class Beta9_1 extends Alpha9_1            " +
         "  {                                         " +
         "     public function Beta9_1(): Function    " +
         "     { }                                    " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown9_1 = true;
   actualError9_1 = error;
}   


// -----------------------------------------------------------------------
var exceptionThrown9_2 = false;
var actualError9_2 = "";
var expectedError9_2 = "SyntaxError: A constructor function may not have a return type";
 
try
{
   eval ("  class Alpha9_2                      " +
         "  {                                   " +
         "     public function Alpha9_2()       " +
         "     { }                              " +
         "  }                                   " +
         "  class Beta9_2 extends Alpha9_2      " +
         "  {                                   " +
         "     public function Beta9_2(): void  " +
         "     { }                              " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown9_2 = true;
   actualError9_2 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown10_1 = false;
var actualError10_1 = "";
var expectedError10_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha10_1                                    " +
         "  {                                                  " +
         "     public function Alpha10_1 (x: int): Function    " +
         "     { }                                             " +
         "  }                                                  " , "unsafe");
}
catch (error)
{
   exceptionThrown10_1 = true;
   actualError10_1 = error;
}      

// -----------------------------------------------------------------------
var exceptionThrown10_2 = false;
var actualError10_2 = "";
var expectedError10_2 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha10_2                                 " +
         "  {                                               " +
         "     public function Alpha10_2 (x: double): void  " +
         "     { }                                          " +
         "  }                                               " , "unsafe");
}
catch (error)
{
   exceptionThrown10_2 = true;
   actualError10_2 = error;
}      


// -----------------------------------------------------------------------
var exceptionThrown11_1 = false;
var actualError11_1 = "";
var expectedError11_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha11_1                                    " +
         "  {                                                  " +
         "     public function Alpha11_1 (x: boolean): Object  " +
         "     { }                                             " +
         "  }                                                  " +
         "  class Beta11_1 extends Alpha11_1                   " +
         "  {                                                  " +
         "  }                                                  ", "unsafe");
}            
catch (error)
{
   exceptionThrown11_1 = true;
   actualError11_1 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown11_2 = false;
var actualError11_2 = "";
var expectedError11_2 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha11_2                                    " +
         "  {                                                  " +
         "     public function Alpha11_2 (x: String): Date     " +
         "     { }                                             " +
         "  }                                                  " +
         "  class Beta11_2 extends Alpha11_2                   " +
         "  {                                                  " +
         "  }                                                  ", "unsafe");
}            
catch (error)
{
   exceptionThrown11_2 = true;
   actualError11_2 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown11_3 = false;
var actualError11_3 = "";
var expectedError11_3 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha11_3                                    " +
         "  {                                                  " +
         "     public function Alpha11_3 (x: String): Array    " +
         "     { }                                             " +
         "  }                                                  " +
         "  class Beta11_3 extends Alpha11_3                   " +
         "  {                                                  " +
         "  }                                                  ", "unsafe");
}            
catch (error)
{
   exceptionThrown11_3 = true;
   actualError11_3 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown12_1 = false;
var actualError12_1 = "";
var expectedError12_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha12_1                                    " +
         "  {                                                  " +
         "  }                                                  " +
         "  class Beta12_1 extends Alpha12_1                   " +
         "  {                                                  " +
         "     public function Beta12_1 (x: int): boolean      " +
         "     { }                                             " +
         "  }                                                  ", "unsafe");
}            
catch (error)
{
   exceptionThrown12_1 = true;
   actualError12_1 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown12_2 = false;
var actualError12_2 = "";
var expectedError12_2 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha12_2                                    " +
         "  {                                                  " +
         "  }                                                  " +
         "  class Beta12_2 extends Alpha12_2                   " +
         "  {                                                  " +
         "     public function Beta12_2 (x: boolean): double   " +
         "     { }                                             " +
         "  }                                                  ", "unsafe");
}            
catch (error)
{
   exceptionThrown12_2 = true;
   actualError12_2 = error;
} 


// -----------------------------------------------------------------------
var exceptionThrown13_1 = false;
var actualError13_1 = "";
var expectedError13_1 = "SyntaxError: A constructor function may not have a return type";

try
{
   eval ("  class Alpha13_1                     " +
         "  {                                   " +
         "     public function Alpha13_1(): int " +
         "     {                                " +
         "        var i: int;                   " +
         "        i = 10;                       " +
         "        return i;                     " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown13_1 = true;
   actualError13_1 = error;
}


// -----------------------------------------------------------------------
var exceptionThrown13_2 = false;
var actualError13_2 = "";
var expectedError13_2 = "TypeError: Cannot return a value from a constructor or void function";

try
{
   eval ("  class Alpha13_2                     " +
         "  {                                   " +
         "     public function Alpha13_2()      " +
         "     {                                " +
         "        var i: int;                   " +
         "        i = 10;                       " +
         "        return i;                     " +
         "     }                                " +
         "  }                                   ", "unsafe");
}
catch (error)
{
   exceptionThrown13_2 = true;
   actualError13_2 = error;
}



// -----------------------------------------------------------------------
function constructors01()
{
   apInitTest ("Constructors01");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Class with no constructors");
   // If a field is not initialized, it is given the default value of undefined 
   // (coerced to the type of the field if necessary).
   
   var alpha1: Alpha1 = new Alpha1();
   
   actual = alpha1.booleanField1;
   expected = false;
   if (actual != expected)
      apLogFailInfo ("Error in Boolean", expected, actual, "");

   actual = alpha1.booleanField2;
   expected = false;
   if (actual != expected)
      apLogFailInfo ("Error in boolean", expected, actual, "");

   actual = alpha1.numberField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in Number", expected, actual, "");

   actual = alpha1.doubleField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in double", expected, actual, "");

   actual = alpha1.stringField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in String (1)", expected, actual, "");

   actual = alpha1.objectField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Object", expected, actual, "");

   actual = alpha1.dateField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Date", expected, actual, "");

   actual = alpha1.arrayField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Array", expected, actual, "");

   actual = alpha1.functionField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Function", expected, actual, "");

   actual = alpha1.regExpField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in RegExp", expected, actual, "");

   actual = alpha1.errorField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Error", expected, actual, "");

   actual = alpha1.byteField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in byte", expected, actual, "");

   actual = alpha1.charField;
   expected = "";
   if (actual != expected)
      apLogFailInfo ("Error in char", expected, actual, "");

   actual = alpha1.shortField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in short", expected, actual, "");

   actual = alpha1.intField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in int", expected, actual, "");

   actual = alpha1.longField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in long", expected, actual, "");

   actual = alpha1.floatField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in float", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("2. Inheritance with no constructors");      
   
   var beta2: Beta2 = new Beta2();
   
   actual = beta2.booleanField1;
   expected = false;
   if (actual != expected)
      apLogFailInfo ("Error in Boolean", expected, actual, "");

   actual = beta2.booleanField2;
   expected = false;
   if (actual != expected)
      apLogFailInfo ("Error in boolean", expected, actual, "");

   actual = beta2.numberField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in Number", expected, actual, "");

   actual = beta2.doubleField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in double", expected, actual, "");

   actual = beta2.stringField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in String (2)", expected, actual, "");

   actual = beta2.objectField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Object", expected, actual, "");

   actual = beta2.dateField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Date", expected, actual, "");

   actual = beta2.arrayField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Array", expected, actual, "");

   actual = beta2.functionField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Function", expected, actual, "");

   actual = beta2.regExpField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in RegExp", expected, actual, "");

   actual = beta2.errorField;
   expected = undefined;
   if (actual != expected)
      apLogFailInfo ("Error in Error", expected, actual, "");

   actual = beta2.byteField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in byte", expected, actual, "");

   actual = beta2.charField;
   expected = "";
   if (actual != expected)
      apLogFailInfo ("Error in char", expected, actual, "");

   actual = beta2.shortField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in short", expected, actual, "");

   actual = beta2.intField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in int", expected, actual, "");

   actual = beta2.longField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in long", expected, actual, "");

   actual = beta2.floatField;
   expected = 0;
   if (actual != expected)
      apLogFailInfo ("Error in float", expected, actual, "");  
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("3. Order of execution between field initialization & default constructor -- without inheritance");
   /*
   The order of execution should be:
      1.	instance field initialization
      2.	constructor
   */

   globalCtr = 1;
   var alpha3: Alpha3 = new Alpha3();

   expected = 1;
   actual = 0;
   actual = alpha3.instanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 3(1)", expected, actual, "");
      
   expected = 2;
   actual = 0;
   actual = alpha3.instanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 3(2)", expected, actual, "");

   expected = 3;
   actual = 0;
   actual = alpha3.constructor;
   if (actual != expected)
      apLogFailInfo ("Error in 3(3)", expected, actual, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("4. Order of execution between field initialization & default constructor -- with inheritance");
   /*
   The order of execution should be:
      1.	Superclass instance field initialization
      2.	Superclass constructor
      3.	Subclass instance field initialization
      4.	Subclass constructor
   */

   globalCtr = 1;
   var beta4: Beta4 = new Beta4();

   expected = 1;
   actual = 0;
   actual = beta4.alphaInstanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 4(1)", expected, actual, "");
      
   expected = 2;
   actual = 0;
   actual = beta4.alphaInstanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 4(2)", expected, actual, "");
      
   expected = 3;
   actual = 0;
   actual = beta4.alphaConstructor;
   if (actual != expected)
      apLogFailInfo ("Error in 4(3)", expected, actual, "");

   expected = 4;
   actual = 0;
   actual = beta4.betaInstanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 4(4)", expected, actual, "");
   
   expected = 5;
   actual = 0;
   actual = beta4.betaInstanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 4(5)", expected, actual, "");
   
   expected = 6;
   actual = 0;
   actual = beta4.betaConstructor;
   if (actual != expected)
      apLogFailInfo ("Error in 4(6)", expected, actual, "");
   
   
   // -----------------------------------------------------------------------
   apInitScenario ("5. Order of execution between field initialization & constructor w/ parameters -- w/o inheritance");
   /*
   The order of execution should be:
      1.	instance field initialization
      2.	constructor w/ parameters
   */

   globalCtr = 1;
   var alpha5: Alpha5 = new Alpha5();
   
   expected = 1;
   actual = 0;
   actual = alpha5.instanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 5(1)", expected, actual, "");

   expected = 2;
   actual = 0;
   actual = alpha5.instanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 5(2)", expected, actual, "");

   expected = 3;
   actual = 0;
   actual = alpha5.constructor;
   if (actual != expected)
      apLogFailInfo ("Error in 5(3)", expected, actual, "");
      

   // -----------------------------------------------------------------------
   apInitScenario ("6. Order of execution between field initialization & constructor w/ parameters -- with inheritance");
   /*
   The order of execution should be:
      1.	Superclass instance field initialization
      2.	Superclass default constructor
      3.	Subclass instance field initialization
      4.	Subclass constructor w/ parameter
   */

   globalCtr = 1;
   var beta6: Beta6 = new Beta6();

   expected = 1;
   actual = 0;
   actual = beta6.alphaInstanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 6(1)", expected, actual, "");
      
   expected = 2;
   actual = 0;
   actual = beta6.alphaInstanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 6(2)", expected, actual, "");
    
   expected = 3;
   actual = 0;
   actual = beta6.alphaConstructor;
   if (actual != expected)
      apLogFailInfo ("Error in 6(3)", expected, actual, "");

   expected = 4;
   actual = 0;
   actual = beta6.betaInstanceField1;
   if (actual != expected)
      apLogFailInfo ("Error in 6(4)", expected, actual, "");

   expected = 5;
   actual = 0;
   actual = beta6.betaInstanceField2;
   if (actual != expected)
      apLogFailInfo ("Error in 6(5)", expected, actual, "");

   expected = 6;
   actual = 0;
   actual = beta6.betaConstructor;
   if (actual != expected)
      apLogFailInfo ("Error in 6(6)", expected, actual, "");
      
          
   // -----------------------------------------------------------------------
   apInitScenario ("7.1 Default constructor is type annotated with boolean");
   // Compile error
   
   if (exceptionThrown7_1 == false)
      apLogFailInfo ("Default constructors type annotated with boolean don't give compile errors", "Should give a compile error", exceptionThrown7_1, "");
   if (actualError7_1 != expectedError7_1)
      apLogFailInfo ("Wrong compile error message", expectedError7_1, actualError7_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7.2 Default constructor is type annotated with double");
   // Compile error
   
   if (exceptionThrown7_2 == false)
      apLogFailInfo ("Default constructors type annotated with double don't give compile errors", "Should give a compile error", exceptionThrown7_2, "");
   if (actualError7_2 != expectedError7_2)
      apLogFailInfo ("Wrong compile error message", expectedError7_2, actualError7_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("7.3 Default constructor is type annotated with String");
   // Compile error
   
   if (exceptionThrown7_3 == false)
      apLogFailInfo ("Default constructors type annotated with String don't give compile errors", "Should give a compile error", exceptionThrown7_3, "");
   if (actualError7_3 != expectedError7_3)
      apLogFailInfo ("Wrong compile error message", expectedError7_3, actualError7_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.1 Superclass default constructor is type annotated with Object");
   // Compile error
   
   if (exceptionThrown8_1 == false)
      apLogFailInfo ("Superclass default constructors type annotated with Object don't give compile errors", "Should give a compile error", exceptionThrown8_1, "");
   if (actualError8_1 != expectedError8_1)
      apLogFailInfo ("Wrong compile error message", expectedError8_1, actualError8_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.2 Superclass default constructor is type annotated with Date");
   // Compile error
   
   if (exceptionThrown8_2 == false)
      apLogFailInfo ("Superclass default constructors type annotated with Date don't give compile errors", "Should give a compile error", exceptionThrown8_2, "");
   if (actualError8_2 != expectedError8_2)
      apLogFailInfo ("Wrong compile error message", expectedError8_2, actualError8_2, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("8.3 Superclass default constructor is type annotated with Array");
   // Compile error
   
   if (exceptionThrown8_3 == false)
      apLogFailInfo ("Superclass default constructors type annotated with Array don't give compile errors", "Should give a compile error", exceptionThrown8_3, "");
   if (actualError8_3 != expectedError8_3)
      apLogFailInfo ("Wrong compile error message", expectedError8_3, actualError8_3, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9.1 Subclass default constructor is type annotated with Function");
   // Compile error
   
   if (exceptionThrown9_1 == false)
      apLogFailInfo ("Subclass default constructors type annotated with Function don't give compile errors", "Should give a compile error", exceptionThrown9_1, "");
   if (actualError9_1 != expectedError9_1)
      apLogFailInfo ("Wrong compile error message", expectedError9_1, actualError9_1, "");


   // -----------------------------------------------------------------------
   apInitScenario ("9.2 Subclass default constructor is type annotated with void");
   // Compile error
   
   if (exceptionThrown9_2 == false)
      apLogFailInfo ("Subclass default constructors type annotated with void don't give compile errors", "Should give a compile error", exceptionThrown9_2, "");
   if (actualError9_2 != expectedError9_2)
      apLogFailInfo ("Wrong compile error message", expectedError9_2, actualError9_2, "");


   // -----------------------------------------------------------------------
   apInitScenario ("10.1 Constructor (with parameters) is type annotated with Function");
   // Compile error
   
   if (exceptionThrown10_1 == false)
      apLogFailInfo ("Constructor (with parameters) type annotated with Function don't give compile errors", "Should give a compile error", exceptionThrown10_1, "");
   if (actualError10_1 != expectedError10_1)
      apLogFailInfo ("Wrong compile error message", expectedError10_1, actualError10_1, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10.2 Constructor (with parameters) is type annotated with void");
   // Compile error
   
   if (exceptionThrown10_2 == false)
      apLogFailInfo ("Constructor (with parameters) type annotated with void don't give compile errors", "Should give a compile error", exceptionThrown10_2, "");
   if (actualError10_2 != expectedError10_2)
      apLogFailInfo ("Wrong compile error message", expectedError10_2, actualError10_2, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11.1 Superclass constructor (with parameters) is type annotated with Object");
   // Compile error
   
   if (exceptionThrown11_1 == false)
      apLogFailInfo ("Superclass constructor (with parameters) type annotated with Object don't give compile errors", "Should give a compile error", exceptionThrown11_1, "");
   if (actualError11_1 != expectedError11_1)
      apLogFailInfo ("Wrong compile error message", expectedError11_1, actualError11_1, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11.2 Superclass constructor (with parameters) is type annotated with Date");
   // Compile error
      
   if (exceptionThrown11_2 == false)
      apLogFailInfo ("Superclass constructor (with parameters) type annotated with Date don't give compile errors", "Should give a compile error", exceptionThrown11_2, "");
   if (actualError11_2 != expectedError11_2)
      apLogFailInfo ("Wrong compile error message", expectedError11_2, actualError11_2, "");    
   

   // -----------------------------------------------------------------------
   apInitScenario ("11.3 Superclass constructor (with parameters) is type annotated with Array");
   // Compile error
      
   if (exceptionThrown11_3 == false)
      apLogFailInfo ("Superclass constructor (with parameters) type annotated with Array don't give compile errors", "Should give a compile error", exceptionThrown11_3, "");
   if (actualError11_3 != expectedError11_3)
      apLogFailInfo ("Wrong compile error message", expectedError11_3, actualError11_3, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12.1 Subclass constructor (with parameters) is type annotated with boolean");
   // Compile error
      
   if (exceptionThrown12_1 == false)
      apLogFailInfo ("Subclass constructor (with parameters) type annotated with boolean don't give compile errors", "Should give a compile error", exceptionThrown12_1, "");
   if (actualError12_1 != expectedError12_1)
      apLogFailInfo ("Wrong compile error message", expectedError12_1, actualError12_1, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("12.2 Subclass constructor (with parameters) is type annotated with double");
   // Compile error
      
   if (exceptionThrown12_2 == false)
      apLogFailInfo ("Subclass constructor (with parameters) type annotated with double don't give compile errors", "Should give a compile error", exceptionThrown12_2, "");
   if (actualError12_2 != expectedError12_2)
      apLogFailInfo ("Wrong compile error message", expectedError12_2, actualError12_2, "");     
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("13.1 Constructor returns a value; type annotated");
   // Compile error
   
   if (exceptionThrown13_1 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown13_1, "197528");
   if (actualError13_1 != expectedError13_1)
      apLogFailInfo ("Wrong compile error message", expectedError13_1, actualError13_1, "197528");     
   
      
   // -----------------------------------------------------------------------
   apInitScenario ("13.2 Constructor returns a value; not type annotated");
   // Compile error
   
   if (exceptionThrown13_2 == false)
      apLogFailInfo ("No compile error", "Should give a compile error", exceptionThrown13_2, "");
   if (actualError13_2 != expectedError13_2)
      apLogFailInfo ("Wrong compile error message", expectedError13_2, actualError13_2, "");  
      
      
   apEndTest();
}



constructors01();


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
