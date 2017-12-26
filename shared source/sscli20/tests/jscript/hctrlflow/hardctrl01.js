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


var iTestID = 193784;

///////////////////////////////////////////////////////////////////////////////////

var result1:Object[] = new Object[20]
var expect1:Object[] = new Object[20]
var x

@if(!@aspx)
 try{
  x = 0
  eval("enum test1e:sbyte {val1, val2, val3=10, val4}; result1[x] = x<1?test1e.val2:test1e.val4; expect1[x]=test1e.val2")

  x = 1
  eval("enum test2e:sbyte {val1, val2, val3=10, val4}; result1[x] = x<1?test2e.val2:Number(test2e.val4);")
  expect1[x] = "11"

  x = 2
  eval(" enum test3e:sbyte {val1, val2, val3=10, val4};  result1[x] = x<1?test3e.val2:double(3.14);")
  expect1[x] = 3.14

  x = 3
  eval(" enum test4e:sbyte {val1, val2, val3=10, val4};  result1[x] = test4e.val2?test4e.val3:2;  expect1[x] = test4e.val3")

  x = 4
  eval(" enum test5e:sbyte {val1, val2, val3=10, val4};  result1[x] = test5e.val2?test5e.val3+.5:'hi'")
  expect1[x] = 10.5

  x = 5
  eval(" enum test6e:sbyte {val1=0, val2, val3=10, val4};  result1[x] = test6e.val1*1?4:test6e")
  expect1[x] = "test6e"

 }catch(e){
  apLogFailInfo("unexpected exception, stage: " + x, "", e.description, "")
 }
@end

function test2(){
    var i:sbyte = -10
    var count = 0

    do{
        count++
        i++
    }while(i)
    if (count != 10) apLogFailInfo("looping failed", 10, count, "");
}




function test3(){
    test3a()
}
function test3a(){
    var i:sbyte = -10
    var count = 0

    for(i=-10; i;){
        count++
        i++
    }
    if (count != 10) apLogFailInfo("looping failed", 10, count, "");
}

@if(!@aspx)
package HC01{
@end
    class foo{
        protected var i:sbyte
        function test4(invar:sbyte){
	    i = invar
            switch(i){
	       case 0: return 5
	       case -1: return 6
	       case 200: return 7
            }
        }
        static internal var j:sbyte
        static function test5(invar:sbyte){
            j = invar
            if (j<0) return j-1
            if (j<200) return ++j
	}
    }
@if(!@aspx)
}
@end

var tvar6:sbyte
class test6{
    function go(){
        var count=0
        eval("tvar6=1;  eval('  while(++tvar6){ count++; if (count>5) break;}  ')  ")
        return tvar6
    }
}


function test7(){
    test7a()
}
function test7a(){
    var i:sbyte = -10
    var count = 0

    while(i){
        count++
        i++
    }
    if (count != 10) apLogFailInfo("looping failed", 10, count, "");
}



///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function hardctrl01() {

    apInitTest("HardCtrl01");
    var res,x

    @if(!@aspx)
    apInitScenario("conditional of sbyte enum in global eval with local vars")
    var i;
    for (i=0; i<result1.length; i++){
      if (result1[i] != expect1[i]){
        apLogFailInfo(" wrong results obtained, stage: "+i, expect1[i], result1[i], "")
        //print(typeof expect1[i] + "  " + typeof result1[i])
      }
    }
    @else
    /**/apInitScenario("conditional of sbyte enum in global eval with local vars (skipped)")
    @end



    apInitScenario("do loop with sbyte in function")
    test2()
        
    apInitScenario("for loop with sbyte in nested function")
    test3()
        
    apInitScenario("switch with sbyte in class instance from package")
@if(@ASPX)
    x = new foo();
@else
    x = new HC01.foo();
@end
    res = x.test4(-1)
    if (res != 6) apLogFailInfo("wrong results", 6, res, "")

    apInitScenario("if test with and internal sbyte from a static function")
@if(@ASPX)
    res = foo.test5(-100)
    if (res != -101) apLogFailInfo("wrong results", -101, res, "")
    res = foo.test5(0)
    if (res != 1) apLogFailInfo("wrong results", 1, res, "")
@else
    res = HC01.foo.test5(-100)
    if (res != -101) apLogFailInfo("wrong results", -101, res, "")
    res = HC01.foo.test5(0)
    if (res != 1) apLogFailInfo("wrong results", 1, res, "")
@end

    apInitScenario("break of a loop using an sbyte in a nested eval in a class instance")
    x = new test6;
    res = x.go()
    if (res != 7) apLogFailInfo("wrong results", 7, res, "")

    apInitScenario("while loop with sbyte in nested function")
    test7()


    apEndTest();



}


hardctrl01();


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
