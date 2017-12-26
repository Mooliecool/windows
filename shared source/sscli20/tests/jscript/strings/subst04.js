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


var iTestID = 53662;

function function1 ( Arg1) {
    return Arg1 + Arg1;
}

function function2 ( Arg2) {
    return Arg2;
}

function function3 ( Arg3) {
    return Arg3.substring(1, 3);
}

function subst04() {

    apInitTest("subst04 ");

    var Result , Expected;
    var StringVar;

    apInitScenario(" 1: Use a function for the string expression");

    StringVar = "Test";
    Result = ("" + function1(StringVar)).substring(1, 6);
    Expected = "estTe";

    if ( Result != Expected ) {
	 apLogFailInfo("substring() with function for string failed" ,Expected,Result,"");
    }

    apInitScenario(" 2: Use a function for the START");

    StringVar = "TEST   ";
    Result = StringVar.substring(function2(1), 4);
    Expected = "EST";

    if ( Result != Expected ) {
	 apLogFailInfo("substring() with function for START failed" ,Expected,Result,"");
    }

    apInitScenario(" 3: Use a function for the end");

    StringVar = "TESTING";
    Result = StringVar.substring(2, function2(5));
    Expected = "STI";

    if ( Result != Expected ) {
	 apLogFailInfo("substring() with function for end failed" ,Expected,Result,"");
    }

    apInitScenario(" 4: Use substring in a function");

    StringVar = "TEST";
    Result = function3(StringVar);
    Expected = "ES";

    if ( Result != Expected ) {
	 apLogFailInfo("substring in a function failed" ,Expected,Result,"");
    }

    apInitScenario(" 5: test limits for substring");

    StringVar = "TEST";
    Result = StringVar.substring(0x100000001, 0x100000002);
    Expected = "";

    if ( Result != Expected ) {
	 apLogFailInfo("substring in a function failed" ,Expected,Result,"");
    }

    apEndTest();

    

}


subst04();


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
