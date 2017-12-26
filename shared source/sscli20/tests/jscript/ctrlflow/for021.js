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


var iTestID = 52736;


function for021() {

    apInitTest("for021 ");

    var t = 10000, n;


	//----------------------------------------------------------------------------
    apInitScenario("1. omitting all clauses");

    n = 0;
    for ( ; ; )
        if (++n > t) break;
        n--;

    if (n != t) apLogFailInfo( "omitting all clauses--failed ", t, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. omitting test and update clauses");

    for (n=0; ;)
        if (++n > t) break;
        n--;

    if (n != t) apLogFailInfo( "omitting test and update clauses--failed ", t, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. omitting update clause");

    for (n=0;n<t+1; )
        n++;
        n--;

    if (n != t) apLogFailInfo( "omitting update clause--failed ", t, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. omitting test clause");

    for (n=0; ;n++)
        if (n > t) break;
        n--;

    if (n != t) apLogFailInfo( "omitting test clause--failed ", t, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. omitting init clause");

    var expect = t % 2 ? t : t+1;
    n = 0;
    for ( ;n<t+1;n++)
        n++;
        n--;

    if (n != expect) apLogFailInfo( "omitting init clause--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. omitting init and update clauses");

    n = 0;
    for ( ;n<t+1; )
        n++;
        n--;

    if (n != t) apLogFailInfo( "omitting init and update clauses--failed ", t, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. omitting init and test clauses");

    n=0;
    for ( ; ;n++)
        if (n > t) break;
        n--;

    if (n != t) apLogFailInfo( "omitting init and update clauses--failed ", t, n,"");


    apEndTest();

}


for021();


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
