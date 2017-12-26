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


var iTestID = 52712;


function verify (vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

function udFunc () { /* nothing */ }

function fnRetArgLengthExplicit (a,b,c,d)
{
  @if(!@_fast)
    return fnRetArgLengthExplicit.arguments.length;
  @end
}

function fnRetArgLengthImplicit (a,b,c,d)
{
  @if(!@_fast)
    return arguments.length;
  @end
}

function fnRetArgsJoinedExplicit (a,b,c)
{
  @if(!@_fast)
    var sArgs = "";
    for (var i=0; i<fnRetArgsJoinedExplicit.arguments.length; i++)
        sArgs += ","+fnRetArgsJoinedExplicit.arguments[i];

    return sArgs.substring(1,sArgs.length);
  @end
}

function fnRetArgsJoinedImplicit (a,b,c)
{
    var sArgs = "";
  @if(!@_fast)
    for (var i=0; i<arguments.length; i++)
        sArgs += ","+arguments[i];
  @end
    return sArgs.substring(1,sArgs.length);
}

function arg001()
{
    apInitTest("arg001 - not run in fast mode: arguments obj is null");


    //----------------------------------------------------------------------------
    apInitScenario("verify null external existence");
  @if(!@_fast)
    verify(Array.arguments, null, null);
    verify(Boolean.arguments, null, null);
    verify(Date.arguments, null, null);
    verify(Number.arguments, null, null);
    verify(Object.arguments, null, null);
    verify(String.arguments, null, null);
    verify(udFunc.arguments, null, null);
  @end
    //----------------------------------------------------------------------------
    apInitScenario("verify explicit length for param count == arg count");
  @if(!@_fast)
    verify(fnRetArgLengthExplicit(11,22,33,44), 4, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify explicit length for param count > arg count");
  @if(!@_fast)
    verify(fnRetArgLengthExplicit(11,22), 2, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify explicit length for param count < arg count");
  @if(!@_fast)
    verify(fnRetArgLengthExplicit(11,22,33,44,55,66,77), 7, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit length for param count == arg count");
  @if(!@_fast)
    verify(fnRetArgLengthImplicit(11,22,33,44), 4, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit length for param count > arg count");
  @if(!@_fast)
    verify(fnRetArgLengthImplicit(11,22), 2, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit length for param count < arg count");
  @if(!@_fast)
    verify(fnRetArgLengthImplicit(11,22,33,44,55,66,77), 7, null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify explicit arg access for param count == arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedExplicit(11,22,33,44), "11,22,33,44", null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify explicit arg access for param count > arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedExplicit(11,22), "11,22", null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify explicit arg access for param count < arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedExplicit(11,22,33,44,55,66,77), "11,22,33,44,55,66,77", null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit arg access for param count == arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedImplicit(11,22,33,44), "11,22,33,44", null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit arg access for param count > arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedImplicit(11,22), "11,22", null);
  @end

    //----------------------------------------------------------------------------
    apInitScenario("verify implicit arg access for param count < arg count");
  @if(!@_fast)
    verify(fnRetArgsJoinedImplicit(11,22,33,44,55,66,77), "11,22,33,44,55,66,77", null);
  @end

    apEndTest();

}


arg001();


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
