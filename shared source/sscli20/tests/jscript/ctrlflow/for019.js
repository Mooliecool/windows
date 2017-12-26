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


var iTestID = 52734;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function for019() {

    apInitTest("for019 ");

    var n = 0;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;
    for ( ;Math && false; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "non-executable object references--Math failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 0;
    for ( ;!(Array + ""); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Array failed ", 1, n, "");

    n = 0;
    for ( ;!Boolean + 0; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n, "");

    n = 0;
    for ( ;!!Date * false; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Date failed ", 1, n, "");

    n = 0;
    for ( ;!("" + Number); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "");

    n = 0;
    for ( ;0 / !!Object; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Object failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 0;
    for ( ;!(new Array() + ""); )
        if (--n) break;
        n++;

    if (n != 0) apLogFailInfo( "executable object references--Array failed ", 0, n, "");

    n = 0;
    for ( ;!(new Boolean()) + 0; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n, "");

    n = 0;
    for ( ;!!(new Date()) * false; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Date failed ", 1, n, "");

    n = 0;
    for ( ;!("" + new Number()); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "");

    n = 0;
    for ( ;0 / !!(new Object()); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--Object failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 0;
    for ( ;!!obFoo ^ 1; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--user-defined failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    n = 0;
    var x = new obFoo();
    for ( ;!x << 4; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n, "");

    n = 0;
    for ( ;!(new obFoo()) << 4; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        for ( ;!(1 - 0); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 0;
        for ( ;!(1234567890 - 123456789); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 0;
        for ( ;!(2147400000 + 83647); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 0;
        for ( ;!(0 - 1); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(123456789 - 1234567890); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(-2147400000 - 83647); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 0;
        for ( ;1 - 1; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 0;
        for ( ;-0 + 0; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 0;    
        for ( ;!(2.225073858507202e-308); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 0;
        for ( ;!(1.2345678e90 / 9.98); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 0;
        for ( ;!(1.0e308 + 7.97693134862315807e307); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

        n = 0;
        for ( ;!(1.797693134862315807e308 + 1.0e308); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "> max pos (1.#INF) failed ", 1, n, "");

    // <0

        n = 0;
        for ( ;!(-2.225073858507202e-308); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(-1.2345678e90 * 2); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(1.797693134862315807e308 * -1); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

        n = 0;
        for ( ;!(1.797693134862315807e309 - 1.999e307); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "< min neg (-1.#INF) failed ", 1, n, "");

    // 0.0

        n = 0;
        for ( ;1.9 - 1.9; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 0;
        for ( ;0.0 * -1.0; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 0;    
        for ( ;!(0x1 - 0x0); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 0;
        for ( ;!(0x9ABCDEF - 0x2468ACE); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 0;
        for ( ;!(0xFFFF0000 + 0xFFFF); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 0;
        for ( ;!0x0 - 0x1; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(0x2468ACE - 0x9ABCDEF); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(-0xFFFF0000 - 0xFFFF); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 0;
        for ( ;0x1 - 0x1; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 0;
        for ( ;-0x0 + 0x0; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 0;    
        for ( ;!(01 - 00); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos failed ", 1, n, "");
    
        n = 0;
        for ( ;!(07654321 - 01234567); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min pos < n < max pos failed ", 1, n, "");

        n = 0;
        for ( ;!(037777700000 + 077777); )
        if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max pos failed ", 1, n, "");

    // <0

        n = 0;
        for ( ;!(00 - 01); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(01234567 - 07654321); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg < n < max neg failed ", 1, n, "");

        n = 0;
        for ( ;!(-037777700000 - 077777); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "min neg failed ", 1, n, "");

    // 0

        n = 0;
        for ( ;01 - 01; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "zero failed ", 1, n, "");

        n = 0;
        for ( ;-00 + 00; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "-zero failed ", 1, n, "");

        
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 0;    
        for ( ;!("" + " "); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "single space failed ", 1, n, "");

        n = 0;
        for ( ;!("                                 " + "                                  "); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "multiple spaces failed ", 1, n, "");

        n = 0;
        for ( ;!("" + false); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as false failed ", 1, n, "");


        n = 0;
        for ( ;!("" + 0); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "ns 0 failed ", 1, n, "");

        n = 0;
        for ( ;!("" + 1234567890); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "ns >0 failed ", 1, n, "");

        n = 0;
        for ( ;!(" " + 1234567890); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "ns >0, leading space failed ", 1, n, "");

        n = 0;
        for ( ;!(1234567890 + " "); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "ns >0, trailing space failed ", 1, n, "");

        n = 0;
        for ( ;!(-1234567890+ ""); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "ns <0 failed ", 1, n, "");

        n = 0;
        for ( ;!("f" + "oo"); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as single word failed ", 1, n, "");

        n = 0;
        for ( ;!(" f" + "oo"); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as single word, leading space failed ", 1, n, "");

        n = 0;
        for ( ;!("fo" + "o "); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as single word, trailing space failed ", 1, n, "");

        n = 0;
        for ( ;!("foo " + "bar"); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as multiple word failed ", 1, n, "");

        n = 0;
        for ( ;!(" foo" + " bar"); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as multiple word, leading space failed ", 1, n, "");

        n = 0;
        for ( ;!("foo" + " bar "); )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "as multiple word, trailing space failed ", 1, n, "");

        n = 0;
        for ( ;"" + ""; )
            if (--n) break;
            n++;

        if (n != 1) apLogFailInfo( "zls failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    for ( ;true * false; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "constants--true failed ", 1, n, "");

    n = 0;
    for ( ;!(false + ""); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "constants--1 failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 0;
    for ( ;null || false; )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "object references--null failed ", 1, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");

    n = 0;
    var obUndef;
    for ( ;!(obUndef + ""); )
        if (--n) break;
        n++;

    if (n != 1) apLogFailInfo( "undefined failed ", 1, n, "");

        
    apEndTest();

}


for019();


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
