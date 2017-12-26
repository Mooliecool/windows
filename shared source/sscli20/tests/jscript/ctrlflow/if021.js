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


var iTestID = 52843;


function if021() {

    apInitTest("if021 ");

    var f = false;

    //----------------------------------------------------------------------------
    apInitScenario("1. empty terminated single statement if");

    if (true) ;

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("2. empty block if");

    if (true) {}

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("3. empty terminated block if");

    if (true) {};

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("4. empty terminated single statement if, empty tss else");

    if (true) ; else ;

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("5. empty block if, empty block else");

    if (true) ; else {}

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("6. empty terminated block if, empty term. block else");

    if (true) ; else {};

	// success if compiled without error


    //----------------------------------------------------------------------------
    apInitScenario("7. executable if, empty terminated single statement else");

    f = false;
    if (true) f=true; else ;

	if (f!=true) apLogFailInfo( "clause 7 failed ", true, f,"");


    //----------------------------------------------------------------------------
    apInitScenario("8. executable if, empty block else");

    f = false;
    if (true) f=true; else {}

	if (f!=true) apLogFailInfo( "clause 8 failed ", true, f,"");


    //----------------------------------------------------------------------------
    apInitScenario("9. executable if, empty terminated block else");

    f = false;
    if (true) f=true; else {};

	if (f!=true) apLogFailInfo( "clause 9 failed ", true, f,"");


    //----------------------------------------------------------------------------
    apInitScenario("10. empty terminated single if, executable else");

    f = false;
    if (false) ; else f=true;

	if (f!=true) apLogFailInfo( "clause 10 failed ", true, f,"");


    //----------------------------------------------------------------------------
    apInitScenario("11. empty block if, executable else");

    f = false;
    if (false) {} else f=true;

	if (f!=true) apLogFailInfo( "clause 11 failed ", true, f,"");

    //----------------------------------------------------------------------------
//  this is no longer legal (didn't make sense anyway)
//  apInitScenario("12. empty terminated block if, executable else");
/*
    f = false;
    if (false) {}; else f=true;

	if (f!=true) apLogFailInfo( "clause 12 failed ", true, f,"");
*/

    apEndTest();

}


if021();


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
