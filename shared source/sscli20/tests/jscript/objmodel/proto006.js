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


var iTestID = 53139;


/** Function prototype **********************************************************/

function verify(sCat, val, bugNum){
@if(!@_fast)
    if (null == bugNum) bugNum = "";

    curClass = Function;

    curClass.prototype.mem = val;

    if ((new curClass()).mem != val)
        apLogFailInfo(m_scen+": "+sCat+" failed.", val, 
            (new curClass()).val, bugNum);
@end
}


@if(!@aspx)
   function udObj () { /* nothing */ }
@else
   expando function udObj () { /* nothing */ }
@end

var m_scen = "";

function proto006 ()
{
 
    apInitTest("proto006 (no tests in fast mode)");

    //----------------------------------------------------------------------------
    apInitScenario("numbers, integers");

    m_scen = "numbers, integers";

    verify("pos min", 1, null);
    verify("pos min < n < VT_UI1 pos max", 127, null);
    verify("VT_UI1 pos max", 255, null);
    verify("VT_I2 pos excl-min", 256, null);
    verify("VT_I2 pos excl-min < n < VT_I2 pos max", 12869, null);
    verify("VT_I2 pos max", 32767, null);
    verify("VT_I4 pos excl-min", 32768, null);
    verify("VT_I4 pos excl-min < n < pos max", 1143483646, null);
    verify("pos max", 2147483647, null);

    verify("neg max", -1, null);
    verify("neg max > n > VT_UI1 neg min", -201, null);
    verify("VT_UI1 neg min", -256, null);
    verify("VT_I2 neg excl-max", -257, null);
    verify("VT_I2 neg excl-max > n > VT_I2 neg min", -12869, null);
    verify("VT_I2 neg min", -32768, null);
    verify("VT_I4 neg excl-max", -32769, null);
    verify("VT_I4 neg excl-max > n > neg min", -1143483646, null);
    verify("neg min", -2147483648, null);

    verify("zero", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("numbers, floats");

    m_scen = "numbers, floats";

    verify("pos min", 2.2250738585072014e-308, null);
    verify("pos min < n < VT_R4 pos max", 9.87654321e12, null);
    verify("VT_R4 pos max", 3.402823466e+38, null);
    verify("VT_R8 pos excl-min", 3.402823467e+38, null);
    verify("VT_R8 pos excl-min < n < pos max", 4.43269743388067e107, null);
    verify("pos max", 1.7976931348623158e308, null);

    verify("neg max", -2.2250738585072012595e-308, null);
    verify("neg max > n > VT_R4 neg min", -6.61005335930233e19, null);
    verify("VT_R4 neg min", -3.402823466e+38, null);
    verify("VT_R8 neg excl-max", -3.402823467e+38, null);
    verify("VT_R8 neg excl-max > n > neg min", -7.17170763763262e243, null);
    verify("neg min", -1.797693134862315807e308, null);

    verify("zero", 0.0, null);


    //----------------------------------------------------------------------------
    apInitScenario("null");

    m_scen = "null";

    verify("", null, null);


    //----------------------------------------------------------------------------
    apInitScenario("strings");
    
    m_scen = "strings";

    verify("zls", "", null);
    verify("ordinary", "ordinary", null);

    //----------------------------------------------------------------------------
    apInitScenario("built-in classes");

    m_scen = "built-in classes";

    verify("Array", Array, null);
    verify("Boolean", Boolean, null);
    verify("Date", Date, null);
    verify("Function", Function, null);
//    verify("Math", Math, null);
    verify("Number", Number, null);
    verify("Object", Object, null);
    verify("String", String, null);


    //----------------------------------------------------------------------------
    apInitScenario("built-in objects");

    m_scen = "built-in objects";

    verify("Array", new Array(), null);
    verify("Boolean", new Boolean(), null);
    verify("Date", new Date(), null);
    verify("Function", new Function(), null);
//    verify("Math", new Math(), null);
    verify("Number", new Number(), null);
    verify("Object", new Object(), null);
    verify("String", new String(), null);


    //----------------------------------------------------------------------------
    apInitScenario("udef objects");

    m_scen = "udef objects";

    verify("", new udObj(), null);


    //----------------------------------------------------------------------------
    apInitScenario("functions");
    
    m_scen = "functions";

    verify("", udObj, null);

    apEndTest();

}


proto006();


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
