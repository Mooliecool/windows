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


var iTestID = 194157;

///////////////////////////////////////////////////////////////////////////////////
var res

@if(!@aspx)
package t1{
@end
  class test1a{
    static internal var i:long;
    static internal var res:Number;
  }
  class test1{
    function go(){
      var x
      
      x = 0
      test1a.i = 4294967296
      test1a.res = test1a.i?test1a.i+1:test1a.i-1
      if (test1a.res != 4294967297) apLogFailInfo(x+": wrong value obtained", 4294967297, test1a.res, "")
      
      x = 1
      test1a.i = 0
      test1a.res = test1a.i?Infinity:test1a.i-1
      if (test1a.res != -1) apLogFailInfo(x+": wrong value obtained", -1, test1a.res, "")
      
      x = 2
      test1a.i = 1
      test1a.i = test1a.i?ulong(0):test1a.i-1
      if (test1a.i != 0) apLogFailInfo(x+": wrong value obtained", 0, test1a.i, "")
      
      x = 3
      test1a.i = 0
      test1a.i = test1a.i?test1a.i-1:9223372036854775807
      if (test1a.i != 9223372036854775807) 
        apLogFailInfo(x+": wrong value obtained", 9223372036854775807, test1a.i, "")
    }
  }
@if(!@aspx)
}
@end

class t2{
  public static var i:long;
}
function test2(){test2a()}
function test2a(){test2b()}
function test2b(){
  var count = 0
  t2.i = -10
  do{ count++ } while (++t2.i)
  if (count != 10) apLogFailInfo("looping failed", 10, count, "");
}
  
var tvar3:long
function test3(){
  var count = 0;
  for(tvar3 = 10; --tvar3; count++);
  if (count != 9) apLogFailInfo("looping failed", 9, count, "");
}

enum t4:long { v1 = -3, v2, v3, v4, v5, v6=5, v7=5}
var result4:Object[] = new Object[20]
var expect4:Object[] = new Object[20]

result4[0] = 0
switch(t4.v1){
  case -3: result4[0] = 1
}
expect4[0] = 1 

result4[1] = 0
switch(t4.v1){
  case t4.v1: result4[1] = 1
}
expect4[1] = 1 

result4[2] = 0
switch(undefined){
  case t4.v4: result4[2] = 1
}
expect4[2] = 0

result4[3] = 0
switch(t4.v6){
  case t4.v7: result4[3] = 1
}
expect4[3] = 1

result4[4] = 0
switch(t4.v4){
  case 3.14: result4[4] = 1
  case 1e-10: result4[4] = 2
  case NaN: result4[4] = 3
  case undefined: result4[4] = 4
  default: result4[4] = 5
}
expect4[4] = 5

function test5(invar:long){return test5a(invar)}
function test5a(invar:long){
  if (invar) return true
  else return false
}

class t6{
  var i:long;
}
class test6{
  static function go(){
    var x = new t6
    var count = 0
    x.i = -10
    with(x){
      while(i) count++, i++;
    }
    if (count != 10) apLogFailInfo("looped wrong number of times", 10, count, "")
  }
}

///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl04() {

    apInitTest("HardCtrl04");
    var res,x


    apInitScenario("conditional using internal int64 in a package class inst")
@if(!@aspx)
    x = new t1.test1
@else
    x = new test1
@end
    x.go()

    apInitScenario("do using public int64 in a nested function")
    test2()

    apInitScenario("do using global int64 in a function")
    test3()

    apInitScenario("switch using global int64 enum")
    for (res=0; res<result4.length; res++){
      if (result4[res] != expect4[res])
        apLogFailInfo(res+ ": wrong branch taken", expect4[res], result4[res], "")
    }

    apInitScenario("if using int64 in a nested function")
    res = test5(long(6))
    if (res!= true) apLogFailInfo("wrong return value", true, res, "")
    res = test5(long(0))
    if (res!= false) apLogFailInfo("wrong return value", false, res, "")

    apInitScenario("while using int64 in a with block in a static method")
    test6.go()

    // regression test for bug 273605
    var n : short = 0;
    var m : ulong = n > 1 ? 0 : ulong(n);
    if (m != 0) apLogFailInfo("wrong value obtained", 0, m, "")

    apEndTest();



}

hardctrl04();


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
