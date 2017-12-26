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


var iTestID = 52758;


function for121() {

    apInitTest("for121 ");

    var t = 100000, n, nn, expect;

  
    //----------------------------------------------------------------------------
    apInitScenario("1. omitting all clauses");

    expect = t + t % 2 + 1;
    n = 0;
    for ( ; ; ) {
        n++;
        if (++n > t) break;
    }
    n--;

    if (n != expect) apLogFailInfo( "omitting all clauses--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. omitting test and update clauses");

    expect = t + t % 2 + 1;
    for (n=0; ;) {
        n++;
        if (++n > t) break;
    }
    n--;

    if (n != expect) apLogFailInfo( "omitting test and update clauses--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. omitting update clause");

    expect = t + t % 2 - 1;
    for (n=0;n<t; ) {
        n++;
        n++;
    }
    n--;

    if (n != expect) apLogFailInfo( "omitting update clause--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. omitting test clause");

    expect = t + t % 2 - 1;
    for (nn=n=0; ;n++) {
        nn+=2;
        if (++n > t) break;
    }
    n--; nn--;

    if (n & nn != expect) apLogFailInfo( "omitting test clause--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. omitting init clause");

    expect = t + t % 2 - 1;
    n=nn=0;        
    for ( ;n<t;n++) {
        n++;
        nn+=2;
    }
    n--; nn--;

    if (n & nn != expect) apLogFailInfo( "omitting init clause--failed ", "n=="+n+"\tnn=="+nn, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. omitting init and update clauses");

    expect = t + t % 2 - 1;
    n=0;
    for ( ;n<t; ) {
        n++;
        n++;
    }
    n--;

    if (n != expect) apLogFailInfo( "omitting test clause--failed ", expect, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. omitting init and test clauses");

    expect = t + t % 2 - 1;
    n=nn=0;
    for ( ; ;n++) {
        nn+=2;
        if (++n > t) break;
    }
    n--; nn--;

    if (n & nn != expect) apLogFailInfo( "omitting init and test clauses--failed ", "n=="+n+"\tnn=="+nn, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("8. Regression test for Bug 157902 -- JS5.6 asserts when throwing from swtich block");

    var expectedNumber = -2146823281;
    var expectedDescription = "";
    var i, actualNumber, actualDescription;

    if (ScriptEngineMajorVersion() >= 7)
        expectedDescription = "'null' is null or not an object";
    else
        expectedDescription = "'null.foo' is null or not an object";

    try
    {
        eval (" for (i = 0; i <= null.foo; i++)     " +
              " {                                   " +
              "     print (\"Hello\");              " +
              " }                                   ");
    }
    catch (exception)
    {
        actualNumber = exception.number;
        actualDescription = exception.description;		
    }
           	
    if (actualNumber != expectedNumber)	
        apLogFailInfo ("Wrong error number in 8.", expectedNumber, actualNumber, 157902);
    if (actualDescription != expectedDescription)
        apLogFailInfo ("Wrong error description in 8.", expectedDescription, actualDescription, 157902);



    apEndTest();

} 


for121();


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
