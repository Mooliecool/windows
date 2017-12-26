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


var iTestID = 51722;


function obFoo() {}

var m_scen = "";

function abs001() {
@if(@_fast)
    var sCat, sExp, sAct;
@end
    apInitTest("abs001 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    sCat = "min pos: 1";
    sExp = 1;
    sAct = Math.abs(1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 42";
    sExp = 42;
    sAct = Math.abs(42);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 64688";
    sExp = 64688;
    sAct = Math.abs(64688);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos: 2147483647";
    sExp = 2147483647;
    sAct = Math.abs(2147483647);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg: -1";
    sExp = 1;
    sAct = Math.abs(-1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    sCat = "min neg < n < max neg: -21";
    sExp = 21;
    sAct = Math.abs(-21);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    sCat = "min neg < n < max neg: -1234567890";
    sExp = 1234567890;
    sAct = Math.abs(-1234567890);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    sCat = "min neg: -2147483647";
    sExp = 2147483647;
    sAct = Math.abs(-2147483647);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");



    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs(0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs(-0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("2. number, decimal, float");

    m_scen = "number, decimal, float";


    sCat = "min pos: 2.2250738585072014e-308";
    sExp = 2.2250738585072014e-308;
    sAct = Math.abs(2.2250738585072014e-308);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, 16479);

    sCat = "max pos";
    sExp = 1.7976931348623158e+308;
    sAct = Math.abs(1.7976931348623158e308);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, 16479);

    sCat = "max pos";
    sExp = 1.7976931348623158e+308;
    sAct = Math.abs(1.7976931348623158e+308);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, 16479);

    sCat = "> max pos float (1.#INF)";
    sExp = 1.797693134862315807e309;
    sAct = Math.abs(1.797693134862315807e309);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg";
    sExp = 2.2250738585072012595e-308;
    sAct = Math.abs(-2.2250738585072012595e-308);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, 16479);

    sCat = "min neg";
    sExp = 1.797693134862315807e308;
    sAct = Math.abs(-1.797693134862315807e308);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, 16479);

    sCat = "< min neg float (-1.#INF)";
    sExp = 1.797693134862315807e309;
    sAct = Math.abs(-1.797693134862315807e309);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs(0.0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs(-0.0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("3. number, hexidecimal");

    m_scen = "number, hexidecimal";


    sCat = "min pos: 0x1";
    sExp = 1;
    sAct = Math.abs(0x1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 0xC";
    sExp = 12;
    sAct = Math.abs(0xC);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 0x562";
    sExp = 1378;
    sAct = Math.abs(0x562);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos: 0xFFfFffFf";
    sExp = 4294967295;
    sAct = Math.abs(0xFFfFffFf);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg: -0x1";
    sExp = 1;
    sAct = Math.abs(-0x1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0x2";
    sExp = 2;
    sAct = Math.abs(-0x2);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0x54e6";
    sExp = 21734;
    sAct = Math.abs(-0x54e6);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0xA347599c";
    sExp = 2739362204;
    sAct = Math.abs(-0xA347599c);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg: -0xffFffFFF";
    sExp = 4294967295;
    sAct = Math.abs(-0xffFffFFF);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs(0x0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs(-0x0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("4. number, octal");

    m_scen = "number, octal";


    sCat = "min pos";
    sExp = 1;
    sAct = Math.abs(01);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 02";
    sExp = 2;
    sAct = Math.abs(02);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 072";
    sExp = 58;
    sAct = Math.abs(072);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 050661";
    sExp = 20913;
    sAct = Math.abs(050661);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos: 037777777777";
    sExp = 4294967295;
    sAct = Math.abs(037777777777);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg: -01";
    sExp = 1;
    sAct = Math.abs(-01);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -07";
    sExp = 7;
    sAct = Math.abs(-07);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -044221";
    sExp = 18577;
    sAct = Math.abs(-044221);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg: -037777777777";
    sExp = 4294967295;
    sAct = Math.abs(-037777777777);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs(00);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs(-00);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";


    sCat = "min pos: 1";
    sExp = 1;
    sAct = Math.abs("1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos: +1";
    sExp = 1;
    sAct = Math.abs("+1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 42";
    sExp = 42;
    sAct = Math.abs("42");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 64688";
    sExp = 64688;
    sAct = Math.abs("64688");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos: 2147483647";
    sExp = 2147483647;
    sAct = Math.abs("2147483647");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg: -1";
    sExp = 1;
    sAct = Math.abs("-1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -21";
    sExp = 21;
    sAct = Math.abs("-21");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -404860839";
    sExp = 404860839;
    sAct = Math.abs("-404860839");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg: -2147483647";
    sExp = 2147483647;
    sAct = Math.abs("-2147483647");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs("0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs("-0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("6. num string, decimal, float");

    m_scen = "num string, decimal, float";


    sCat = "min pos: 2.2250738585072014e-308";
    sExp = 2.2250738585072014e-308;
    sAct = Math.abs("2.2250738585072014e-308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos";
    sExp = 1.7976931348623158e308;
    sAct = Math.abs("1.7976931348623158e308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos";
    sExp = 1.7976931348623158e+308;
    sAct = Math.abs("1.7976931348623158e+308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "> max pos float (1.#INF)";
    sExp = 1.797693134862315807e309;
    sAct = Math.abs("1.797693134862315807e309");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg";
    sExp = 2.2250738585072014E-308;
    sAct = Math.abs("-2.2250738585072012595e-308");

    if (sAct + "" != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg";
    sExp = 1.797693134862315807e308;
    sAct = Math.abs("-1.797693134862315807e308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "< min neg float (-1.#INF)";
    sExp = 1.797693134862315807e309;
    sAct = Math.abs("-1.797693134862315807e309");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs("0.0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs("-0.0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("8. num string, octal");

    m_scen = "num string, octal";


    sCat = "min pos";
    sExp = 1;
    sAct = Math.abs("01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 02";
    sExp = 2;
    sAct = Math.abs("02");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 072";
    sExp = 72;
    sAct = Math.abs("072");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max pos: 037777777777";
    sExp = 37777777777;
    sAct = Math.abs("037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "max neg: -01";
    sExp = 1;
    sAct = Math.abs("-01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0201615";
    sExp = 201615;
    sAct = Math.abs("-0201615");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -035164014173";
    sExp = 35164014173;
    sAct = Math.abs("-035164014173");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "min neg: -037777777777";
    sExp = 37777777777;
    sAct = Math.abs("-037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.abs("00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.abs("-00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");
    
  
    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    sCat = "true";
    sExp = 1;
    sAct = Math.abs(true);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    sCat = "false";
    sExp = 0;
    sAct = Math.abs(false);

    if (sAct != sExp)
       apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    sCat = "";
    sExp = 0;
    sAct = Math.abs(null);

    if (sAct != sExp)
       apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");



    /*****************************************************************************
    // All of the rest cause a runtime error (spec) of 'not a number'

    //----------------------------------------------------------------------------
//    apInitScenario("n. Alpha string");
    

    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, non-exec");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, exec, not inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, built-in, exec, inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, user-def, not inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. objects, user-def, inst");


    //----------------------------------------------------------------------------
//    apInitScenario("n. undefined");
    
    *****************************************************************************/


    apEndTest();
}



abs001();


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
