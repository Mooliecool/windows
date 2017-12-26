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


var iTestID = 212515;

//////////////////////////////////////////////////////////////////////////////
//		This testcase tests the pri 2 scenario of method return types
//
//

//
//
//

@if(!@aspx)
	import System
@end

package pkg{
  class foo{var bar=21}
}
function fun1(){}
function fun2():int{return 5}
function outer(){
  var x = 50
  function inner(){ return x }
  return inner
}
function outer2(){
  var x = 50
  function inner2(){ return x }
  return inner2
}


class cls{
  function fun3(){}
  function fun4():int{return 6}
  function outer(){
    var x = 60
    function inner(){ return x }
    return inner
  }

  function f1(){return pkg}
  function f2(){return fun1}
  function f3(){return fun2}
  function f4(){return outer}
  function f5(){return outer()}
  function f6(){var x = new Object(pkg); return x}
  function f7(){var x = new Object(fun1); return x}
  function f8(){var x = new Object(fun2); return x}
  function f9(){var x = new Object(outer); return x}
  function f10(){var x = new Object(outer()); return x}
  function f11(){return outer2}
  function f12(){return outer2()}
  function f13(){var x = new Object(outer2); return x}
  function f14(){var x = new Object(outer2()); return x}

  function f15(){return 15}

  function f16():byte{return 16}
  function f17():ushort{return 17000}
  function f18():int{return -18000000}
  function f19():long{return -1125899906842624}
  function f20():ulong{return 13835058055282163712}
  function f21():double{return 1.5}
  function f22():decimal{return 1.0000000000001}
  function f23():String{return 12345}

  function f24():sbyte{return "16"}
  function f25():ushort{return "17000"}
  function f26():int{return "-18000000"}
  function f27():long{return "-1125899906842624"}
  function f28():ulong{return "13835058055282163712"}
  function f29():double{return "1.5"}
  function f30():decimal{return "1.0000000000001"}
  function f31():String{return "12345"}

  function f32():byte{return new Number(16)}
  function f33():ushort{return new Number(17000)}
  function f34():int{return new Number(-18000000)}
  function f35():long{return new Number(-1125899906842624)}
  function f36():ulong{return new Number(13835058055282163712)}
  function f37():double{return new Number(1.5)}
  //function f38():decimal{return new Number(1.0000000000001)}  // TODOPGM
  function f39():String{return new Number(12345)}

  function f40():byte{return long(16)}
  function f41():ushort{return long(17000)}
  function f42():int{return long(-18000000)}
  function f43():long{return long(-1125899906842624)}
  function f44():ulong{return long(11258999068426246)}
  function f45():double{return long(15)}
  function f46():decimal{return long(314)}
  function f47():String{return long(12345)}

  function f48():byte{return 16}
  function f49():ushort{return 17000}
  function f50():int{return -18000000}
  function f51():long{return -1125899906842624}
  function f52():ulong{return 13835058055282163712}
}




function methret01() {


  var i

  apInitTest("MethRet01");
  var test:cls = new cls

  apInitScenario("Ensure a method can return a package");
  if (test.f1() != "Microsoft.JScript.Namespace") apLogFailInfo("package not returned correctly", "", "", "")
  
  apInitScenario("Ensure a method can return a void function");
  if (test.f2().GetType() != "Microsoft.JScript.Closure") 
    apLogFailInfo("function not returned correctly", "", test.f2().GetType(), "")
  if (test.f2()() != undefined) apLogFailInfo("function not returned correctly", "", "", "")

  apInitScenario("Ensure a method can return a typed return function");
  if (test.f3()() != 5) apLogFailInfo("function not returned correctly", 5, test.f3()(), "")

  apInitScenario("Ensure a method can return a function which creates a closure - postponed");
  //if (test.f4()()() != 60) apLogFailInfo("closure not returned correctly", 60, test.f4()()(), "")

  apInitScenario("Ensure a method can return a closure");
  if (test.f5()() != 60) apLogFailInfo("closure not returned correctly", 60, test.f5()(), "")



  apInitScenario("Ensure a method can return a package object - postponed");
  //if (test.f6() != "Microsoft.JScript.Namespace") apLogFailInfo("package not returned correctly", "", "", "")
  
  apInitScenario("Ensure a method can return a void function object");
  if (test.f7().GetType() != "Microsoft.JScript.Closure") 
    apLogFailInfo("function not returned correctly", "", test.f7().GetType(), "")
  if (test.f7()() != undefined) apLogFailInfo("function not returned correctly", "", test.f7()(), "")

  apInitScenario("Ensure a method can return a typed return function object");
  if (test.f8()() != 5) apLogFailInfo("function not returned correctly", 5, test.f8()(), "")

  apInitScenario("Ensure a method can return a function which creates closure (O) - postponed");
  //if (test.f9()()() != 60) apLogFailInfo("closure not returned correctly", 60, test.f9()()(), "")

  apInitScenario("Ensure a method can return a closure object");
  if (test.f10()() != 60) apLogFailInfo("closure not returned correctly", 60, test.f10()(), "")



  apInitScenario("Ensure method can return a function which creates closure (OG)");
  if (test.f11()()() != 50) apLogFailInfo("closure not returned correctly", 50, test.f11()()(), "")

  apInitScenario("Ensure method can return a closure object (Global)");
  if (test.f12()() != 50) apLogFailInfo("closure not returned correctly", 50, test.f12()(), "")

  apInitScenario("Ensure method can return a function which creates closure (OG)");
  if (test.f13()()() != 50) apLogFailInfo("closure not returned correctly", 50, test.f13()()(), "")

  apInitScenario("Ensure method can return a closure object (Global)");
  if (test.f14()() != 50) apLogFailInfo("closure not returned correctly", 50, test.f14()(), "")



  apInitScenario("return a literal through a byte");
  if (test.f16() != 16) apLogFailInfo("wrong results", 16, test.f16(), "")
  
  apInitScenario("return a literal through a ushort");
  if (test.f17() != 17000) apLogFailInfo("wrong results", 17000, test.f17(), "")

  apInitScenario("return a literal through a int");
  if (test.f18() != -18000000) apLogFailInfo("wrong results", -18000000, test.f18(), "")

  apInitScenario("return a literal through a long");
  if (test.f19() != -1125899906842624) apLogFailInfo("wrong results", -1125899906842624, test.f19(), "")

  apInitScenario("return a literal through a ulong");
  if (test.f20() != 13835058055282163712) apLogFailInfo("wrong results", 13835058055282163712, test.f20(), "")

  apInitScenario("return a literal through a double");
  if (test.f21() != 1.5) apLogFailInfo("wrong results", 1.5, test.f21(), "")

  apInitScenario("return a literal through a decimal");
  if (test.f22() != 1.0000000000001) apLogFailInfo("wrong results", 1.0000000000001, test.f22(), "")

  apInitScenario("return a literal through a string");
  if (test.f23() != 12345) apLogFailInfo("wrong results", 12345, test.f23(), "")



  apInitScenario("return a string through a byte");
  if (test.f24() != 16) apLogFailInfo("wrong results", 16, test.f24(), "")
  
  apInitScenario("return a string through a ushort");
  if (test.f25() != 17000) apLogFailInfo("wrong results", 17000, test.f25(), "")

  apInitScenario("return a string through a int");
  if (test.f26() != -18000000) apLogFailInfo("wrong results", -18000000, test.f26(), "")

  apInitScenario("return a string through a long");
  if (test.f27() != -1125899906842624) apLogFailInfo("wrong results", -1125899906842624, test.f27(), "")

  apInitScenario("return a string through a ulong");
  if (test.f28() != 13835058055282163712) apLogFailInfo("wrong results", 13835058055282163712, test.f28(), "")

  apInitScenario("return a string through a double");
  if (test.f29() != 1.5) apLogFailInfo("wrong results", 1.5, test.f29(), "")

  apInitScenario("return a string through a decimal");
  if (test.f30() != 1.0000000000001) apLogFailInfo("wrong results", 1.0000000000001, test.f30(), "")

  apInitScenario("return a string through a string");
  if (test.f31() != 12345) apLogFailInfo("wrong results", 12345, test.f31(), "")



  apInitScenario("return a number object through a byte");
  if (test.f32() != 16) apLogFailInfo("wrong results", 16, test.f32(), "")
  
  apInitScenario("return a number object through a ushort");
  if (test.f33() != 17000) apLogFailInfo("wrong results", 17000, test.f33(), "")

  apInitScenario("return a number object through a int");
  if (test.f34() != -18000000) apLogFailInfo("wrong results", -18000000, test.f34(), "")

  apInitScenario("return a number object through a long");
  if (test.f35() != -1125899906842624) apLogFailInfo("wrong results", -1125899906842624, test.f35(), "")

  apInitScenario("return a number object through a ulong");
  if (test.f36() != 13835058055282163712) apLogFailInfo("wrong results", 13835058055282163712, test.f36(), "")

  apInitScenario("return a number object through a double");
  if (test.f37() != 1.5) apLogFailInfo("wrong results", 1.5, test.f37(), "")

  apInitScenario("return a number object through a decimal - posponed");

  apInitScenario("return a number object through a string");
  if (test.f39() != 12345) apLogFailInfo("wrong results", 12345, test.f39(), "")



  apInitScenario("return a long object through a byte");
  if (test.f40() != 16) apLogFailInfo("wrong results", 16, test.f40(), "")
  
  apInitScenario("return a long object through a ushort");
  if (test.f41() != 17000) apLogFailInfo("wrong results", 17000, test.f41(), "")

  apInitScenario("return a long object through a int");
  if (test.f42() != -18000000) apLogFailInfo("wrong results", -18000000, test.f42(), "")

  apInitScenario("return a long object through a long");
  if (test.f43() != -1125899906842624) apLogFailInfo("wrong results", -1125899906842624, test.f43(), "")

  apInitScenario("return a long object through a ulong");
  if (test.f44() != ulong(11258999068426246)) apLogFailInfo("wrong results", 11258999068426246, test.f44(), "")

  apInitScenario("return a long object through a double");
  if (test.f45() != 15) apLogFailInfo("wrong results", 15, test.f45(), "")

  apInitScenario("return a long object through a decimal");
  if (test.f46() != 314) apLogFailInfo("wrong results", 314, test.f46(), "")

  apInitScenario("return a long object through a string");
  if (test.f47() != 12345) apLogFailInfo("wrong results", 12345, test.f47(), "")




  apEndTest();


}



methret01();


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
