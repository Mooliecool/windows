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


var iTestID = 52256;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function do018() {

    apInitTest("do018 ");

    var n = 1;
    var ob = new Object();
    ob.mem = null;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 1;
    ob.mem = Math;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "non-executable object references--Math failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 1;
    ob.mem = Array;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Array failed ", 1, n, "");

    n = 1;
    ob.mem = Boolean;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n, "");

    n = 1;
    ob.mem = Date;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Date failed ", 1, n, "");

    n = 1;
    ob.mem = Number;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "");

    n = 1;
    ob.mem = Object;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Object failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 1;
    ob.mem = new Array();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Array failed ", 1, n, "");

    n = 1;
    ob.mem = new Boolean();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n, "");

    n = 1;
    ob.mem = new Date();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Date failed ", 1, n, "");

    n = 1;
    ob.mem = new Number();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "");

    n = 1;
    ob.mem = new Object();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Object failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 1;
    ob.mem = obFoo;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--user-defined failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    n = 1;
    ob.mem = new obFoo();
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 1;    
        ob.mem = 1;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 1;
        ob.mem = 1234567890;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 1;
        ob.mem = 2147483647;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 1;
        ob.mem = -1;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -1234567890;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -2147483647;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 1;
        ob.mem = 0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 1;
        ob.mem = -0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 1;    
        ob.mem = 2.225073858507202e-308;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 1;
        ob.mem = 1.2345678e90;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 1;
        ob.mem = 1.797693134862315807e308;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

        n = 1;
        ob.mem = 1.797693134862315807e309;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "> max pos (1.#INF) failed ", 1, n, "");

    // <0

        n = 1;
        ob.mem = -2.225073858507202e-308;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -1.2345678e90;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -1.797693134862315807e308;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

        n = 1;
        ob.mem = -1.797693134862315807e309;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "< min neg (-1.#INF) failed ", 1, n, "");

    // 0.0

        n = 1;
        ob.mem = 0.0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 1;
        ob.mem = -0.0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 1;    
        ob.mem = 0x1;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 1;
        ob.mem = 0x2468ACE;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 1;
        ob.mem = 0xFFFFFFFF;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 1;
        ob.mem = -0x1;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -0x2468ACE;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -0xFFFFFFFF;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 1;
        ob.mem = 0x0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 1;
        ob.mem = -0x0;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 1;    
        ob.mem = 01;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 1;
        ob.mem = 01234567;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 1;
        ob.mem = 037777777777;
        do {
        if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 1;
        ob.mem = -01;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -01234567;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 1;
        ob.mem = -037777777777;
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 1;
        ob.mem = 00;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 1;
        ob.mem = -00;
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");

        
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 1;    
        ob.mem = " ";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "single space failed ", 1, n, "");

        n = 1;
        ob.mem = "                                                                   ";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "multiple spaces failed ", 1, n, "");

        n = 1;
        ob.mem = "false";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as false failed ", 1, n, "");


        n = 1;
        ob.mem = "0";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "ns 0 failed ", 1, n, "");

        n = 1;
        ob.mem = "1234567890";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "ns >0 failed ", 1, n, "");

        n = 1;
        ob.mem = " 1234567890";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "ns >0, leading space failed ", 1, n, "");

        n = 1;
        ob.mem = "1234567890 ";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "ns >0, trailing space failed ", 1, n, "");

        n = 1;
        ob.mem = "-1234567890";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "ns <0 failed ", 1, n, "");

        n = 1;
        ob.mem = "foo";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as single word failed ", 1, n, "");

        n = 1;
        ob.mem = " foo";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as single word, leading space failed ", 1, n, "");

        n = 1;
        ob.mem = "foo ";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as single word, trailing space failed ", 1, n, "");

        n = 1;
        ob.mem = "foo bar";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as multiple word failed ", 1, n, "");

        n = 1;
        ob.mem = " foo bar";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as multiple word, leading space failed ", 1, n, "");

        n = 1;
        ob.mem = "foo bar ";
        do {
            if (--n) break;
        } while (!ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "as multiple word, trailing space failed ", 1, n, "");

        n = 1;
        ob.mem = "";
        do {
            if (--n) break;
        } while (ob.mem);
            n++;

        if (n != 1) apLogFailInfo( "zls failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 1;
    ob.mem = true;
    do {
        if (--n) break;
    } while (!ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "constants--true failed ", 1, n, "");

    n = 1;
    ob.mem = false;
    do {
        if (--n) break;
    } while (ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "constants--1 failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 1;
    ob.mem = null;
    do {
        if (--n) break;
    } while (ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "object references--null failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");

    n = 1;
    var obUndef;
    ob.mem = obUndef;
    do {
        if (--n) break;
    } while (ob.mem);
        n++;

    if (n != 1) apLogFailInfo( "undefined failed ", 1, n, "");

        
    apEndTest();

}


do018();


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
