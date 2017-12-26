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


var iTestID = 194083;

///////////////////////////////////////////////////////////////////////////////////
var res

var tvar1:int = 35000
function test1(){test1a()}
function test1a(){test1b()}
function test1b(){test1c()}
function test1c(){
    try{
    var x = 0
    eval("res = x<1?tvar1:tvar1+1")
    if (res != 35000) apLogFailInfo(x+": wrong value obtained", 35000, res, "")

    var x = 1
    eval("res = x<1?tvar1:tvar1+1")
    if (res != 35001) apLogFailInfo(x+": wrong value obtained", 35001, res, "")

    var x = 2
    eval("res = x<1?tvar1:double(3.14)")
    if (res != 3.14) apLogFailInfo(x+": wrong value obtained", 3.14, res, "")

    var x = 3
    eval("x<1?tvar1:double(3.14)")

    var x = 4
    eval("tvar1 = x<5?tvar1+1:100")
    if (tvar1 != 35001) apLogFailInfo(x+": wrong value obtained", 35001, tvar1, "")


    }catch(e){
      apLogFailInfo("unexpected exception, stage: " + x, e.description, "", "")
    }
}

@if(!@aspx)
package t2{
@end
class test2{
  internal var i:int
  function go(){
    var count = 0
    i = -10
    do{ count++ } while (++i)
    if (count != 10) apLogFailInfo("looping failed", 10, count, "");
  }
}
@if(!@aspx)
}
@end

function test3(){test3a()}
function test3a(){
    var count = 0
    
    for(var i:int=-2; 
        i<10;
        count++, i++);
    if (count != 12) apLogFailInfo("looping failed", 12, count, "");
}

@if(!@aspx)
package t4{
@end
class varstor{
  static internal var i:int
}

class test4{
  static function go(invar:int){
    varstor.i = invar
    switch(varstor.i){
      case -100000: return -1
      case 100000: return 1
      case 0: return 45
    }
  }
}
@if(!@aspx)
}
@end

function test5(invar:int){return test5a(invar)}
function test5a(invar:int){
	if (invar) return true
	else return false
}




@if(!@aspx)
package t6{
@end
class test{
  static public var j:int
}

class test6{
  function go(){
    var count 
    
    count = 0;
    test.j=-10;   
    eval("while(test.j) count++, test.j++;")
    if (count != 10) apLogFailInfo("looping failed", 10, count, "295442");
  }
}
@if(!@aspx)
}
@end

///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl03() {

    apInitTest("HardCtrl03");
    var res,x

    apInitScenario("conditional using global int32 a nested function")
    test1()

    apInitScenario("do using internal int32 in a class instance")
@if(@aspx)
    x = new test2
@else
    x = new t2.test2
@end
    x.go

    apInitScenario("for using int32 in a nested function")
    test3()

    apInitScenario("switch using internal int32 in a static class")
@if(@aspx)
    res = test4.go(-100000)
    if (res != -1) apLogFailInfo("wrong value obtained", -1, res, "")
    res = test4.go(0)
    if (res != 45) apLogFailInfo("wrong value obtained", 45, res, "")
@else
    res = t4.test4.go(-100000)
    if (res != -1) apLogFailInfo("wrong value obtained", -1, res, "")
    res = t4.test4.go(0)
    if (res != 45) apLogFailInfo("wrong value obtained", 45, res, "")
@end

    apInitScenario("if using int32 in a nested function")
    res = test5(-10000)
    if (res != true) apLogFailInfo("wrong value obtained", true, res, "")
    res = test5(0)
    if (res != false) apLogFailInfo("wrong value obtained", false, res, "")

    apInitScenario("while using a private int32 in a eval in a class inst")
@if(@aspx)
    x = new test6
@else
    x = new t6.test6
@end
    x.go()

    apEndTest();
}



hardctrl03();


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
