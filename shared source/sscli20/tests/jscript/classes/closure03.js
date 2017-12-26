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


var iTestID = 198216;

//////////////////////////////////////////////////////////////////////////////////////
//



///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

var t13
var t14
var t15
var t16

function closure03() {

  apInitTest("closure03");
  var res,x


  try{

  apInitScenario("closure of function in function scope (param)")
  function fun1(invar){
    var val = 1
    invar.x = function(){return val}
  }
  var o1:Object = {}
  var t1 = fun1(o1)
  if (o1.x()!= 1) apLogFailInfo("wrong result obtained", 1, o1.x(), "")


  apInitScenario("closure of function using eval in function scope (param)")
  function fun2(invar){
    var val = 2
    invar.x = eval("(function(){return val})", "unsafe")
  }
  var o2:Object = {}
  var t2 = fun2(o2)
  if (o2.x()!= 2) apLogFailInfo("wrong result obtained", 2, o2.x(), "")


  @if (!@_fast)
  apInitScenario("closure of function (defined in eval) in function scope(param)")
  eval(
  "function fun3(invar){			\n"+
  "  var val = 3				\n"+
  "  invar.x = function(){return val}	\n"+
  "}					\n", "unsafe")
  var o3:Object = {}
  var t3 = fun3(o3)
  if (o3.x()!= 3) apLogFailInfo("wrong result obtained", 3, o3.x(), "")

  apInitScenario("closure (created in eval) of function in function scope(param)")
  function fun4(invar){
    var val = 4
    invar.x = function(){return val}
  }
  var o4:Object = {}
  eval("var t4 = fun4(o4)")
  if (o4.x()!= 4) apLogFailInfo("wrong result obtained", 4, o4.x(), "")

  @else
  
  /**/apInitScenario("closure (created in eval) of function in function scope(param)")
  function fun4(invar){
    var val = 4
    invar.x = function(){return val}
  }
  var o4:Object = {}
  eval("var t4 = fun4(o4) \n"+
     "if (o4.x() != 4) apLogFailInfo('wrong result obtained', 4, o4.x(), '')")
  /**/apInitScenario("skipped")
  
  @end



  apInitScenario("closure of function in function scope (return)")
  function fun5(invar){

    var val = 5
    return function(){return val}
  }
  var t5 = fun5()
  if (t5()!= 5) apLogFailInfo("wrong result obtained", 5, t5(), "")

  apInitScenario("closure of function using eval in function scope (return)")
  function fun6(invar){
    var val = 6
    return eval("(function(){return val})", "unsafe")
  }
  var t6 = fun6()
  if (t6()!= 6) apLogFailInfo("wrong result obtained", 6, t6(), "")


  @if (!@_fast)
  apInitScenario("closure of function (defined in eval) in function scope(return)")
  eval(
  "function fun7(invar){			\n"+
  "  var val = 7				\n"+
  "  return function(){return val}	\n"+
  "}					\n", "unsafe")
  var t7 = fun7()
  if (t7()!= 7) apLogFailInfo("wrong result obtained", 7, t7(), "")

  apInitScenario("closure (created in eval) of function in function scope(return)")
  function fun8(invar){
    var val = 8
    return function(){return val}
  }
  eval("var t8 = fun8()")
  if (t8()!= 8) apLogFailInfo("wrong result obtained", 8, t8(), "")

  @else
  
  /**/apInitScenario("closure (created in eval) of function in function scope(return)")
  function fun8(invar){
    var val = 8
    return function(){return val}
  }
  eval("var t8 = fun8() \n"+
     "if (t8() != 8) apLogFailInfo('wrong result obtained', 8, t8(), '')")
  /**/apInitScenario("skipped")
  
  @end


  apInitScenario("closure of function in function scope (local)")
  var t9
  function fun9(invar){
    var val = 9
    t9 = function(){return val}
  }
  fun9()
  if (t9()!= 9) apLogFailInfo("wrong result obtained", 9, t9(), "")

  apInitScenario("closure of function using eval in function scope (local)")
  var t10
  function fun10(invar){
    var val = 10
    eval("t10 = (function(){return val})", "unsafe")
  }
  fun10()
  if (t10()!= 10) apLogFailInfo("wrong result obtained", 10, t10(), "")


  @if (!@_fast)
  apInitScenario("closure of function (defined in eval) in function scope (local)")
  var t11
  eval(
  "function fun11(invar){			\n"+
  "  var val = 11				\n"+
  "  t11 =  function(){return val}	\n"+
  "}					\n", "unsafe")
  fun11()
  if (t11()!= 11) apLogFailInfo("wrong result obtained", 11, t11(), "")

  apInitScenario("closure (created in eval) of function in function scope (local)")
  var t12
  function fun12(invar){
    var val = 12
    t12 = function(){return val}
  }
  eval("fun12()")
  if (t12()!= 12) apLogFailInfo("wrong result obtained", 12, t12(), "")

  @else
  
  /**/apInitScenario("closure (created in eval) of function in function scope (local)")
  var t12
  function fun12(invar){
    var val = 12
    t12 = function(){return val}
  }
  eval("fun12() \n"+
     "if (t12() != 12) apLogFailInfo('wrong result obtained', 12, t12(), '')")
  /**/apInitScenario("skipped")
  
  @end


  apInitScenario("closure of function in function scope (global)")
  function fun13(invar){
    var val = 13
    t13 = function(){return val}
  }
  fun13()
  if (t13()!= 13) apLogFailInfo("wrong result obtained", 13, t13(), "")

  apInitScenario("closure of function using eval in function scope (global)")
  function fun14(invar){
    var val = 14
    eval("t14 = (function(){return val})", "unsafe")
  }
  fun14()
  if (t14()!= 14) apLogFailInfo("wrong result obtained", 14, t14(), "")


  @if (!@_fast) 
  apInitScenario("closure of function (defined in eval) in function scope(global)")
  eval(
  "function fun15(invar){			\n"+
  "  var val = 15				\n"+
  "  t15 =  function(){return val}	\n"+
  "}					\n", "unsafe")
  fun15()
  if (t15()!= 15) apLogFailInfo("wrong result obtained", 15, t15(), "")

  apInitScenario("closure (created in eval) of function in function scope(global)")
  function fun16(invar){
    var val = 16
    t16 = function(){return val}
  }
  eval("fun16()")
  if (t16()!= 16) apLogFailInfo("wrong result obtained", 12, t12(), "")

  @else
   
  /**/apInitScenario("closure (created in eval) of function in function scope(global)")
  function fun16(invar){
    var val = 16
    t16 = function(){return val}
  }
  eval("fun16() \n"+
     "if (t16() != 16) apLogFailInfo('wrong result obtained', 16, t16(), '')")
  /**/apInitScenario("skipped")
  
  @end


  }catch(e){    
    apLogFailInfo("exception thrown somewhere in global code!", "", e, "")
  }

  apEndTest();

}


closure03();


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
