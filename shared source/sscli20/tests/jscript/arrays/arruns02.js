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


var iTestID = 241591;

// Global variables
//



function arruns02() {

  apInitTest("arruns02: Array.prototype.unshift"); 

  var nativeInt:int[]
  var nativeObj:Object[]
  var nativeArr:Array[]
  var wrapInt:Array
  var wrapObj:Array
  var wrapArr:Array

  apInitScenario("1. unshift nothing onto a native array");

  nativeInt = new int[5]
  nativeInt.unshift()
  if (nativeInt.length != 5) apLogFailInfo("something was inserted into a native array", 5, nativeInt.length, "")

  nativeInt = new int[0]
  nativeInt.unshift()
  if (nativeInt.length != 0) apLogFailInfo("something was inserted into a native array", 0, nativeInt.length, "")

  nativeObj = new Object[5]
  nativeObj.unshift()
  if (nativeObj.length != 5) apLogFailInfo("something was inserted into a native array", 5, nativeObj.length, "")

  nativeArr = new Array[5]
  nativeArr.unshift()
  if (nativeArr.length != 5) apLogFailInfo("something was inserted into a native array", 5, nativeArr.length, "")


  apInitScenario("2. unshift nothing onto a native array wrapper");

  wrapInt = new int[5]
  wrapInt.unshift()
  if (wrapInt.length != 5) apLogFailInfo("something was inserted into a native array", 5, wrapInt.length, "")

  wrapInt = new int[0]
  wrapInt.unshift()
  if (wrapInt.length != 0) apLogFailInfo("something was inserted into a native array", 0, wrapInt.length, "")

  wrapObj = new Object[5]
  wrapObj.unshift()
  if (wrapObj.length != 5) apLogFailInfo("something was inserted into a native array", 5, wrapObj.length, "")

  wrapArr = new Array[5]
  wrapArr.unshift()
  if (wrapArr.length != 5) apLogFailInfo("something was inserted into a native array", 5, wrapArr.length, "")


  apInitScenario("3. unshift something onto a native array");

  nativeInt = new int[0]
  try{
    nativeInt.unshift(0)
    apLogFailInfo("something was inserted into a native array", "", nativeInt.length, "")
  }catch(e){
  }
  
  var nativeLong:long[] = new long[16]
  try{
    nativeLong.unshift(0)
    apLogFailInfo("something was inserted into a native array", "", nativeInt.length, "")
  }catch(e){
  }

  apInitScenario("4. unshift a native array into a JSArray");

  var x:Array = new Array(3);
  x.unshift(nativeLong)

  apEndTest();
}



arruns02();


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
