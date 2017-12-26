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


var iTestID = 212514;

//////////////////////////////////////////////////////////////////////////////
//		This testcase tests the pri 1 scenarios for hiding/overriding methods
//
//

//
//
//

@if(!@aspx)
	import System
@end

expando class base1{
  function method1(){
    return 1
  }
}

expando class derived1 extends base1{
  function method1(){
    return 2
  }
  function method2(){
    return 3
  }
}


expando class derived2 extends derived1{
  hide function method1(){
    return 4
  }
  override function method2(){
    return 5
  }
}

expando class derived3 extends derived2{
  hide function method1(){
    return 6
  }
  override function method2(){
    return 7
  }
}


function hideover02() {


  var i

  apInitTest("HideOver02 ");

  apInitScenario("override a base class public method");
  var test1a:derived1 = new derived1
  if (test1a.method1() != 2) apLogFailInfo("1 the base class method was not overridden", 2, test1a.method1(), "")
  var test1b:base1 = new base1
  if (test1b.method1() != 1) apLogFailInfo("2 the base class method was not overridden", 1, test1b.method1(), "")
  var test1c:base1 = new derived1
  if (test1c.method1() != 2) apLogFailInfo("3 the base class method was not overridden", 2, test1c.method1(), "")


  apInitScenario("hide a derived class method");
  var test2a: derived2 = new derived2 
  if (test2a.method1() != 4) apLogFailInfo("4 the derived class method was not hidden", 4, test2a.method1(), "")
  if (test2a.method2() != 5) apLogFailInfo("5 the derived class method was not hidden", 5, test2a.method2(), "")
  var test2b: derived1 = new derived2 
  if (test2b.method1() != 2) apLogFailInfo("6 the derived class method was not hidden", 2, test2b.method1(), "")
  if (test2b.method2() != 5) apLogFailInfo("7 the derived class method was not hidden", 5, test2b.method2(), "")
  var test2c: base1 = new derived2 
  if (test2c.method1() != 2) apLogFailInfo("8 the derived class method was not hidden", 2, test2c.method1(), "")

  apInitScenario("test a nested hide class method");
  var test3a: derived3 = new derived3 
  if (test3a.method1() != 6) apLogFailInfo("9 the derived class method was not hidden", 6, test3a.method1(), "")
  if (test3a.method2() != 7) apLogFailInfo("10 the derived class method was not hidden", 7, test3a.method2(), "")
  var test3b: derived2 = new derived3 
  if (test3b.method1() != 4) apLogFailInfo("12 the derived class method was not hidden", 4, test3b.method1(), "")
  if (test3b.method2() != 7) apLogFailInfo("13 the derived class method was not hidden", 7, test3b.method2(), "")
  var test3c: derived1 = new derived3 
  if (test3c.method1() != 2) apLogFailInfo("14 the derived class method was not hidden", 2, test3c.method1(), "")
  if (test3c.method2() != 7) apLogFailInfo("15 the derived class method was not hidden", 7, test3c.method2(), "")
  var test3d: base1 = new derived3
  if (test3d.method1() != 2) apLogFailInfo("16 the derived class method was not hidden", 2, test3d.method1(), "")

    apEndTest();


}


hideover02();


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
