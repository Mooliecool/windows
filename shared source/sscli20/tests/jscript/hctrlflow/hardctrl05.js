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


var iTestID = 194214;

///////////////////////////////////////////////////////////////////////////////////
var res

@if(!@aspx)
package t1a{
@end
  class t1{
    static class sub{
      static var i:float
    }
  }
@if(!@aspx)
}
@end

@if(!@aspx)
package t1b{
@end
  class t1{
    static class sub{
      static function go(){
        var res
        var x = 0
        try{
          t1a.t1.sub.i = float.Epsilon
          res = t1a.t1.sub.i?1:2
          if (res != 1) apLogFailInfo("1: wrong results obtained", 1, res, "")

          t1a.t1.sub.i = -Infinity
          res = t1a.t1.sub.i?1:2
          if (res != 1) apLogFailInfo("2: wrong results obtained", 1, res, "")

          t1a.t1.sub.i = float.MinValue
          res = t1a.t1.sub.i?1:2
          if (res != 1) apLogFailInfo("3: wrong results obtained", 1, res, "")

          t1a.t1.sub.i = NaN
          res = t1a.t1.sub.i?1:2
          if (res != 2) apLogFailInfo("4: wrong results obtained", 2, res, "")

          t1a.t1.sub.i = 3.14
          t1a.t1.sub.i = t1a.t1.sub.i?++t1a.t1.sub.i:"hi"
          if (t1a.t1.sub.i != float(3.14)+1) 
            apLogFailInfo("5: wrong results obtained", float(float(3.14)+1), t1a.t1.sub.i, "")

          with(t1a.t1.sub){
            i = 0
            i = i?true:--i
          }
          if (t1a.t1.sub.i != -1) apLogFailInfo("6: wrong results obtained", -1, t1a.t1.sub.i, "")
        }catch(e){
          apLogFailInfo("unexpected failure group: "+x, "", e.description, "")
        }
      }
    }
  }
@if(!@aspx)
}
@end

expando class t2{
  static var i:float;
}
function test2(){
  t2.i = 10;
  var count = 0
  do{ t2.i--; count++;} while(t2.i)
  if (count != 10) apLogFailInfo("looped wrong number of times", 10, count, "")
}

var tvar3:float = 10
function test3(){test3a()}
function test3a(){
  var count = 0
  eval("for (count = 0; tvar3; tvar3--, count++);")
  if (count!=10) apLogFailInfo("looped wrong number of times", 10, count, "")
}

class test4{
  protected var i:float
  expando function go(invar:float){
    i = invar
    switch (i){
      case -1: return true
      case 3.14: return 4
      case Infinity: return "msbob"
    }
  }
}

@if(!@aspx)
package t5{
@end
  class test5{
    var i:float
    function go(invar:float){
      return eval("i = invar; if (i == 2) true; else false;")
    }
  }
@if(!@aspx)
}
@end

function test6(){test6a()}
function test6a(){
  var i:float = 10, count = 0
  while(i){i--; count++}
  if (count!=10) apLogFailInfo("looped wrong number of times", 10, count, "")
}

///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl05() {

    apInitTest("HardCtrl05");
    var res,x

    apInitScenario("conditional of a single in a nested class")
    //apLogFailInfo("section commented out!!", "", "", "295442")
@if(!@aspx)
    t1b.t1.sub.go();
@else
    t1.sub.go();
@end

    apInitScenario("do of a static single from a function")
    test2()

    apInitScenario("for of a global single from a nested function")
    test3()

    apInitScenario("switch of a protected single from an expando method")
    x = new test4
    res = x.go(-1)
    if (res != true) apLogFailInfo("Wrong return value", true, res, "")
    res = x.go(3.14)
    if (res != 4) apLogFailInfo("Wrong return value", 4, res, "")
    res = x.go(Infinity)
    if (res != "msbob") apLogFailInfo("Wrong return value", "msbob", res, "")

    apInitScenario("if of an internal single from a method")
@if(!@aspx)
    x = new t5.test5;
@else
    x = new test5();
@end
    res = x.go(2)
    if (res != true) apLogFailInfo("Wrong return value", true, res, "")
    res = x.go(Infinity)
    if (res != false) apLogFailInfo("Wrong return value", false, res, "")

    apInitScenario("while of a single from a nested function")
    test6()


    apEndTest();



}


hardctrl05();


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
