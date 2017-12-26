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


var iTestID = 51825;


function cont001() {

    apInitTest("cont001 ");


    //----------------------------------------------------------------------------
    apInitScenario("1. verify continue nesting");

    var a = new Array(8);
    var r = new Array(8);

    for (a[7]=r[7]=0; a[7]<8; a[7]++) {
        if (a[7]>=7) continue;        
        r[7]++;
        for (a[6]=r[6]=0; a[6]<8; a[6]++) {
            if (a[6]>=6) continue;        
            r[6]++;
            for (a[5]=r[5]=0; a[5]<7; a[5]++) {
                if (a[5]>=5) continue;        
                r[5]++;
                for (a[4]=r[4]=0; a[4]<6; a[4]++) {
                    if (a[4]>=4) continue;        
                    r[4]++;
                    for (a[3]=r[3]=0; a[3]<5; a[3]++) {
                        if (a[3]>=3) continue;            
                        r[3]++;
                        for (a[2]=r[2]=0; a[2]<4; a[2]++) {
                            if (a[2]>=2) continue;                
                            r[2]++;
                            for (a[1]=r[1]=0; a[1]<3; a[1]++) {
                                if (a[1]>=1) continue;                    
                                r[1]++;
                                for (a[0]=r[0]=0; a[0]<2; a[0]++) {
                                    if (a[0]>=0) continue;                        
                                    r[0]++;
					            }
				            }
                        }
                    }
                }
            }
        }
    }

    for (var i=0; i<8; i++)
        if (r[i]!=i) apLogFailInfo( "continue "+i+"failed ", i, r[i],"");
                                    

    apEndTest();

}


cont001();


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
