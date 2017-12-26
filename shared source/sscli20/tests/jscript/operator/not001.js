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


var iTestID = 52910;

function not001() {

    apInitTest( "NOT001 ");

    var IntVar1  , IntVar2;
    var LongVar1  , LongVar2;
    var DoubleVar1  , DoubleVar2;
    var ByteVar1  , ByteVar2;

    var Result	, Expected;

    apInitScenario( " 1: Use ! with constants of each type");

    Result = ! true
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (const) failed" + " " ,"","","");
    }

    Result = ! false
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (const) failed" + " " ,"","","");
    }

    Result = ! -1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (integer const) failed" + " " ,"","","");
    }

    Result = ! 0
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (integer const) failed" + " " ,"","","");
    }

    Result = ! -1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (long const) failed" + " " ,"","","");
    }

    Result = ! 0
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (long const) failed" + " " ,"","","");
    }

    Result = ! -1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (double const) failed" + " " ,"","","");
    }

    Result = ! 0
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (double const) failed" + " " ,"","","");
    }

    apInitScenario(" 2: Use ! with variables of each type");

    IntVar1 = -1
    Result = ! IntVar1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (integer variable) failed" + " " ,"","","");
    }

    IntVar1 = 0
    Result = ! IntVar1
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (integer variable) failed" + " " ,"","","");
    }

    LongVar1 = -1
    Result = ! LongVar1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (long variable) failed" + " " ,"","","");
    }

    LongVar1 = 0
    Result = ! LongVar1
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (long variable) failed" + " " ,"","","");
    }

    DoubleVar1 = -1
    Result = ! DoubleVar1
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (double variable) failed" + " " ,"","","");
    }

    DoubleVar1 = 0
    Result = ! DoubleVar1
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (double variable) failed" + " " ,"","","");
    }

    apInitScenario(" 3: Use ! with expressions of each type");

    IntVar1 = -2
    IntVar2 = 1
    Result = ! (IntVar1 + IntVar2)
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (integer expression) failed" + " " ,"","","");
    }

    IntVar1 = 0
    IntVar2 = 1
    Result = ! (IntVar1 * IntVar2)
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (integer expression) failed" + " " ,"","","");
    }

    LongVar1 = 0
    LongVar2 = 1
    Result = ! (LongVar1 - LongVar2)
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (long expression) failed" + " " ,"","","");
    }

    LongVar1 = 0
    LongVar2 = 1
    Result = ! (LongVar1 / LongVar2)
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (long expression) failed" + " " ,"","","");
    }

    DoubleVar1 = -1
    DoubleVar2 = 0
    Result = ! (DoubleVar1 + DoubleVar2)
    Expected = false

    if (Result != Expected) {
     apLogFailInfo("! true (double expression) failed" + " " ,"","","");
    }

    DoubleVar1 = 1
    DoubleVar2 = -1
    Result = ! (DoubleVar1 + DoubleVar2)
    Expected = true

    if (Result != Expected) {
     apLogFailInfo("! false (double expression) failed" + " " ,"","","");
    }

    apEndTest();
}


not001();


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
