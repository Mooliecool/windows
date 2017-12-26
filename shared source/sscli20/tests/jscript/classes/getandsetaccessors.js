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


var iTestID = 142069;


// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var actualError = "";
var expectedError = "";



// -----------------------------------------------------------------------
class Alpha1
{
   public function get Prop(): String
   {
      return "Alpha1";
   }
}


// -----------------------------------------------------------------------
function getandsetaccessors()
{
   apInitTest ("GetAndSetAccessors");


   // -----------------------------------------------------------------------
   apInitScenario ("1.1 Get accessor -- without eval");
   
   var alpha1_1: Alpha1 = new Alpha1();
   
   actual = "";
   expected = "Alpha1";
   actual = alpha1_1.Prop;
   if (actual != expected)
      apLogFailInfo ("Get accessor returns the wrong value", expected, actual, "172007");
   

   // -----------------------------------------------------------------------
   apInitScenario ("1.2 Get accessor -- with eval");
   
   var alpha1_2: Alpha1 = new Alpha1();
   
   expected = "Alpha1";
   actual = "";
   exceptionThrown = false;
   actualError = "";
   
   try
   {
      eval ("actual = alpha1_2.Prop;");
   }
   catch (error)
   {
      exceptionThrown = true;
      actualError = error;
   }
   
   if (actual != expected)
      apLogFailInfo ("Get accessor returns the wrong value", expected, actual, "172007");
   if (exceptionThrown == true)
      apLogFailInfo ("Get accessor causes compile error","Shouldn't give a compile error", actualError, "172007");

   apEndTest();
}



getandsetaccessors();


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
