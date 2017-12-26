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


var iTestID = 52875;


function lstnd07 () {

    apInitTest("lstnd07 ");

    var VariableString = "ABLE WAS I ERE I SAW ELBA";
    var Temp;

    apInitScenario("1. Verify the return value is correct when the start is negative");

    Temp = VariableString.lastIndexOf("ERE",-1);
    var ex = -1;

    if (Temp != ex) {
	 apLogFailInfo("negative start string had wrong result",ex,Temp,"");
    }
    if (typeof(Temp) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(Temp),"");
    }

    apInitScenario("2. Verify the return value is correct when the start left of target");

    Temp = VariableString.lastIndexOf("ERE",10);
    ex = -1;

    if (Temp != ex) {
	 apLogFailInfo("wrong result when starting left of target",ex,Temp,"");
    }
    if (typeof(Temp) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(Temp),"");
    }

    apInitScenario("3. Verify the result when target not in string ");

    Temp = VariableString.lastIndexOf("PUNK");
    ex = -1;

    if (Temp != ex) {
	 apLogFailInfo("wrong result when starting left of target",ex,Temp,"");
    }
    if (typeof(Temp) != "number") {
	apLogFailInfo("result had wrong type","number",typeof(Temp),"");
    }

    apEndTest();
}


lstnd07();


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
