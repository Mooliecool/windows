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


var iTestID = 52853;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function if111() {

    apInitTest("if111 ");

    var n, bOb;

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;
    bOb = 1;
    if (eval('Math')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }
    
    if (n != 2*bOb) apLogFailInfo( "non-executable object references--Math failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    n = 0;
    bOb = 1;
    if (eval('Array')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Array failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('Boolean')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Boolean failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('Date')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Date failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('Number')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Number failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('Object')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Object failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    n = 0;
    bOb = 1;
    if (eval('new Array()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Array failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('new Boolean()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Boolean failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('new Date()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Date failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('new Number()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Number failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('new Object()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Object failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    n = 0;
    bOb = 1;
    if (eval('obFoo')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--user-defined failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    var ob = new obFoo();
    n = 0;
    bOb = 1;
    if (eval('ob')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--var instantiated user-defined failed ", 2*bOb, n,"");

    n = 0;
    bOb = 1;
    if (eval('new obFoo()')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--instantiated user-defined failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        bOb = 1;
        if (eval('1')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        bOb = 1;
        if (eval('1234567890')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('2147483647')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max pos failed ", 2*bOb, n,"");

    // <0

        n = 0;
        bOb = 1;
        if (eval('-1')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-1234567890')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-2147483647')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg failed ", 2*bOb, n,"");

    // 0

        n = 0;
        bOb = -1;
        if (eval('0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        bOb = -1;
        if (eval('-0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "-zero failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("7. number, decimal, float");

    // >0.0

        n = 0;    
        bOb = 1;
        if (eval('2.225073858507202e-308')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        bOb = 1;
        if (eval('1.2345678e90')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos float < n < max pos failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('1.797693134862315807e308')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max pos failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('1.797693134862315807e309')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "> max pos (1.#INF) failed ", 2*bOb, n,"");

    // <0

        n = 0;
        bOb = 1;
        if (eval('-2.225073858507202e-308')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-1.2345678e90')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-1.797693134862315807e308')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-1.797693134862315807e309')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "< min neg (-1.#INF) failed ", 2*bOb, n,"");

    // 0.0

        n = 0;
        bOb = -1;
        if (eval('0.0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        bOb = -1;
        if (eval('-0.0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "-zero failed ", 2*bOb, n,"");

        
    //----------------------------------------------------------------------------
    apInitScenario("8. number, hexidecimal");

    // >0

        n = 0;    
        bOb = 1;
        if (eval('0x1')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        bOb = 1;
        if (eval('0x2468ACE')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('0xFFFFFFFF')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max pos failed ", 2*bOb, n,"");

    // <0

        n = 0;
        bOb = 1;
        if (eval('-0x1')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-0x2468ACE')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-0xFFFFFFFF')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg failed ", 2*bOb, n,"");

    // 0

        n = 0;
        bOb = -1;
        if (eval('0x0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        bOb = -1;
        if (eval('-0x0')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "-zero failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("9. number, octal");

    // >0

        n = 0;    
        bOb = 1;
        if (eval('01')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        bOb = 1;
        if (eval('01234567')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('037777777777')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max pos failed ", 2*bOb, n,"");

    // <0

        n = 0;
        bOb = 1;
        if (eval('-01')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-01234567')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('-037777777777')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg failed ", 2*bOb, n,"");

    // 0

        n = 0;
        bOb = -1;
        if (eval('00')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        bOb = -1;
        if (eval('-00')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "-zero failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("10. string");

        n = 0;    
        bOb = 1;
        if (eval('" "')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "single space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"                                                                   "')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "multiple spaces failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"false"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as false failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"0"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "ns 0 failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"1234567890"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "ns >0 failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('" 1234567890"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "ns >0, leading space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"1234567890 "')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "ns >0, trailing space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"-1234567890"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "ns <0 failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"foo"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as single word failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('" foo"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as single word, leading space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"foo "')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as single word, trailing space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"foo bar"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as multiple word failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('" foo bar"')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as multiple word, leading space failed ", 2*bOb, n,"");

        n = 0;
        bOb = 1;
        if (eval('"foo bar "')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "as multiple word, trailing space failed ", 2*bOb, n,"");

        n = 0;
        bOb = -1;
        if (eval('""')) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zls failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    bOb = 1;
    if (eval('true')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "constants--true failed ", 2*bOb, n,"");


    n = 0;
    bOb = -1;
    if (eval('false')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "constants--false failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("12. null");

    n = 0;
    bOb = -1;
    if (eval('null')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "null failed ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("13. undefined");
    
    n = 0;
	var obUndef;
    bOb = -1;
    if (eval('obUndef')) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "null failed ", 2*bOb, n,"");


    apEndTest();

}


if111();


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
