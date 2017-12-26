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


var iTestID = 51735;


function verify (vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

function rgJoin (rg,sep)
{
    for (var s="",i=0; i<rg.length; i++)
        s += sep+rg[i];
    return s;
}


function arsort01 ()
{
@if(@_fast)
    var rg;
@end
    apInitTest("arSort01 ");


    //----------------------------------------------------------------------------
    apInitScenario("zero elements");

    rg = new Array();
    rg.sort();

    verify(rgJoin(rg,""), "", null);


    //----------------------------------------------------------------------------
    apInitScenario("single ucase char");

    rg = new Array("Q");
    rg.sort();

    verify(rgJoin(rg,""), "Q", null);


    //----------------------------------------------------------------------------
    apInitScenario("single lcase char");

    rg = new Array("x");
    rg.sort();

    verify(rgJoin(rg,""), "x", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char ucase string, 2+ elmt rg, no dupes, sorted");

    rg = new Array("A","B","C","D","E","F","G");
    rg.sort();

    verify(rgJoin(rg,""), "ABCDEFG", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char lcase string, 2+ elmt rg, no dupes, sorted");

    rg = new Array("a","b","c","d","e","f","g");    
    rg.sort();

    verify(rgJoin(rg,""), "abcdefg", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char mixed case string, 2+ elmt rg, no dupes, sorted");

    rg = new Array("a","B","C","d","e","F","G");    
    rg.sort();

    verify(rgJoin(rg,""), "BCFGade", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char ucase string, 2+ elmt rg, no dupes");

    rg = new Array("A","E","R","O","L","I","T","H");    
    rg.sort();

    verify(rgJoin(rg,""), "AEHILORT", null);


    //----------------------------------------------------------------------------
    apInitScenario("Single char lcase string, 2+ elmt rg, no dupes");

    rg = new Array("a","e","r","o","l","i","t","h");    
    rg.sort();

    verify(rgJoin(rg,""), "aehilort", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char mixed case string, 2+ elmt rg, no dupes");

    rg = new Array("A","E","r","o","L","i","T","h");    
    rg.sort();

    verify(rgJoin(rg,""), "AELThior", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char ucase string, 2+ elmt rg, dupes");

    rg = new Array("D","E","N","O","U","E","M","E","N","T");
    rg.sort();

    verify(rgJoin(rg,""), "DEEEMNNOTU", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char lcase string, 2+ elmt rg, dupes");

    rg = new Array("d","e","e","e","m","n","n","o","u","t");
    rg.sort();

    verify(rgJoin(rg,""), "deeemnnotu", null);


    //----------------------------------------------------------------------------
    apInitScenario("single char lcase string, 2+ elmt rg, dupes");

    rg = new Array("d","E","E","e","m","N","n","o","U","T");
    rg.sort();

    verify(rgJoin(rg,""), "EENTUdemno", null);


    apEndTest();

}


arsort01();


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
