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


var iTestID = 53719;

function tostrs02() {

    apInitTest("toStrS02 ");

    var Result , Expected;
    var StringVar;
    var SINGLEVar;

    apInitScenario(" 1: Use toString() with a numeric constant for the start value.");

    StringVar = "Testing MID";
    Result = StringVar.toString(5,2.01);
    Expected = "Testing MID"

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE constant START failed" ,Expected,Result,"");
    }

    apInitScenario(" 2: Use toString() with a numeric variable for the start value.");

    SINGLEVar = 2.999;
    StringVar = "Testing MID";
    Result = StringVar.toString( SINGLEVar, 6);
    Expected = "Testing MID";

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE variable START failed" ,Expected,Result,"");
    }

    apInitScenario(" 3: Use toString() with a numeric expression for the start value.");

    SINGLEVar = 1.99;
    StringVar = "Testing MID";
    Result = StringVar.toString( 5,SINGLEVar * 2);
    Expected = "Testing MID";

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE variable START failed" ,Expected,Result,"");
    }

    apInitScenario(" 4: Use toString() with a numeric constant for the end value.");

    StringVar = "Testing MID";
    Result = StringVar.toString( 4.999,2);
    Expected = "Testing MID";

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE constant LENGTH failed" ,Expected,Result,"");
    }

    apInitScenario(" 5: Use toString() with a numeric variable for the end value.");

    SINGLEVar = 7.999;
    StringVar = "Testing MID";
    Result = StringVar.toString( 4, SINGLEVar);
    Expected = "Testing MID";

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE variable LENGTH failed" ,Expected,Result,"");
    }

    apInitScenario(" 6: Use toString() with a numeric expression for the end value.");

    SINGLEVar = 15.999;
    StringVar = "Testing MID";
    Result = StringVar.toString( 5, SINGLEVar / 2)
    Expected = "Testing MID"

    if ( Result != Expected ) {
	 apLogFailInfo("SINGLE expression LENGTH failed" ,Expected,Result,"");
    }

    apEndTest();

}


tostrs02();


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
