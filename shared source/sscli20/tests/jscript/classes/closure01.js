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


var iTestID = 198190;

//////////////////////////////////////////////////////////////////////////////////////
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

function fun1(invar){
  var val = 1
  invar.x = function(){return val}
}
var o1:Object = {}
var t1 = fun1(o1)
if (o1.x()!= 1) fail.add(1,"wrong result obtained", 1, o1.x(), "")


function fun2(invar){
  var val = 2
  invar.x = eval("(function(){return val})","unsafe")
}
var o2:Object = {}
var t2 = fun2(o2)
if (o2.x()!= 2) fail.add(2,"wrong result obtained", 2, o2.x(), "")


@if (!@_fast)
eval(
"function fun3(invar){			\n"+
"  var val = 3				\n"+
"  invar.x = function(){return val}	\n"+
"}					\n", "unsafe")
var o3:Object = {}
var t3 = fun3(o3)
if (o3.x()!= 3) fail.add(3,"wrong result obtained", 3, o3.x(), "")

function fun4(invar){
  var val = 4
  invar.x = function(){return val}
}
var o4:Object = {}
eval("var t4 = fun4(o4)")
if (o4.x()!= 4) fail.add(4,"wrong result obtained", 4, o4.x(), "")

@else

function fun4(invar){
  var val = 4
  invar.x = function(){return val}
}
var o4:Object = {}
eval("var t4 = fun4(o4) \n"+
     "if (o4.x() != 4) fail.add(4,'wrong result obtained', 4, o4.x(), '')")

@end



function fun5(invar){
  var val = 5
  return function(){return val}
}
var t5 = fun5()
if (t5()!= 5) fail.add(5,"wrong result obtained", 5, t5(), "")

function fun6(invar){
  var val = 6
  return eval("(function(){return val})","unsafe")
}
var t6 = fun6()
if (t6()!= 6) fail.add(6,"wrong result obtained", 6, t6(), "")


@if (!@_fast)
eval(
"function fun7(invar){			\n"+
"  var val = 7				\n"+
"  return function(){return val}	\n"+
"}					\n", "unsafe")
var t7 = fun7()
if (t7()!= 7) fail.add(7,"wrong result obtained", 7, t7(), "")

function fun8(invar){
  var val = 8
  return function(){return val}
}
eval("var t8 = fun8()")
if (t8()!= 8) fail.add(8,"wrong result obtained", 8, t8(), "")

@else

function fun8(invar){
  var val = 8
  return function(){return val}
}
eval("var t8 = fun8() \n"+
     "if (t8() != 8) fail.add(8,'wrong result obtained', 8, t8(), '')")

@end


var t9
function fun9(invar){
  var val = 9
  t9 = function(){return val}
}
fun9()
if (t9()!= 9) fail.add(9,"wrong result obtained", 9, t9(), "")

var t10
function fun10(invar){
  var val = 10
  eval("t10 = (function(){return val})", "unsafe")
}
fun10()
if (t10()!= 10) fail.add(10,"wrong result obtained", 10, t10(), "")


@if (!@_fast)
var t11
eval(
"function fun11(invar){			\n"+
"  var val = 11				\n"+
"  t11 =  function(){return val}	\n"+
"}					\n", "unsafe")
fun11()
if (t11()!= 11) fail.add(11,"wrong result obtained", 11, t11(), "")

var t12
function fun12(invar){
  var val = 12
  t12 = function(){return val}
}
eval("fun12()")
if (t12()!= 12) fail.add(12,"wrong result obtained", 12, t12(), "")

@else

var t12
function fun12(invar){
  var val = 12
  t12 = function(){return val}
}
eval("fun12() \n"+
     "if (t12() != 12) fail.add(12,'wrong result obtained', 12, t12(), '')")

@end



}catch(e){exception = e}
///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function closure01() {

  apInitTest("closure01");
  var res,x

  if (exception != undefined) 
    apLogFailInfo("exception thrown somewhere in global code!", "", exception, "")

  apInitScenario("closure of function in global scope (param)")
  fail.show(1)

  apInitScenario("closure of function using eval in global scope (param)")
  fail.show(2)

  apInitScenario("closure of function (defined in eval) in global scope (param)")
  fail.show(3)

  apInitScenario("closure (created in eval) of function in global scope (param)")
  fail.show(4)

  apInitScenario("closure of function in global scope (return)")
  fail.show(5)

  apInitScenario("closure of function using eval in global scope (return)")
  fail.show(6)

  apInitScenario("closure of function (defined in eval) in global scope (return)")
  fail.show(7)

  apInitScenario("closure (created in eval) of function in global scope (return)")
  fail.show(8)

  apInitScenario("closure of function in global scope (global)")
  fail.show(9)

  apInitScenario("closure of function using eval in global scope (global)")
  fail.show(10)

  apInitScenario("closure of function (defined in eval) in global scope (global)")
  fail.show(11)

  apInitScenario("closure (created in eval) of function in global scope (global)")
  fail.show(12)

  apEndTest();

}


closure01();


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
