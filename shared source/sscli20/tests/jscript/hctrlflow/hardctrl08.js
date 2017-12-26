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


var iTestID = 196454;

///////////////////////////////////////////////////////////////////////////////////
var res


class err{
  var num = 0
  var results:Object[,] = new Object[10,10]
  function add(str, exp, act, bug){
    results[num,0] = str
    results[num,1] = exp
    results[num,2] = act
    results[num,3] = bug
    num++
  }
  function show(){
    for (var i=0; i<num; i++){
      apLogFailInfo(results[i,0], results[i,1], results[i,2], results[i,3])
    }
  }
}

 


var tvar1:char

function test1(){test1a()}
function test1a(){
  try{
    eval('eval("tvar1 = \'A\';  res = tvar1?true:432")')
    if (res != true) apLogFailInfo("1 wrong return value", true, res, "")

    eval('eval("tvar1 = false;  res = tvar1?true:432")')
    if (res != 432) apLogFailInfo("2 wrong return value", char(432), res, "")

    eval('eval("tvar1 = false;  tvar1 = tvar1?false:true")')
    if (tvar1 != true) apLogFailInfo("3 wrong return value", true, tvar1, "")

    eval('eval("tvar1 = false;  tvar1 = tvar1?true:\'A\'")')
    if (tvar1 != "A") apLogFailInfo("4 wrong return value", "A", tvar1, "")

    eval('eval("tvar1 = 256;  tvar1 = tvar1?false:true")')
    if (tvar1 != false) apLogFailInfo("5 wrong return value", false, tvar1, "")

    tvar1 = false;  tvar1 = tvar1?'hi':45
    if (tvar1 != char(45)) apLogFailInfo("6 wrong return value", true, tvar1, "")

  }catch(e){
    apLogFailInfo("unexpected failure ", "", e.description, "")
  }
}

expando class test2{
  private var i:char
  expando function go(){now()}
  function now(){
    i = 10
    var count = 0;
    do{i--; count++} while (i);
    if (count != 10) apLogFailInfo("looped wrong number of times", 10, count, "")
  }
}

function test3(){test3a()}
function test3a(){
  for (var i:char="\u000a", count=0; i; i--, count++);
  if (count != 10) apLogFailInfo("looped wrong number of times", 10, count, "")
}

class test4{
  static internal var b:char
  static function go(invar:char){
    b = invar
    switch(b){
      case char(49):  return 0; 
      case "1": return 1; 
      case 256: return 2; 
      case false: return 3;
      case -1: return 4;
      case .5: return 5;
      case 65: return 6;
      case "A": return 7;
      default: return 8;
    }
  }
}


expando class t5{
  static var i:char;
}
function test5(invar:char){return test5a(invar)}
function test5a(invar:char){
  t5.i = invar
  if (t5.i) return false;
  else return true;
}

  
class test6{
  function go(){
    var tvar6:char = "A"
    var count = 0
    eval("while (tvar6){count++; tvar6--}")
    if (count != 65) apLogFailInfo("looped wrong number of times", 65, count, "")
  }
}


///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl08() {

  apInitTest("HardCtrl08");
  var res,x

  apInitScenario("cond with a global char in a nested eval in a nested function")
  test1()

  apInitScenario("do with a private char in a nested method")
  x = new test2
  x.go()

  apInitScenario("for with a char in a nested function")
  test3()

  apInitScenario("switch with a char in a static class method")
  res = test4.go("1")
  if (res != 0) apLogFailInfo("1 wrong branch taken", 0, res, "")  
  res = test4.go(49)
  if (res != 0) apLogFailInfo("2 wrong branch taken", 0, res, "")  
  res = test4.go(char(256))
  if (res != 2) apLogFailInfo("3 wrong branch taken", 2, res, "")  
  res = test4.go(0)
  if (res != 3) apLogFailInfo("4 wrong branch taken", 3, res, "")  
  res = test4.go("A")
  if (res != 6) apLogFailInfo("5 wrong branch taken", 6, res, "")  
  res = test4.go(65)
  if (res != 6) apLogFailInfo("6 wrong branch taken", 6, res, "")  
  res = test4.go(100)
  if (res != 8) apLogFailInfo("7 wrong branch taken", 8, res, "")  

  apInitScenario("if with a char in a static class method")
  res = test5(1)
  if (res != false) apLogFailInfo("wrong return value", false, res, "")  
  res = test5(0)
  if (res != true) apLogFailInfo("wrong return value", true, res, "")  

  apInitScenario("while with a external char in an eval in a method")
  x = new test6
  x.go()

  apEndTest();



}

hardctrl08();


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
