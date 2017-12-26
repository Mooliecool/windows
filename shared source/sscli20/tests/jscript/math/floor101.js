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


var iTestID = 52701;


function obFoo() {}

var m_scen = "";

function floor101() {
 @if(@_fast)
    var sAct, sCat, sExp;
 @end
    apInitTest("floor101 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    sCat = "min pos: 1";
    sExp = 1;
    sAct = Math.floor(1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 42";
    sExp = 42;
    sAct = Math.floor(42);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 64688";
    sExp = 64688;
    sAct = Math.floor(64688);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max pos: 2147483647";
    sExp = 2147483647;
    sAct = Math.floor(2147483647);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max neg: -1";
    sExp = -1;
    sAct = Math.floor(-1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    sCat = "min neg < n < max neg: -21";
    sExp = -21;
    sAct = Math.floor(-21);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    sCat = "min neg < n < max neg: -1234567890";
    sExp = -1234567890;
    sAct = Math.floor(-1234567890);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    sCat = "min neg: -2147483647";
    sExp = -2147483647;
    sAct = Math.floor(-2147483647);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);



    sCat = "pos zero";
    sExp = 0;
    sAct = Math.floor(0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    sCat = "neg zero";
    sExp = 0;
    sAct = Math.floor(-0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";


    sCat = "min pos: 0x1";
    sExp = 1;
    sAct = Math.floor(0x1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 0xC";
    sExp = 12;
    sAct = Math.floor(0xC);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 0x562";
    sExp = 1378;
    sAct = Math.floor(0x562);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max pos: 0xFFfFffFf";
    sExp = 4294967295;
    sAct = Math.floor(0xFFfFffFf);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max neg: -0x1";
    sExp = -1;
    sAct = Math.floor(-0x1);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -0x2";
    sExp = -2;
    sAct = Math.floor(-0x2);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -0x54e6";
    sExp = -21734;
    sAct = Math.floor(-0x54e6);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -0xA347599c";
    sExp = -2739362204;
    sAct = Math.floor(-0xA347599c);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg: -0xffFffFFF";
    sExp = -4294967295;
    sAct = Math.floor(-0xffFffFFF);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.floor(0x0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.floor(-0x0);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";


    sCat = "min pos";
    sExp = 1;
    sAct = Math.floor(01);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 02";
    sExp = 2;
    sAct = Math.floor(02);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 072";
    sExp = 58;
    sAct = Math.floor(072);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 050661";
    sExp = 20913;
    sAct = Math.floor(050661);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max pos: 037777777777";
    sExp = 4294967295;
    sAct = Math.floor(037777777777);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max neg: -01";
    sExp = -1;
    sAct = Math.floor(-01);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -07";
    sExp = -7;
    sAct = Math.floor(-07);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -044221";
    sExp = -18577;
    sAct = Math.floor(-044221);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg: -037777777777";
    sExp = -4294967295;
    sAct = Math.floor(-037777777777);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.floor(00);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.floor(-00);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. num string, decimal, integer");

    m_scen = "number, decimal, integer";


    sCat = "min pos: 1";
    sExp = 1;
    sAct = Math.floor("1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos: +1";
    sExp = 1;
    sAct = Math.floor("+1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 42";
    sExp = 42;
    sAct = Math.floor("42");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 64688";
    sExp = 64688;
    sAct = Math.floor("64688");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max pos: 2147483647";
    sExp = 2147483647;
    sAct = Math.floor("2147483647");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max neg: -1";
    sExp = -1;
    sAct = Math.floor("-1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -21";
    sExp = -21;
    sAct = Math.floor("-21");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -404860839";
    sExp = -404860839;
    sAct = Math.floor("-404860839");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg: -2147483647";
    sExp = -2147483647;
    sAct = Math.floor("-2147483647");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.floor("0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.floor("-0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("6. num string, octal");

    m_scen = "num string, octal";


    sCat = "min pos";
    sExp = 1;
    sAct = Math.floor("01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 02";
    sExp = 2;
    sAct = Math.floor("02");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min pos < n < max pos: 072";
    sExp = 72;
    sAct = Math.floor("072");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max pos: 037777777777";
    sExp = 37777777777;
    sAct = Math.floor("037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "max neg: -01";
    sExp = -1;
    sAct = Math.floor("-01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -0201615";
    sExp = -201615;
    sAct = Math.floor("-0201615");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg < n < max neg: -035164014173";
    sExp = -35164014173;
    sAct = Math.floor("-035164014173");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "min neg: -037777777777";
    sExp = -37777777777;
    sAct = Math.floor("-037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "pos zero";
    sExp = 0;
    sAct = Math.floor("00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "neg zero";
    sExp = 0;
    sAct = Math.floor("-00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("9. constants");

    m_scen = "constants";

    sCat = "true";
    sExp = 1;
    sAct = Math.floor(true);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);

    sCat = "false";
    sExp = 0;
    sAct = Math.floor(false);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, null);


    //----------------------------------------------------------------------------
    apInitScenario("10. null");

    m_scen = "null";

    sCat = "";
    sExp = 0;
    sAct = Math.floor(null);

    if (sAct != sExp)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, '');


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


floor101();


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
