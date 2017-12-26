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


var iTestID = 53655;

function lcase07 () {
 
    apInitTest("lcase07 ");

    var Expected, Result;
    var Temp1;
    var Temp2;

    apInitScenario("Scenario 1 - Use lcase with string constant");

    Result = "ABCDEFG".toLowerCase();
    Expected = "abcdefg";
    if ( Result != Expected ) {
	 apLogFailInfo("(lcase with constant returned " + Result + " not " + Expected + ")", "", "", ""  );
    }

    apInitScenario("Scenario 2 - Use lcase with string variable");

    Temp1 = "ABCDEFGH";
    Result = Temp1.toLowerCase();
    Expected = "abcdefgh";
    if ( Result != Expected ) {
	 apLogFailInfo("(lcase with variable returned " + Result + " not " + Expected + ")", "", "", "");
    }

    apInitScenario("Scenario 4 - Use lcase with string expressions");

    Temp1 = "ABC";
    Temp2 = "DEFGHIJKLM";
    Result = (Temp1 + Temp2).toLowerCase();
    Expected = "abcdefghijklm";
    if ( Result != Expected ) {
	 apLogFailInfo("(lcase with string expression returned " + Result + " not " + Expected + ")", "", "", "");
    }
 
    apEndTest();
}


lcase07();


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
