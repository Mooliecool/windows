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


var iTestID = 53733;


function verify(sCat, ob, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    var vAct = Math.atan(ob);

    var nEpsilon = 2.2204460492503131e-016;
    if ( 0==vExp ? Math.abs(vExp-vAct) > nEpsilon : Math.abs(vExp-vAct)/vExp > nEpsilon )
        apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", vExp, vAct,bugNum);
}

var m_scen = "";

function atan002()
{
 
    apInitTest("atan002 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", 1, .7853981633974483, null);
    verify("pos min < n < VT_UI1 pos max", 127, 1.562922473770796, null);
    verify("VT_UI1 pos max", 255, 1.5668747782701367, null);
    verify("VT_I2 pos excl-min", 256, 1.5668900966629296, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", 12869, 1.5707186206795818, null);
    verify("VT_I2 pos max", 32767, 1.57076580828543, null);
    verify("VT_I4 pos excl-min", 32768, 1.570765809216781, null);
    verify("VT_I4 pos excl-min < n < pos max", 1143483646, 1.570796325920376, null);
    verify("pos max", 2147483647, 1.5707963263292353, null);
    
    verify("neg max", -1, -.7853981633974483, null);
    verify("neg max > n > VT_UI1 neg min", -201, -1.5658212434640426, null);
    verify("VT_UI1 neg min", -256, -1.5668900966629296, null);
    verify("VT_I2 neg excl-max", -257, -1.566905295848252, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", -12869, -1.5707186206795818, null);
    verify("VT_I2 neg min", -32768, -1.570765809216781, null);
    verify("VT_I4 neg excl-max", -32769, -1.5707658101480753, null);
    verify("VT_I4 neg excl-max > n > neg min", -1143483646, -1.570796325920376, null);
    verify("neg min", -2147483648, -1.5707963263292353, null);
    
    verify("zero", 0, 0, null); 

    
    //----------------------------------------------------------------------------
    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    verify("pos min < n < pos hex excl-min", 0xabcdef, 1.5707962379800202, null);
    verify("pos hex excl-min: 0x80000000", 0x80000000, 1.5707963263292353, null);
    verify("max pos: 0xffffffff", 0xffffffff, 1.570796326562066, null);

    verify("neg max > n > neg hex excl-max", -0xabcdef, -1.5707962379800202, null);
    verify("pos hex excl-min: -0x80000001", -0x80000001, -1.5707963263292353, null);
    verify("min neg: -0xffffffff", -0xffffffff, -1.570796326562066, null);

    verify("zero", 0x0, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    verify("pos min < n < pos hex excl-min", 01234567, 1.5707934061573798, null);
    verify("pos hex excl-min: 020000000000", 020000000000, 1.5707963263292353, null);
    verify("max pos: 037777777777", 037777777777, 1.570796326562066, null);

    verify("neg max > n > neg hex excl-max", -076543210, -1.570796265948489, null);
    verify("pos hex excl-min: -020000000001", -020000000001, -1.5707963263292353, null);
    verify("min neg: -037777777777", -037777777777, -1.570796326562066, null);

    verify("pos zero", 00, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. number, float");

    m_scen = "number, float";

    verify("pos min", 2.2250738585072014e-308, 2.2250738585072014e-308, null);
    verify("pos min < n < VT_R4 pos max", 9876543210000, 1.5707963267947953, null);
    verify("VT_R4 pos max", 3.402823466e38, 1.5707963267948966, null);
    verify("VT_R8 pos excl-min", 3.402823467e38, 1.5707963267948966, null);
    verify("VT_R8 pos excl-min < n < pos max", 4.43269743388067e107, 1.5707963267948966, null);
    verify("pos max", 1.7976931348623157e308, 1.5707963267948966, null);

    verify("neg max", -2.2250738585072012595e-308, 0, null);
    verify("neg max > n > VT_R4 neg min", -66100533593023300000, -1.5707963267948966, null);
    verify("VT_R4 neg min", -3.402823466e38, -1.5707963267948966, null);
    verify("VT_R8 neg excl-max", -3.402823467e38, -1.5707963267948966, null);
    verify("VT_R8 neg excl-max > n > neg min", -7.17170763763262e243, -1.5707963267948966, null);
    verify("neg min", -1.7976931348623157e308, -1.5707963267948966, null);

    verify("zero", 0, 0, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    verify("pos min", "1", .7853981633974483, null);
    verify("pos min < n < VT_UI1 pos max", "127", 1.562922473770796, null);
    verify("VT_UI1 pos max", "255", 1.5668747782701367, null);
    verify("VT_I2 pos excl-min", "256", 1.5668900966629296, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", "12869", 1.5707186206795818, null);
    verify("VT_I2 pos max", "32767", 1.57076580828543, null);
    verify("VT_I4 pos excl-min", "32768", 1.570765809216781, null);
    verify("VT_I4 pos excl-min < n < pos max", "1143483646", 1.570796325920376, null);
    verify("pos max", "2147483647", 1.5707963263292353, null);

    verify("neg max", "-1", -.7853981633974483, null);
    verify("neg max > n > VT_UI1 neg min", "-201", -1.5658212434640426, null);
    verify("VT_UI1 neg min", "-256", -1.5668900966629296, null);
    verify("VT_I2 neg excl-max", "-257", -1.566905295848252, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", "-12869", -1.5707186206795818, null);
    verify("VT_I2 neg min", "-32768", -1.570765809216781, null);
    verify("VT_I4 neg excl-max", "-32769", -1.5707658101480753, null);
    verify("VT_I4 neg excl-max > n > neg min", "-1143483646", -1.570796325920376, null);
    verify("neg min", "-2147483648", -1.5707963263292353, null);

    verify("zero", "0", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    verify("pos min < n < pos hex excl-min", "01234567", 1.5707955167943053, null);
    verify("pos hex excl-min: 020000000000", "020000000000", 1.5707963267448966, null);
    verify("max pos: 037777777777", "037777777777", 1.570796326768426, null);

    verify("neg max > n > neg hex excl-max", "-076543210", -1.5707963137303804, null);
    verify("pos hex excl-min: -020000000001", "-020000000001", -1.5707963267448966, null);
    verify("min neg: -037777777777", "-037777777777", -1.570796326768426, null);

    verify("pos zero", "00", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. num string, float");

    m_scen = "number, float";

    verify("pos min", "2.2250738585072014e-308", 2.2250738585072014e-308, null);
    verify("pos min < n < VT_R4 pos max", "9.87654321e12", 1.5707963267947953, null);
    verify("VT_R4 pos max", "3.402823466e+38", 1.5707963267948966, null);
    verify("VT_R8 pos excl-min", "3.402823467e+38", 1.5707963267948966, null);
    verify("VT_R8 pos excl-min < n < pos max", "4.43269743388067e107", 1.5707963267948966, null);
    verify("pos max", "1.7976931348623158e308", 1.5707963267948966, null);

    verify("neg max", "-2.2250738585072012595e-308", -2.225073858507201e-308, null);
    verify("neg max > n > VT_R4 neg min", "-6.61005335930233e19", -1.5707963267948966, null);
    verify("VT_R4 neg min", "-3.402823466e+38", -1.5707963267948966, null);
    verify("VT_R8 neg excl-max", "-3.402823467e+38", -1.5707963267948966, null);
    verify("VT_R8 neg excl-max > n > neg min", "-7.17170763763262e243", -1.5707963267948966, null);
    verify("neg min", "-1.797693134862315807e308", -1.5707963267948966, null);

    verify("zero", "0.0", 0, null);
   
   
    //----------------------------------------------------------------------------
    apInitScenario("13. constants");

    m_scen = "constants";

    verify("true",true, .7853981633974483, null);
    verify("false",false, 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("14. null");

    m_scen = "null";

    verify("",null, 0, null);


    

    apEndTest();

}


atan002();


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
