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


var iTestID = 109825;



// -----------------------------------------------------------------------
var actual = "";
var expected = "";
var expectedError: String = "";
var actualError: String = "";
var errorThrown = false;


// -----------------------------------------------------------------------
/*
Inheritance tree:
-----------------
Alpha 
   Beta 
      Charlie
   Delta
      Echo 
         Foxrat
            Gamma 
*/

class Alpha
{
   public function GetName() : String
   {
      return "Alpha";
   }
}

class Beta extends Alpha
{
   public function GetName() : String
   {
      return "Beta";
   }
}

class Charlie extends Beta
{
   // No method
}

class Delta extends Alpha
{
   // No method
}

class Echo extends Delta
{
   public function GetName() : String
   {
      return "Echo";
   }
}

class Foxrat extends Echo
{
   // No method
}

class Gamma extends Foxrat
{
   public function GetName() : String
   {
      return "Gamma";
   }
}


function FooAlpha (alpha: Alpha)
{
   return alpha.GetName();
}

function BarBeta (beta: Beta)
{
   return beta.GetName();
}



class Foo
{
   public function CallGetName (alpha: Alpha)
   {
      return alpha.GetName();
   }
}

class Bar
{
   public function CallGetName (beta: Beta)
   {
      return beta.GetName();
   }
}


// -----------------------------------------------------------------------
class Alpha8
{
   public function Bar(): String
   {
      var temp: String = "";
      temp = Foo();
      return temp;
   }

   private function Foo(): String
   {
      return "Alpha8.Foo()";
   }
}

class Beta8 extends Alpha8
{
   protected function Foo(): String
   {
      return "Beta8.Foo()";
   }
}


// -----------------------------------------------------------------------
class Alpha9
{
   public function Bar(): String
   {
      var temp: String = "";
      temp = Foo();
      return temp;
   }

   private function Foo(): String
   {
      return "Alpha9.Foo()";
   }
}

class Beta9 extends Alpha9
{
   private function Foo(): String
   {
      return "Beta9.Foo()";
   }
}


// -----------------------------------------------------------------------
class Alpha10
{
   public function Bar(): String
   {
      var temp: String = "";
      temp = Foo();
      return temp;
   }

   private function Foo(): String
   {
      return "Alpha10.Foo()";
   }
}

class Beta10 extends Alpha10
{
   internal function Foo(): String
   {
      return "Beta10.Foo()";
   }
}


// -----------------------------------------------------------------------
class Alpha11
{
   public function Bar(): String
   {
      var temp: String = "";
      temp = Foo();
      return temp;
   }

   private function Foo(): String
   {
      return "Alpha11.Foo()";
   }
}

class Beta11 extends Alpha11
{
   public function Foo(): String
   {
      return "Beta11.Foo()";
   }
}





// -----------------------------------------------------------------------
function methodoverriding()
{
   apInitTest ("MethodOverriding");

   // -----------------------------------------------------------------------
   apInitScenario ("1.1 Declare and instantiate all classes. Call each object's method.");
   
   var alpha1_1: Alpha = new Alpha();
   var beta1_1: Beta = new Beta();
   var charlie1_1: Charlie = new Charlie();
   var delta1_1: Delta = new Delta();
   var echo1_1: Echo = new Echo();
   var foxrat1_1: Foxrat = new Foxrat();
   var gamma1_1: Gamma = new Gamma();

   expected = "Alpha"
   actual = "";
   actual = alpha1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Alpha::GetName() returned the wrong value", expected, actual, "");

   expected = "Beta";
   actual = "";
   actual = beta1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Beta::GetName() returned the wrong value", expected, actual, "");

   expected = "Beta";
   actual = "";
   actual = charlie1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Charlie::GetName() returned the wrong value", expected, actual, "104153");

   expected = "Alpha";
   actual = "";
   actual = delta1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Delta::GetName() returned the wrong value", expected, actual, "104153");
      
   expected = "Echo";
   actual = "";
   actual = echo1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Echo::GetName() returned the wrong value", expected, actual, "");

   expected = "Echo";
   actual = "";
   actual = foxrat1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Foxrat::GetName() returned the wrong value", expected, actual, "104153");
      
   expected = "Gamma";
   actual = ""
   actual = gamma1_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Gamma::GetName() returned the wrong value", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("1.2 Declare and instantiate all classes. Pass each object to a function with an argument of type Alpha");

   var alpha1_2: Alpha = new Alpha();
   var beta1_2: Beta = new Beta();
   var charlie1_2: Charlie = new Charlie();
   var delta1_2: Delta = new Delta();
   var echo1_2: Echo = new Echo();
   var foxrat1_2: Foxrat = new Foxrat();
   var gamma1_2: Gamma = new Gamma();

   expected = "Alpha";
   actual = "";
   actual = FooAlpha (alpha1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with an Alpha object", expected, actual, "");

   expected = "Beta";
   actual = "";
   actual = FooAlpha (beta1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Beta object", expected, actual, "");

   expected = "Beta";
   actual = "";
   actual = FooAlpha (charlie1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Charlie object", expected, actual, "");

   expected = "Alpha";
   actual = "";
   actual = FooAlpha (delta1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Delta object", expected, actual, "");

   expected = "Echo";
   actual = "";
   actual = FooAlpha (echo1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with an Echo object", expected, actual, "");

   expected = "Echo";
   actual = "";
   actual = FooAlpha (foxrat1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Foxrat object", expected, actual, "");

   expected = "Gamma";
   actual = "";
   actual = FooAlpha (gamma1_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Gamma object", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("1.3 Declare and instantiate all classes. Pass each object to a method with an argument of type Alpha");
      
   var foo1_3: Foo = new Foo();
   
   var alpha1_3: Alpha = new Alpha();
   var beta1_3: Beta = new Beta();
   var charlie1_3: Charlie = new Charlie();
   var delta1_3: Delta = new Delta();
   var echo1_3: Echo = new Echo();
   var foxrat1_3: Foxrat = new Foxrat();
   var gamma1_3: Gamma = new Gamma();
      
   expected = "Alpha";
   actual = "";
   actual = foo1_3.CallGetName (alpha1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with an Alpha object", expected, actual, "");

   expected = "Beta";
   actual = "";
   actual = foo1_3.CallGetName (beta1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Beta object", expected, actual, "128832");      
      
   expected = "Beta";
   actual = "";
   actual = foo1_3.CallGetName (charlie1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Charlie object", expected, actual, "");
      
   expected = "Alpha";
   actual = "";
   actual = foo1_3.CallGetName (delta1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Delta object", expected, actual, "");

   expected = "Echo";
   actual = "";
   actual = foo1_3.CallGetName (echo1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with an Echo object", expected, actual, "");

   expected = "Echo";
   actual = "";
   actual = foo1_3.CallGetName (foxrat1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Foxrat object", expected, actual, "");
      
   expected = "Gamma";
   actual = "";
   actual = foo1_3.CallGetName (gamma1_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Gamma object", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("2.1 Declare an object as the base class. Instantiate it with each subclass. Call each object's method");
   
   var alpha2_1: Alpha;
   
   alpha2_1 = new Alpha();
   expected = "Alpha";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Alpha::GetName() returned the wrong value", expected, actual, "");

   alpha2_1 = new Beta();
   expected = "Beta";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Beta::GetName() returned the wrong value", expected, actual, "104153, 128832");
      
   alpha2_1 = new Charlie();
   expected = "Beta";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Charlie::GetName() returned the wrong value", expected, actual, "104153, 128832");
   
   alpha2_1 = new Delta();
   expected = "Alpha";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Delta::GetName() returned the wrong value", expected, actual, "104153, 128832");

   alpha2_1 = new Echo();
   expected = "Echo";
   actual = "";
   actual = alpha2_1.GetName();     
   if (actual != expected)
      apLogFailInfo ("Echo::GetName() returned the wrong value", expected, actual, "104153, 128832");
         
   alpha2_1 = new Foxrat();
   expected = "Echo";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Foxrat::GetName() returned the wrong value", expected, actual, "104153, 128832");

   alpha2_1 = new Gamma();
   expected = "Gamma";
   actual = "";
   actual = alpha2_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Gamma::GetName() returned the wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("2.2 Declare an object as the base class.  Instantiate it with each subclass. Pass each object to a function with an argument of type Alpha");
                   
   var alpha2_2: Alpha;
   
   alpha2_2 = new Alpha();
   expected = "Alpha";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected) 
      apLogFailInfo ("Function returns a wrong value with an Alpha instance", expected, actual, "");

   alpha2_2 = new Beta();
   expected = "Beta";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Beta instance", expected, actual, "128832");

   alpha2_2 = new Charlie();
   expected = "Beta";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Charlie instance", expected, actual, "128832");

   alpha2_2 = new Delta();
   expected = "Alpha";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Delta instance", expected, actual, "128832");

   alpha2_2 = new Echo();
   expected = "Echo";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with an Echo instance", expected, actual, "128832");

   alpha2_2 = new Foxrat();
   expected = "Echo";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Foxrat instance", expected, actual, "128832");

   alpha2_2 = new Gamma();
   expected = "Gamma";
   actual = "";
   actual = FooAlpha (alpha2_2);
   if (actual != expected)
      apLogFailInfo ("Function returns a wrong value with a Gamma instance", expected, actual, "128832");


   // -----------------------------------------------------------------------
   apInitScenario ("2.3 Declare an object as the base class.  Instantiate it with each subclass. Pass each object to a method with an argument of type Alpha");
                   
   var alpha2_3: Alpha;
   var foo2_3: Foo = new Foo();
   
   alpha2_3 = new Alpha();
   expected = "Alpha";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with an Alpha instance", expected, actual, "");

   alpha2_3 = new Beta();
   expected = "Beta";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Beta instance", expected, actual, "128832");

   alpha2_3 = new Charlie();
   expected = "Beta";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Charlie instance", expected, actual, "128832");

   alpha2_3 = new Delta();
   expected = "Alpha";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Delta instance", expected, actual, "128832");

   alpha2_3 = new Echo();
   expected = "Echo";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with an Echo instance", expected, actual, "128832");
                              
   alpha2_3 = new Foxrat();
   expected = "Echo";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Foxrat instance", expected, actual, "128832");

   alpha2_3 = new Gamma();
   expected = "Gamma";
   actual = "";
   actual = foo2_3.CallGetName (alpha2_3);
   if (actual != expected)
      apLogFailInfo ("Method returns a wrong value with a Gamma instance", expected, actual, "128832");
                      

   // -----------------------------------------------------------------------
   apInitScenario ("3. Declare using the subclass. Instantiate using the superclass.");
   // Error

   var beta3: Beta;  
   
   expectedError = "TypeError: Type mismatch";
   actualError = "";
   errorThrown = false;
   
   try
   {
      eval ("beta3 = new Alpha();");
   }
   catch (error)
   {
      errorThrown = true;
      actualError = error;
   }
   
   if (errorThrown == false)
      apLogFailInfo ("Exception not thrown", "An exception should be thrown", errorThrown, "104153, 129550");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "147815, 131840, 104153, 129550");      


   // -----------------------------------------------------------------------
   apInitScenario ("4.1 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the superclass; Call the object's method directly.");
      
   var beta4_1: Beta;
   beta4_1 = new Charlie();

   expected = "Beta";
   actual = "";
   actual = beta4_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("4.2 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the superclass; Pass the object to a function.");

   var beta4_2: Beta;
   beta4_2 = new Charlie();
   
   expected = "Beta";
   actual = "";
   actual = FooAlpha (beta4_2);
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("4.3 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the superclass; Pass the object to a method.");

   var foo4_3: Foo = new Foo();
   var beta4_3: Beta;
   beta4_3 = new Charlie();
   
   expected = "Beta";
   actual = "";
   actual = foo4_3.CallGetName (beta4_3);
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("5.1 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the subclass; Call the object's method directly");

   var delta5_1: Delta;
   delta5_1 = new Echo();
   
   expected = "Echo";
   actual = "";
   actual = delta5_1.GetName();
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("5.2 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the subclass; Pass the object to a function");
   
   var delta5_2: Delta;
   delta5_2 = new Echo();
   
   expected = "Echo";
   actual = "";
   actual = FooAlpha (delta5_2);
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("5.3 Declare using the superclass (mid-level in the inheritance tree); Instantiate using the subclass; Method in the subclass; Pass the object to a method");
   
   var foo5_3: Foo = new Foo();
   var delta5_3: Delta;
   delta5_3 = new Echo();
   
   expected = "Echo";
   actual = "";
   actual = foo5_3.CallGetName (delta5_3);
   if (actual != expected)
      apLogFailInfo ("Wrong value", expected, actual, "104153, 128832");


   // -----------------------------------------------------------------------
   apInitScenario ("6. Declare and instantiate a superclass. Pass to a function with an argument of type subclass");      
      
   var alpha6: Alpha = new Alpha();
   
   actual = "";
   errorThrown = false;
   expectedError = "TypeError: Type mismatch";
   actualError = "";

   try
   {
      eval ("actual = BarBeta (alpha6);");
   }
   catch (error)
   {
      errorThrown = true;
      actualError = error;
   }
   
   if (errorThrown == false)
      apLogFailInfo ("Error not thrown", "Should throw an error", errorThrown, "128840");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
   if (actual == "Alpha")
      apLogFailInfo ("passed to a function with an argument of type subclass","Should give a compile error", actual, "128840");
                           

   // -----------------------------------------------------------------------
   apInitScenario ("7. Declare and instantiate a superclass. Pass to a method with an argument of type subclass");  
                   
   var alpha7: Alpha = new Alpha();
   var bar7: Bar = new Bar();
   
   errorThrown = false;
   actual = "";
   expectedError = "TypeError: Type mismatch";
   actualError = "";
   
   try
   {
      eval ("actual = bar7.CallGetName (alpha7);");
   }
   catch (error)
   {
      errorThrown = true;
      actualError = error;
   }
   
   if (errorThrown == false)
      apLogFailInfo ("Error not thrown", "Should throw an error", errorThrown, "128840");
   if (actualError != expectedError)
      apLogFailInfo ("Wrong error message", expectedError, actualError, "");
   if (actual == "Alpha")
      apLogFailInfo ("passed to a method with an argument of type subclass", "Should give a compile error", actual, "128840");
                     
                     
   // -----------------------------------------------------------------------
   apInitScenario ("8. public Alpha.Bar(); private Alpha.Foo(); protected Beta.Foo()");  
                     
   var alpha8: Alpha8;
   
   alpha8 = new Beta8();
   expected = "Alpha8.Foo()";
   actual = "";
   actual = alpha8.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 8", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("9. public Alpha.Bar(); private Alpha.Foo(); private Beta.Foo()");  
                     
   var alpha9: Alpha9;
   
   alpha9 = new Beta9();
   expected = "Alpha9.Foo()";
   actual = "";
   actual = alpha9.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 9", expected, actual, "");      
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("10. public Alpha.Bar(); private Alpha.Foo(); internal Beta.Foo()");  
                     
   var alpha10: Alpha10;
   
   alpha10 = new Beta10();
   expected = "Alpha10.Foo()";
   actual = "";
   actual = alpha10.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 10", expected, actual, "");    
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("11. public Alpha.Bar(); private Alpha.Foo(); public Beta.Foo()");  
                     
   var alpha11: Alpha11;
   
   alpha11 = new Beta11();
   expected = "Alpha11.Foo()";
   actual = "";
   actual = alpha11.Bar();
   if (actual != expected)
      apLogFailInfo ("Error in 11", expected, actual, "");              
   
  
      
   apEndTest();
}



methodoverriding();


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
