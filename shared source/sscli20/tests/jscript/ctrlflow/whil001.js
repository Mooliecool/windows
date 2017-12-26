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


var iTestID = 53899;


function whil001() {

    apInitTest( "whil001 ");

    var LoopExit;
    var Counter;
    var Expected;

    //--------------------------------------------------------------------
    apInitScenario( "1a: Use while loop with symblic constant");

    LoopExit = false;
    Counter = 0;

    while (true && !(LoopExit)) {
        Counter++;
        if (Counter == 100) 
           LoopExit = true;
    }

    Expected = 100;
    if (Counter != Expected) {
        apLogFailInfo("While loop with Symbolic constant failed","Counter","Expected","");
    }

    //--------------------------------------------------------------------
    apInitScenario( "1b: Use while loop with constant");

    LoopExit = false;
    Counter = 0;

    while (100 && !(LoopExit)) {
        Counter++;
        if (Counter == 100) {
           LoopExit = true;
        }
    }


    Expected = 100;
    if (Counter != Expected) {
        apLogFailInfo("While loop with Constant failed","Counter","Expected","");
    }

    apEndTest();
    

}



whil001();


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
