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


var iTestID = 53104;


@if(@_fast)
	var test1, test5, test7, test9;
@end

function verify(sCat, vAct, vExp, bugNum){
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function number01(){

    var SV = ScriptEngineMajorVersion() + (ScriptEngineMinorVersion()/10);
    var loc_id = apGetLocale();
    apInitTest("number01 ");

//----------------------------------------------------------------------------
    apInitScenario("1. test construction");

    test1 = new Number();
    // success means no error.

//----------------------------------------------------------------------------
    apInitScenario("2. verify construction with typeof");

    verify("typeof verification", typeof (new Number()), "object", null);

//----------------------------------------------------------------------------
    apInitScenario("3. verify initialization");

    verify("typeof verification", (new Number(0xffffffff))-0, 0xffffffff, null);

//----------------------------------------------------------------------------
    apInitScenario("4. verify default value");

    verify("verify default value", (new Number())-0, 0, null);

//----------------------------------------------------------------------------
    apInitScenario("5. verify indexed member addition, explicit lval");

    verify("obj member addition, explicit lval", (test5 = new Number())[87] = "bar", "bar", null);

//----------------------------------------------------------------------------
    apInitScenario("6. verify indexed member addition, implicit lval");

    verify("obj member addition, implicit lval", (new Number())[87] = "bar", "bar", null);

//----------------------------------------------------------------------------
    apInitScenario("7. verify named member addition, explicit lval");

    verify("obj member addition, explicit lval", (test7 = new Number()).foo = "bar", "bar", null);

//----------------------------------------------------------------------------
    apInitScenario("8. verify named member addition, implicit lval");

    verify("obj member addition, implicit lval", (new Number()).foo = "bar", "bar", null);

//----------------------------------------------------------------------------
    apInitScenario("9. verify toString() and toLocalString() ");

    var testNum = new Number(1);
    verify("verify toString() and toLocalString()", testNum.toString(), 1, "");
    if((loc_id==1033)||(loc_id==1041)||(loc_id==1025)) verify("verify toString() and toLocalString()", testNum.toLocaleString(), 1, "");
    else verify("verify toString() and toLocalString()", testNum.toLocaleString(), "1,00", "");

//----------------------------------------------------------------------------
    apInitScenario("9.5 verify toFixed() - 5.5 and up");

    if (SV >= 5.5)
      if (SV >= 7 ){
        var testNum = new Number(-1.5e-254);
        verify("verify toFixed()", testNum.toFixed(), "0", "VisualStudio");
      }else{
        var testNum = new Number(-1.5e-254);
        verify("verify toFixed()", testNum.toFixed(), "-0", "VisualStudio");
      }

//----------------------------------------------------------------------------
//ECMA 3 compliance
    apInitScenario("10. verify MIN_VALUE");

    verify("verify MIN_VALUE value", Number.MIN_VALUE, 5e-324, "VS7");
    verify("verify < Number.MIN_VALUE is -Infinity", Number.MIN_VALUE - 5e-324, 0, "VS7");

//----------------------------------------------------------------------------
    apInitScenario("11. verify MAX_VALUE");

    verify("verify MAX_VALUE value", Number.MAX_VALUE, 1.7976931348623157e+308, null);
    verify("verify > Number.MAX_VALUE is Infinity", Number.MAX_VALUE + 1e292, Infinity, null);

//----------------------------------------------------------------------------
//ECMA 3 compliance
    apInitScenario("12. verify Number.prototype.toFixed(fractionDigits)")
  @if (@_jscript_version >= 5.5 )
    verify("length property of toFixed()", Number.prototype.toFixed().length, 1, "VS");
    verify("length property after calling toFixed()", (2/3).toFixed(5).length, 7, null);
    verify("coerce 16 digit fraction", (0.01234567890123456).toFixed(16), 0.0123456789012346, "VS");
    verify("coerce fraction to 16 digits", (1/11).toFixed(16), 0.0909090909090909, "VS7");
    var test9 = -1.5e-254
    verify("coerce very small numbers", test9.toFixed(), -0, "VS7 344223");
   @end

//----------------------------------------------------------------------------
    apInitScenario("13. verify Number.prototype.toExponential(fractionDigits)")
  @if (@_jscript_version >= 5.5 )
    verify("length property of toExponential()", Number.prototype.toExponential.length, 1, null);
    verify("coerce NaN", NaN.toExponential(), "NaN", null);
    verify("coerce PI", (Math.PI).toExponential(4), 3.1416e+0, null);
    verify("coerce fraction", (1/3).toExponential(2), 3.33e-1, null);
    verify("coerce decimal number", (365.52).toExponential(20), 3.65520000000000000000e+2, null);
    verify("coerce 0", (0).toExponential(3), 0.000e+0, null);
   @end

//----------------------------------------------------------------------------
    apInitScenario("14. verify Number.prototype.toPrecision(precision)");
   @if (@_jscript_version >= 5.5 )
    verify("length property of toExponential()", Number.prototype.toPrecision().length, 1, null);
    verify("coerce NaN", (NaN).toPrecision(2), "NaN", null);
    verify("coerce PI", (Math.PI).toPrecision(4), 3.142, null);
    verify("coerce fraction", (1/3).toPrecision(2), 0.33, null);
    verify("coerce decimal number", (365.52).toPrecision(20), 365.52000000000000000, null);
    verify("coerce 0", (0).toPrecision(3), 0.00, null);
    verify("coerce many digits", (123456789.012345678901234567890).toPrecision(3), 1.23e8, null);
   @end

    apEndTest();
}


number01();


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
