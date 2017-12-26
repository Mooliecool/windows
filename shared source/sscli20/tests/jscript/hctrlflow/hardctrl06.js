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


var iTestID = 194481;

///////////////////////////////////////////////////////////////////////////////////
var res



expando class test1{
  static function go(){
    var res = 0
    var x = 0
        
      try{
      eval("var d1:decimal=10; res = d1?1:2")
      if (res != 1) apLogFailInfo(x++ +": wrong results obtained", 1, res, "")

      eval("var d2:decimal=decimal.One; res = d2?1:2")
      if (res != 1) apLogFailInfo(x++ +": wrong results obtained", 1, res, "")

      eval ("var d3:decimal; res = d3?1:2")
      if (res != 2) apLogFailInfo(x++ +": wrong results obtained", 2, res, "")

      eval ("var d4:decimal=0; res = d4?1:2")
      if (res != 2) apLogFailInfo(x++ +": wrong results obtained", 2, res, "")

      
      res = true;
      eval("var d5:decimal=3; d5 = res?++d5:'hi'; res = d5")
      if (res != 4) apLogFailInfo(x++ +": wrong results obtained", 4, res, "")

      with(test1.go){
        var d6:decimal = x+1?"43":Infinity;
        res = d6
      }
      if (res != 43) apLogFailInfo(x++ +": wrong results obtained", -1, res, "")
    }catch(e){
      apLogFailInfo("unexpected failure group: "+x, "", e.description, "")
    }
  }
}


class t2{
  public var i:decimal
}
function test2(){test2a()}
function test2a(){
  var x = new t2
  x.i = 10
  var count = 0;
  do{x.i+=-.5; count++} while (x.i);
  if (count != 20) apLogFailInfo("looped wrong number of times", 20, count, "")
}


function test3(){
  for (var i:decimal=-10, count=0; i; i+=.25, count++);
  if (count != 40) apLogFailInfo("looped wrong number of times", 40, count, "")
}

function test4(dec:decimal){
  switch(dec){
    case 1: return 2
    case 1.5: return 3
    case -20: return 4.5
    default: return 0
  }
}

class test6{
  static var d:decimal;
  static function go(){
    var count=0;
    d = 32
    while(d+1) {d /=-2; count++}
    if (count != 5) apLogFailInfo("looped wrong number of times", 5, count, "")  
  }
}

///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl06() {

    apInitTest("HardCtrl06");
    var res,x

    apInitScenario("conditional of a decimal in an eval in a static method")
    test1.go()

    apInitScenario("do of a public decimal in a nested function")
    test2()

    apInitScenario("for of a decimal in a function")
    test3()

    apInitScenario("switch of a decimal in a function")
    res = test4(1)
    if (res != 2) apLogFailInfo("wrong branch taken", 2, res, "")
    res = test4(1.5)
    if (res != 3) apLogFailInfo("wrong branch taken", 3, res, "")
    res = test4(-20)
    if (res != 4.5) apLogFailInfo("wrong branch taken", 4.5, res, "")
    res = test4(999999999999999999999999)
    if (res != 0) apLogFailInfo("wrong branch taken", 0, res, "")

    apInitScenario("if of a decimal in an eval in a function")
    var d1:decimal = decimal.MinusOne
    res = eval("if (d1) true; else false;")
    if (!res) apLogFailInfo("wrong return value", true, res, "")
    d1 = 45
    res = eval("if (d1) true; else false;")
    if (!res) apLogFailInfo("wrong return value", true, res, "")
    d1 = 0
    res = eval("if (d1) true; else false;")
    if (res) apLogFailInfo("wrong return value", false, res, "")
    d1 = decimal.MaxValue
    res = eval("if (d1) true; else false;")
    if (!res) apLogFailInfo("wrong return value", true, res, "")

    apInitScenario("while of a static decimal in an static method")
    test6.go()

    apEndTest();
}


hardctrl06();


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
