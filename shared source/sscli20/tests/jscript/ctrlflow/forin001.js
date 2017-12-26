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


var iTestID = 52759;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function forin001() 
{
    apInitTest("forIn001 ");

    var i, cMem;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec"); 
    
    cMem = 0;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var n;        
     
@end       

    n = 1; i = null;

    for (i in Math) 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo ("built-in, non-executable--Math failed", cMem, n, "");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    cMem=0;
    n = 1; i = null;

    for (i in Array) n++
        n--;

    if (n != cMem) apLogFailInfo( "built-in, exec, not instantiated--Array failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in Boolean) 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo( "built-in, exec, not instantiated--Boolean failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in Date) 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo( "built-in, exec, not instantiated--Date failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in Number) 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo( "built-in, exec, not instantiated--Number failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in Object) n++
        n--;

    if (n != cMem) apLogFailInfo( "built-in, exec, not instantiated--Object failed", cMem, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. user-defined, not instanciated");

    cMem=0;
    n = 1; i = null;

    for (i in obFoo) n++
        n--;

    if (n != cMem) apLogFailInfo( "user-defined, not instantiated failed", cMem, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. string");

    cMem=0;
    n = 1; i = null;

    for (i in " ") 
        n++
    n--;

    if (n != cMem) apLogFailInfo( "string--single space failed", cMem, n, "120142");

    cMem=0;
    n = 1; i = null;

    for (i in "                                                                   ") 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo ("string--multiple spaces failed", cMem, n, "120142");

    cMem=0;
    n = 1; i = null;

    for (i in "foo") 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo ("string--as single word failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in " foo") 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo ("string--as single word, leading space failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in "foo ") 
        n++
    n--;

    if (n != cMem) 
        apLogFailInfo ("string--as single word, trailing space failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in "foo bar") n++
        n--;

    if (n != cMem) apLogFailInfo( "string--as multiple word failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in " foo bar") n++
        n--;

    if (n != cMem) apLogFailInfo( "string--as multiple word, leading space failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in "foo bar ") n++
        n--;

    if (n != cMem) apLogFailInfo( "string--as multiple word, trailing space failed", cMem, n,"");

    cMem=0;
    n = 1; i = null;

    for (i in "") n++
        n--;

    if (n != cMem) apLogFailInfo( "string--zls failed", cMem, n,"");

    apEndTest();
}



forin001();


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
