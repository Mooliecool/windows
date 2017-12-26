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


var iTestID = 194678;

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
var test1 = new err
 


var tvar1:boolean

@if(@aspx)
function HC_test(){
@end
try{
  eval("tvar1 = true;  res = tvar1?true:432")
  if (res != true) test1.add("wrong return value", true, res, "")

  eval("tvar1 = false;  res = tvar1?true:432")
  if (res != 432) test1.add("wrong return value", 432, res, "")

  eval("tvar1 = false;  tvar1 = tvar1?false:true")
  if (tvar1 != true) test1.add("wrong return value", true, tvar1, "")

  tvar1 = false;  tvar1 = tvar1?'hi':45
  if (tvar1 != true) test1.add("wrong return value", true, tvar1, "")

}catch(e){
  apLogFailInfo("unexpected failure ", "", e.description, "")
}
@if(@aspx)
}
@end

class t2{
  public var i:boolean
}
function test2(){test2a()}
function test2a(){
  var x = new t2
  x.i = 0
  var count = 0;
  do{x.i--; count++} while (x.i);
  if (count != 2) apLogFailInfo("looped wrong number of times", 2, count, "")
}


function test3(){test3a()}
function test3a(){
  for (var i:boolean=true, count=0; i; count++) if (count>9) i = false;
  if (count != 11) apLogFailInfo("looped wrong number of times", 11, count, "")
}

class test4{
  public var b:boolean
  function go(invar:boolean){
    b = invar
    return eval("var ret; switch(b){case true: ret=10; break; case false: ret=5; break; case 1: ret=1; break; defualt: ret=-1; break;}; ret") 
  }
}

@if(!@aspx)
package t5{
@end
  class test5{
    static internal var i:boolean
    static function go(invar:boolean){
      i = invar
      if (i) return false
      else return true
    }
  }
@if(!@aspx)
}
@end
  
var tvar6:boolean = true
function test6(){
  var count = 0
  while (tvar6){count++; if (count>6) tvar6 = false}
  if (count != 7) apLogFailInfo("looped wrong number of times", 7, count, "")
}

///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl07() {

  apInitTest("HardCtrl07");
  var res,x
@if(@aspx)
  HC_test();
@end

  apInitScenario("cond with a boolean in eval in the global scope")
  test1.show()

  apInitScenario("do with a public boolean in a nested funtion")
  test2()

  apInitScenario("for with a boolean in a funtion")
  test3()

  apInitScenario("switch with a boolean in an enum in a class instance")
  x = new test4
  res = x.go(true)
  if (res != 10) apLogFailInfo("wrong branch taken", 10, res, "")  
  res = x.go(false)
  if (res != 5) apLogFailInfo("wrong branch taken", 5, res, "")  
  res = x.go(1)
  if (res != 10) apLogFailInfo("wrong branch taken", 10, res, "")  
  res = x.go(2)
  if (res != 10) apLogFailInfo("wrong branch taken", 10, res, "")  

  apInitScenario("if with a static boolean in a in a class")
@if(!@aspx)
  res = t5.test5.go(true);
  if (res != false) apLogFailInfo("wrong branch taken", false, res, "");
  res = t5.test5.go(false);
  if (res != true) apLogFailInfo("wrong branch taken", true, res, "");
@else
  res = test5.go(true);
  if (res != false) apLogFailInfo("wrong branch taken", false, res, "");
  res = test5.go(false);
  if (res != true) apLogFailInfo("wrong branch taken", true, res, "");
@end

  apInitScenario("for with a boolean in a funtion")
  test6()

  apEndTest();
}


hardctrl07();


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
