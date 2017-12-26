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


var iTestID = 193766;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;



/*
Testcase error
// -----------------------------------------------------------------------
var exceptionThrown30_4 = false;
var actualError30_4 = "";
var expectedError30_4 = "More than one constructor matches this parameter list";

try
{
   eval ("  class Alpha30_4                           " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     public function Alpha30_4 (x: String)  " +
         "     {                                      " +
         "        this();                             " +
         "     }                                      " +
         "     public function Alpha30_4 (x: double)  " +
         "     {                                      " +
         "        value = \"nine\";                   " +
         "     }                                      " +
         "     public function Alpha30_4 (x: int)     " +
         "     {                                      " +
         "        value = \"eight\";                  " +
         "     }                                      " +
         "  }                                         ");
}
catch (error)
{
   exceptionThrown30_4 = true;
   actualError30_4 = error;
}
*/

// -----------------------------------------------------------------------
var exceptionThrown30_5 = false;
var actualError30_5 = "";
var expectedError30_5 = "Error: It is not valid to call a base class constructor from this location";

try
{
   eval ("  class Alpha30_5                           " +
         "  {                                         " +
         "     public var value: String = \"none\";   " +
         "     public function Alpha30_5 (x: int)     " +
         "     {                                      " +
         "        value = \"eight\";                  " +
         "     }                                      " +
         "     public function Alpha30_5 (x: String)  " +
         "     {                                      " +
         "        value = \"hello\";                  " +
         "        this (25);                          " +
         "     }                                      " +
         "  }                                         ", "unsafe");
}
catch (error)
{
   exceptionThrown30_5 = true;
   actualError30_5 = error;
}


// -----------------------------------------------------------------------
class Alpha30_6
{
   public var value: String = "none";
   
   public function Alpha30_6()
   {
      this (35);
   }
   
   public function Alpha30_6 (x: int, y: String)
   {
      value = "two";
   }

   public function Alpha30_6 (x: int)
   {
      value = "one";
   }
}


// -----------------------------------------------------------------------
class Alpha30_7
{
   public var value: String = "none";
   
   public function Alpha30_7()
   {
      this (35, "hello");
   }
   
   public function Alpha30_7 (x: int)
   {
      value = "three";
   }

   public function Alpha30_7 (x: int, y: String)
   {
      value = "four";
   }
}


// -----------------------------------------------------------------------
class Alpha31_1
{  
   public var value: String = "none";
   
   public function Alpha31_1()
   {
      value = "one";
   }
   
   public function Alpha31_1 (x: boolean)
   {
      value = "two";
   }
}

class Beta31_1 extends Alpha31_1
{
   public function Beta31_1()
   {
      super();
   }
}


// -----------------------------------------------------------------------
class Alpha31_2
{
   public var value: String = "none";
   
   public function Alpha31_2()
   {
      value = "five";
   }
   
   public function Alpha31_2 (x: boolean)
   {
      value = "six";
   }
}

class Beta31_2 extends Alpha31_2
{
   public function Beta31_2 (x: boolean)
   {
      super();
   }
}


// -----------------------------------------------------------------------
class Alpha31_3
{
   public var value: String = "none";
   
   public function Alpha31_3 (x: String)
   {
      value = "seven";
   }
}

class Beta31_3 extends Alpha31_3
{
   public function Beta31_3()
   {
      super ("hello");
   }
}


// -----------------------------------------------------------------------
class Alpha31_4
{
   public var value: String = "none";
   
   public function Alpha31_4 (x: String)
   {
      value = x;
   }
}

class Beta31_4 extends Alpha31_4
{
   public function Beta31_4 (x: String)
   {
      super (x);
   }
}


// -----------------------------------------------------------------------
class Alpha31_5
{
   public var value: String = "none";
   
   public function Alpha31_5()
   {
      value = "ten";
   }
}

class Beta31_5 extends Alpha31_5
{
   public function Beta31_5()
   {
      super();
   }
}

class Charlie31_5 extends Beta31_5
{
   public function Charlie31_5()
   {
      super();
   }
}







// -----------------------------------------------------------------------
function constructors11()
{
   apInitTest ("Constructors11");

   // -----------------------------------------------------------------------
   apInitScenario ("30.4 Calling other constructors using 'this'. Alpha(String) calls this() with Alpha(int) and Alpha(double) present");

/*
Testcase error
   
   if (exceptionThrown30_4 == false)
      apLogFailInfo ("No compile error in 30.4", "Should give a compile error", exceptionThrown30_4, "");
   if (actualError30_4 != expectedError30_4) 
      apLogFailInfo ("Wrong compile error in 30.4", expectedError30_4, actualError30_4, "200532");
*/      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30.5 Calling other constructors using 'this'. Call to 'this' is not the first line of a constructor");
   
   if (exceptionThrown30_5 == false)
      apLogFailInfo ("No compile error in 30.5", "Should give a compile error", exceptionThrown30_5, "");
   if (actualError30_5 != expectedError30_5)
      apLogFailInfo ("Wrong compile error in 30.5", expectedError30_5, actualError30_5, "200532");   
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30.6 Calling other constructors using 'this'. Call to this (25) with Alpha (int) & Alpha (int, String) available");
         
   var alpha30_6: Alpha30_6 = new Alpha30_6();

   expected = "one";
   actual = "";
   actual = alpha30_6.value;
   if (actual != expected)
      apLogFailInfo ("Error in 30.6", expected, actual, ""); 
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("30.7 Calling other constructors using 'this'. Call to this(25,'hello') with Alpha(int) & Alpha(int,String) available");
           
   var alpha30_7: Alpha30_7 = new Alpha30_7();
   
   expected = "four";
   actual = "";
   actual = alpha30_7.value;
   if (actual != expected)
      apLogFailInfo ("Error in 30.7", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.1 Calling superclass' constructors using 'super'. Beta() calls super()");
      
   var beta31_1: Beta31_1 = new Beta31_1();
   
   expected = "one";
   actual = "";
   actual = beta31_1.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.1", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.2 Calling superclass' constructors using 'super'. Beta(boolean) calls super()");
      
   var beta31_2: Beta31_2 = new Beta31_2 (false);
   
   expected = "five";
   actual = "";
   actual = beta31_2.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.2", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.3 Calling superclass' constructors using 'super'. Beta() calls super(String)");
                        
   var beta31_3: Beta31_3 = new Beta31_3();
   
   expected = "seven";
   actual = "";
   actual = beta31_3.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.3", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.4 Calling superclass' constructors using 'super'. Beta (String) calls super (String)");
      
   var beta31_4: Beta31_4 = new Beta31_4 ("blah");
   
   expected = "blah";
   actual = "";
   actual = beta31_4.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.4", expected, actual, "");
      
      
   // -----------------------------------------------------------------------
   apInitScenario ("31.5 Calling superclass' constructors using 'super'. Charlie() calls super() and Beta() calls super() again");
   
   var charlie31_5: Charlie31_5 = new Charlie31_5();
   
   expected = "ten";
   actual = "";
   actual = charlie31_5.value;
   if (actual != expected)
      apLogFailInfo ("Error in 31.5", expected, actual, "");

   apEndTest();
}



constructors11();


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
