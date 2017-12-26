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


var iTestID = 222978;

//////////////////////////////////////////////////////////////////////////////////////
//

//
//
//      purpose checks the different ways of exiting out of a finally block      			
//
//////////////////////////////////////////////////////////////////////////////////////





function foo(){
  var i = {a:123}
  try{
    var x
    x.go()
  }catch(e){
    return i
  }finally{
    i.b = 321
    return i
  }
}

function foo2(){
  var i = {a:123}
  var bad = {a:666}
  try{
    var x
    x.go()
  }catch(e){
    return bad
    i.a = 789
  }finally{
    i.b = 321
    return i
  }
}

function bar(){
  var i = {a:123}
  try{
    var x
    x.go()
  }catch(e){
    x.go()
  }finally{
    i.b = 321
    return i
  }
}

function foobar(){
  var i = 123
  try{
    var x
    x.go()
  }catch(e){
    return i
  }finally{
    var b = 321
    return b
  }
}



function finally02() {

  apInitTest("finally02");

  var i

  apInitScenario("Exit a finally using return statments")
  i = foo()
  if (i.a != 123) apLogFailInfo("incorrect value returned", 123, i.a, "")
  if (i.b != 321) apLogFailInfo("incorrect value returned", 321, i.b, "")

  i = foo2()
  if (i.a != 123) apLogFailInfo("incorrect value returned", 123, i.a, "")
  if (i.b != 321) apLogFailInfo("incorrect value returned", 321, i.b, "")

  try{
    i = bar()
  }catch(e){apLogFailInfo("runtime through an exception", "", e, "")}
  if (i.a != 123) apLogFailInfo("incorrect value returned", 123, i.a, "")
  if (i.b != 321) apLogFailInfo("incorrect value returned", 321, i.b, "")

  var x = foobar()
  if (x != 321) apLogFailInfo("incorrect value returned", 321, x, "")


  apEndTest()
}


finally02();


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
