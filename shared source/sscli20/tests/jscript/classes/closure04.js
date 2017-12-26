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


var iTestID = 216449;

//////////////////////////////////////////////////////////////////////////////////////
//

//      			oct 03, 2001		ported over to test method closures as well
//

class err{
  var results:Object[,] = new Object[100,5]
  function add(num, str, exp, act, bug){
    if (results[num,0]) throw("overwrite of results?!?!?")
    results[num,0] = true
    results[num,1] = str
    results[num,2] = exp
    results[num,3] = act
    results[num,4] = bug
  }
  function show(i){
    if (results[i, 0]) 
      apLogFailInfo(results[i,1], results[i,2], results[i,3], results[i,4])
  }
}
var fail = new err
var exception
try{



						//  plain old closure
class cls1{
  function fun1(invar){
    var val = 1
    invar.x = function(){return val}
  }
}
var c1 = new cls1
var o1:Object = {}
var t1 = c1.fun1(o1)
if (o1.x()!= 1) fail.add(1,"wrong result obtained", 1, o1.x(), "")


class cls2{
  static function fun2(invar){
    var val = 2
    invar.x = function foo(){return val}
  }
}
var o2:Object = {}
var t2 = cls2.fun2(o2)
if (o2.x()!= 2) fail.add(2,"wrong result obtained", 2, o2.x(), "")


class cls3{
  expando function fun3(invar){
    var val = 3
    invar.x = function(){return val}
  }
}
var c3 = new cls3
var o3:Object = {}
var t3 = c3.fun3(o3)
if (o3.x()!= 3) fail.add(3,"wrong result obtained", 3, o3.x(), "")





expando class cls4{					// plain closure in an expando class
  function fun4(invar){
    var val = 4
    invar.x = function(){return val}
    this["y"] = function(){return val}
  }
}

var c4 = new cls4
var o4:Object = {}
var t4 = c4.fun4(o4)
if (o4.x()!= 4) fail.add(4,"wrong result obtained", 4, o4.x(), "")
if (c4["y"]()!= 4) fail.add(4,"wrong result obtained", 4, c4["y"](), "")


expando class cls5{
  static function fun5(invar){
    var val = 5
    invar.x = function foo(){return val}
    c5["y"] = function foo(){return val}
  }
}
var c5 = new cls5
var o5:Object = {}
var t5 = cls5.fun5(o5)
if (o5.x()!= 5) fail.add(5,"wrong result obtained", 5, o5.x(), "")
if (c5["y"]()!= 5) fail.add(5,"wrong result obtained", 5, c5["y"], "")


expando class cls6{
  expando function fun6(invar){
    var val = 6
    invar.x = function(){return val}
  }
}
var c6 = new cls6
var o6:Object = {}
var t6 = c6.fun6(o6)
if (o6.x()!= 6) fail.add(6,"wrong result obtained", 6, o6.x(), "")






}catch(e){exception = e}
///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function closure04() {

  apInitTest("closure01");
  var res,x

  if (exception != undefined) 
    apLogFailInfo("exception thrown somewhere in global code!", "", exception, "")


  apInitScenario("closure of a method")
  fail.show(1)

  apInitScenario("closure of a static method")
  fail.show(2)

  apInitScenario("closure of an expando method")
  fail.show(3)


  apInitScenario("closure of a method in an expando class")
  fail.show(4)

  apInitScenario("closure of a static method in an expando class")
  fail.show(5)

  apInitScenario("closure of a expando method in an expando class")
  fail.show(6)

  apEndTest()

}


closure04();


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
