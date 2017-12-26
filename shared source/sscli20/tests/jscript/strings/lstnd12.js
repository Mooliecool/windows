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


var iTestID = 52877;

function StrFun (X) {
    return X;
}

function lstnd12 () {

    apInitTest("lstnd12 ");

    var CS = "String1";

    var S, F, L, ex;

    apInitScenario(" 3. lastIndexOf()(i, x$, y$), len(x$) < len(y$), y$ consists of x$");

    F = "String1234";
    L = CS.lastIndexOf(F,0);

    if (L != -1) {
       apLogFailInfo("case 3: lastIndexOf() returns " + L, "", "", "");
    }

    apInitScenario(" 5. lastIndexOf()(i, x$, y$), len(x$) = len(y$), y$ consists of x$");

    F = "fixlenstr1";
    S = "fixlenstr1";
    L = F.lastIndexOf(S,-1);


    @if (@_jscript_version < 5.5)
        ex = -1; 
    @else
        ex = 0;
    @end

    if (L != ex) {
      apLogFailInfo("case 5: lastIndexOf() returns " + L, ex,L, "");
    }

    apInitScenario(" 6. lastIndexOf()(i, x$, y$), len(x$) > len(y$), y$ does not consist of x$");

    L = ("" + StrFun("Longer than")).lastIndexOf(CS, 6);

    if (L != -1) {
       apLogFailInfo("case 6: lastIndexOf() returns " + L, "", "", "");
    }

    apEndTest();
}


lstnd12();


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
