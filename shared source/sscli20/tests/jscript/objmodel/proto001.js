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


var iTestID = 53134;


function verify(vAct, vExp, bugNum){
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}


@if(!@aspx)
   function udObj1 () { /* nothing */ }
   function udObj2 () { /* nothing */ }
@else
   expando function udObj1 () { /* nothing */ }
   expando function udObj2 () { /* nothing */ }
@end

function proto001 ()
{
 
    apInitTest("proto001 ");

    
    //----------------------------------------------------------------------------
    // we're spec'd different from nav here--they'd return "undefined"
    apInitScenario("verify prototype existence before first use for udef objs");
    
    verify(typeof udObj1.prototype, "object", null);


    //----------------------------------------------------------------------------
    // this is to seperate our behaviour (above) from nav in case we should change to match
    apInitScenario("confirm existence after first use");

    // first use
    udObj1.prototype.mymem = "myval";

    verify(typeof udObj1.prototype, "object", null);


    //----------------------------------------------------------------------------
    apInitScenario("verify mem value in prototype object");

    verify(udObj1.prototype.mymem, "myval", null);


    //----------------------------------------------------------------------------
    apInitScenario("verify mem value in an object instance: obj created after pt change");

    verify((new udObj1).mymem, "myval", null);


    //----------------------------------------------------------------------------
    apInitScenario("verify mem value in an object instance: obj created before pt change");

    var myobj = new udObj2();
    udObj2.prototype.myothermem = "myotherval";
    verify(myobj.myothermem, "myotherval", null);


    //----------------------------------------------------------------------------
    apInitScenario("verify prototype existence for built-in classes and has the correct type");

    verify(typeof Array.prototype, "object", null);
    verify(typeof Boolean.prototype, "object", null);
    verify(typeof Date.prototype, "object", null);
    verify(typeof Function.prototype, "function", null);
//    verify(typeof Math.prototype, "object", null);
    verify(typeof Number.prototype, "object", null);
    verify(typeof Object.prototype, "object", null);
    verify(typeof String.prototype, "object", null);


    //----------------------------------------------------------------------------
    apInitScenario("verify prototype mems can be added (not in strict) to all built-in classes");
@if(!@_fast)
    Array.prototype.a = 1;
    Boolean.prototype.b = 2;
    Date.prototype.c = 3;
    Function.prototype.d = 4;
//    Math.prototype.e = 5;
    Number.prototype.f = 6;
    Object.prototype.g = 7;
    String.prototype.h = 8;

    verify(Array.prototype.a, 1, null);
    verify(Boolean.prototype.b, 2, null);
    verify(Date.prototype.c, 3, null);
    verify(Function.prototype.d, 4, null);
//    verify(Math.prototype.e, 5, null);
    verify(Number.prototype.f, 6, null);
    verify(Object.prototype.g, 7, null);
    verify(String.prototype.h, 8, null);
@end

    apEndTest();

}


proto001();


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
