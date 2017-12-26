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


var iTestID = 194195;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = ""; 
var actualError = "";


// -----------------------------------------------------------------------
var exceptionThrown32 = false;
var actualError32 = "";
var expectedError32 = "TypeError: Invalid procedure call or argument";

try
{
   eval ("  abstract class Alpha32                    " +
         "  {                                         " +
         "     abstract public function Bar();        " +
         "  }                                         " +
         "  class Beta32 extends Alpha32              " +
         "  {                                         " +
         "     public function Bar()                  " +
         "     {                                      " +
         "     }                                      " +
         "     public function Foo()                  " +
         "     {                                      " +
         "        super.Bar();                        " +
         "     }                                      " +
         "  }                                         ", "unsafe");
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
   
   abstract public function Bar(): String;
   
   public function Foo()
   {
      Bar();    // try also -- print (this.Bar());
   }
}

class Beta33 extends Alpha33
{
   public function Bar(): String
   {
      value = "Beta33.Bar()";
   }
   
   public function Zoo()
   {
      Foo();
   }
}


// -----------------------------------------------------------------------
abstract class Alpha34
{
   public var value: String = "none";
   
   abstract public function Bar(): String;
   
   public function Foo()
   {
      this.Bar();    
   }
}

class Beta34 extends Alpha34
{
   public function Bar(): String
   {
      value = "Beta34.Bar()";
   }
   
   public function Zoo()
   {
      Foo();
   }
}


// -----------------------------------------------------------------------
class Alpha35
{
   public var value: String = "none";
   public function Bar()
   {
      value = "Alpha35.Bar()";
   }
}

class Beta35 extends Alpha35
{
   public function Bar()
   {
      super.Bar();
      value = value + ";" + "Beta35.Bar()";
   }
}


// -----------------------------------------------------------------------
class Alpha36
{
   public var value: String = "none";
   public function Bar()
   {
      value = value + ";" + "Alpha36.Bar()";
   }
}

class Beta36 extends Alpha36
{
   public function Bar()
   {
      value = "Beta36.Bar()";
      super.Bar();
   }
}


// -----------------------------------------------------------------------
class Alpha37
{
   public var value: String = "none";
   public function Bar()
   {
      value = super.ToString();
   }
}  


// -----------------------------------------------------------------------
class Alpha38
{
   public var value: String = "none";
}  

class Beta38 extends Alpha38
{
   public function Bar()
   {
      value = super.ToString();
   }
}


// -----------------------------------------------------------------------
class Alpha39
{
   public var value: String = "none";
}  

class Beta39 extends Alpha39
{
}

class Charlie39 extends Beta39
{
   public function Bar()
   {
      value = super.ToString();
   }
}


// -----------------------------------------------------------------------
var exceptionThrown40 = false;
var actualError40 = "";
var expectedError40 = "SyntaxError: Expected identifier";

try
{
   eval ("  class Alpha40                 " +
         "  {                             " +
         "     public function super()    " +
         "     {  }                       " +
         "  }                             ", "unsafe");
}
catch (error)
{
   exceptionThrown40 = true;
   actualError40 = error;
}  


// -----------------------------------------------------------------------
var exceptionThrown41 = false;
var actualError41 = "";
var expectedError41 = "SyntaxError: Expected identifier";

try
{
   eval ("  class Alpha41                    " +
         "  {                                " +
         "     public var super: String;     " +
         "  }                                ");
}
catch (error)
{
   exceptionThrown41 = true;
   actualError41 = error;
}


// -----------------------------------------------------------------------
class Alpha42
{
   public function Bar()
   {
      actual = "Alpha42.Bar()";
   }
}

class Beta42 extends Alpha42
{
   public function Bar()
   {
      expected = "Alpha42.Bar()";
      super.Bar();
      if (actual != expected)
         apLogFailInfo ("cannot invoke superclass methods using super.methodname()", expected, actual, "179583, 172949");
   }
}

 

  

// -----------------------------------------------------------------------
function super14()
{
   apInitTest ("super14");


   // -----------------------------------------------------------------------
   apInitScenario ("32. Beta implements the abstract Alpha.Bar(); Beta.Foo() calls super.Bar()");  
   
   if (exceptionThrown32 == false)
      apLogFailInfo ("No compile error in 32.", "Should give a compile error", exceptionThrown32, "");
   if (actualError32 != expectedError32)
      apLogFailInfo ("Wrong compile error in 32.", expectedError32, actualError32, "");
       
       
   // -----------------------------------------------------------------------
   apInitScenario ("33. Alpha.Bar() is abstract; Beta implements Bar(); Beta.Zoo() calls Alpha.Foo(); Alpha.Foo() calls Bar()");  
       
   var beta33: Beta33 = new Beta33();
   
   expected = "Beta33.Bar()";
   actual = "";
   beta33.Zoo();
   actual = beta33.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 33.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("34. Alpha.Bar() is abstract; Beta implements Bar(); Beta.Zoo() calls Alpha.Foo(); Alpha.Foo() calls this.Bar()");  
       
   var beta34: Beta34 = new Beta34();
   
   expected = "Beta34.Bar()";
   actual = "";
   beta34.Zoo();
   actual = beta34.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 34.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("35. Beta.Bar() overrides Alpha.Bar() and calls super.Bar() at the start");  
      
   var beta35: Beta35 = new Beta35();
   
   expected = "Alpha35.Bar();Beta35.Bar()";
   actual = "";
   beta35.Bar();
   actual = beta35.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 35.", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("36. Beta.Bar() overrides Alpha.Bar() and calls super.Bar() in the middle/end");  
      
   var beta36: Beta36 = new Beta36();
   
   expected = "Beta36.Bar();Alpha36.Bar()";
   actual = "";
   beta36.Bar();
   actual = beta36.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 36.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("37. Alpha.Bar() calls super.ToString(); ToString() is in System.Object");  
            
   var alpha37: Alpha37 = new Alpha37();
   
   expected = "Alpha37";
   actual = "";
   alpha37.Bar();
   actual = alpha37.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 37.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("38. Beta.Bar() calls super.ToString(); ToString() is in System.Object");  
      
   var beta38: Beta38 = new Beta38();
   
   expected = "Beta38";
   actual = "";
   beta38.Bar();
   actual = beta38.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 38.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("39. Charlie.Bar() calls super.ToString(); ToString() is in System.Object");  
      
   var charlie39: Charlie39 = new Charlie39();
   
   expected = "Charlie39";
   actual = "";
   charlie39.Bar();
   actual = charlie39.value;
   if (actual != expected) 
      apLogFailInfo ("Error in 39.", expected, actual, "");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("40. Super is used as a member of a class -- method");  

   if (exceptionThrown40 == false)
      apLogFailInfo ("No compile error in 40.", "Should give a compile error", exceptionThrown40, "220862");
   if (actualError40 != expectedError40)
      apLogFailInfo ("Wrong compile error in 40.", expectedError40, actualError40, "220862");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("41. Super is used as a member of a class -- field");  
      
   if (exceptionThrown41 == false)
      apLogFailInfo ("No compile error in 41.", "Should give a compile error", exceptionThrown41, "200532");
   if (actualError41 != expectedError41)
      apLogFailInfo ("Wrong compile error in 41.", expectedError41, actualError41, "200532");
   

   // -----------------------------------------------------------------------
   apInitScenario ("42. cannot invoke superclass methods using super.methodname()");
   /*
      This bug occurs in 
         super.foo(); 
      and not in
         x = super.foo();
      Previous testcases use
         x = super.foo();
   */
   
   var alpha42: Alpha42 = new Alpha42();
   alpha42.Bar();   
   
   
   apEndTest();
}


super14();


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
