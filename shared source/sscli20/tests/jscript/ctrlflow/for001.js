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


var iTestID = 52716;


function for001() {

    apInitTest( "for001 ");

    var Result;
    var Expected;

    var Counter;

    //--------------------------------------------------------------------
    apInitScenario( " 1: For loop with a counter holding different numeric types and +1 update");


    Result = 0;
    for ( Counter = 1; Counter <= 100; Counter++ ) {
        Result++;
    }

    Expected = 100;
    if (Result != Expected) {
        apLogFailInfo( "Integer loop (increment 1) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = 40001; Counter <= 40100; Counter++ ) {
        Result++;
    }

    Expected = 100;
    if (Result != Expected) {
        apLogFailInfo( "Long loop (increment 1) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = 1.000000001; Counter <= 100.000000001; Counter++ ) {
        Result++;
    }

    Expected = 100;
    if (Result != Expected) {
        apLogFailInfo( "Double loop (increment 1) failed.", Result, Expected,"");
    }


    //--------------------------------------------------------------------
    apInitScenario( " 2: For loop with a counter holding different numeric types and >1 update");

    Result = 0;
    for ( Counter = 1; Counter <= 100; Counter += 2 ) {
        Result++;
    }

    Expected = 50;
    if (Result != Expected) {
        apLogFailInfo( "Integer loop (increment 2) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = -500000; Counter <= 500000; Counter += 40000 ) {
        Result++;
    }

    Expected = 26;
    if (Result != Expected) {
        apLogFailInfo( "Long loop (increment 40000) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = 1.001 ; Counter <= 1.100 ; Counter += 0.001 ) {
        Result++;
    }

    Expected = 100;
    if (Result != Expected) {
        apLogFailInfo( "Double loop (increment 0.001) failed.", Result, Expected,"");
    }


    //--------------------------------------------------------------------
    apInitScenario( " 3: For loop with a counter holding different numeric types and <0 update");

    Result = 0;
    for ( Counter = 50 ; Counter >= -50 ; Counter-- ) {
        Result++;
    }

    Expected = 101;
    if (Result != Expected) {
        apLogFailInfo( "Integer loop (decrement 1) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = 1500000; Counter >= -500000; Counter -= 40000 ) {
        Result++;
    }

    Expected = 51;
    if (Result != Expected) {
        apLogFailInfo( "Long loop (decrement 40000) failed.", Result, Expected,"");
    }


    Result = 0;
    for ( Counter = 100.00000000001; Counter >= 9.000000001; Counter -= 10.00000000001 ) {
        Result++;
    }

    Expected = 10;
    if (Result != Expected) {
        apLogFailInfo( "Double loop (decrement 10.00000000001) failed.", Result, Expected,"");
    }


    apEndTest();
    

}

for001();


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
