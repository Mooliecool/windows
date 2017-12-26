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


var iTestID = 52911;

function not002() 
{
   apInitTest("NOT002");


   var SingleVar1, SingleVar2;
   var Result, Expected;


   apInitScenario(" 1: Use ! with constants of each type");

   Result = ! -1;
   Expected = false;
   if (Result != Expected) 
      apLogFailInfo ("! True (Single const) failed", Expected, Result, "");

   Result = ! 0;
   Expected = true;
   if (Result != Expected) 
      apLogFailInfo("! False (Single const) failed", Expected, Result, "");


   apInitScenario(" 2: Use ! with variables of each type");

   SingleVar1 = -1;
   Result = ! SingleVar1;
   Expected = false;
   if (Result != Expected) 
      apLogFailInfo ("! True (Single variable) failed", Expected, Result, "");

   SingleVar1 = 0;
   Result = ! SingleVar1;
   Expected = true;
   if (Result != Expected) 
      apLogFailInfo("! False (Single variable) failed", Expected, Result, "");
      

   apInitScenario(" 3: Use ! with expressions of each type");

   SingleVar1 = 0;
   SingleVar2 = 1;
   Result = ! SingleVar1 - SingleVar2;
   Expected = 0;
   if (Result != Expected) 
      apLogFailInfo("! True (Single expression) failed", Expected, Result, "147763, 152057");

   SingleVar1 = 0;
   SingleVar2 = 1;
   Result = ! SingleVar1 / SingleVar2;
   Expected = 1;
   if (Result != Expected) 
      apLogFailInfo("! False (Single expression) failed", Expected, Result, "147763, 152057");

   apEndTest();
}




not002();


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
