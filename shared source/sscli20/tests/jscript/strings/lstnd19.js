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


var iTestID = 52879;

function lstnd19 () {

    apInitTest("lstnd19 ");

    var VariableString;
    var FixedString;
    var Temp;
    var NullString;
    var Expected;

    apInitScenario("Scenario 1 - Omit START, verify that 0 is assumed");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";
    FixedString = "ABCDEFG-ABCDEFG-ABCDEFG"    ;

    Temp = "ABCDEFG".lastIndexOf("A")
    Expected = 0;
    if (Temp != Expected) {
	 apLogFailInfo("(lastIndexOf() without START - constant string)", Expected, Temp, "");
    }

    Temp = VariableString.lastIndexOf("A")
    Expected = 16;
    if (Temp != Expected) {
	 apLogFailInfo("(lastIndexOf() without START - variable string)", Expected, Temp, "");
    }

    Temp = FixedString.lastIndexOf("A")
    Expected = 16;
    if (Temp != Expected) {
	 apLogFailInfo("(lastIndexOf() without START - fixed string)", Expected, Temp, "");
    }

    apInitScenario("Scenario 3 - Verify that the return value is correct when the string");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf("BC",2);
    if (Temp != 1) {
	 apLogFailInfo("(START != 1 - constant string)", 1,Temp, "");
    }

    Temp = VariableString.lastIndexOf("CDE",3);
    if (Temp != 2 ) {
	 apLogFailInfo("(START != 1 - variable string)", 2,Temp, "");
    }

    apInitScenario("Scenario 4 - Verify that when START is greater than the length");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf("CD",1)
    if (Temp != 0-1) {
	 apLogFailInfo("(START > Length - constant string)",-1,Temp, "");
    }

    Temp = VariableString.lastIndexOf( "CDE",1 )
    if (Temp != -1) {
	 apLogFailInfo("(START > Length - variable string)",-1,Temp, "");
    }

    apInitScenario("Scenario 5 - Verify that a null string expression 1 returns zero");

    VariableString = "";

    Temp = "".lastIndexOf( " ",1)
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - constant string)", "", "", "");
    }

    Temp = VariableString.lastIndexOf( " ")
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - variable string)", "", "", "");
    }

    apInitScenario("Scenario 6 - Verify that when string expression 2 cannot be found,");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf("*BC")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found - constant string)", "", "", "");
    }

    Temp = VariableString.lastIndexOf("CDE*")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found  - variable string)", "", "", "");
    }

    apInitScenario("Scenario 7 - Verify that when string expression 2 is null, the start");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"
    NullString = ""

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf(NullString,1);
    if (Temp != 1) {
	 apLogFailInfo("(String 2 is null  - constant string)", "", "", "");
    }

    Temp = VariableString.lastIndexOf( NullString,2)
    if (Temp != 2) {
	 apLogFailInfo("(String 2 is null  - variable string)", "", "", "");
    }
	
    apInitScenario("Scenario 8 - Verify that start is treated as largest posssible index when greater than length");
	
    Temp = "checking script for cool bugs".lastIndexOf("c", 50)
    Expected = 20;
    if (Temp != Expected) {
         apLogFailInfo("(START > length - substring not found)", Expected, Temp, "");
    }

    apEndTest();
}


lstnd19();


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
