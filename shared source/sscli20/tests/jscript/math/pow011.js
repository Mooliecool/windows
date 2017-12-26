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


var iTestID = 53119;


function verify(sCat, vAct, vExp, options, bugNum){
 @if(@_fast)
    var nEpsilon;
 @end
    if (bugNum == null) bugNum = "";

    if (vExp==null)
    {
      if (!isNaN(vAct))
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", "NaN", vAct,bugNum);
      return;
    }

    if (typeof vExp=="string" && vExp=="Infinity")
    {
        if (!(vAct > 1.7976931348623157e+308))
            apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", vExp, vAct,bugNum);
    }
    else if ( typeof vExp=="number" && ((""+vExp).indexOf(".") != -1) ) /* use epsilon */
    {
        if ((Math.abs(vExp-vAct)/vExp) > (nEpsilon=2.2204460492503131e-016))
            apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+
            " failed w/ epsilon of "+nEpsilon, vExp, vAct,bugNum);
    }
    else
    {
        if (vAct != vExp)
            apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", vExp, vAct,bugNum);
    }
}

var m_scen = "";

function pow011() {
 
    apInitTest("pow011 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.pow(2,1), 2, null);
    verify("pos min < n < VT_UI1 pos max", Math.pow(2,127), 1.7014118346046923e+038, null);
    verify("VT_UI1 pos max", Math.pow(2,255), 5.7896044618658098e+076, null);
    verify("VT_I2 pos excl-min", Math.pow(2,256), 1.157920892373162e+077, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.pow(2,12869), "Infinity", null);
    verify("VT_I2 pos max", Math.pow(2,32767), "Infinity", null);
    verify("VT_I4 pos excl-min", Math.pow(2,32768), "Infinity", null);
    verify("VT_I4 pos excl-min < n < pos max", Math.pow(2,1143483646), "Infinity", null);
    verify("pos max", Math.pow(2,2147483647), "Infinity", null);

    verify("neg max", Math.pow(2,-1), 0.5, null);
    verify("neg max > n > VT_UI1 neg min", Math.pow(2,-201), 3.1115076389305709e-061, null);
    verify("VT_UI1 neg min", Math.pow(2,-256), 8.6361685550944446e-078, null);
    verify("VT_I2 neg excl-max", Math.pow(2,-257), 4.3180842775472223e-078, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.pow(2,-12869), 0, null);
    verify("VT_I2 neg min", Math.pow(2,-32768), 0, null);
    verify("VT_I4 neg excl-max", Math.pow(2,-32769), 0, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.pow(2,-1143483646), 0, null);
    verify("neg min", Math.pow(2,-2147483648), 0, null);

    verify("zero", Math.pow(2,0), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", Math.pow(2,11259375), "Infinity", null);
    verify("pos hex excl-min: 0x80000000", Math.pow(2,2147483648), "Infinity", null);
    verify("max pos: 0xffffffff", Math.pow(2,4294967295), "Infinity", null);

    verify("neg max > n > neg hex excl-max", Math.pow(2,-11259375), 0, null);
    verify("pos hex excl-min: -0x80000001", Math.pow(2,-2147483649), 0, null);
    verify("min neg: -0xffffffff", Math.pow(2,-4294967295), 0, null);

    verify("zero", Math.pow(2,0), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", Math.pow(2,342391), "Infinity", null);
    verify("pos hex excl-min: 020000000000", Math.pow(2,2147483648), "Infinity", null);
    verify("max pos: 037777777777", Math.pow(2,4294967295), "Infinity", null);

    verify("neg max > n > neg hex excl-max", Math.pow(2,-16434824), 0, null);
    verify("pos hex excl-min: -020000000001", Math.pow(2,-2147483649), 0, null);
    verify("min neg: -037777777777", Math.pow(2,-4294967295), 0, null);

    verify("pos zero", Math.pow(2,0), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    verify("pos min", Math.pow(2,2.2250738585072014e-308), 1, null);
    verify("pos min < n < VT_R4 pos max", Math.pow(2,98765.432100000005), "Infinity", null);
    verify("VT_R4 pos max", Math.pow(2,3.4028234660000002e+038), "Infinity", null);
    verify("VT_R8 pos excl-min", Math.pow(2,3.4028234670000002e+038), "Infinity", null);
    verify("VT_R8 pos excl-min < n < pos max", Math.pow(2,4.4326974338806703e+107), "Infinity", null);
    verify("pos max", Math.pow(2,1.7976931348623157e+308), "Infinity", null);

    verify("neg max", Math.pow(2,0), 1, null);
    verify("neg max > n > VT_R4 neg min", Math.pow(2,-6610.0533593023301), 0, null);
    verify("VT_R4 neg min", Math.pow(2,-3.4028234660000002e+038), 0, null);
    verify("VT_R8 neg excl-max", Math.pow(2,-3.4028234670000002e+038), 0, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.pow(2,-7.1717076376326201e+243), 0, null);
    verify("neg min", Math.pow(2,-1.7976931348623157e+308), 0, null);

    verify("zero", Math.pow(2,0), 1, null);

    @cc_on
    @if (!@_fast)

    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "num string, decimal, integer";

    verify("pos min", Math.pow(2,"1"), 2, null);
    verify("pos min < n < VT_UI1 pos max", Math.pow(2,"127"), 1.7014118346046923e+038, null);
    verify("VT_UI1 pos max", Math.pow(2,"255"), 5.7896044618658098e+076, null);
    verify("VT_I2 pos excl-min", Math.pow(2,"256"), 1.157920892373162e+077, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.pow(2,"12869"), "Infinity", null);
    verify("VT_I2 pos max", Math.pow(2,"32767"), "Infinity", null);
    verify("VT_I4 pos excl-min", Math.pow(2,"32768"), "Infinity", null);
    verify("VT_I4 pos excl-min < n < pos max", Math.pow(2,"1143483646"), "Infinity", null);
    verify("pos max", Math.pow(2,"2147483647"), "Infinity", null);

    verify("neg max", Math.pow(2,"-1"), 0.5, null);
    verify("neg max > n > VT_UI1 neg min", Math.pow(2,"-201"), 3.1115076389305709e-061, null);
    verify("VT_UI1 neg min", Math.pow(2,"-256"), 8.6361685550944446e-078, null);
    verify("VT_I2 neg excl-max", Math.pow(2,"-257"), 4.3180842775472223e-078, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.pow(2,"-12869"), 0, null);
    verify("VT_I2 neg min", Math.pow(2,"-32768"), 0, null);
    verify("VT_I4 neg excl-max", Math.pow(2,"-32769"), 0, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.pow(2,"-1143483646"), 0, null);
    verify("neg min", Math.pow(2,"-2147483648"), 0, null);

    verify("zero", Math.pow(2,"0"), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    verify("pos min < n < pos hex excl-min", Math.pow(2,"01234567"), "Infinity", null);
    verify("pos hex excl-min: 020000000000", Math.pow(2,"020000000000"), "Infinity", null);
    verify("max pos: 037777777777", Math.pow(2,"037777777777"), "Infinity", null);

    verify("neg max > n > neg hex excl-max", Math.pow(2,"-076543210"), 0, null);
    verify("pos hex excl-min: -020000000001", Math.pow(2,"-020000000001"), 0, null);
    verify("min neg: -037777777777", Math.pow(2,"-037777777777"), 0, null);

    verify("pos zero", Math.pow(2,"00"), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. nun string, float");

    m_scen = "nun string, float";

    verify("pos min", Math.pow(2,"2.2250738585072014e-308"), 1, null);
    verify("pos min < n < VT_R4 pos max", Math.pow(2,"9.87654321e12"), "Infinity", null);
    verify("VT_R4 pos max", Math.pow(2,"3.402823466e+38"), "Infinity", null);
    verify("VT_R8 pos excl-min", Math.pow(2,"3.402823467e+38"), "Infinity", null);
    verify("VT_R8 pos excl-min < n < pos max", Math.pow(2,"4.43269743388067e107"), "Infinity", null);
    verify("pos max", Math.pow(2,"1.7976931348623158e308"), "Infinity", null);

    verify("neg max", Math.pow(2,"-2.2250738585072012595e-308"), 1, null);
    verify("neg max > n > VT_R4 neg min", Math.pow(2,"-6.61005335930233e19"), 0, null);
    verify("VT_R4 neg min", Math.pow(2,"-3.402823466e+38"), 0, null);
    verify("VT_R8 neg excl-max", Math.pow(2,"-3.402823467e+38"), 0, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.pow(2,"-7.17170763763262e243"), 0, null);
    verify("neg min", Math.pow(2,"-1.797693134862315807e308"), 0, null);

    verify("zero", Math.pow(2,"0.0"), 1, null);
    @else
    eval("apInit"+"Scenario('5. skipped');");
    eval("apInit"+"Scenario('6. skipped');");
    eval("apInit"+"Scenario('7. skipped');");
    @end

    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true", Math.pow(2,true), 2, null);
    verify("false", Math.pow(2,false), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("", Math.pow(2,null), 1, null);


    apEndTest();

}


pow011();


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
