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


var iTestID = 199120;

/*/////////////////////////////////////////////////////////////////////////////


	purpose:  to test expando methods in different circumstances/scopes



////////////////////////////////////////////////////////////////////////////*/

class cls1{			// basic test of expando method in a class
  var x = 5
  expando function test(){
    this.x = 10
  }
}
var t1fail:boolean = false
var t1 = new cls1
var t1a = new t1.test()
if (t1a.x != 10) t1fail = true
if (t1.x != 5) t1fail = true
t1.test()
if (t1.x != 10) t1fail = true



class cls3base{
  var fail:boolean = false
  class cls3{			// basic test of expando method in a subclass
    var x = 5
    expando function test(){
      this.x = 10
    }
  }
  function test(){
    var t3 = new cls3
    var t3a = new t3.test()
    if (t3a.x != 10) fail = true
    if (t3.x != 5) fail = true
    t3.test()
    if (t3.x != 10) fail = true
  }
}
var t3 = new cls3base
t3.test()


package pack{
class cls5{			// basic test of expando method in a class
  var x = 5
  expando function test(){
    this.x = 10
  }
}

class cls6base{
  var fail:boolean = false
  class cls6{			// basic test of expando method in a subclass
    var x = 5
    expando function test(){
      this.x = 10
    }
  }
  function test(){
    var t6 = new cls6
    var t6a = new t6.test()
    if (t6a.x != 10) fail = true
    if (t6.x != 5) fail = true
    t6.test()
    if (t6.x != 10) fail = true
  }
}


class test{
  static var t5fail:boolean = false
  static var t6fail:boolean = false
  static function go5(){
    var t5 = new cls5
    var t5a = new t5.test()
    if (t5a.x != 10) t5fail = true
    if (t5.x != 5) t5fail = true
    t5.test()
    if (t5.x != 10) t5fail = true
  }
  static function go6(){
    var t6 = new cls6base
    t6.test()
    t6fail = t6.fail
  }
}
}



function expando04() 
{
  apInitTest("expando04");


  apInitScenario("expando method of a class in global scope");
  if (t1fail) apLogFailInfo("Unexpected failure", "", "", "");    

  apInitScenario("expando method of a subclass in global scope");
  if (t3.fail) apLogFailInfo("Unexpected failure", "", "", "");    


  apInitScenario("expando method of a class in a function scope");
  var t2 = new cls1
  var t2a = new t2.test()
  if (t2a.x != 10) apLogFailInfo("Unexpected failure", 10, t2a.x, "")
  if (t2.x != 5) apLogFailInfo("Unexpected failure", 5, t2.x, "")
  t2.test()
  if (t2.x != 10) apLogFailInfo("Unexpected failure", 10, t2.x, "")

  apInitScenario("expando method of a subclass in a function scope");
  var t4 = new cls3base
  t4.test()
  if (t4.fail) apLogFailInfo("Unexpected failure", "", "", "");    


  apInitScenario("expando method of a class in package scope");
  pack.test.go5()
  if (pack.test.t5fail) apLogFailInfo("Unexpected failure", "", "", "");    

  apInitScenario("expando method of a subclass in package scope");
  pack.test.go6()
  if (pack.test.t6fail) apLogFailInfo("Unexpected failure", "", "", "");    


  apEndTest();

}


expando04();


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
