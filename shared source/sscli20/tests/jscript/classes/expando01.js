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


var iTestID = 198992;

//////////////////////////////////////////////////////////////////////////////////////
//

//
///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//////////////////////////////////////////////////////////////////////////////
expando class t1{}
class t3{
  expando class sub{
    function add(field, value){
      this[field] = value
    }
    var x:long
  }
}

var test6 = false
var v5 = new (new t3).sub
eval("class foo{var g=10};  v5['x'] = foo")
var v6 = new (v5['x'])
if (v6.g != 10) test6 = true

var test7 = false
var v7 = new (new t3).sub
function retz(){return "z"}
v7[retz()] = function():long{return -1}
if (v7["z"]() != -1) test7 = true

var test8 = false
var v8 = new (new t3).sub
v8.add(retz, function(){return {a:1234, b:4321}})
var temp = retz
if (v8[temp]().b != 4321) test8 = true

class outer1{
  expando class z1{}

  function test(){
    var v1 = new z1
    v1[0] = decimal.MaxValue
    delete v1[0]
    return v1[0]
  }
}


function expando01(){
  var lng:long

  apInitTest("Expando01")



  apInitScenario("add properties to and retrieve them from an expando class")

  var v1 = new t1
  // simple vars
  v1[0] = "stringvar"
  if (v1[0] != "stringvar") apLogFailInfo("unable to store and retrieve a string", "stringvar", v1[0], "")
  v1[1] = long(9876543210)
  if (v1[1] != 9876543210) apLogFailInfo("unable to store and retrieve a long", 9876543210, v1[0], "")
  v1[2] = long(9786543210)
  if (v1[2] != 9786543210) apLogFailInfo("unable to store and retrieve a typed long", 9786543210, v1[0], "")
  v1[0] = true
  if (v1[0] != true) apLogFailInfo("unable to store and retrieve a boolean", true, v1[0], "")
  v1[0] = decimal.MaxValue
  if (v1[0] != -decimal.MinValue) apLogFailInfo("unable to store and retrieve a decimal", true, v1[0], "")
  // object vars
  v1[1] = new Number(5.12)
  if (v1[1] != 5.12) apLogFailInfo("unable to store and retrieve a Number object", 5.12, v1[1], "")
  if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
  v1[-1] = new Date(0)
  if (v1[-1] != "Wed Dec 31 16:00:00 PST 1969") apLogFailInfo("unable to store and retrieve a decimal", true, "Wed Dec 31 16:00:00 PST 1969", "")
  }
  // arrays
  v1[4] = new Array()
  v1[4][4] = "number"
  v1[4][6] = new Array()
  v1[4][6][2] = 4321
  if (v1[4][4] != "number") apLogFailInfo("unable to store and retrieve Array items", "number", v1[4][4], "")
  if (v1[4][6][2] != 4321) apLogFailInfo("unable to store and retrieve Array items", "number", v1[1][6][2], "")
  v1[10] = new int[5]
  v1[10][3] = 1
  if (v1[10][3] != 1) apLogFailInfo("unable to store and retrieve array items", "number", v1[10][3], "")
  // freakish things
  //v1["foo"] = new ActiveXObject("WScript.shell")
  //v1["foo"].run("c:\\school\\vbstest.exe")   // no need to actuall run anything
  //if (v1["foo"] == undefined) apLogFailInfo("unable to store and retrieve an activeX obj ", "", "", "")
  v1[""] = function(){return 4}
  if (eval('v1[""]')() != 4) apLogFailInfo("unable to store and retrieve function ", "4", v1[1](), "")



  apInitScenario("delete properties from an expando class")

  if (v1[0]== undefined) apLogFailInfo("decimal property doeas not exist", undefined, v1[0], "")
  delete v1[0]
  if (v1[0]!= undefined) apLogFailInfo("unable to delete the decimal property", undefined, v1[0], "")

  if (v1[1]== undefined) apLogFailInfo("number object property doeas not exist", undefined, v1[1], "")
  delete v1[1]
  if (v1[1]!= undefined) apLogFailInfo("unable to delete the number object property", undefined, v1[1], "")


  apInitScenario("test side-by-side fields & expando properties with same name")

  var v3 = new (new t3).sub

  v3.x = 10
  v3["x"] = 20
  if (v3.x != 10) apLogFailInfo("the member has been overwritten by the expando property", 10, v3.x, "")
  v3.x = 15
  if (v3["x"] != 20) apLogFailInfo("the expando has been overwritten by the method property", 10, v3.x, "")
  
  var v4 = new (new t3).sub
  v4.x = 1
  v4.add("x", 2)
  if (v4.x != 1) apLogFailInfo("the member has been overwritten by the expando property", 1, v4.x, "")
  v4.x = 3
  if (v4["x"] != 2) apLogFailInfo("the expando has been overwritten by the method property", 2, v4.x, "")


  apInitScenario("using an expando property for a class definition")
  if (test6) apLogFailInfo("failure while using an expando prop for a type", false, true, "")

  apInitScenario("calling a function through an expando property")
  if (test7) apLogFailInfo("failure while using an expando prop for a function", false, true, "")

  apInitScenario("calling a function through an expando property indexed by ref")
  if (test8) apLogFailInfo("failure while using an expando prop for a function", false, true, "")

  apInitScenario("delete a expando subclass property from a method")
  var x = new outer1
//  if (x.test() != undefined) apLogFailInfo("failure to delete expando property on subclass", false, true, "")
//  PGMTODO  TODOPGM

  apEndTest();

}


expando01();


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
