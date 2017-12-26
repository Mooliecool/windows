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


var iTestID = 53146;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

var m_scen = "";

function psflt01b() {
@if(@_fast)
    var sCat,sExp,sAct;
@end
    apInitTest("psFlt01b ");

    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    apInitScenario("6. num string, decimal, float");

    m_scen = "num string, decimal, float";


    sCat = "min pos: 2.2250738585072014e-308";
    sExp =  2.2250738585072014e-308;
    sAct = parseFloat("2.2250738585072014e-308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max pos";
    sExp = 1.7976931348623158e308;
    sAct = parseFloat("1.7976931348623158e308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max pos";
    sExp = 1.7976931348623158e+308;
    sAct = parseFloat("1.7976931348623158e+308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "> max pos float (1.#INF)";
    sExp = 1.797693134862315807e309;
    sAct = parseFloat("1.797693134862315807e309");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max neg";
    sExp = -2.2250738585072014E-308;
    sAct = parseFloat("-2.2250738585072012595e-308");

    if (sAct + "" != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg";
    sExp = -1.797693134862315807e308;
    sAct = parseFloat("-1.797693134862315807e308");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "< min neg float (-1.#INF)";
    sExp = -1.797693134862315807e309;
    sAct = parseFloat("-1.797693134862315807e309");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = parseFloat("0.0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = parseFloat("-0.0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, hexidecimal");

    m_scen = "num string, hexidecimal";


    sCat = "min pos: 0x1";
    sExp = 0;
    sAct = parseFloat("0x1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 0xC";
    sExp = 0;
    sAct = parseFloat("0xC");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 0xA7a864cf";
    sExp = 0;
    sAct = parseFloat("0xA7a864cf");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max pos: 0xFFfFffFf";
    sExp = 0;
    sAct = parseFloat("0xFFfFffFf");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max neg: -0x1";
    sExp = 0;
    sAct = parseFloat("-0x1");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0x2";
    sExp = 0;
    sAct = parseFloat("-0x2");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0xb15870";
    sExp = 0;
    sAct = parseFloat("-0xb15870");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg: -0xffFffFFF";
    sExp = 0;
    sAct = parseFloat("-0xffFffFFF");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp = 0;
    sAct = parseFloat("0x0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp = 0;
    sAct = parseFloat("-0x0");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("8. num string, octal");

    m_scen = "num string, octal";


    sCat = "min pos";
    sExp = 1;
    sAct = parseFloat("01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 02";
    sExp =  2;
    sAct = parseFloat("02");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min pos < n < max pos: 072";
    sExp =  72;
    sAct = parseFloat("072");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max pos: 037777777777";
    sExp =  37777777777;
    sAct = parseFloat("037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "max neg: -01";
    sExp =  -1;
    sAct = parseFloat("-01");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -0201615";
    sExp =  -201615;
    sAct = parseFloat("-0201615");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg < n < max neg: -035164014173";
    sExp =  -35164014173;
    sAct = parseFloat("-035164014173");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "min neg: -037777777777";
    sExp =  -37777777777;
    sAct = parseFloat("-037777777777");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "pos zero";
    sExp =  0;
    sAct = parseFloat("00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    sCat = "neg zero";
    sExp =  0;
    sAct = parseFloat("-00");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" -- "+sCat+" "+" failed", sExp, sAct, "");

    apEndTest();
}


psflt01b();


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
