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


var iTestID = 52743;


function verifyBool(scen, ob, nob, boolVal) {
    var t = 10;
    var n = 0;

    for ( ; eval(ob) ; )
        if (++n > t) ob = false;
        n--;

    if (n != (boolVal ? t : -1)) apLogFailInfo( scen+"--"+nob+" failed ", boolVal ? t : -1, n,"");
}


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function for106() {

    apInitTest("for106 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    verifyBool("built-in, non-exec",Math,"Math",true);


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    verifyBool("built-in, exec, not instanciated",Array,"Array",true);
    verifyBool("built-in, exec, not instanciated",Boolean,"Boolean",true);
    verifyBool("built-in, exec, not instanciated",Date,"Date",true);
    verifyBool("built-in, exec, not instanciated",Number,"Number",true);
    verifyBool("built-in, exec, not instanciated",Object,"Object",true);


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    verifyBool("built-in, exec, instanciated",new Array(),"new Array()",true);
    verifyBool("built-in, exec, instanciated",new Boolean(),"new Boolean()",true);
    verifyBool("built-in, exec, instanciated",new Date(),"new Date()",true);
    verifyBool("built-in, exec, instanciated",new Number(),"new Number()",true);
    verifyBool("built-in, exec, instanciated",new Object(),"new Object()",true);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    verifyBool("user-defined, not instanciated",obFoo,"obFoo",true);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    verifyBool("user-defined, instanciated",new obFoo(),"new obFoo()",true);


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    verifyBool("literal, number, integer",1,"min pos",true);
    verifyBool("literal, number, integer",1234567890,"min pos < n < max pos",true);
    verifyBool("literal, number, integer",2147483647,"max pos",true);

    verifyBool("literal, number, integer",-1,"max neg",true);
    verifyBool("literal, number, integer",-1234567890,"min neg < n < max neg",true);
    verifyBool("literal, number, integer",-2147483647,"min neg",true);

    verifyBool("literal, number, integer",0,"pos zero",false);
    verifyBool("literal, number, integer",-0,"neg zero",false);
   

    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    verifyBool("literal, number, float",2.225073858507202e-308,"min pos",true);
    verifyBool("literal, number, float",1.2345678e90,"min pos < n < max pos",true);
    verifyBool("literal, number, float",1.797693134862315807e308,"max pos",true);
    verifyBool("literal, number, float",1.797693134862315807e309,"> max pos float (1.#INF)",true);

    verifyBool("literal, number, float",-2.225073858507202e-308,"max neg",true);
    verifyBool("literal, number, float",-1.2345678e90,"min neg < n < max neg",true);
    verifyBool("literal, number, float",-1.797693134862315807e308,"min neg",true);
    verifyBool("literal, number, float",-1.797693134862315807e309,"< min neg float (-1.#INF)",true);

    verifyBool("literal, number, float",0.0,"pos zero",false);
    verifyBool("literal, number, float",-0.0,"neg zero",false);

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    verifyBool("literal, number, integer",0x1,"min pos",true);
    verifyBool("literal, number, integer",0x2468ACE,"min pos < n < max pos",true);
    verifyBool("literal, number, integer",0xFFFFFFFF,"max pos",true);

    verifyBool("literal, number, integer",-0x1,"max neg",true);
    verifyBool("literal, number, integer",-0x2468ACE,"min neg < n < max neg",true);
    verifyBool("literal, number, integer",-0xFFFFFFFF,"min neg",true);

    verifyBool("literal, number, integer",0x0,"pos zero",false);
    verifyBool("literal, number, integer",-0x0,"neg zero",false);


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    verifyBool("literal, number, integer",01,"min pos",true);
    verifyBool("literal, number, integer",01234567,"min pos < n < max pos",true);
    verifyBool("literal, number, integer",037777777777,"max pos",true);

    verifyBool("literal, number, integer",-01,"max neg",true);
    verifyBool("literal, number, integer",-01234567,"min neg < n < max neg",true);
    verifyBool("literal, number, integer",-037777777777,"min neg",true);

    verifyBool("literal, number, integer",00,"pos zero",false);
    verifyBool("literal, number, integer",-00,"neg zero",false);

    
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

    verifyBool("literal, string","' '","single space",true);
    verifyBool("literal, string","'                                                                   '","multiple spaces",true);
    verifyBool("literal, string","'false'","as false",true);
	verifyBool("literal, string","'0'","ns 0",true);
    verifyBool("literal, string","'1234567890'","ns > 0",true);
    verifyBool("literal, string","' 1234567890'","ns > 0, leading space",true);
    verifyBool("literal, string","'1234567890 '","ns > 0, trailing space",true);
    verifyBool("literal, string","'-1234567890'","ns < 0",true);
    verifyBool("literal, string","'foo'","as single word",true);
    verifyBool("literal, string","' foo'","as single word, leading space",true);
    verifyBool("literal, string","'foo '","as single word, trailing space",true);
    verifyBool("literal, string","'foo bar'","as multiple word",true);
    verifyBool("literal, string","' foo bar'","as multiple word, leading space",true);
    verifyBool("literal, string","'foo bar '","as multiple word, trailing space",true);
    verifyBool("literal, string","''","zls",false);

              
    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    verifyBool("constants",true, "true", true);
    verifyBool("constants",false,"false",false);


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    verifyBool("object references",null,"null",false);


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");
    
    var obUndef;
    verifyBool("undefined", obUndef,"", false);


    apEndTest();

}


for106();


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
