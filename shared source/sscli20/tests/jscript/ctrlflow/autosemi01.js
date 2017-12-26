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


var iTestID = 215505;

///////////////////////////////////////////////////////////////////////////////////

//import System

var y;
function foo(){y = 5; return}

function autosemi01() {

  apInitTest("autosemi01");
  y = 0;
  var x = 0;
  
  apInitScenario("test for regression")
  for (x=1; x<10; x++){break}
  if (x != 1) apLogFailInfo("break broke", 1, x, "293494")
  for (x=1; x<10; x++){if (true) break}
  if (x != 1) apLogFailInfo("break broke", 1, x, "293494")


  for (x=1; x<10; x++){continue}
  if (x != 10) apLogFailInfo("continue broke", 10, x, "293494")
  for (x=1; x<10; x++){if (true) continue}
  if (x != 10) apLogFailInfo("continue broke", 10, x, "293494")

  foo()
  if (y != 5) apLogFailInfo("continue broke", 5, x, "293494")


  for(x=0; x<20; x++){
    try{}
    finally{continue}
  }

  if (x != 20) apLogFailInfo("continue broke", 20, x, "293494")
  

  apEndTest();
}




autosemi01();


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
