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


var iTestID = 52948;

var m_scen = "";

function verify(iArg1, iArg2)
{
    var arg1 = arr[iArg1].ob, arg2 = arr[iArg2].ob;

    var vAct = Math.min( arg1, arg2 );

    var vFirst = (typeof arg1=="string") ? parseFloat( arg1 ) : arg1+0;
    var vSecond = (typeof arg2=="string") ? parseFloat( arg2 ) : arg2+0;

    var vExp;

    if (vFirst <= vSecond)
        vExp = vFirst;
    else
        vExp = vSecond;

    if (isNaN(vFirst) || isNaN(vSecond))
      {
        if (!isNaN(vAct))
          apLogFailInfo( m_scen+" failed", "NaN",vAct,"");
        return;
      }

    if (vAct != vExp)
    {
        apLogFailInfo( "arg1["+iArg1+"]  "+(arr[iArg1].sCat.length ? arr[iArg1].sCat : "")+";"+
                       "arg2["+iArg2+"]  "+(arr[iArg2].sCat.length ? arr[iArg2].sCat : "")+" failed", 
                       vExp, vAct,"");
    }
    
}

var vExp = null;
var arr = new Array(113);
var i;

function min101(){


 for (i=0; i<113; i++)
    arr[i] = new Object();

    apInitTest("min101 ");

    //----------------------------------------------------------------------------
//    apInitScenario("1. number, decimal, integer");

    m_scen = "number, decimal, integer";

    arr[0].sCat = "pos min";                                arr[0].ob = 1;
    arr[1].sCat = "pos min < n < VT_UI1 pos max";           arr[1].ob = 127;
    arr[2].sCat = "VT_UI1 pos max";                         arr[2].ob = 255;
    arr[3].sCat = "VT_I2 pos excl-min";                     arr[3].ob = 256;
    arr[4].sCat = "VT_I2 pos excl-min < n < VT_I2 pos max"; arr[4].ob = 12869;
    arr[5].sCat = "VT_I2 pos max";                          arr[5].ob = 32767;
    arr[6].sCat = "VT_I4 pos excl-min";                     arr[6].ob = 32768;
    arr[7].sCat = "VT_I4 pos excl-min < n < pos max";       arr[7].ob = 1143483646;
    arr[8].sCat = "pos max";                                arr[8].ob = 2147483647;

    arr[9].sCat = "neg max";                                arr[9].ob = -1;
    arr[10].sCat = "neg max > n > VT_UI1 neg min";          arr[10].ob = -201;
    arr[11].sCat = "VT_UI1 neg min";                        arr[11].ob = -256;
    arr[12].sCat = "VT_I2 neg excl-max";                    arr[12].ob = -257;
    arr[13].sCat = "VT_I2 neg excl-max > n> VT_I2 neg min"; arr[13].ob = -12869;
    arr[14].sCat = "VT_I2 neg min";                         arr[14].ob = -32768;
    arr[15].sCat = "VT_I4 neg excl-max";                    arr[15].ob = -32769;
    arr[16].sCat = "VT_I4 neg excl-max > n > neg min";      arr[16].ob = -1143483646;
    arr[17].sCat = "neg min";                               arr[17].ob = -2147483648;

    arr[18].sCat = "zero";                                  arr[18].ob = 0;


    //----------------------------------------------------------------------------
//    apInitScenario("2. number, hexidecimal");

    m_scen = "number, hexidecimal";

    arr[19].sCat = "pos min < n < pos hex excl-min";        arr[19].ob = 0xabcdef;
    arr[20].sCat = "pos hex excl-min: 0x80000000";          arr[20].ob = 0x80000000;
    arr[21].sCat = "max pos: 0xffffffff";                   arr[21].ob = 0xffffffff;

    arr[22].sCat = "neg max > n > neg hex excl-max";        arr[22].ob = -0xabcdef;
    arr[23].sCat = "pos hex excl-min: -0x80000001";         arr[23].ob = -0x80000001;
    arr[24].sCat = "min neg: -0xffffffff";                  arr[24].ob = -0xffffffff;

    arr[25].sCat = "zero";                                  arr[25].ob = 0x0;


    //----------------------------------------------------------------------------
//    apInitScenario("3. number, octal");

    m_scen = "number, octal";

    arr[26].sCat = "pos min < n < pos hex excl-min";        arr[26].ob = 01234567;
    arr[27].sCat = "pos hex excl-min: 020000000000";        arr[27].ob = 020000000000;
    arr[28].sCat = "max pos: 037777777777";                 arr[28].ob = 037777777777;

    arr[29].sCat = "neg max > n > neg hex excl-max";        arr[29].ob = -076543210;
    arr[30].sCat = "pos hex excl-min: -020000000001";       arr[30].ob = -020000000001;
    arr[31].sCat = "min neg: -037777777777";                arr[31].ob = -037777777777;

    arr[32].sCat = "pos zero";                              arr[32].ob = 00;


    //----------------------------------------------------------------------------
//    apInitScenario("4. number, float");

    m_scen = "number, float";

    arr[33].sCat = "pos min";                               arr[33].ob = 2.2250738585072014e-308;
    arr[34].sCat = "pos min < n < VT_R4 pos max";           arr[34].ob = 9.87654321e12;
    arr[35].sCat = "VT_R4 pos max";                         arr[35].ob = 3.402823466e+38;
    arr[36].sCat = "VT_R8 pos excl-min";                    arr[36].ob = 3.402823467e+38;
    arr[37].sCat = "VT_R8 pos excl-min < n < pos max";      arr[37].ob = 4.43269743388067e107;
    arr[38].sCat = "pos max";                               arr[38].ob = 1.7976931348623158e308;

    arr[39].sCat = "neg max";                               arr[39].ob = -2.2250738585072012595e-308;
    arr[40].sCat = "neg max > n > VT_R4 neg min";           arr[40].ob = -6.61005335930233e19;
    arr[41].sCat = "VT_R4 neg min";                         arr[41].ob = -3.402823466e+38;
    arr[42].sCat = "VT_R8 neg excl-max";                    arr[42].ob = -3.402823467e+38;
    arr[43].sCat = "VT_R8 neg excl-max > n > neg min";      arr[43].ob = -7.17170763763262e243;
    arr[44].sCat = "neg min";                               arr[44].ob = -1.797693134862315807e308;

    arr[45].sCat = "zero";                                  arr[45].ob = 0.0;

    
    //----------------------------------------------------------------------------
//    apInitScenario("5. num string, decimal, integer");

    m_scen = "number, decimal, integer";

    arr[46].sCat = "pos min";                               arr[46].ob = "1";
    arr[47].sCat = "pos min < n < VT_UI1 pos max";          arr[47].ob = "127";
    arr[48].sCat = "VT_UI1 pos max";                        arr[48].ob = "255";
    arr[49].sCat = "VT_I2 pos excl-min";                    arr[49].ob = "256";
    arr[50].sCat = "VT_I2 pos excl-min < n< VT_I2 pos max"; arr[50].ob = "12869";
    arr[51].sCat = "VT_I2 pos max";                         arr[51].ob = "32767";
    arr[52].sCat = "VT_I4 pos excl-min";                    arr[52].ob = "32768";
    arr[53].sCat = "VT_I4 pos excl-min < n < pos max";      arr[53].ob = "1143483646";
    arr[54].sCat = "pos max";                               arr[54].ob = "2147483647";

    arr[55].sCat = "neg max";                               arr[55].ob = "-1";
    arr[56].sCat = "neg max > n > VT_UI1 neg min";          arr[56].ob = "-201";
    arr[57].sCat = "VT_UI1 neg min";                        arr[57].ob = "-256";
    arr[58].sCat = "VT_I2 neg excl-max";                    arr[58].ob = "-257";
    arr[59].sCat = "VT_I2 neg excl-max > n> VT_I2 neg min"; arr[59].ob = "-12869";
    arr[60].sCat = "VT_I2 neg min";                         arr[60].ob = "-32768";
    arr[61].sCat = "VT_I4 neg excl-max";                    arr[61].ob = "-32769";
    arr[62].sCat = "VT_I4 neg excl-max > n > neg min";      arr[62].ob = "-1143483646";
    arr[63].sCat = "neg min";                               arr[63].ob = "-2147483648";

    arr[64].sCat = "zero";                                  arr[64].ob = "0";


    //----------------------------------------------------------------------------
//    apInitScenario("7. num string, octal");

    m_scen = "num string, octal";

    arr[65].sCat = "pos min < n < pos hex excl-min";        arr[65].ob = "01234567";
    arr[66].sCat = "pos hex excl-min: 020000000000";        arr[66].ob = "020000000000";
    arr[67].sCat = "max pos: 037777777777";                 arr[67].ob = "037777777777";

    arr[68].sCat = "neg max > n > neg hex excl-max";        arr[68].ob = "-076543210";
    arr[69].sCat = "pos hex excl-min: -020000000001";       arr[69].ob = "-020000000001";
    arr[70].sCat = "min neg: -037777777777";                arr[70].ob = "-037777777777";

    arr[71].sCat = "pos zero";                              arr[71].ob = "00";


    //----------------------------------------------------------------------------
//    apInitScenario("8. num string, float");

    m_scen = "number, float";

    arr[72].sCat = "pos min";                               arr[72].ob = "2.2250738585072014e-308";
    arr[73].sCat = "pos min < n < VT_R4 pos max";           arr[73].ob = "9.87654321e12";
    arr[74].sCat = "VT_R4 pos max";                         arr[74].ob = "3.402823466e+38";
    arr[75].sCat = "VT_R8 pos excl-min";                    arr[75].ob = "3.402823467e+38";
    arr[76].sCat = "VT_R8 pos excl-min < n < pos max";      arr[76].ob = "4.43269743388067e107";
    arr[77].sCat = "pos max";                               arr[77].ob = "1.7976931348623158e308";

    arr[78].sCat = "neg max";                               arr[78].ob = "-2.2250738585072012595e-308";
    arr[79].sCat = "neg max > n > VT_R4 neg min";           arr[79].ob = "-6.61005335930233e19";
    arr[80].sCat = "VT_R4 neg min";                         arr[80].ob = "-3.402823466e+38";
    arr[81].sCat = "VT_R8 neg excl-max";                    arr[81].ob = "-3.402823467e+38";;
    arr[82].sCat = "VT_R8 neg excl-max > n > neg min";      arr[82].ob = "-7.17170763763262e243";
    arr[83].sCat = "neg min";                               arr[83].ob = "-1.797693134862315807e308";

    arr[84].sCat = "zero";                                  arr[84].ob = "0.0";

    
    //----------------------------------------------------------------------------
//    apInitScenario("13. constants");

    m_scen = "constants";

    arr[85].sCat = "true";             arr[85].ob = true;
    arr[86].sCat = "false";            arr[86].ob = false;


    //----------------------------------------------------------------------------
//    apInitScenario("14. null");

    m_scen = "null";

    arr[87].sCat = "null"; arr[87].ob = null;


    /*****************************************************************************
    // All of the rest cause a runtime error (spec) of 'not a number'

    //----------------------------------------------------------------------------
    // apInitScenario("n. Alpha string");
    

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
    

    //----------------------------------------------------------------------------  
    // Start processing here
    apInitScenario("This test is just one big scenario.  Needs to be rewritten logically.");

    for (var i=0; i<88; i++)
    {
        for (var j=0; j<88; j++)
        {           
            verify(i,j);
        }
    }


    apEndTest();

}


min101();


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
