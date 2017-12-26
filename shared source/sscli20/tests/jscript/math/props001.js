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


var iTestID = 53132;


function props001(){

    apInitTest("props001 ");

    var rgExp, rgAct, mem, c, i;

    var rgMathMemNames = new Array("E", "LN10", "LN2", "LOG10E", "LOG2E", "PI", "SQRT1_2", "SQRT2");

    var rgMathMemValues = new Array("2.718281828459045", "2.302585092994046", "0.6931471805599453", 
                                    "0.4342944819032518", "1.4426950408889634", "3.141592653589793", 
                                    "0.7071067811865476", "1.4142135623730951");

    //----------------------------------------------------------------------------
    apInitScenario("1. Verify visible member count");

    c = 0;
    for (mem in Math) ++c;

    if (c != 0)
        apLogFailInfo( "Verify member count failed", 0, c, "");


    //----------------------------------------------------------------------------
    apInitScenario("2. Verify member existence");

    var cMem=0;

    for (i=0; i<rgMathMemNames.length; i++)
	    if (null != Math[rgMathMemNames[i]])
            cMem++;

    if (cMem != 8)
        apLogFailInfo( "Verify member existence failed", cMem, 8, "");

    
    //----------------------------------------------------------------------------
    apInitScenario("4. Verify member types");

    c=0
    for (i=0; i<rgMathMemNames.length; i++)
	    if (typeof Math[rgMathMemNames[i]] == "number")
            c++;

    if (c != 8)
        apLogFailInfo( "Verify member types failed", 8, c, "");

    
    //----------------------------------------------------------------------------
    apInitScenario("5. Verify member values");

    c=0
    for (i=0; i<rgMathMemNames.length; i++)
	    if (near(Math[rgMathMemNames[i]],rgMathMemValues[i],15))
            c++;

    if (c != 8)
        apLogFailInfo( "Verify member values failed", 8, c, "");
    

    apEndTest();

}

function near(a,b,digits) {
  if ((a-b)/a < (a / Math.pow(10,digits) ))
    return true;
  return false;
}


props001();


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
