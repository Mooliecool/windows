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


var iTestID = 51807;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function cond008() {

    apInitTest("cond008 ");

    var n, tmp;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;        
    tmp = "";
    if ((Math + tmp ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--Math failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 0;
    tmp = "";
    if ((Array + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp = "";
    if ((Boolean + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp = "";
    if ((Date + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp = "";
    if ((Number + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "144877");

    n = 0;
    tmp = "";
    if ((Object + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 0;
    tmp = "";
    if (((new Array()) + tmp ? ++n : --n) != -1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp = "";
    if (((new Boolean()) + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp = "";
    if (((new Date()) + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp = "";
    if (((new Number()) + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "144877");

    n = 0;
    tmp = "";
    if (((new Object()) + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 0;
    tmp = "";
    if ((obFoo + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    n = 0;
    tmp = "";
    if (((new obFoo()) + tmp ? ++n : --n) != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        tmp = 0;
        if ((1 - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 123456789;
        if ((1234567890 - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 83647;
        if ((2147400000 + tmp ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 1;
        if ((0 - tmp ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 1234567890;
        if ((123456789 - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = 83647;
        if ((-2147400000 - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        if ((1 - 1 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        if ((-0 + 0 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 0;    
        tmp = 0;
        if ((2.225073858507202e-308 + tmp ? ++n : --n) != 1) apLogFailInfo( "min pos float failed ", 1, n,"");
    
        n = 0;
        tmp = 9.98;
        if ((1.2345678e90 / tmp ? ++n : --n) != 1) apLogFailInfo( "min pos float < n < max pos float failed ", 1, n,"");

        n = 0;
        tmp = 7.97693134862315807e307;
        if ((1.0e308 + tmp ? ++n : --n) != 1) apLogFailInfo( "max pos float failed ", 1, n,"");

        n = 0;
        tmp = 1.0e308;
        if ((1.797693134862315807e308 + tmp ? ++n : --n) != 1) apLogFailInfo( "> max pos float (1.#INF) failed ", 1, n,"");

    // <0

        n = 0;
        tmp = -2.225073858507202e-308;
        if ((0 + tmp ? ++n : --n) != 1) apLogFailInfo( "max neg float failed ", 1, n,"");

        n = 0;
        tmp = 2;
        if ((-1.2345678e90 * tmp ? ++n : --n) != 1) apLogFailInfo( "min neg float < n < max neg float failed ", 1, n,"");

        n = 0;
        tmp = -1;
        if ((1.797693134862315807e308 * tmp ? ++n : --n) != 1) apLogFailInfo( "min neg float failed ", 1, n,"");

        n = 0;
        tmp = 1.999e307;
        if ((1.797693134862315807e309 - tmp ? ++n : --n) != 1) apLogFailInfo( "< min neg float (-1.#INF) failed ", 1, n,"");

    // 0.0

        n = 0;
        tmp = 1.9;
        if ((1.9 - tmp ? ++n : --n) != -1) apLogFailInfo( "zero float failed ", -1, n,"");

        n = 0;
        tmp = -1.0;
        if ((0.0 * tmp ? ++n : --n) != -1) apLogFailInfo( "-zero float failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 0;    
        tmp = 0x0;
        if ((0x1 - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 0x2468ACE;
        if ((0x9ABCDEF - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 0xFFFF;
        if ((0xFFFF0000 + tmp ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 0x1;
        if ((0x0 - tmp ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 0x9ABCDEF;
        if ((0x2468ACE - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = 0xFFFF;
        if ((-0xFFFF0000 - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp = 0x1;
        if ((0x1 - tmp ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp = 0x0;
        if ((-0x0 + tmp ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 0;    
        tmp = 00;
        if ((01 - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 01234567;
        if ((07654321 - tmp ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 077777;
        if ((037777700000 + tmp ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 01;
        if ((00 - tmp ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 07654321;
        if ((01234567 - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = 077777;
        if ((-037777700000 - tmp ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp = 01;
        if ((01 - tmp ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp = 00;
        if ((-00 + tmp ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 0;    
        tmp = " ";
        if (("" + tmp ? ++n : --n) != 1) apLogFailInfo( "single space failed ", 1, n,"");

        n = 0;
        if (("                                 " + "                                  " ? ++n : --n) != 1) apLogFailInfo( "multiple spaces failed ", 1, n,"");

        n = 0;
        tmp = false;
        if (("" + tmp ? ++n : --n) != 1) apLogFailInfo( "as false failed ", 1, n,"");

        n = 0;
        tmp = 0;
        if (("" + tmp ? ++n : --n) != 1) apLogFailInfo( "ns 0 failed ", 1, n,"");

        n = 0;
        tmp = 1234567890;
        if (("" + tmp ? ++n : --n) != 1) apLogFailInfo( "ns >0 failed ", 1, n,"");

        n = 0;
        tmp = 1234567890;
        if ((" " + tmp ? ++n : --n) != 1) apLogFailInfo( "ns >0, leading space failed ", 1, n,"");

        n = 0;
        tmp = " ";
        if ((1234567890 + tmp ? ++n : --n) != 1) apLogFailInfo( "ns >0, trailing space failed ", 1, n,"");

        n = 0;
        tmp = "";
        if ((-1234567890+ tmp ? ++n : --n) != 1) apLogFailInfo( "ns <0 failed ", 1, n,"");

        n = 0;
        tmp = "oo";
        if (("f" + tmp ? ++n : --n) != 1) apLogFailInfo( "as single word failed ", 1, n,"");

        n = 0;
        tmp = "oo";
        if ((" f" + tmp ? ++n : --n) != 1) apLogFailInfo( "as single word, leading space failed ", 1, n,"");

        n = 0;
        tmp = "o ";
        if (("fo" + tmp ? ++n : --n) != 1) apLogFailInfo( "as single word, trailing space failed ", 1, n,"");

        n = 0;
        tmp = "bar";
        if (("foo " + tmp ? ++n : --n) != 1) apLogFailInfo( "as multiple word failed ", 1, n,"");

        n = 0;
        tmp = " bar";
        if ((" foo" + tmp ? ++n : --n) != 1) apLogFailInfo( "as multiple word, leading space failed ", 1, n,"");

        n = 0;
        tmp = " bar ";
        if (("foo" + tmp ? ++n : --n) != 1) apLogFailInfo( "as multiple word, trailing space failed ", 1, n,"");

        n = 0;
        tmp = "";
        if (("" + tmp ? ++n : --n) != -1) apLogFailInfo( "zls failed ", -1, n,"");

    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    tmp = false;
    if ((true * tmp ? ++n : --n) != -1) apLogFailInfo( "constants--true failed ", -1, n,"");


    n = 0;
    tmp = "";
    if ((false + tmp ? ++n : --n) != 1) apLogFailInfo( "constants--false failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 0;
    tmp = "";
    if ((null + tmp ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--null failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");

    var obUndef;
    n = 0;
    tmp = "";
    if ((obUndef + tmp ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--undefined failed ", 1, n,"");

    apEndTest();

}


cond008();


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
