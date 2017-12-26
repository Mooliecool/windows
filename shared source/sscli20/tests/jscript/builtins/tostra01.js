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


var iTestID = 53715;


function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function tostra01()
{
 
    apInitTest("toStrA01 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. test method existence for Class");

    verify("Class method existence", typeof Array.prototype.toString, "function", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("2. test method existence for object");

    verify("Class method existence", typeof (new Array()).toString, "function", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("3. test BF, implicit: consecutive members zero-based, default delim");

    verify("BF, implicit: consecutive members zero-based, default delim", 
        (new Array("J","S","c","r","i","p","t"))+"", "J,S,c,r,i,p,t", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("4. test BF, explicit: consecutive members zero-based, default delim");

    verify("BF, explicit: consecutive members zero-based, default delim", 
        (new Array("J","S","c","r","i","p","t")).toString(), "J,S,c,r,i,p,t", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. test BF, explicit: consecutive members zero-based, explict delim");

    verify("BF, explicit: consecutive members zero-based, explict zls delim", 
        (new Array("J","S","c","r","i","p","t")).toString("|"), "J,S,c,r,i,p,t", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("6. test return type is string");

    verify("return type is string", typeof (new Array(1,2,3,4)).join(), "string", null);


    apEndTest();

}


tostra01();


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
