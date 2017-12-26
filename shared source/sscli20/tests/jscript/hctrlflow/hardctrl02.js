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


var iTestID = 193984;

///////////////////////////////////////////////////////////////////////////////////
var res

class test1{
  static var tvar1:short = 5
  static function go(){

    try{
    var x = 0
    eval("res = x<1?tvar1:tvar1+1")
    if (res != 5) apLogFailInfo(x+": wrong value obtained", 5, res, "")

    var x = 1
    eval("res = x<1?tvar1:tvar1+1")
    if (res != 6) apLogFailInfo(x+": wrong value obtained", 6, res, "")

    var x = 2
    eval("res = x<1?tvar1:double(3.14)")
    if (res != 3.14) apLogFailInfo(x+": wrong value obtained", 3.14, res, "")

    var x = 3
    eval("x<1?tvar1:double(3.14)")

    var x = 4
    //eval("tvar1 = x<5?tvar1+1:100")
    if (tvar1 != 5) apLogFailInfo(x+": wrong value obtained", 5, tvar1, "")


    }catch(e){
      apLogFailInfo("unexpected exception, stage: " + x, e.description, "", "")
    }
    }
}

@if(@aspx)
    enum tvar2:short{val1=-3, val2, val3, val4=400, val5=-3}
@else
package t2{
    enum tvar2:short{val1=-3, val2, val3, val4=400, val5=-3}
}
import t2
@end

function test2(){
    var count = 0
    var i:tvar2 = tvar2.val1
    do{
        count++
        i++
        if (count>20) break
    }while(i)
    if (count != 3) apLogFailInfo("looping failed", 7, count, "");
}

var count3 = 0
for (var tvar3:short = 6; tvar3>=0; --tvar3) count3++

function test4(invar:short){return test4a(invar)}
function test4a(invar:short){
    switch(invar){
        case 0: return 5
        case -1: return 6
        case 200: return 7
    }
}


class test5{
  function go(invar:short){
    return eval("if (invar<20) invar-1; else invar*2")
  }
}


class test6{
  public var i:short
  function go(){
    var count = 0
    i = -10
    while (++i) count++
    if (count != 9) apLogFailInfo("looping failed", 9, count, "");
  }
}


///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl02() {

    apInitTest("HardCtrl02");
    var res,x

    apInitScenario("conditional using static int16 in an eval in a static function")
    test1.go()

    apInitScenario("do using an int16 enum in a function")
    test2()

    apInitScenario("for using an int16 global scope")
    if (count3 != 7) apLogFailInfo("looping failed", 7, count3, "");

    apInitScenario("switch using an int16 in a nested function")
    if (test4(-1) !=6) apLogFailInfo("wrong value obtained", 6, test4(-1), "")

    apInitScenario("if using an int16 in an eval within a class instance")
    x = new test5
    if (x.go(22) != 44) apLogFailInfo("wrong value obtained", 44, x.go(22), "")
    if (x.go(10) != 9) apLogFailInfo("wrong value obtained", 9, x.go(10), "")

    apInitScenario("while using a public int16 from in a class method")
    x = new test6
    x.go()

    apEndTest();



}

hardctrl02();


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
