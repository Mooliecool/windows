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


var iTestID = 53105;

@if(@_fast)
	var test1,test3,test5;
@end

function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function object01()
{
 
    apInitTest("object01 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. test construction");

    test1 = new Object();
    // success means no error.

    
    //----------------------------------------------------------------------------
    apInitScenario("2. verify construction with typeof");

    verify("typeof verification", typeof (new Object()), "object", null);


    //----------------------------------------------------------------------------
    apInitScenario("3. verify indexed member addition, explicit lval");

    verify("obj member addition, explicit lval", (test3 = new Object())[0] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("4. verify indexed member addition, implicit lval");

    verify("obj member addition, implicit lval", (new Object())[0] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("5. verify named member addition, explicit lval");

    verify("obj member addition, explicit lval", (test5 = new Object()).foo = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("6. verify named member addition, implicit lval");

    verify("obj member addition, implicit lval", (new Object()).foo = "bar", "bar", null);


    apEndTest();

}


object01();


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
