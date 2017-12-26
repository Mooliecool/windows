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


var iTestID = 51818;

function cc001() {
  apInitTest("cc001");

  // this is an acceptance test for cc.  Because of this, all cc expressions
  // are commented in this test to avoid a compilation error if cc is accidentally disabled.

  @cc_on

  var res;

  apInitScenario("1. Verify conditional compilation eats /*@ ");     // comment
  res = 0;
  /*@ res = 1;                                                       // comment
  if (res != 1) {
    apLogFailInfo("Cond comp /*@ expression not evaluated",1,res,"");   // comment
  }

  apInitScenario("2. Verify conditional compilation eats //@");
  res = 0;
  //@ res = 1;
  if (res != 1) {
    apLogFailInfo("Cond comp //@ expression not evaluated",1,res,"");
  }

  apInitScenario("3. Verify undef'd cc variables are null");
  res = 0;
  @if (@abinitio != @abinitio) res=1; @end
  if (res != 1) {
    apLogFailInfo("Cond comp variable not null",1,res,"");
  }

  apInitScenario("4. Verify cc variables may be defined");
  res = 0;
  @set @fiatlux=1
  res = 0 //@ + @fiatlux
  if (res != 1) {
    apLogFailInfo("Cond comp variable not defined",1,res,"");
  }

  apInitScenario("5.Verify cc expressions work");
  res = 0;
  @if (@fiatlux == 1) res = 1; @else res = 2; @end
  if (res != 1) {
    apLogFailInfo("Cond comp expression not evaluated correctly",1,res,"");
  }


  apEndTest();

}





cc001();


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
