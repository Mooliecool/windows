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


var iTestID = 52909;


function or001() {

    apInitTest( "logOR001 ");

    var IntVar1, IntVar2;

    var Result, Expected;

    //--------------------------------------------------------------------
    apInitScenario( " 1: Use || with constants true/false");

    Result = (true || true);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (const) failed " ,"","","");
    }

    Result = (true || false);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (const) failed " ,"","","");
    }

    Result = (false || true);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (const) failed " ,"","","");
    }

    Result = (false || false);
    Expected = false;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (const) failed " ,"","","");
    }

    //--------------------------------------------------------------------
    apInitScenario( " 2: Use || with literals 1 for true and 0 for false");

    Result = (1 || 2);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (literal 1/1) failed " ,"","","");
    }

    Result = (1 || 0);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (literal 1/0) failed " ,"","","");
    }

    Result = (0 || 1);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (literal 0/1) failed " ,"","","");
    }

    Result = (0 || 0);
    Expected = 0;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (literal 0/0) failed " ,"","","");
    }

    //--------------------------------------------------------------------
    apInitScenario( " 3: Use || with literals (!=0) for true and 0 for false");

    Result = (25 || 32768);
    Expected = 25;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (literal 25/32768) failed " ,"","","");
    }

    Result = (1.2e208 || 0);
    Expected = 1.2e208;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (literal 1.2e208/0) failed " ,"","","");
    }

    Result = (0 || -257);
    Expected = -257;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (literal 0/-257) failed " ,"","","");
    }

    Result = (0 || 0);
    Expected = 0;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (literal 0/0) failed " ,"","","");
    }


    //--------------------------------------------------------------------
    apInitScenario( " 4: Use || with variables holding boolean constants");

    IntVar1 = true;
    IntVar2 = true;
    Result = (IntVar1 || IntVar2);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (boolean vars) failed " ,"","","");
    }

    IntVar1 = true;
    IntVar2 = false;
    Result = (IntVar1 || IntVar2);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (boolean vars) failed " ,"","","");
    }

    IntVar1 = false;
    IntVar2 = true;
    Result = (IntVar1 || IntVar2);
    Expected = true;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (boolean vars) failed " ,"","","");
    }

    IntVar1 = false;
    IntVar2 = false;
    Result = (IntVar1 || IntVar2);
    Expected = false;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (boolean vars) failed " ,"","","");
    }

    //--------------------------------------------------------------------
    apInitScenario( " 5: Use || with variables holding 1 for true and 0 for false");

    IntVar1 = 1;
    IntVar2 = 1;
    Result = (IntVar1 || IntVar2);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (literal vars) failed " ,"","","");
    }

    IntVar1 = 1;
    IntVar2 = 0;
    Result = (IntVar1 || IntVar2);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (literal vars) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 1;
    Result = (IntVar1 || IntVar2);
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (literal vars) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 0;
    Result = (IntVar1 || IntVar2);
    Expected = 0;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (literal vars) failed " ,"","","");
    }
    
    //--------------------------------------------------------------------
    apInitScenario( " 6: Use || with variables holding literals (!=0) for true and 0 for false");

    IntVar1 = 1.2e208;
    IntVar2 = -255;
    Result = (IntVar1 || IntVar2);
    Expected = 1.2e208;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (literal vars) failed " ,"","","");
    }

    IntVar1 = 288;
    IntVar2 = 0;
    Result = (IntVar1 || IntVar2);
    Expected = 288;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (literal vars) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 1.6e-10;
    Result = (IntVar1 || IntVar2);
    Expected = 1.6e-10;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (literal vars) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 0;
    Result = (IntVar1 || IntVar2);
    Expected = 0;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (literal vars) failed " ,"","","");
    }

    
    //--------------------------------------------------------------------
    apInitScenario( " 7: Use || with expressions");

    IntVar1 = 1;
    IntVar2 = 0;
    Result = ((IntVar1 - IntVar2) || (IntVar1 - IntVar2));
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/T (expression) failed " ,"","","");
    }

    IntVar1 = 1;
    IntVar2 = 0;
    Result = ((IntVar1 + IntVar2) || (IntVar2 / IntVar1));
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or T/F (expression) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 1;
    Result = ((IntVar1 / IntVar2) || (IntVar2 + IntVar1));
    Expected = 1;

    if (Result != Expected) {
     apLogFailInfo( "or F/T (expression) failed " ,"","","");
    }

    IntVar1 = 0;
    IntVar2 = 0;
    Result = ((IntVar1 + IntVar2) || (IntVar2 - IntVar2));
    Expected = 0;

    if (Result != Expected) {
     apLogFailInfo( "or F/F (expression) failed " ,"","","");
    }


    apEndTest();

}



or001();


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
