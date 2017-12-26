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


var iTestID = 51821;


function cc004() {
  apInitTest("cc004");

   //control flow (@if...@elif...@else...@end)

  @cc_on

  apInitScenario("1. control flow:  if statement")

  var res = 7;
  var exp = 13;

  @if (true)
    res=13;
  @elif (true)
    res++;
  @else
    res *= 2;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }

  apInitScenario("2. control flow:  if statement (elif)")

  res = 7;
  exp = 8;

  @if (false)
    res=13;
  @elif (true)
    res++;
  @else
    res *= 2;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }

  apInitScenario("3. control flow:  if statement (else)")

  res = 7;
  exp = 14;

  @if (false)
    res=13;
  @elif (false)
    res++;
  @else
    res *= 2;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }

  apInitScenario("4. control flow:  if statement (multiple elifs->else)")

  res = 7;
  exp = 14;

  @if (false)
    res=13;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @else
    res *= 2;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }

  apInitScenario("5. control flow:  if statement (multiple elifs->elif)")

  res = 7;
  exp = 6;

  @if (false)
    res=13;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (true)
    res--;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @elif (false)
    res++;
  @else
    res *= 2;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }

  apInitScenario("6. control flow:  if statement nested")

  res = 7;
  exp = 13;

  @if (true)
    @if (false)
      res *= 2;
    @elif (false)
      res *= 3;
    @else
      @if (true)
        res = 13;
      @else
        res--;
      @end
    @end
  @else
    res++;
  @end

  if (exp != res) {
    apLogFailInfo("didn't go through if statement correctly",exp,res,"");
  }


  apEndTest();

}


cc004();


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
