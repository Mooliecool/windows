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


var iTestID = 51800;


var ctr=0;

function verify(m,n) {
    ctr++;
    if (m != n)
        apLogFailInfo("Clause "+m+" failed",n,m,"");
}

function cond001() {

    apInitTest("cond001 ");
    
    var i;

    // Create 2Dim array 8x3
    var af = new Array(8);    
        for (i=0; i<8; i++) {
            af[i] = new Array(3);
        }
   
    // Fill array eliments with validation nums of a def pattern.
    for (i=0; i<4; i++) {
        af[i][1] = 1;
        af[i + 4][1] = 0;
    }

    for (i=0; i<5; i+=4) {
        af[i][2] = 1;
        af[i+1][2] = 1;
        af[i+2][2] = 0;
        af[i+3][2] = 0;
    }

    for (i=0; i<7; i+=2) {
        af[i][3] = 1;
        af[i + 1][3] = 0;
    }


    //----------------------------------------------------------------------------
    apInitScenario("1. verify conditional exp evaluation");

    for (var c = 0; c < 8; c++) {
        af[c][1] ?
            af[c][2] ?
                af[c][3] ? verify(c,0) : verify(c,1)
            :
                af[c][3] ? verify(c,2) : verify(c,3)            
        :
            af[c][2] ?
                af[c][3] ? verify(c,4) : verify(c,5)
            :
                af[c][3] ? verify(c,6) : verify(c,7);
    }

    if (ctr != 8) {
        apLogFailInfo("Count is incorrect",8,ctr,"");
    }
   
    apEndTest();

}


cond001();


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
