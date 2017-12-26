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


var iTestID = 53107;


function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function string01()
{
  @if (@_fast)
     var test1, temp4, test7, test9;
  @end
    apInitTest("string01 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. test construction");

    test1 = new String();
    // success means no error.

    
    //----------------------------------------------------------------------------
    apInitScenario("2. verify construction with typeof");

    verify("typeof verification", typeof (new String()), "object", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("3. verify initialization");

    verify("typeof verification", (new String("hello")).toString(), "hello", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("4. verify length creation/accuracy with explicit lval");

    verify("typeof verification", (temp4 = new String("hello")).length, 5, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. verify length creation/accuracy with implicit lval");

    verify("typeof verification", (new String("hello")).length, 5, null);


    //----------------------------------------------------------------------------
    apInitScenario("6. verify default value");

    verify("verify default value", (new String()).toString(), "", null);


    //----------------------------------------------------------------------------
    apInitScenario("7. verify indexed member addition, explicit lval");

    verify("obj member addition, explicit lval", (test7 = new String())[1] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("8. verify indexed member addition, implicit lval");

    verify("obj member addition, implicit lval", (new String())[1] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("9. verify named member addition, explicit lval");

    verify("obj member addition, explicit lval", (test9 = new String()).foo = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("10. verify named member addition, implicit lval");

    verify("obj member addition, implicit lval", (new String()).foo = "bar", "bar", null);


    apEndTest();

}


string01();


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
