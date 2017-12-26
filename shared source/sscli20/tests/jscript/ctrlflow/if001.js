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


var iTestID = 52823;

function if001() 
{
    apInitTest("if001 ");
    

    // Create 2Dim array 8x3

    var aValid = new Array(8);    
        for (var i = 1; i <= 8; i++) {
            aValid[i] = new Array(3);
        }    

   
    // Fill array eliments with validation nums of a def pattern.

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (!@_fast)  
        var i;
     
@end       

    for (i = 1; i <= 4; i++) {
        aValid[i][1] = 1;
        aValid[i + 4][1] = 0;
    }

    for (i = 1; i <= 5; i += 4) {
        aValid[i][2] = 1;
        aValid[i + 1][2] = 1;
        aValid[i + 2][2] = 0;
        aValid[i + 3][2] = 0;
    }

    for (i = 1; i <= 7; i += 2) {
        aValid[i][3] = 1;
        aValid[i + 1][3] = 0;
    }


    //----------------------------------------------------------------------------
    apInitScenario("1-3. in a loop");


    var c;
    var ctr = 0;    

    // Test for the pattern
    for (c = 1; c <= 8; c++) {
        if ( aValid[c][1] ) {
            if ( aValid[c][2] ) {
                if ( aValid[c][3] ) {
                    ctr++;
                    if (c != 1) {
                        apLogFailInfo("Clause 1 Failed   c =" + c + " " ,"","","");
                    }
                }
                else {
                    ctr++;
                    if (c != 2) {
                        apLogFailInfo("Clause 2 Failed   c =" + c + " " ,"","","");
                    }
                }
            }
            else if ( aValid[c][3] ) {
                ctr++;
                if (c != 3) {
                    apLogFailInfo("Clause 3 Failed   c =" + c + " " ,"","","");
                }
            }
            else {
                ctr++;
                if (c != 4) {
                    apLogFailInfo("Clause 4 Failed   c =" + c + " " ,"","","");
                }
            }                
        }
        else if ( aValid[c][2] ) {
            if ( aValid[c][3] ) {
                ctr++;
                if (c != 5) {
                    apLogFailInfo("Clause 5 Failed   c =" + c + " " ,"","","");
                }
            }
            else {
                ctr++;
                if (c != 6) {
                    apLogFailInfo("Clause 6 Failed   c =" + c + " " ,"","","");
                }
            }
        }
        else if ( aValid[c][3] ) {
            ctr++;
            if (c != 7) {
                apLogFailInfo("Clause 7 Failed   c =" + c + " " ,"","","");
            }
        }
        else {
            ctr++;
            if (c != 8) {
                apLogFailInfo("Clause 8 Failed   c =" + c + " " ,"","","");
            }
        }
    }

    if (ctr != 8) {
        apLogFailInfo("Count is incorrect   ctr =" + ctr + " " ,"","","");
    }
   
    apEndTest();
}




if001();


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
