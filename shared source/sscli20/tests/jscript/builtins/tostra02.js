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


var iTestID = 53716;


function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function tostra02(){
 @if(@_fast)
    var rg;
 @end
 
    apInitTest("toStrA02 ");
    
    //----------------------------------------------------------------------------
    apInitScenario("1. test no members returns zls");

    verify("no members returns zls", (new Array()).toString(), "", null);


    //----------------------------------------------------------------------------
    apInitScenario("2. test negative integer members are not output");

    rg = new Array();
    rg[-32769] = 1;
    rg[-256] = 2;
    rg[-1] = 3;

    verify("no members returns zls", rg.toString(), "", null);


    //----------------------------------------------------------------------------
    apInitScenario("3. test that output contains delim placeholder for non-" +
        "consecutive pos integer members; default delim");

    rg = new Array();
    rg[1] = 1;
    rg[4] = 2;
    rg[5] = 3;
    rg[7] = 4;

    verify("non-consecutive pos integer members; default delim", 
        rg.toString(), ",1,,,2,3,,4", null);


    //----------------------------------------------------------------------------
    apInitScenario("4. ensure output contains delim placeholder for non-" +
        "consecutive pos integer members; explicit delim");

    rg = new Array();
    rg[1] = 1;
    rg[4] = 2;
    rg[5] = 3;
    rg[7] = 4;

    verify("non-consecutive pos integer members; explicit delim", 
        rg.join("!"), "!1!!!2!3!!4", null);


    //----------------------------------------------------------------------------
    apInitScenario("5. ensure non-integer members are ignored: reals");

    rg = new Array();
    rg[1.0] = 1;
    rg[1e204] = 2;
    rg[1] = 3;
    rg[-8.9] = 4;

    verify("non-integer members are ignored: reals", rg.toString(), ",3", null);


    //----------------------------------------------------------------------------
    apInitScenario("5. ensure non-integer members are ignored: keyed strings");

    rg = new Array();
    rg["foo"] = 1;
    rg[0] = 2;
    rg["bar"] = 3;
    rg["baz"] = 4;

    verify("non-integer members are ignored: keyed strings", rg.toString(), "2", null);


    //----------------------------------------------------------------------------
    apInitScenario("6. ensure non-integer members are ignored: named strings");

    rg = new Array();
    rg.foo = 1;
    rg[0] = 2;
    rg.bar = 3;
    rg.baz = 4;

    verify("non-integer members are ignored: named strings", rg.toString(), "2", null);


    // Objects & functions would just be converted to strings


    apEndTest();

}


tostra02();


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
