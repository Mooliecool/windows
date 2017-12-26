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


var iTestID = 53544;


function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    var nEpsilon = 2.2204460492503131e-016;
    var fFailed = false;
    if (Number.NaN == vExp)
    {
        if (!isNaN(vAct))
            fFailed = true;
    }
    else
        if ( 0==vExp ? Math.abs(vExp-vAct) > nEpsilon : Math.abs(vExp-vAct)/vExp > nEpsilon )
            fFailed = true;

    if (fFailed)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", vExp, vAct,bugNum);
}

var m_scen = "";

function sqrt001()
{
 
    apInitTest("sqrt001 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.sqrt(1), 1, null);
    verify("pos min < n < VT_UI1 pos max", Math.sqrt(127), 11.269427669584644, null);
    verify("VT_UI1 pos max", Math.sqrt(255), 15.968719422671311, null);
    verify("VT_I2 pos excl-min", Math.sqrt(256), 16, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.sqrt(12869), 113.44161493913951, null);
    verify("VT_I2 pos max", Math.sqrt(32767), 181.01657382681842, null);
    verify("VT_I4 pos excl-min", Math.sqrt(32768), 181.01933598375618, null);
    verify("VT_I4 pos excl-min < n < pos max", Math.sqrt(1143483646), 33815.43502603508, null);
    verify("pos max", Math.sqrt(2147483647), 46340.950001051984, null);
    
    verify("neg max", Math.sqrt(-1), Number.NaN, null);
    verify("neg max > n > VT_UI1 neg min", Math.sqrt(-201), Number.NaN, null);
    verify("VT_UI1 neg min", Math.sqrt(-256), Number.NaN, null);
    verify("VT_I2 neg excl-max", Math.sqrt(-257), Number.NaN, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.sqrt(-12869), Number.NaN, null);
    verify("VT_I2 neg min", Math.sqrt(-32768), Number.NaN, null);
    verify("VT_I4 neg excl-max", Math.sqrt(-32769), Number.NaN, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.sqrt(-1143483646), Number.NaN, null);
    verify("neg min", Math.sqrt(-2147483648), Number.NaN, null);
    
    verify("zero", Math.sqrt(0), 0, null); 

    
    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", Math.sqrt(0xabcdef), 3355.4992177021886, null);
    verify("pos hex excl-min: 0x80000000", Math.sqrt(0x80000000), 46340.95001184158, null);
    verify("max pos: 0xffffffff", Math.sqrt(0xffffffff), 65535.999992370605, null);

    verify("neg max > n > neg hex excl-max", Math.sqrt(-0xabcdef), Number.NaN, null);
    verify("pos hex excl-min: -0x80000001", Math.sqrt(-0x80000001), Number.NaN, null);
    verify("min neg: -0xffffffff", Math.sqrt(-0xffffffff), Number.NaN, null);

    verify("zero", Math.sqrt(0x0), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", Math.sqrt(01234567), 585.1418631408968, null);
    verify("pos hex excl-min: 020000000000", Math.sqrt(020000000000), 46340.95001184158, null);
    verify("max pos: 037777777777", Math.sqrt(037777777777), 65535.999992370605, null);

    verify("neg max > n > neg hex excl-max", Math.sqrt(-076543210), Number.NaN, null);
    verify("pos hex excl-min: -020000000001", Math.sqrt(-020000000001), Number.NaN, null);
    verify("min neg: -037777777777", Math.sqrt(-037777777777), Number.NaN, null);

    verify("pos zero", Math.sqrt(00), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    verify("pos min", Math.sqrt(2.2250738585072014e-308), 1.4916681462400413e-154, null);
    verify("pos min < n < VT_R4 pos max", Math.sqrt(9876543210000), 3142696.8052931866, null);
    verify("VT_R4 pos max", Math.sqrt(3.402823466e38), 18446743522909403000, null);
    verify("VT_R8 pos excl-min", Math.sqrt(3.402823467e38), 18446743525619909000, null);
    verify("VT_R8 pos excl-min < n < pos max", Math.sqrt(4.43269743388067e107), 6.657850579489352e53, null);
    verify("pos max", Math.sqrt(1.7976931348623157e308), 1.3407807929942596e154, null);

    verify("neg max", Math.sqrt(-2.2250738585072012595e-308), 0, null);
    verify("neg max > n > VT_R4 neg min", Math.sqrt(-66100533593023300000), Number.NaN, null);
    verify("VT_R4 neg min", Math.sqrt(-3.402823466e38), Number.NaN, null);
    verify("VT_R8 neg excl-max", Math.sqrt(-3.402823467e38), Number.NaN, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.sqrt(-7.17170763763262e243), Number.NaN, null);
    verify("neg min", Math.sqrt(-1.7976931348623157e308), Number.NaN, null);

    verify("zero", Math.sqrt(0), 0, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.sqrt("1"), 1, null);
    verify("pos min < n < VT_UI1 pos max", Math.sqrt("127"), 11.269427669584644, null);
    verify("VT_UI1 pos max", Math.sqrt("255"), 15.968719422671311, null);
    verify("VT_I2 pos excl-min", Math.sqrt("256"), 16, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.sqrt("12869"), 113.44161493913951, null);
    verify("VT_I2 pos max", Math.sqrt("32767"), 181.01657382681842, null);
    verify("VT_I4 pos excl-min", Math.sqrt("32768"), 181.01933598375618, null);
    verify("VT_I4 pos excl-min < n < pos max", Math.sqrt("1143483646"), 33815.43502603508, null);
    verify("pos max", Math.sqrt("2147483647"), 46340.950001051984, null);

    verify("neg max", Math.sqrt("-1"), Number.NaN, null);
    verify("neg max > n > VT_UI1 neg min", Math.sqrt("-201"), Number.NaN, null);
    verify("VT_UI1 neg min", Math.sqrt("-256"), Number.NaN, null);
    verify("VT_I2 neg excl-max", Math.sqrt("-257"), Number.NaN, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.sqrt("-12869"), Number.NaN, null);
    verify("VT_I2 neg min", Math.sqrt("-32768"), Number.NaN, null);
    verify("VT_I4 neg excl-max", Math.sqrt("-32769"), Number.NaN, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.sqrt("-1143483646"), Number.NaN, null);
    verify("neg min", Math.sqrt("-2147483648"), Number.NaN, null);

    verify("zero", Math.sqrt("0"), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    verify("pos min < n < pos hex excl-min", Math.sqrt("01234567"), 1111.1107055554814, null);
    verify("pos hex excl-min: 020000000000", Math.sqrt("020000000000"), 141421.35623730952, null);
    verify("max pos: 037777777777", Math.sqrt("037777777777"), 194365.0631595092, null);

    verify("neg max > n > neg hex excl-max", Math.sqrt("-076543210"), Number.NaN, null);
    verify("pos hex excl-min: -020000000001", Math.sqrt("-020000000001"), Number.NaN, null);
    verify("min neg: -037777777777", Math.sqrt("-037777777777"), Number.NaN, null);

    verify("pos zero", Math.sqrt("00"), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. num string, float");

    m_scen = "number, float";

    verify("pos min", Math.sqrt("2.2250738585072014e-308"), 1.4916681462400413e-154, null);
    verify("pos min < n < VT_R4 pos max", Math.sqrt("9.87654321e12"), 3142696.8052931866, null);
    verify("VT_R4 pos max", Math.sqrt("3.402823466e+38"), 18446743522909403000, null);
    verify("VT_R8 pos excl-min", Math.sqrt("3.402823467e+38"), 18446743525619909000, null);
    verify("VT_R8 pos excl-min < n < pos max", Math.sqrt("4.43269743388067e107"), 6.657850579489352e53, null);
    verify("pos max", Math.sqrt("1.7976931348623158e308"), 1.3407807929942596e154, null);

    verify("neg max", Math.sqrt("-2.2250738585072012595e-308"), 0, null);
    verify("neg max > n > VT_R4 neg min", Math.sqrt("-6.61005335930233e19"), Number.NaN, null);
    verify("VT_R4 neg min", Math.sqrt("-3.402823466e+38"), Number.NaN, null);
    verify("VT_R8 neg excl-max", Math.sqrt("-3.402823467e+38"), Number.NaN, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.sqrt("-7.17170763763262e243"), Number.NaN, null);
    verify("neg min", Math.sqrt("-1.797693134862315807e308"), Number.NaN, null);

    verify("zero", Math.sqrt("0.0"), 0, null);

   
    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true",Math.sqrt(true), 1, null);
    verify("false",Math.sqrt(false), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("",Math.sqrt(null), 0, null);


    /*****************************************************************************
    // All of the rest cause a runtime error (spec) of 'not a number'

    //----------------------------------------------------------------------------
    // apInitScenario("n. Alpha string");
    

    //----------------------------------------------------------------------------
    //apInitScenario("n. objects, built-in, non-exec");


    //----------------------------------------------------------------------------
    //apInitScenario("n. objects, built-in, exec, not inst");


    //----------------------------------------------------------------------------
    //apInitScenario("n. objects, built-in, exec, inst");


    //----------------------------------------------------------------------------
    //apInitScenario("n. objects, user-def, not inst");


    //----------------------------------------------------------------------------
    //apInitScenario("n. objects, user-def, inst");


    //----------------------------------------------------------------------------
    //apInitScenario("n. undefined");
    *****************************************************************************/
    

    apEndTest();

}


sqrt001();


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
