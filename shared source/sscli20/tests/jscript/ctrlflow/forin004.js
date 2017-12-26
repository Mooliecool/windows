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


var iTestID = 52762;


function verify (scen, ob, nob, cMem) 
{
    var n = 1;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var pOb;
     
@end       

    pOb = new Object();
    pOb[0] = ob;

    for (var i in pOb[0]) 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo( scen+"--"+nob+" failed ",cMem,n,"");
}


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function forin004() {

    apInitTest("forIn004 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    verify("built-in, non-executable",Math,"Math",0);


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    verify("built-in, exec, not instantiated",Array,"Array",0);
    verify("built-in, exec, not instantiated",Boolean,"Boolean",0);
    verify("built-in, exec, not instantiated",Date,"Date",0);
    verify("built-in, exec, not instantiated",Number,"Number",0);
    verify("built-in, exec, not instantiated",Object,"Object",0);


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    verify("built-in, exec, instantiated",new Array(),"new Array()",0);
    verify("built-in, exec, instantiated",new Boolean(),"new Boolean()",0);
    verify("built-in, exec, instantiated",new Date(),"new Date()",0);
    verify("built-in, exec, instantiated",new Number(),"new Number()",0);
    verify("built-in, exec, instantiated",new Object(),"new Object()",0);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    // should have members: length, arguments, caller, and prototype

    verify("user-defined, not instantiated", obFoo, "obFoo", 0);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    verify("user-defined, instantiated", new obFoo(), "new obFoo()", 0);


    //----------------------------------------------------------------------------
    apInitScenario("6. string");

    verify("string"," ","single space",0);
    verify("string","                                                                   ","multiple spaces",0);
    verify("string","foo","as single word",0);
    verify("string"," foo","as single word, leading space",0);
    verify("string","foo ","as single word, trailing space",0);
    verify("string","foo bar","as multiple word",0);
    verify("string"," foo bar","as multiple word, leading space",0);
    verify("string","foo bar ","as multiple word, trailing space",0);
    verify("string","","zls",0);


    apEndTest();

}


forin004();


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
