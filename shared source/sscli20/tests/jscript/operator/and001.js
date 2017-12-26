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


var iTestID = 52908;


function and001() {

    apInitTest( "logAND001 " );

    var Result;
    var Expected;

    //--------------------------------------------------------------------
    apInitScenario( " 1: Use logical AND with true/false constants " );

    Result = true && true;
    Expected = true;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/T (const) failed ","","","" );

    Result = true && false;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/F (consts) failed ","","","" );

    Result = false && true;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/T (consts) failed ","","","" );

    Result = false && false;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/F (consts) failed ","","","" );

    Result = -1 && -1;
    Expected = -1;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/T (num const) failed ","","","" );

    Result = -1 && 0;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/F (num const) failed ","","","" );

    Result = 0 && -1;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/T (num const) failed ","","","" );

    Result = 0 && 0;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/F (num const) failed ","","","" );

    //--------------------------------------------------------------------
    apInitScenario( " 2: Use logical AND with variables holding consts " );

    var Var1, Var2;

    Var1 = true;
    Var2 = true;
    Result = Var1 && Var2;
    Expected = true;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/T (variable const) failed ","","","" );

    Var1 = true;
    Var2 = false;
    Result = Var1 && Var2;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/F (variable const) failed ","","","" );

    Var1 = false;
    Var2 = true;
    Result = Var1 && Var2;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/T (variable const) failed ","","","" );

    Var1 = false;
    Var2 = false;
    Result = Var1 && Var2;
    Expected = false;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/F (variable const) failed ","","","" );

    //--------------------------------------------------------------------
    apInitScenario( " 3: Use logical AND with expressions of each type " );

    Var1 = 0;
    Var2 = 1;
    Result = Var1 - Var2 && Var1 - Var2;
    Expected = -1;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/T (integer expr) failed " ,"","","" );

    Var1 = -1;
    Var2 = 0;
    Result = Var1 + Var2 && Var2 / Var1;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND T/F (integer expr) failed " ,"","","" );

    Var1 = 0;
    Var2 = -1;
    Result = Var1 / Var2 && Var2 + Var1;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/T (integer expr) failed " ,"","","" );

    Var1 = 0;
    Var2 = 0;
    Result = Var1 + Var2 && Var2 - Var2;
    Expected = 0;

    if ( Result != Expected )
        apLogFailInfo( "logical AND F/F (integer expr) failed " ,"","","" );

    apEndTest();
}


and001();


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
