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


var iTestID = 53114;


function verify(sCat, sAct, ob, bugNum){
 @if(@_fast)
    var nEpsilon;
 @end
    if (bugNum == null) bugNum = "";

    var vExp=1, exp=3;

    for (var i=0; i<exp; i++) vExp*=ob;


//    if (verify.length==5) vExp = verify[4];

    var fFailed = false;
    if (isNaN(vExp))
    {
        if (!isNaN(sAct))
            fFailed = true;
    }
    else
    {
        if (fUseEpsilon)
        {
            if ((Math.abs(vExp-sAct)/vExp) > (nEpsilon=2.2204460492503131e-016))
                fFailed = true;
        }
        else
            if (sAct != vExp)
                fFailed = true;
    }

    if (fFailed)
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed",vExp,sAct,bugNum);
}

var m_scen = "", fUseEpsilon=false;

function pow001() {
 
    apInitTest("pow001 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", Math.pow(1,3), 1, null);
    verify("pos min < n < VT_UI1 pos max", Math.pow(127,3), 127, null);
    verify("VT_UI1 pos max", Math.pow(255,3), 255, null);
    verify("VT_I2 pos excl-min", Math.pow(256,3), 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.pow(12869,3), 12869, "VBE 22455");
    verify("VT_I2 pos max", Math.pow(32767,3), 32767, null);
    verify("VT_I4 pos excl-min", Math.pow(32768,3), 32768, null);
    verify("VT_I4 pos excl-min < n < pos max", Math.pow(1143483646,3), 1143483646, null);
    verify("pos max", Math.pow(2147483647,3), 2147483647, null);

    verify("neg max", Math.pow(-1,3), -1, null);
    verify("neg max > n > VT_UI1 neg min", Math.pow(-201,3), -201, null);
    verify("VT_UI1 neg min", Math.pow(-256,3), -256, null);
    verify("VT_I2 neg excl-max", Math.pow(-257,3), -257, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.pow(-12869,3), -12869, null);
    verify("VT_I2 neg min", Math.pow(-32768,3), -32768, null);
    verify("VT_I4 neg excl-max", Math.pow(-32769,3), -32769, null);
    verify("VT_I4 neg excl-max > n > neg min", Math.pow(-1143483646,3), -1143483646, null);
    verify("neg min", Math.pow(-2147483648,3), -2147483648, null);

    verify("zero", Math.pow(0,3), 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", Math.pow(0xabcdef,3), 0xabcdef, null);
    verify("pos hex excl-min: 0x80000000", Math.pow(0x80000000,3), 0x80000000, null);
    verify("max pos: 0xffffffff", Math.pow(0xffffffff,3), 0xffffffff, null);

    verify("neg max > n > neg hex excl-max", Math.pow(-0xabcdef,3), -0xabcdef, null);
    verify("pos hex excl-min: -0x80000001", Math.pow(-0x80000001,3), -0x80000001, null);
    verify("min neg: -0xffffffff", Math.pow(-0xffffffff,3), -0xffffffff, null);

    verify("zero", Math.pow(0x0,3), 0x0, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", Math.pow(01234567,3), 01234567, null);
    verify("pos hex excl-min: 020000000000", Math.pow(020000000000,3), 020000000000, null);
    verify("max pos: 037777777777", Math.pow(037777777777,3), 037777777777, null);

    verify("neg max > n > neg hex excl-max", Math.pow(-076543210,3), -076543210, null);
    verify("pos hex excl-min: -020000000001", Math.pow(-020000000001,3), -020000000001, null);
    verify("min neg: -037777777777", Math.pow(-037777777777,3), -037777777777, null);

    verify("pos zero", Math.pow(00,3), 00, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    fUseEpsilon = true;

    verify("pos min",Math.pow(2.2250738585072014e-308,3), 2.2250738585072014e-308, null);
    verify("pos min < n < VT_R4 pos max", Math.pow(9.87654321e12,3), 9.87654321e12, null);
    verify("VT_R4 pos max", Math.pow(3.402823466e+38,3), 3.402823466e+38, null);
    verify("VT_R8 pos excl-min", Math.pow(3.402823467e+38,3), 3.402823467e+38, null);
    verify("VT_R8 pos excl-min < n < pos max", Math.pow(4.43269743388067e107,3), 4.43269743388067e107, null);
    verify("pos max", Math.pow(1.7976931348623158e308,3), 1.7976931348623158e308, null);

    verify("neg max", Math.pow(-2.2250738585072012595e-308,3), -2.2250738585072012595e-308, null);
    verify("neg max > n > VT_R4 neg min", Math.pow(-6.61005335930233e19,3), -6.61005335930233e19, null);
    verify("VT_R4 neg min", Math.pow(-3.402823466e+38,3), -3.402823466e+38, null);
    verify("VT_R8 neg excl-max", Math.pow(-3.402823467e+38,3), -3.402823467e+38, null);
    verify("VT_R8 neg excl-max > n > neg min", Math.pow(-7.17170763763262e243,3), -7.17170763763262e243, null);
    verify("neg min", Math.pow(-1.797693134862315807e308,3), -1.797693134862315807e308, null);

    verify("zero", Math.pow(0.0,3), 0.0, null);

    fUseEpsilon = false;

    
    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min",Math.pow("1",3), "1", null);
    verify("pos min < n < VT_UI1 pos max", Math.pow("127",3), "127", null);
    verify("VT_UI1 pos max", Math.pow("255",3), "255", null);
    verify("VT_I2 pos excl-min", Math.pow("256",3), "256", null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", Math.pow("12869",3), "12869", null);
    verify("VT_I2 pos max", Math.pow("32767",3), "32767", null);
    verify("VT_I4 pos excl-min", Math.pow("32768",3), "32768", null);
    verify("VT_I4 pos excl-min < n < pos max", Math.pow("1143483646",3), "1143483646", null);
    verify("pos max", Math.pow("2147483647",3), "2147483647", null);

    verify("neg max", Math.pow("-1",3), "-1", null);
    verify("neg max > n > VT_UI1 neg min", Math.pow("-201",3), "-201", null);
    verify("VT_UI1 neg min", Math.pow("-256",3), "-256", null);
    verify("VT_I2 neg excl-max", Math.pow("-257",3), "-257", null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", Math.pow("-12869",3), "-12869", null);
    verify("VT_I2 neg min", Math.pow("-32768",3), "-32768", null);
    verify("VT_I4 neg excl-max", Math.pow("-32769",3), "-32769", null);
    verify("VT_I4 neg excl-max > n > neg min", Math.pow("-1143483646",3), "-1143483646", null);
    verify("neg min", Math.pow("-2147483648",3), "-2147483648", null);

    verify("zero", Math.pow("0",3), "0", null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");
    @cc_on
    @if (!@_fast)

    m_scen = "num string, octal";
    fUseEpsilon=true;

    verify("pos min < n < pos hex excl-min", Math.pow("01234567",3), "01234567", null);
    verify("pos hex excl-min: 020000000000", Math.pow("020000000000",3), "020000000000", null);
    verify("max pos: 037777777777", Math.pow("037777777777",3), "037777777777", null, Math.pow(37777777777,3));

    verify("neg max > n > neg hex excl-max", Math.pow("-076543210",3), "-076543210", null);
    verify("pos hex excl-min: -020000000001", Math.pow("-020000000001",3), "-020000000001", null);
    verify("min neg: -037777777777", Math.pow("-037777777777",3), "-037777777777", null, Math.pow(-37777777777,3));

    verify("pos zero", Math.pow("00",3), "00", null);

    @end

    //----------------------------------------------------------------------------
    apInitScenario("8. number, float");

    m_scen = "number, float";

    fUseEpsilon = true;

    verify("pos min",Math.pow("2.2250738585072014e-308",3), "2.2250738585072014e-308", null);
    verify("pos min < n < VT_R4 pos max", Math.pow("9.87654321e12",3), "9.87654321e12", null);
    verify("VT_R4 pos max", Math.pow("3.402823466e+38",3), "3.402823466e+38", null);
    verify("VT_R8 pos excl-min", Math.pow("3.402823467e+38",3), "3.402823467e+38", null);
    verify("VT_R8 pos excl-min < n < pos max", Math.pow("4.43269743388067e107",3), "4.43269743388067e107", null);
    verify("pos max", Math.pow("1.7976931348623158e308",3), "1.7976931348623158e308", null);

    verify("neg max", Math.pow("-2.2250738585072012595e-308",3), "-2.2250738585072012595e-308", null);
    verify("neg max > n > VT_R4 neg min", Math.pow("-6.61005335930233e19",3), "-6.61005335930233e19", null);
    verify("VT_R4 neg min", Math.pow("-3.402823466e+38",3), "-3.402823466e+38", null);
    verify("VT_R8 neg excl-max", Math.pow("-3.402823467e+38",3), "-3.402823467e+38", null);
    verify("VT_R8 neg excl-max > n > neg min", Math.pow("-7.17170763763262e243",3), "-7.17170763763262e243", null);
    verify("neg min", Math.pow("-1.797693134862315807e308",3), "-1.797693134862315807e308", null);

    verify("zero", Math.pow("0.0",3), "0.0", null);

    fUseEpsilon = false;

    
    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true",Math.pow(true,3), true, null);
    verify("false",Math.pow(false,3), false, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("",Math.pow(null,3), null, null);


    apEndTest();

}


pow001();


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
