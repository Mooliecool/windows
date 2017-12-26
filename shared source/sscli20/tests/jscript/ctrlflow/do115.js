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


var iTestID = 52272;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end



function verify(scen, ob, nob, f) {
    var n = 2;

    if (f) {
        do {
            n--;
            if (n--) break;
        } while (!ob);
        n++;
    }
    else {
        do {
            n--;
            if (n--) break;
        } while (ob);
        n++;
    }

    
      

    if (n != 1) apLogFailInfo( scen+"--"+nob+" failed ", 1, n, "");


}


function do115() {

    apInitTest("do115 ");


    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    verify("built-in, non-exec",Math,"Math",true);


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    verify("built-in, exec, not instanciated",Array,"Array",true);
    verify("built-in, exec, not instanciated",Boolean,"Boolean",true);
    verify("built-in, exec, not instanciated",Date,"Date",true);
    verify("built-in, exec, not instanciated",Number,"Number",true);
    verify("built-in, exec, not instanciated",Object,"Object",true);


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    verify("built-in, exec, instanciated",new Array(),"new Array()",true);
    verify("built-in, exec, instanciated",new Boolean(),"new Boolean()",true);
    verify("built-in, exec, instanciated",new Date(),"new Date()",true);
    verify("built-in, exec, instanciated",new Number(),"new Number()",true);
    verify("built-in, exec, instanciated",new Object(),"new Object()",true);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    verify("user-defined, not instanciated",obFoo,"obFoo",true);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    verify("user-defined, instanciated",new obFoo(),"new obFoo()",true);


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    verify("literal, number, integer",1,"min pos",true);
    verify("literal, number, integer",1234567890,"min pos < n < max pos",true);
    verify("literal, number, integer",2147483647,"max pos",true);

    verify("literal, number, integer",-1,"max neg",true);
    verify("literal, number, integer",-1234567890,"min neg < n < max neg",true);
    verify("literal, number, integer",-2147483647,"min neg",true);

    verify("literal, number, integer",0,"pos zero",false);
    verify("literal, number, integer",-0,"neg zero",false);
   

    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    verify("literal, number, float",2.225073858507202e-308,"min pos",true);
    verify("literal, number, float",1.2345678e90,"min pos < n < max pos",true);
    verify("literal, number, float",1.797693134862315807e308,"max pos",true);
    verify("literal, number, float",1.797693134862315807e309,"> max pos float (1.#INF)",true);

    verify("literal, number, float",-2.225073858507202e-308,"max neg",true);
    verify("literal, number, float",-1.2345678e90,"min neg < n < max neg",true);
    verify("literal, number, float",-1.797693134862315807e308,"min neg",true);
    verify("literal, number, float",-1.797693134862315807e309,"< min neg float (-1.#INF)",true);

    verify("literal, number, float",0.0,"pos zero",false);
    verify("literal, number, float",-0.0,"neg zero",false);

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    verify("literal, number, integer",0x1,"min pos",true);
    verify("literal, number, integer",0x2468ACE,"min pos < n < max pos",true);
    verify("literal, number, integer",0xFFFFFFFF,"max pos",true);

    verify("literal, number, integer",-0x1,"max neg",true);
    verify("literal, number, integer",-0x2468ACE,"min neg < n < max neg",true);
    verify("literal, number, integer",-0xFFFFFFFF,"min neg",true);

    verify("literal, number, integer",0x0,"pos zero",false);
    verify("literal, number, integer",-0x0,"neg zero",false);


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    verify("literal, number, integer",01,"min pos",true);
    verify("literal, number, integer",01234567,"min pos < n < max pos",true);
    verify("literal, number, integer",037777777777,"max pos",true);

    verify("literal, number, integer",-01,"max neg",true);
    verify("literal, number, integer",-01234567,"min neg < n < max neg",true);
    verify("literal, number, integer",-037777777777,"min neg",true);

    verify("literal, number, integer",00,"pos zero",false);
    verify("literal, number, integer",-00,"neg zero",false);

    
    //----------------------------------------------------------------------------
    apInitScenario("10. string");

    verify("literal, string"," ","single space",true);
    verify("literal, string","                                                                   ","multiple spaces",true);
    verify("literal, string","false","as false",true);
	verify("literal, string","0","ns 0",true);
    verify("literal, string","1234567890","ns > 0",true);
    verify("literal, string"," 1234567890","ns > 0, leading space",true);
    verify("literal, string","1234567890 ","ns > 0, trailing space",true);
    verify("literal, string","-1234567890","ns < 0",true);
    verify("literal, string","foo","as single word",true);
    verify("literal, string"," foo","as single word, leading space",true);
    verify("literal, string","foo ","as single word, trailing space",true);
    verify("literal, string","foo bar","as multiple word",true);
    verify("literal, string"," foo bar","as multiple word, leading space",true);
    verify("literal, string","foo bar ","as multiple word, trailing space",true);
    verify("literal, string","","zls",false);

              
    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    verify("constants",true, "true", true);
    verify("constants",false,"false",false);


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    verify("object references",null,"null",false);


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");
    
    var obUndef;
    verify("undefined", obUndef,"", false);


    apEndTest();

}


do115();


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
