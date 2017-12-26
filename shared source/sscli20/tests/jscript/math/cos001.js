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


var iTestID = 53737;

@cc_on
function verify(sCat, vAct, vExp, bugNum)
{
@if(@_fast)
    var nEpsilon;
@end
    if (null == bugNum) bugNum = "";

    @if ((@_win32 || @_win64) && !@_alpha)
    nEpsilon=2.2204460492503131e-014;
    @else
    nEpsilon=1.22e-03;
    @end

    var fFailed = false;
    if (isNaN(vExp))
    {
        if (!isNaN(vAct))
            fFailed = true;
    }
    else
        if ((Math.abs(vExp-vAct)/vExp) > nEpsilon)
            fFailed = true;

    if (fFailed)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed",vExp,vAct,bugNum);
}

var m_scen = "";

function cos001()
{
 
    apInitTest("cos001 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.cos(1), .5403023058681398, null);
    verify("pos min < n < VT_UI1 pos max", Math.cos(127), .23235910202965793, null);
    verify("VT_UI1 pos max", Math.cos(255), -.8623036078310824, null);
    verify("VT_I2 pos excl-min", Math.cos(256), -0.03979075993115771, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.cos(12869), .5092434002202905, null);
    verify("VT_I2 pos max", Math.cos(32767), .9822633517692823, null);
    verify("VT_I4 pos excl-min", Math.cos(32768), .3729378293277149, null);
    verify("VT_I4 pos excl-min < n < pos max", Math.cos(1143483646), -.7876724045497843, null);
    verify("pos max", Math.cos(2147483647), -.6888366918759397, null);
    
    verify("neg max", Math.cos(-1), .5403023058681398, null);
    verify("neg max > n > VT_UI1 neg min", Math.cos(-201), .9980829609135574, null);
    verify("VT_UI1 neg min", Math.cos(-256), -0.03979075993115771, null);
    verify("VT_I2 neg excl-max", Math.cos(-257), .8193055291449822, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.cos(-12869), .5092434002202905, null);
    verify("VT_I2 neg min", Math.cos(-32768), .3729378293277149, null);
    verify("VT_I4 neg excl-max", Math.cos(-32769), -.579265013506836, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.cos(-1143483646), -.7876724045497843, null);
    verify("neg min", Math.cos(-2147483648), .23781619457548875, "VBE 22458");
    
    verify("zero", Math.cos(0), 1, null); 

    
    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", Math.cos(0xabcdef), .3834071341487052, null);
    verify("pos hex excl-min: 0x80000000", Math.cos(0x80000000), .23781619457548875, null);
    verify("max pos: 0xffffffff", Math.cos(0xffffffff), -.8679353473599486, null);

    verify("neg max > n > neg hex excl-max", Math.cos(-0xabcdef), .3834071341487052, null);
    verify("pos hex excl-min: -0x80000001", Math.cos(-0x80000001), .9458219684797852, null);
    verify("min neg: -0xffffffff", Math.cos(-0xffffffff), -.8679353473599486, null);

    verify("zero", Math.cos(0x0), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", Math.cos(01234567), .18663954083433135, null);
    verify("pos hex excl-min: 020000000000", Math.cos(020000000000), .23781619457548875, null);
    verify("max pos: 037777777777", Math.cos(037777777777), -.8679353473599486, null);

    verify("neg max > n > neg hex excl-max", Math.cos(-076543210), -.9908430854832141, null);
    verify("pos hex excl-min: -020000000001", Math.cos(-020000000001), .9458219684797852, null);
    verify("min neg: -037777777777", Math.cos(-037777777777), -.8679353473599486, null);

    verify("pos zero", Math.cos(00), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    verify("pos min", Math.cos(2.2250738585072014e-308), 1, null);
    verify("pos min < n < VT_R4 pos max", Math.cos(9876543210000), .33705536984714296, null);
    verify("VT_R4 pos max", Math.cos(3.402823466e38), -0.6920678434819332, null);
    verify("VT_R8 pos excl-min", Math.cos(3.402823467e38), -0.8749771678987744, null);

    verify("neg max", Math.cos(-2.2250738585072012595e-308), 1, null);
    verify("VT_R4 neg min", Math.cos(-3.402823466e38), -0.6920678434819332, null);
    verify("VT_R8 neg excl-max", Math.cos(-3.402823467e38), -0.8749771678987744, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.cos(-7.17170763763262e243), -0.9735561202072218, null);

    @if (@_jscript_version<7) // testing cos of numbers > 2^63 is just testing roundoff error.  This is not supported by the math libraries in v7.
    verify("pos max", Math.cos(1.7976931348623157e308), 0.9057811022070704, "VS7 #177195");
    verify("neg max > n > VT_R4 neg min", Math.cos(-66100533593023300000), 0.3757110152912341, null);
    verify("VT_R8 pos excl-min < n < pos max", Math.cos(4.43269743388067e107), 0.4268738813323151, null);
    verify("neg min", Math.cos(-1.7976931348623157e308), 0.9057811022070704, null);
    @end

    verify("zero", Math.cos(0), 1, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.cos("1"), .5403023058681398, null);
    verify("pos min < n < VT_UI1 pos max", Math.cos("127"), .23235910202965793, null);
    verify("VT_UI1 pos max", Math.cos("255"), -.8623036078310824, null);
    verify("VT_I2 pos excl-min", Math.cos("256"), -0.03979075993115771, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.cos("12869"), .5092434002202905, null);
    verify("VT_I2 pos max", Math.cos("32767"), .9822633517692823, null);
    verify("VT_I4 pos excl-min", Math.cos("32768"), .3729378293277149, null);
    verify("VT_I4 pos excl-min < n < pos max", Math.cos("1143483646"), -.7876724045497843, null);
    verify("pos max", Math.cos("2147483647"), -.6888366918759397, null);

    verify("neg max", Math.cos("-1"), .5403023058681398, null);
    verify("neg max > n > VT_UI1 neg min", Math.cos("-201"), .9980829609135574, null);
    verify("VT_UI1 neg min", Math.cos("-256"), -0.03979075993115771, null);
    verify("VT_I2 neg excl-max", Math.cos("-257"), .8193055291449822, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.cos("-12869"), .5092434002202905, null);
    verify("VT_I2 neg min", Math.cos("-32768"), .3729378293277149, null);
    verify("VT_I4 neg excl-max", Math.cos("-32769"), -.579265013506836, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.cos("-1143483646"), -.7876724045497843, null);
    verify("neg min", Math.cos("-2147483648"), .23781619457548875, null);

    verify("zero", Math.cos("0"), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    verify("pos min < n < pos hex excl-min", Math.cos("01234567"), -.9312221068534733, null);

    verify("neg max > n > neg hex excl-max", Math.cos("-076543210"), .11522221386929113, null);

    verify("pos zero", Math.cos("00"), 1, null);

    @if (@_jscript_version<7) // testing cos of numbers > 2^63 is just testing roundoff error.  This is not supported by the math libraries in v7.
    verify("pos hex excl-min: 020000000000", Math.cos("020000000000"), .5246757510286704, null);
    verify("max pos: 037777777777", Math.cos("037777777777"), 0.08344857185271164, null);
    verify("pos hex excl-min: -020000000001", Math.cos("-020000000001"), .9998295794545644, null);
    verify("min neg: -037777777777", Math.cos("-037777777777"), 0.08344857185271164, null);
    @end

    //----------------------------------------------------------------------------
    apInitScenario("8. num string, float");

    m_scen = "number, float";

    verify("pos min", Math.cos("2.2250738585072014e-308"), 1, null);
    verify("pos min < n < VT_R4 pos max", Math.cos("9.87654321e12"), .33705536984714296, null);
    verify("VT_R4 pos max", Math.cos("3.402823466e+38"), -0.6920678434819332, null);
    verify("VT_R8 pos excl-min", Math.cos("3.402823467e+38"), -0.8749771678987744, null);

    verify("neg max", Math.cos("-2.2250738585072012595e-308"), 1, null);
    verify("VT_R4 neg min", Math.cos("-3.402823466e+38"), -0.6920678434819332, null);
    verify("VT_R8 neg excl-max", Math.cos("-3.402823467e+38"), -0.8749771678987744, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.cos("-7.17170763763262e243"), -0.9735561202072218, null);

    verify("zero", Math.cos("0.0"), 1, null);

    @if (@_jscript_version<7) // testing cos of numbers > 2^63 is just testing roundoff error.  This is not supported by the math libraries in v7.
    verify("neg min", Math.cos("-1.797693134862315807e308"), 0.9057811022070704, null);
    verify("pos max", Math.cos("1.7976931348623158e308"), 0.9057811022070704, null);
    verify("VT_R8 pos excl-min < n < pos max", Math.cos("4.43269743388067e107"), 0.4268738813323151, null);
    verify("neg max > n > VT_R4 neg min", Math.cos("-6.61005335930233e19"), 0.3757110152912341, null);
    @end   

    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true",Math.cos(true), .5403023058681398, null);
    verify("false",Math.cos(false), 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("",Math.cos(null), 1, null);


    apEndTest();

}


cos001();


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
