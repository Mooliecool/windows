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


var iTestID = 215512;

//////////////////////////////////////////////////////////////////////////////
//		This testcase tests the pri 2 scenario of method return types
//
//


//
//

@if(!@aspx)
	import System
@end

package p1{
  class cls1{
    function f1(){return {a:1, b:2}}
    function f2():byte{return decimal(123)}
    function f3():long{return eval("123456789")}
  }
}

eval(
"  class cls2{"+
"    function f1(){return {a:1, b:2}};"+
"    function f2():byte{return decimal(123)};"+
"    function f3():long{return eval('123456789')};"+
"  }", "unsafe")

class cls3{
  static function f1(){return "foo"}
  static function f2(){ var a = new Object; a.b = 4; return a}
  static function f3():char{return "h"}
}

class cls4{
  expando function f1():byte{return -0}
  expando function f2():int{this.x = 4;  return 5}
  expando function f3():ulong{this.me = new Object(true); return 999999999999}
}


class cls5{
  static class sub1{

    static class sub2{
      static function f2(){
        var x = new (new sub2).sub3
        x["expand"] = new x.sub4
        return x["expand"].test()
      }

      expando class sub3{
        class sub4{
          function test():String{return ulong(-1)}
        }
      }
    }

  }
}


expando class cls6{
  function cls6(){  this[1] = new sub1  }

  class sub1{
    var sub
    function sub1(){  this.sub = new sub2  }
  }

  expando class sub2{
    function sub2(){  this[3] = new sub3  }
  }

  static class sub3{
    var fin
    function sub3(){  this.fin = "cool"    }
  }
}


//var x = new cls6
//print(x[1].sub[3].fin)



function methret02() {


  apInitTest("MethRet02");

  apInitScenario("1 return of a method in a package");
  var x = new p1.cls1
  if (x.f1().b != 2) apLogFailInfo("wrong return value", 2, x.f1().b, "")
  if (x.f2() != 123) apLogFailInfo("wrong return value", 123, x.f2(), "")
  if (x.f3() != 123456789) apLogFailInfo("wrong return value", 123456789, x.f3(), "")
  
  apInitScenario("2 return of a method in a class defined in eval");
  @if(!@_fast)
    var x = new cls2
    if (x.f1().b != 2) apLogFailInfo("wrong return value", 2, x.f1().b, "")
    if (x.f2() != 123) apLogFailInfo("wrong return value", 123, x.f2(), "")
    if (x.f3() != 123456789) apLogFailInfo("wrong return value", 123456789, x.f3(), "")
  @end

  apInitScenario("3 in an eval return of a method in a package");
  eval(
    'var x = new p1.cls1;'+
    'if (x.f1().b != 2) apLogFailInfo("wrong return value", 2, x.f1().b, "");'+
    'if (x.f2() != 123) apLogFailInfo("wrong return value", 123, x.f2(), "");'+
    'if (x.f3() != 123456789) apLogFailInfo("wrong return value", 123456789, x.f3(), "");')

  apInitScenario("4 in an eval return of a method in a class defined in eval");
  @if(!@_fast)
  eval(
    'var x4:cls2 = new cls2;'+
    'if (x4.f1().b != 2) apLogFailInfo("wrong return value", 2, x4.f1().b, "");'+
    'if (x4.f2() != 123) apLogFailInfo("wrong return value", 123, x4.f2(), "");'+
    'if (x4.f3() != 123456789) apLogFailInfo("wrong return value", 123456789, x4.f3(), "");')
  @end

  apInitScenario("5 return value of a static method")
  if (cls3.f1() != "foo") apLogFailInfo("wrong return value", "foo", cls3.f1(), "")
  if (cls3.f2().b != 4) apLogFailInfo("wrong return value", 4, cls3.f2(), "")
  if (cls3.f3() != "h") apLogFailInfo("wrong return value", "h", cls3.f3(), "")

  apInitScenario("6 in an eval return value of a static method")
  eval(
    'if (cls3.f1() != "foo") apLogFailInfo("wrong return value", "foo", cls3.f1(), "");'+
    'if (cls3.f2().b != 4) apLogFailInfo("wrong return value", 4, cls3.f2(), "");'+
    'if (cls3.f3() != "h") apLogFailInfo("wrong return value", "h", cls3.f3(), "");')

  apInitScenario("7 return value of an expando method")
  x = new cls4
  if (x.f1() != 0) apLogFailInfo("wrong return value", 0, x.f1(), "")
  if (x.f2() != 5) apLogFailInfo("wrong return value", 5, x.f2(), "")
  if ((new x.f2()).x != 4) apLogFailInfo("wrong return value", 4, (new x.f2()).x, "")
  if (x.f3() != long(999999999999)) apLogFailInfo("wrong return value", long(999999999999), x.f3(), "")
  if ((new x.f3()).me != true) apLogFailInfo("wrong return value", true, (new x.f3()).me, "")

  apInitScenario("8 nested methods in static and expando classes")
  if (cls5.sub1.sub2.f2() != ulong(18446744073709551615)) apLogFailInfo("wrong return value", -1, cls5.sub1.sub2.f2(), "")


  apEndTest();


}


methret02();


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
