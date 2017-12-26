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


var iTestID = 226880;

//////////////////////////////////////////////////////////////////////////////
//		This testcase tests the usage of attributes
//
//


//
//

@if(!@aspx)
	import System
@end




public AttributeUsage(System.AttributeTargets.Class)
class a1 extends Attribute{
  var f : int
  function foo(){}
}
a1(f = 5) class cls1{}


public AttributeUsage(System.AttributeTargets.Class)
class a2 extends Attribute{
  var _f : int
  function set f(f: int){
    _f = f;
  }
}
a2(f = 6) class cls2{}


enum numEnum{one, two, three}
public AttributeUsage(System.AttributeTargets.Class)
class a3 extends Attribute{
  var f : numEnum
  function a3(f : numEnum){this.f = f}
}
a3(numEnum.two) class cls3{}


public AttributeUsage(System.AttributeTargets.Property|System.AttributeTargets.Method)
class a4 extends Attribute{}
class cls4{
  a4 function set g(value){}
}


class foo{
  Obsolete var bar
  Obsolete("use bar not bas") const bas = "123"
}


public AttributeUsage(System.AttributeTargets.Class)
class a5 extends Attribute{
  var _a : String[]
  function set a(_a : String[]){this._a = _a}
}
a5(a = ["it", "works"]) class cls5{}


public AttributeUsage(System.AttributeTargets.Class)
class a6 extends Attribute{
  var a : String[]
}
a6(a = ["it", "works"]) class cls6{}


public AttributeUsage(System.AttributeTargets.Class)
class a7 extends Attribute{
  var a : String[]
  function a7(a : String[]){this.a = a}
}
a7(["it", "works!"]) class cls7{}


enum a8enum{fee, foo, fum}
public AttributeUsage(System.AttributeTargets.Class)
class a8 extends Attribute{
  var f : a8enum
  function a8(){}
}

a8(f = a8enum.fum) class cls8{}


public AttributeUsage(System.AttributeTargets.Class)
class a9 extends Attribute{
  var _f : a8enum
  function set f(f: a8enum){
    _f = f;
  }
}
a9(f = a8enum.fum) class cls9{}


public AttributeUsage(System.AttributeTargets.Class, true)
class a10 extends Attribute{
  var x : int
}


public AttributeUsage(System.AttributeTargets.All)
class a11 extends Attribute{
  var x : int
}


public AttributeUsage
class a12 extends Attribute{
}


public AttributeUsage(System.AttributeTargets.Constructor)
class a13 extends Attribute{
}
class cls13{
  a13 function cls13(){}
}


public AttributeUsage(System.AttributeTargets.Property|System.AttributeTargets.Method)
class a14 extends Attribute{
}
class cls14{
  a14 function f(){}
  a14 function get g(){}
}


public AttributeUsage(System.AttributeTargets.All, AllowMultiple=false)
class a15 extends Attribute{
}
a15 class cls15{}
public AttributeUsage(System.AttributeTargets.All, AllowMultiple=true)
class a16 extends Attribute{
}
a16 a16 class cls16{}


class cls17{
  Obsolete function bar(){}
}




function custattr03() {


  var res

  apInitTest("CustAttr03");

// TODOPGM  postponed: 310447
  apInitScenario("add an attibute to a class and extract it at runtime");
  
  res = a1(cls1.GetCustomAttributes(false)[0]).f
  if (res != 5) apLogFailInfo("wrong result obtained", 5, res, "")

  res = a2(cls2.GetCustomAttributes(false)[0])._f
  if (res != 6) apLogFailInfo("wrong result obtained", 6, res, "")

  res = a3(cls3.GetCustomAttributes(false)[0]).f
  if (res != "two") apLogFailInfo("wrong result obtained", "two", res, "")
  



  apInitScenario("add an attibute to a property and extract it at runtime");
  
  res = cls4.GetProperty("g").GetCustomAttributes(false)[0]
  if (res != "a4") apLogFailInfo("wrong result obtained", "a4", res, "")

  res = cls4.GetProperty("g").GetSetMethod().GetCustomAttributes(false).Length
  if (res != 0) apLogFailInfo("wrong result obtained", 0, res, "")
  



  apInitScenario("Test the persistance of the Obsolete attribute");
  
  res = foo.GetField("bar").GetCustomAttributes(false)[0]
  if (res != "System.ObsoleteAttribute") apLogFailInfo("wrong result obtained", "System.ObsoleteAttribute", res, "")

  res = foo.GetField("bas").GetCustomAttributes(false)[0].Message
  if (res != "use bar not bas") apLogFailInfo("wrong result obtained", "use bar not bas", res, "")
  



  apInitScenario("test the usage of properties on an attribute class");
  
  res = String(cls5.GetCustomAttributes(false)[0]._a)
  if (String(res) != "it,works") apLogFailInfo("wrong result obtained", "it,works", res, "")
  



  apInitScenario("test the usage of Array passing to attribute constructors");
  
  res = String(cls6.GetCustomAttributes(false)[0].a)
  if (String(res) != "it,works") apLogFailInfo("wrong result obtained", "it,works", res, "")
  


  apInitScenario("test the usage assigning arrays to named params of attrib");
  
  res = String(cls7.GetCustomAttributes(false)[0].a)
  if (String(res) != "it,works!") apLogFailInfo("wrong result obtained", "it,works!", res, "")
  


  apInitScenario("test the usage of enums as attrib constructor params");
  
  res = String(a8(cls8.GetCustomAttributes(false)[0]).f)
  if (String(res) != "fum") apLogFailInfo("wrong result obtained", "fum", res, "")
  


  apInitScenario("test the usage of enum property as named attrib param");
  
  res = String(a9(cls9.GetCustomAttributes(false)[0])._f)
  if (String(res) != "fum") apLogFailInfo("wrong result obtained", "fum", res, "")
  

  apInitScenario("test that variables in an attrib are properly typed");
  
  try{
    eval("a11(x = 5.5) var bar11")
    apLogFailInfo("Exception not thrown when expected", "", "", "")
  }catch(e){}
  

  apInitScenario("test the restriction policy of a custom attribute");
  
  res = String(cls13.GetConstructors()[0].GetCustomAttributes(true)[0])
  if (String(res) != "a13") apLogFailInfo("wrong result obtained", "a13", res, "")

  res = String(cls14.GetMethod("f").GetCustomAttributes(false)[0])
  if (String(res) != "a14") apLogFailInfo("wrong result obtained", "a14", res, "")
  res = String(cls14.GetProperty("g").GetCustomAttributes(false)[0])
  if (String(res) != "a14") apLogFailInfo("wrong result obtained", "a14", res, "")
  res = String(cls14.GetProperty("g").GetGetMethod().GetCustomAttributes(false).Length)
  if (String(res) != "0") apLogFailInfo("wrong result obtained", "0", res, "")

  

  apInitScenario("test that the Obsolete attribute is recognized");
  

  res = String(cls17.GetMethod("bar").GetCustomAttributes(false)[0].GetType().FullName)
  if (res != "System.ObsoleteAttribute") apLogFailInfo("wrong result obtained", "System.ObsoleteAttribute", res, "")

  

  apEndTest();


}


custattr03();


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
