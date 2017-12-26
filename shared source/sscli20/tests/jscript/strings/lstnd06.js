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


var iTestID = 52874;

var ExitFlag;

function function1 ( Arg1,   Arg2) {
    var Foo;
    Foo = Arg1.lastIndexOf( Arg2);

    ExitFlag = ExitFlag + 1;
    if (ExitFlag > 16) {
	 apLogFailInfo("(Looping in function1)", "", "", "");
	 return 0;
    }

    if (Foo = 8) {
	return Foo;
    }
    return function1("." + Arg1, Arg2);
}

function function2 ( arg) {
    return arg.length;
}

function function3 ( Arg1,   Arg2) {
    return Arg1 + Arg2;
}

function lstnd06 () {

    apInitTest("lstnd06 ");

    var VarString1 , VarString2;
    var Temp  , Expected;
    ExitFlag = 0;

    apInitScenario("Scenario 1 - Use lastIndexOf() in a recursive function");

    VarString1 = "ABCDEFG";
    VarString2 = "........A";

    Temp = function1(VarString1, VarString2);
    Expected = 8;
    if (Temp != Expected) {
	 apLogFailInfo("(In a recursive function)", Expected, Temp, "");
    }

    apInitScenario("Scenario 2 - Use a function for the start value");

    VarString1 = "ABCDEFG";
    VarString2 = "BC";

    Temp = VarString1.lastIndexOf( VarString2,function2(VarString2) );
    Expected = 1;
    if (Temp != Expected) {
	 apLogFailInfo("(function as start)", Expected, Temp, "");
    }

    apInitScenario("Scenario 3 - Use a function for String Values");

    VarString1 = "ABCD";
    VarString2 = "EF";

    Temp = ("" + function3(VarString1, "EFG")).lastIndexOf(VarString2)
    Expected = 4;
    if (Temp != Expected) {
	 apLogFailInfo("(function as CString value 1)", Expected, Temp, "");
    }

    VarString1 = "ABCDEFG";
    VarString2 = "E";

    Temp = VarString1.lastIndexOf(function3(VarString2, "F"));
    Expected = 4;
    if (Temp != Expected) {
	 apLogFailInfo("(function as CString value 2)", Expected, Temp, "");
    }

    apEndTest();
}


lstnd06();


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
