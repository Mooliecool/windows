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


var iTestID = 51808;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function cond009() {

    apInitTest("cond009 ");

    var n, tmp1, tmp2;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;        
    tmp1 = Math; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--Math failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 0;
    tmp1 = Array; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp1 = Boolean; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp1 = Date; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp1 = Number; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "144877");

    n = 0;
    tmp1 = Object; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 0;
    tmp1 = new Array(); tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != -1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp1 = new Boolean(); tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp1 = new Date(); tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp1 = new Number() ; tmp2 = "";
    if ((tmp1+ tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n, "144877");

    n = 0;
    tmp1 = new Object(); tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 0;
    tmp1 = obFoo; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    n = 0;
    tmp1 = new obFoo(); tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        tmp1 = 1; tmp2 = 0;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp1 = 1234567890; tmp2 = 123456789;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp1 = 2147400000; tmp2 = 83647;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp1 = 0; tmp2 = 1;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = 123456789; tmp2 = 1234567890;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = -2147400000; tmp2 = 83647;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp1 = 1; tmp2 = 1;
        if ((tmp1 - tmp2 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp1 = -0; tmp2 = 0;
        if ((tmp1 + tmp2 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 0;    
        tmp1 = 2.225073858507202e-308; tmp2 = 0;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos float failed ", 1, n,"");
    
        n = 0;
        tmp1 = 1.2345678e90; tmp2 = 9.98;
        if ((tmp1 / tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos float < n < max pos float failed ", 1, n,"");

        n = 0;
        tmp1 = 1.0e308; tmp2 = 7.97693134862315807e307;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "max pos float failed ", 1, n,"");

        n = 0;
        tmp1 = 1.797693134862315807e308; tmp2 = 1.0e308;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "> max pos float (1.#INF) failed ", 1, n,"");

    // <0

        n = 0;
        tmp1 = 0; tmp2 = -2.225073858507202e-308;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "max neg float failed ", 1, n,"");

        n = 0;
        tmp1 = -1.2345678e90; tmp2 = 2;
        if ((tmp1 * tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg float < n < max neg float failed ", 1, n,"");

        n = 0;
        tmp1 = 1.797693134862315807e308; tmp2 = -1;
        if ((tmp1 * tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg float failed ", 1, n,"");

        n = 0;
        tmp1 = 1.797693134862315807e309; tmp2 = 1.999e307;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "< min neg float (-1.#INF) failed ", 1, n,"");

    // 0.0

        n = 0;
        tmp1 = 1.9; tmp2 = 1.9;
        if ((tmp1 - tmp2 ? ++n : --n) != -1) apLogFailInfo( "zero float failed ", -1, n,"");

        n = 0;
        tmp1 = 0.0; tmp2 = -1.0;
        if ((tmp1 * tmp2 ? ++n : --n) != -1) apLogFailInfo( "-zero float failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 0;    
        tmp1 = 0x1; tmp2 = 0x0;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp1 = 0x9ABCDEF; tmp2 = 0x2468ACE;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp1 = 0xFFFF0000; tmp2 = 0xFFFF;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp1 = 0x0; tmp2 = 0x1;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = 0x2468ACE; tmp2 = 0x9ABCDEF;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = -0xFFFF0000; tmp2 = 0xFFFF;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp1 = 0x1; tmp2 = 0x1;
        if ((tmp1 - tmp2 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp1 = -0x0; tmp2 = 0x0;
        if ((tmp1 + tmp2 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 0;    
        tmp1 = 01; tmp2 = 00;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp1 = 07654321; tmp2 = 01234567;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp1 = 037777700000; tmp2 = 077777;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp1 = 00; tmp2 = 01;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = 01234567; tmp2 = 07654321;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp1 = -037777700000; tmp2 = 077777;
        if ((tmp1 - tmp2 ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp1 = 01; tmp2 = 01;
        if ((tmp1 - tmp2 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp1 = -00; tmp2 = 00;
        if ((tmp1 + tmp2 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 0;    
        tmp1 = ""; tmp2 = " ";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "single space failed ", 1, n,"");

        n = 0;
        if (("                                 " + "                                  " ? ++n : --n) != 1) apLogFailInfo( "multiple spaces failed ", 1, n,"");

        n = 0;
        tmp1 = ""; tmp2 = false;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as false failed ", 1, n,"");

        n = 0;
        tmp1 = ""; tmp2 = 0;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "ns 0 failed ", 1, n,"");

        n = 0;
        tmp1 = ""; tmp2 = 1234567890;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "ns >0 failed ", 1, n,"");

        n = 0;
        tmp1 = " "; tmp2 = 1234567890;
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "ns >0, leading space failed ", 1, n,"");

        n = 0;
        tmp1 = 1234567890; tmp2 = " ";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "ns >0, trailing space failed ", 1, n,"");

        n = 0;
        tmp1 = -1234567890; tmp2 = "";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "ns <0 failed ", 1, n,"");

        n = 0;
        tmp1 = "f"; tmp2 = "oo";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as single word failed ", 1, n,"");

        n = 0;
        tmp1 = " f"; tmp2 = "oo";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as single word, leading space failed ", 1, n,"");

        n = 0;
        tmp1 = "fo"; tmp2 = "o ";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as single word, trailing space failed ", 1, n,"");

        n = 0;
        tmp1 = "foo "; tmp2 = "bar";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as multiple word failed ", 1, n,"");

        n = 0;
        tmp1 = " foo"; tmp2 = " bar";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as multiple word, leading space failed ", 1, n,"");

        n = 0;
        tmp1 = "foo"; tmp2 = " bar ";
        if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "as multiple word, trailing space failed ", 1, n,"");

        n = 0;
        tmp1 = ""; tmp2 = "";
        if ((tmp1 + tmp2 ? ++n : --n) != -1) apLogFailInfo( "zls failed ", -1, n,"");

    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    tmp1 = true; tmp2 = false;
    if ((tmp1 * tmp2 ? ++n : --n) != -1) apLogFailInfo( "constants--true failed ", -1, n,"");


    n = 0;
    tmp1 = false; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "constants--false failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 0;
    tmp1 = null; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--null failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");

    var obUndef;
    n = 0;
    tmp1 = obUndef; tmp2 = "";
    if ((tmp1 + tmp2 ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--undefined failed ", 1, n,"");

    apEndTest();

}


cond009();


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
