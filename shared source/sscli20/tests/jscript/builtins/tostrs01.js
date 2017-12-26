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


var iTestID = 53718;

function tostrs01() {

    apInitTest("toStrS01 ");

    var Result , Expected;
    var FixedResult;
    var StringVar;
    var FixedVar;
    var IntVar;
    var LongVar;
    var DoubleVar;

    apInitScenario(" 1: Use MID$ with a string constant for the string expression");

    Result = "Test".toString( 1, 2);
    Expected = "Test";

    if ( Result != Expected ) {
	 apLogFailInfo("toString() with string constant failed" ,Expected,Result,"");
    }

    apInitScenario(" 2: Use toString() with a string variable for the string expression");

    StringVar = "Test";
    Result = StringVar.toString( 1, 3);
    Expected = "Test";

    if ( Result != Expected ) {
	 apLogFailInfo("toString() with string variable failed" ,Expected,Result,"");
    }

    apInitScenario(" 3: Use toString() with a string expression for the string expression");

    StringVar = "Test"
    Result = (StringVar + StringVar).toString( 1, 5)
    Expected = "TestTest"

    if ( Result != Expected ) {
	 apLogFailInfo("toString() with string expression failed" ,Expected,Result,"");
    }

    apInitScenario(" 6: Use toString() with a numeric variable for the start value."  );

    IntVar = 2;
    StringVar = "Testing toString()";
    Result = StringVar.toString(IntVar, 7)
    Expected = "Testing toString()"

    if ( Result != Expected ) {
	 apLogFailInfo("INTEGER variable START failed" ,Expected,Result,"");
    }

    DoubleVar = 2.99999999999;
    StringVar = "Testing toString()";
    Result = StringVar.toString( DoubleVar, 7);
    Expected = "Testing toString()";

    if ( Result != Expected ) {
	 apLogFailInfo("DOUBLE variable START failed" ,Expected,Result,"");
    }

    apInitScenario(" 7: Use toString() with a numeric expression for the start value.");

    IntVar = 1;
    StringVar = "Testing toString()";
    Result = StringVar.toString( IntVar * 2, 7);
    Expected = "Testing toString()" ;

    if ( Result != Expected ) {
	 apLogFailInfo("INTEGER expression START failed" ,Expected,Result,"");
    }

    apInitScenario(" 8: Use toString() with a numeric constant for the end value.");

    StringVar = "Testing toString()";
    Result = StringVar.toString( 1, 4.9999999999);
    Expected = "Testing toString()";

    if ( Result != Expected ) {
	 apLogFailInfo("DOUBLE constant LENGTH failed" ,Expected,Result,"");
    }

    apInitScenario(" 9: Use toString() with a numeric variable for the end value.");

    IntVar = 5;
    StringVar = "Testing toString()";
    Result = StringVar.toString( 2, IntVar);
    Expected = "Testing toString()";

    if ( Result != Expected ) {
	 apLogFailInfo("INTEGER variable LENGTH failed" ,Expected,Result,"");
    }

    DoubleVar = 4.99999999999;
    StringVar = "Testing toString()";
    Result = StringVar.toString( 7, DoubleVar);
    Expected = "Testing toString()";

    if ( Result != Expected ) {
	 apLogFailInfo("DOUBLE variable LENGTH failed" ,Expected,Result,"");
    }

    apInitScenario("10: Use toString() with a numeric expression for the end value.");

    IntVar = 3;
    StringVar = "Testing toString()";
    Result = StringVar.toString( 4, IntVar * 2 + 1);
    Expected = "Testing toString()"

    if ( Result != Expected ) {
	 apLogFailInfo("INTEGER expression LENGTH failed" ,Expected,Result,"");
    }

    apInitScenario("Use toString() with double expression in end value");

    DoubleVar = 14.99999999999;
    StringVar = "Testing toString()";
    Result = StringVar.toString( 4, DoubleVar / 2)
    Expected = "Testing toString()";

    if ( Result != Expected ) {
	 apLogFailInfo("DOUBLE expression LENGTH failed" ,Expected,Result,"");
    }

    apEndTest();

}


tostrs01();


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
