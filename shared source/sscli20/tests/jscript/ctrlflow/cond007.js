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


var iTestID = 51806;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function cond007() {

    apInitTest("cond007 ");

    var n, tmp;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;        
    tmp = Math;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--Math failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 0;
    tmp = Array;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp = Boolean;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp = Date;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp = Number;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n,"");

    n = 0;
    tmp = Object;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 0;
    tmp = new Array();
    if ((tmp + "" ? ++n : --n) != -1) apLogFailInfo( "executable object references--Array failed ", -1, n,"");

    n = 0;
    tmp = new Boolean();
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Boolean failed ", 1, n,"");

    n = 0;
    tmp = new Date();
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Date failed ", 1, n,"");

    n = 0;
    tmp = new Number();
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Number failed ", 1, n,"");

    n = 0;
    tmp = new Object();
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--Object failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 0;
    tmp = obFoo;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    n = 0;
    tmp = new obFoo();
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "executable object references--instantiated user-defined failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        tmp = 1;
        if ((tmp - 0 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 1234567890;
        if ((tmp - 123456789 ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 2147400000;
        if ((tmp + 83647 ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 0;
        if ((tmp - 1 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 123456789;
        if ((tmp - 1234567890 ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = -2147400000;
        if ((tmp - 83647 ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp = 1;
        if ((tmp - 1 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp = -0;
        if ((tmp + 0 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 0;    
        tmp = 2.225073858507202e-308;
        if ((tmp + 0 ? ++n : --n) != 1) apLogFailInfo( "min pos float failed ", 1, n,"");
    
        n = 0;
        tmp = 1.2345678e90;
        if ((tmp / 9.98 ? ++n : --n) != 1) apLogFailInfo( "min pos float < n < max pos float failed ", 1, n,"");

        n = 0;
        tmp = 1.0e308;
        if ((tmp + 7.97693134862315807e307 ? ++n : --n) != 1) apLogFailInfo( "max pos float failed ", 1, n,"");

        n = 0;
        tmp = 1.797693134862315807e308;
        if ((tmp + 1.0e308 ? ++n : --n) != 1) apLogFailInfo( "> max pos float (1.#INF) failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 0;
        if ((tmp + -2.225073858507202e-308 ? ++n : --n) != 1) apLogFailInfo( "max neg float failed ", 1, n,"");

        n = 0;
        tmp = -1.2345678e90;
        if ((tmp * 2 ? ++n : --n) != 1) apLogFailInfo( "min neg float < n < max neg float failed ", 1, n,"");

        n = 0;
        tmp = 1.797693134862315807e308;
        if ((tmp * -1 ? ++n : --n) != 1) apLogFailInfo( "min neg float failed ", 1, n,"");

        n = 0;
        tmp = 1.797693134862315807e309;
        if ((tmp - 1.999e307 ? ++n : --n) != 1) apLogFailInfo( "< min neg float (-1.#INF) failed ", 1, n,"");

    // 0.0

        n = 0;
        tmp = 1.9;
        if ((tmp - 1.9 ? ++n : --n) != -1) apLogFailInfo( "zero float failed ", -1, n,"");

        n = 0;
        tmp = 0.0;
        if ((tmp * -1.0 ? ++n : --n) != -1) apLogFailInfo( "-zero float failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 0;    
        tmp = 0x1;
        if ((tmp - 0x0 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 0x9ABCDEF;
        if ((tmp - 0x2468ACE ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 0xFFFF0000;
        if ((tmp + 0xFFFF ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 0x0;
        if ((tmp - 0x1 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 0x2468ACE;
        if ((tmp - 0x9ABCDEF ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = -0xFFFF0000;
        if ((tmp - 0xFFFF ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp = 0x1;
        if ((tmp - 0x1 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp = -0x0;
        if ((tmp + 0x0 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 0;    
        tmp = 01;
        if ((tmp - 00 ? ++n : --n) != 1) apLogFailInfo( "min pos int failed ", 1, n,"");
    
        n = 0;
        tmp = 07654321;
        if ((tmp - 01234567 ? ++n : --n) != 1) apLogFailInfo( "min pos int < n < max pos int failed ", 1, n,"");

        n = 0;
        tmp = 037777700000;
        if ((tmp + 077777 ? ++n : --n) != 1) apLogFailInfo( "max pos int failed ", 1, n,"");

    // <0

        n = 0;
        tmp = 00;
        if ((tmp - 01 ? ++n : --n) != 1) apLogFailInfo( "max neg int failed ", 1, n,"");

        n = 0;
        tmp = 01234567;
        if ((tmp - 07654321 ? ++n : --n) != 1) apLogFailInfo( "min neg int < n < max neg int failed ", 1, n,"");

        n = 0;
        tmp = -037777700000;
        if ((tmp - 077777 ? ++n : --n) != 1) apLogFailInfo( "min neg int failed ", 1, n,"");

    // 0

        n = 0;
        tmp = 01;
        if ((tmp - 01 ? ++n : --n) != -1) apLogFailInfo( "zero int failed ", -1, n,"");

        n = 0;
        tmp = -00;
        if ((tmp + 00 ? ++n : --n) != -1) apLogFailInfo( "-zero int failed ", -1, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 0;    
        tmp = "";
        if ((tmp + " " ? ++n : --n) != 1) apLogFailInfo( "single space failed ", 1, n,"");

        n = 0;
        if (("                                 " + "                                  " ? ++n : --n) != 1) apLogFailInfo( "multiple spaces failed ", 1, n,"");

        n = 0;
        tmp = "";
        if ((tmp + false ? ++n : --n) != 1) apLogFailInfo( "as false failed ", 1, n,"");

        n = 0;
        tmp = "";
        if ((tmp + 0 ? ++n : --n) != 1) apLogFailInfo( "ns 0 failed ", 1, n,"");

        n = 0;
        tmp = "";
        if ((tmp + 1234567890 ? ++n : --n) != 1) apLogFailInfo( "ns >0 failed ", 1, n,"");

        n = 0;
        tmp = " ";
        if ((tmp + 1234567890 ? ++n : --n) != 1) apLogFailInfo( "ns >0, leading space failed ", 1, n,"");

        n = 0;
        tmp = 1234567890;
        if ((tmp + " " ? ++n : --n) != 1) apLogFailInfo( "ns >0, trailing space failed ", 1, n,"");

        n = 0;
        tmp = -1234567890;
        if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "ns <0 failed ", 1, n,"");

        n = 0;
        tmp = "f";
        if ((tmp + "oo" ? ++n : --n) != 1) apLogFailInfo( "as single word failed ", 1, n,"");

        n = 0;
        tmp = " f";
        if ((tmp + "oo" ? ++n : --n) != 1) apLogFailInfo( "as single word, leading space failed ", 1, n,"");

        n = 0;
        tmp = "fo";
        if ((tmp + "o " ? ++n : --n) != 1) apLogFailInfo( "as single word, trailing space failed ", 1, n,"");

        n = 0;
        tmp = "foo ";
        if ((tmp + "bar" ? ++n : --n) != 1) apLogFailInfo( "as multiple word failed ", 1, n,"");

        n = 0;
        tmp = " foo";
        if ((tmp + " bar" ? ++n : --n) != 1) apLogFailInfo( "as multiple word, leading space failed ", 1, n,"");

        n = 0;
        tmp = "foo";
        if ((tmp + " bar " ? ++n : --n) != 1) apLogFailInfo( "as multiple word, trailing space failed ", 1, n,"");

        n = 0;
        tmp = "";
        if ((tmp + "" ? ++n : --n) != -1) apLogFailInfo( "zls failed ", -1, n,"");

    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    tmp = true;
    if ((tmp * false ? ++n : --n) != -1) apLogFailInfo( "constants--true failed ", -1, n,"");


    n = 0;
    tmp = false;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "constants--false failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 0;
    tmp = null;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--null failed ", 1, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");

    var obUndef;
    n = 0;
    tmp = obUndef;
    if ((tmp + "" ? ++n : --n) != 1) apLogFailInfo( "non-executable object references--undefined failed ", 1, n,"");

    apEndTest();

}


cond007();


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
