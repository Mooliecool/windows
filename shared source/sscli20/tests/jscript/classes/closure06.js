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


var iTestID = 227606;

//////////////////////////////////////////////////////////////////////////////
//		create closures from functions with nonstandard definitions
//
//


//
//


function outer1(){
  var a = 10
  var inner1 = function(){return a}
  return inner1
}

function outer2(){
  var a = 11
  var inner2 = eval("function foo(){return a}; foo", "unsafe")
  return inner2
}

@if(!@_fast)
function outer3(){
  var a = 12
  eval("function inner3(){return a}", "unsafe")
  return inner3
}

eval(
" function outer4(){					\n"+
"   var a = 13						\n"+
"   eval('function inner4(){return a}', 'unsafe')	\n"+
"   return inner4					\n"+
" }							\n", "unsafe")


@end

//  call  apply



function closure06() {


  apInitTest("closure06");
  var a, x

  

  apInitScenario("Closure of a function expression")
  var test1 = outer1()
  if (test1() != 10)  apLogFailInfo("Wrong value returned", 10, test1(), "")

  apInitScenario("Closure of a Evaled FuncObj")
  var test2 = outer2()
  if (test2() != 11)  apLogFailInfo("Wrong value returned", 11, test2(), "")

  apInitScenario("Closure of a Evaled FuncObj without passback (fast- only)")
  @if(!@_fast)
  var test3 = outer3()
  if (test3() != 12)  apLogFailInfo("Wrong value returned", 12, test3(), "")
  @end

  apInitScenario("Closure of a Evaled FuncObj inside eval (fast- only)")
  @if(!@_fast)
  var test4 = outer4()
  if (test4() != 13)  apLogFailInfo("Wrong value returned", 13, test4(), "")
  @end


  apEndTest();


}


closure06();


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
