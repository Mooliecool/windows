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


var iTestID = 52870;

function indxf19 () {

    apInitTest("indxf19 ");

    var VariableString;
    var FixedString;
    var Temp;
    var NullString;

    apInitScenario("Scenario 1 - Omit START, verify that 0 is assumed");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";
    FixedString = "ABCDEFG-ABCDEFG-ABCDEFG"    ;

    Temp = "ABCDEFG".indexOf("A")
    if (Temp != 0) {
	 apLogFailInfo("(indexOf() without START - constant string)", "", "", "");
    }

    Temp = VariableString.indexOf("A")
    if (Temp != 0) {
	 apLogFailInfo("(indexOf() without START - variable string)", "", "", "");
    }

    Temp = FixedString.indexOf("A")
    if (Temp != 0) {
	 apLogFailInfo("(indexOf() without START - fixed string)", "", "", "");
    }

    apInitScenario("Scenario 3 - Verify that the return value is correct when the string");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";

    Temp = "ABCDEFG-ABCDEFG".indexOf("BC",2);
    if (Temp != 9) {
	 apLogFailInfo("(START != 1 - constant string)", "", "", "");
    }

    Temp = VariableString.indexOf("CDE",3);
    if (Temp != 10) {
	 apLogFailInfo("(START != 1 - variable string)", "", "", "");
    }

    apInitScenario("Scenario 4 - Verify that when START is greater than the length");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "ABCDEFG-ABCDEFG".indexOf("BC",14)
    if (Temp != -1) {
	 apLogFailInfo("(START > Length - constant string)", -1, Temp, "");
    }

    Temp = VariableString.indexOf( "CDE",24)
    if (Temp != -1) {
	 apLogFailInfo("(START > Length - variable string)", -1, Temp, "");
    }

    apInitScenario("Scenario 5 - Verify that a null string expression 1 returns zero");

    VariableString = "";

    Temp = "".indexOf( " ",1);
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - constant string)", -1, Temp, "");
    }

    Temp = VariableString.indexOf( " ")
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - variable string)", -1,Temp, "");
    }

    apInitScenario("Scenario 6 - Verify that when string expression 2 cannot be found,");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "ABCDEFG-ABCDEFG".indexOf("*BC")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found - constant string)", "", "", "");
    }

    Temp = VariableString.indexOf("CDE*")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found  - variable string)", "", "", "");
    }

    apInitScenario("Scenario 7 - Verify that when string expression 2 is null, the start");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"
    NullString = ""

    Temp = "ABCDEFG-ABCDEFG".indexOf(NullString,1);
    if (Temp != 1) {
	 apLogFailInfo("(String 2 is null  - constant string)", "", "", "");
    }

    Temp = VariableString.indexOf( NullString,2)
    if (Temp != 2) {
	 apLogFailInfo("(String 2 is null  - variable string)", "", "", "");
    }

    apEndTest();
}


indxf19();


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
