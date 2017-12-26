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


var iTestID = 53194;


function rand001() {
 @if(@_fast)
    var i, rndNum, rnd1,rnd2;
 @end
    apInitTest("rand001 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. verify bounds");

    for (i=0; i<10000; i++)
    {
        rndNum = Math.random();
        if ( rndNum < 0 || rndNum > 1 )
            apLogFailInfo( "bounds verification failed", "0 <= n <= 1", rndNum, "");
    }

    //----------------------------------------------------------------------------
    apInitScenario("2. verify uniqueness");
/*
    var cMems = 10000;

    var obj = new Object();

    for (var i=0; i<cMems; i++)
        obj[Math.random()] = 0;

	var cUniqMems = 0;
    for (var iMem in obj) ++cUniqMems;    

    if (cUniqMems != cMems)
        apLogFailInfo( "verify uniqueness failed", cMems, cUniqMems, "");
*/

    //----------------------------------------------------------------------------
    apInitScenario("3. verify variance");

    
    for (i=0; i<100; i++)
    {
        if ( (rnd1 = Math.random()) == (rnd2 = Math.random()) )
        {
            apLogFailInfo( "variance failed with rnd1=="+
                rnd1+" and rnd2=="+rnd2, "rnd1 != rnd2", "rnd1 == rnd2", "");
        }
    }


    apEndTest();

}


rand001();


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
