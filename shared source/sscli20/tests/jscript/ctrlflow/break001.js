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


var iTestID = 51760;


function break001() {

    apInitTest("break001 ");


    //----------------------------------------------------------------------------
    apInitScenario("1. verify break nesting");

    var a = new Array(8);

    for (a[7]=0; a[7]<8; a[7]++) {
        if (a[7]==7) break;        
        for (a[6]=0; a[6]<8; a[6]++) {
            if (a[6]==6) break;            
            for (a[5]=0; a[5]<8; a[5]++) {
                if (a[5]==5) break;                
                for (a[4]=0; a[4]<8; a[4]++) {
                    if (a[4]==4) break;                    
                    for (a[3]=0; a[3]<8; a[3]++) {
                        if (a[3]==3) break;                        
                        for (a[2]=0; a[2]<8; a[2]++) {
                            if (a[2]==2) break;                            
                            for (a[1]=0; a[1]<8; a[1]++) {
                                if (a[1]==1) break;                                
                                for (a[0]=0; a[0]<8; a[0]++)
                                    if (a[0]==0) break;                                    
                            }
                        }
                    }
                }
            }
        }
    }

    for (var i=0; i<8; i++)
        if (a[i]!=i) apLogFailInfo( "break "+i+"failed ", i, a[i],"");
                                    

    apEndTest();

}


break001();


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
