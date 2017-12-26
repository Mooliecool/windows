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


var iTestID = 52873;

function lstnd03 () {

    apInitTest("lstnd03 ");

    var VarString1 , VarString2;
    var IntegerVar1  , IntegerVar2;
    var LongVar1  , LongVar2;
    var DoubleVar1  , DoubleVar2;
    var FixedString1 , FixedString2;

    apInitScenario("Scenario 1 - Use a numeric constant of each type for START");

    VarString1 = "ABCDEFG-1-ABCDEFG-1-ABCDEFG";

    var Temp = VarString1.lastIndexOf( "-1-",12);
    var Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as INTEGER constant)", Expected, Temp, "");
    }

    Temp = VarString1.lastIndexOf( "-1-",9.7);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as single constant)", Expected, Temp, "");
    }

    Temp = VarString1.lastIndexOf("-1-",22.00000099999)
    Expected = 17;
    if (Temp != Expected) {
	 apLogFailInfo("(START as DOUBLE constant)", Expected, Temp, "");
    }

    apInitScenario("Use a numeric variable of each type for START");

    VarString1 = "ABCDEFG-1-ABCDEFG-1-ABCDEFG";

    IntegerVar1 = 12;
    Temp = VarString1.lastIndexOf( "-1-",IntegerVar1);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as INTEGER constant)", Expected, Temp, "");
    }

    DoubleVar1 = 9.7;
    Temp = VarString1.lastIndexOf( "-1-",DoubleVar1);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as single constant)", Expected, Temp, "");
    }

    DoubleVar2 = 22.00000099999;
    Temp = VarString1.lastIndexOf("-1-",DoubleVar2);
    Expected = 17;
    if (Temp != Expected) {
	 apLogFailInfo("(START as DOUBLE constant)", Expected, Temp, "");
    }

    apInitScenario("Use a numeric expression for START");

    VarString1 = "ABCDEFG-1-ABCDEFG-1-ABCDEFG";

    IntegerVar1 = 11;
    Temp = VarString1.lastIndexOf( "-1-",IntegerVar1 + 1);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as INTEGER constant)", Expected, Temp, "");
    }

    DoubleVar1 = 5.7;
    Temp = VarString1.lastIndexOf( "-1-",DoubleVar1 * 2);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as single constant)", Expected, Temp, "");
    }

    DoubleVar2 = 170;
    Temp = VarString1.lastIndexOf("-1-",DoubleVar2 / 9);
    Expected = 17;
    if (Temp != Expected) {
	 apLogFailInfo("(START as DOUBLE constant)", Expected, Temp, "");
    }

    apInitScenario("Use a string constant of each type for START");

    VarString1 = "ABCDEFG-1-ABCDEFG-1-ABCDEFG";

    Temp = VarString1.lastIndexOf( "-1-","12");
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as INTEGER constant)", Expected, Temp, "");
    }

    Temp = VarString1.lastIndexOf( "-1-","9.7");
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as single constant)", Expected, Temp, "");
    }

    Temp = VarString1.lastIndexOf("-1-","22.00000099999")
    Expected = 17;
    if (Temp != Expected) {
	 apLogFailInfo("(START as DOUBLE constant)", Expected, Temp, "");
    }

    apInitScenario("Use a string variable of each type for START");

    VarString1 = "ABCDEFG-1-ABCDEFG-1-ABCDEFG";

    IntegerVar1 = "12";
    Temp = VarString1.lastIndexOf( "-1-",IntegerVar1);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as INTEGER constant)", Expected, Temp, "");
    }

    DoubleVar1 = "10.7";
    Temp = VarString1.lastIndexOf( "-1-",DoubleVar1);
    Expected = 7;
    if (Temp != Expected) {
	 apLogFailInfo("(START as single constant)", Expected, Temp, "");
    }

    DoubleVar2 = "22.00000099999";
    Temp = VarString1.lastIndexOf("-1-",DoubleVar2);
    Expected = 17;
    if (Temp != Expected) {
	 apLogFailInfo("(START as DOUBLE constant)", Expected, Temp, "");
    }

    apEndTest();
}


lstnd03();


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
