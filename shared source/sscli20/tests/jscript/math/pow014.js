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


var iTestID = 53122;


function verify(sCat, ob, vExp, options, bugNum){
 @if(@_fast)
    var nEpsilon;
 @end

    if (bugNum == null) bugNum = "";

    var arr = new Array();
    arr[0] = ob;

    var vAct = Math.pow(2,arr[0]);

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

function pow014() {
 
    apInitTest("pow014 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", 1, 2, null);
    verify("pos min < n < VT_UI1 pos max", 127, 1.7014118346046923e+038, null);
    verify("VT_UI1 pos max", 255, 5.7896044618658098e+076, null);
    verify("VT_I2 pos excl-min", 256, 1.157920892373162e+077, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", 12869, "Infinity", null);
    verify("VT_I2 pos max", 32767, "Infinity", null);
    verify("VT_I4 pos excl-min", 32768, "Infinity", null);
    verify("VT_I4 pos excl-min < n < pos max", 1143483646, "Infinity", null);
    verify("pos max", 2147483647, "Infinity", null);

    verify("neg max", -1, 0.5, null);
    verify("neg max > n > VT_UI1 neg min", -201, 3.1115076389305709e-061, null);
    verify("VT_UI1 neg min", -256, 8.6361685550944446e-078, null);
    verify("VT_I2 neg excl-max", -257, 4.3180842775472223e-078, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", -12869, 0, null);
    verify("VT_I2 neg min", -32768, 0, null);
    verify("VT_I4 neg excl-max", -32769, 0, null);
    verify("VT_I4 neg excl-max > n > neg min", -1143483646, 0, null);
    verify("neg min", -2147483648, 0, null);

    verify("zero", 0, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", 11259375, "Infinity", null);
    verify("pos hex excl-min: 0x80000000", 2147483648, "Infinity", null);
    verify("max pos: 0xffffffff", 4294967295, "Infinity", null);

    verify("neg max > n > neg hex excl-max", -11259375, 0, null);
    verify("pos hex excl-min: -0x80000001", -2147483649, 0, null);
    verify("min neg: -0xffffffff", -4294967295, 0, null);

    verify("zero", 0, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", 342391, "Infinity", null);
    verify("pos hex excl-min: 020000000000", 2147483648, "Infinity", null);
    verify("max pos: 037777777777", 4294967295, "Infinity", null);

    verify("neg max > n > neg hex excl-max", -16434824, 0, null);
    verify("pos hex excl-min: -020000000001", -2147483649, 0, null);
    verify("min neg: -037777777777", -4294967295, 0, null);

    verify("pos zero", 0, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    verify("pos min", 2.2250738585072014e-308, 1, null);
    verify("pos min < n < VT_R4 pos max", 98765.432100000005, "Infinity", null);
    verify("VT_R4 pos max", 3.4028234660000002e+038, "Infinity", null);
    verify("VT_R8 pos excl-min", 3.4028234670000002e+038, "Infinity", null);
    verify("VT_R8 pos excl-min < n < pos max", 4.4326974338806703e+107, "Infinity", null);
    verify("pos max", 1.7976931348623157e+308, "Infinity", null);

    verify("neg max", 0, 1, null);
    verify("neg max > n > VT_R4 neg min", -6610.0533593023301, 0, null);
    verify("VT_R4 neg min", -3.4028234660000002e+038, 0, null);
    verify("VT_R8 neg excl-max", -3.4028234670000002e+038, 0, null);
    verify("VT_R8 neg excl-max > n > neg min", -7.1717076376326201e+243, 0, null);
    verify("neg min", -1.7976931348623157e+308, 0, null);

    verify("zero", 0, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "num string, decimal, integer";

    verify("pos min", "1", 2, null);
    verify("pos min < n < VT_UI1 pos max", "127", 1.7014118346046923e+038, null);
    verify("VT_UI1 pos max", "255", 5.7896044618658098e+076, null);
    verify("VT_I2 pos excl-min", "256", 1.157920892373162e+077, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", "12869", "Infinity", null);
    verify("VT_I2 pos max", "32767", "Infinity", null);
    verify("VT_I4 pos excl-min", "32768", "Infinity", null);
    verify("VT_I4 pos excl-min < n < pos max", "1143483646", "Infinity", null);
    verify("pos max", "2147483647", "Infinity", null);

    verify("neg max", "-1", 0.5, null);
    verify("neg max > n > VT_UI1 neg min", "-201", 3.1115076389305709e-061, null);
    verify("VT_UI1 neg min", "-256", 8.6361685550944446e-078, null);
    verify("VT_I2 neg excl-max", "-257", 4.3180842775472223e-078, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", "-12869", 0, null);
    verify("VT_I2 neg min", "-32768", 0, null);
    verify("VT_I4 neg excl-max", "-32769", 0, null);
    verify("VT_I4 neg excl-max > n > neg min", "-1143483646", 0, null);
    verify("neg min", "-2147483648", 0, null);

    verify("zero", "0", 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    verify("pos min < n < pos hex excl-min", "01234567", "Infinity", null);
    verify("pos hex excl-min: 020000000000", "020000000000", "Infinity", null);
    verify("max pos: 037777777777", "037777777777", "Infinity", null);

    verify("neg max > n > neg hex excl-max", "-076543210", 0, null);
    verify("pos hex excl-min: -020000000001", "-020000000001", 0, null);
    verify("min neg: -037777777777", "-037777777777", 0, null);

    verify("pos zero", "00", 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. nun string, float");

    m_scen = "nun string, float";

    verify("pos min", "2.2250738585072014e-308", 1, null);
    verify("pos min < n < VT_R4 pos max", "9.87654321e12", "Infinity", null);
    verify("VT_R4 pos max", "3.402823466e+38", "Infinity", null);
    verify("VT_R8 pos excl-min", "3.402823467e+38", "Infinity", null);
    verify("VT_R8 pos excl-min < n < pos max", "4.43269743388067e107", "Infinity", null);
    verify("pos max", "1.7976931348623158e308", "Infinity", null);

    verify("neg max", "-2.2250738585072012595e-308", 1, null);
    verify("neg max > n > VT_R4 neg min", "-6.61005335930233e19", 0, null);
    verify("VT_R4 neg min", "-3.402823466e+38", 0, null);
    verify("VT_R8 neg excl-max", "-3.402823467e+38", 0, null);
    verify("VT_R8 neg excl-max > n > neg min", "-7.17170763763262e243", 0, null);
    verify("neg min", "-1.797693134862315807e308", 0, null);

    verify("zero", "0.0", 1, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true", true, 2, null);
    verify("false", false, 1, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("", null, 1, null);

    apEndTest();

}


pow014();


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
