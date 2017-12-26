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


var iTestID = 52851;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function if109() 
{
    apInitTest("if109 ");

    var tmp1, tmp2, n, bOb;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var tmp;        
     
@end       

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

    n = 0;
    tmp1 = Math; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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
    tmp1 = Array; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Array failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = Boolean; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Boolean failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = Date; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Date failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = Number; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Number failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = Object; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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
    tmp1 = new Array(); tmp2 = "";
    bOb = -1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Array failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = new Boolean(); tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Boolean failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = new Date(); tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Date failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = new Number(); tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--Number failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = new Object(); tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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
    tmp1 = obFoo; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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

    n = 0;
    tmp1 = new obFoo(); tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "executable object references--var instantiated user-defined ", 2*bOb, n,"");


    //----------------------------------------------------------------------------
    apInitScenario("6. number, decimal, integer");

    // >0

        n = 0;    
        tmp1 = 1; tmp2 = 0;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        tmp1 = 1234567890; tmp2 = 123456789;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

		n = 0;
        tmp1 = 2147400000; tmp2 = 83647;
        bOb = 1;
        if (tmp1 + tmp2) {
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
        tmp1 = 0; tmp2 = 1;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 123456789; tmp2 = 1234567890;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -2147400000; tmp2 = 83647;
        bOb = 1;
        if (tmp1 - tmp2) {
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
        tmp1 = 1; tmp2 = 1;
        bOb = -1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -0; tmp2 = 0;
        bOb = -1;
        if (tmp1 + tmp2) {
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
        tmp1 = 2.225073858507202e-308; tmp2 = 0;
        bOb = 1;
        if (tmp1 + tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        tmp1 = 1.2345678e90; tmp2 = 9.98;
        bOb = 1;
        if (tmp1 / tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 1.0e308; tmp2 = 7.97693134862315807e307;
        bOb = 1;
        if (tmp1 + tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max pos failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 1.797693134862315807e308; tmp2 = 1.0e308;
        bOb = 1;
        if (tmp1 + tmp2) {
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
        tmp1 = 0; tmp2 = -2.225073858507202e-308;
        bOb = 1;
        if (tmp1 + tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -1.2345678e90; tmp2 = 2;
        bOb = 1;
        if (tmp1 * tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 1.797693134862315807e308; tmp2 = -1;
        bOb = 1;
        if (tmp1 * tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -1.797693134862315807e309; tmp2 = 1.999e307;
        bOb = 1;
        if (tmp1 - tmp2) {
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
        tmp1 = 1.9; tmp2 = 1.9;
        bOb = -1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 0.0; tmp2 = -1.0;
        bOb = -1;
        if (tmp1 * tmp2) {
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
        tmp1 = 0x1; tmp2 = 0x0;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        tmp1 = 0x9ABCDEF; tmp2 = 0x2468ACE;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

		n = 0;
        tmp1 = 0xFFFF0000; tmp2 = 0xFFFF;
        bOb = 1;
        if (tmp1 + tmp2) {
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
        tmp = 0x0;
        bOb = 1;
        if (tmp - 0x1) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 0x2468ACE; tmp2 = 0x9ABCDEF;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -0xFFFF0000; tmp2 = 0xFFFF;
        bOb = 1;
        if (tmp1 - tmp2) {
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
        tmp1 = 0x1; tmp2 = 0x1;
        bOb = -1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -0x0; tmp2 = 0x0;
        bOb = -1;
        if (tmp1 + tmp2) {
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
        tmp = 01;
        bOb = 1;
        if (tmp - 00) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos failed ", 2*bOb, n,"");
    
        n = 0;
        tmp1 = 07654321; tmp2 = 01234567;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min pos < n < max pos failed ", 2*bOb, n,"");

		n = 0;
        tmp1 = 037777700000; tmp2 = 077777;
        bOb = 1;
        if (tmp1 + tmp2) {
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
        tmp1 = 00; tmp2 = 01;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = 01234567; tmp2 = 07654321;
        bOb = 1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "min neg < n < max neg failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -037777700000; tmp2 = 077777;
        bOb = 1;
        if (tmp1 - tmp2) {
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
        tmp1 = 01; tmp2 = 01;
        bOb = -1;
        if (tmp1 - tmp2) {
            n++;
            n++;
        }
        else {
            n--;
            n--;
        }

        if (n != 2*bOb) apLogFailInfo( "zero failed ", 2*bOb, n,"");

        n = 0;
        tmp1 = -00; tmp2 = 00;
        bOb = -1;
        if (tmp1 + tmp2) {
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
    tmp1 = ""; tmp2 = " ";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "single space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = "                                 "; tmp2 = "                                  ";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "multiple spaces failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = ""; tmp2 = false;
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as false failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = ""; tmp2 = 0;
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "ns 0 failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = ""; tmp2 = 1234567890;
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "ns >0 failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = " "; tmp2 = 1234567890;
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "ns >0, leading space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = 1234567890; tmp2 = " ";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "ns >0, trailing space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = -1234567890; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "ns <0 failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = "f"; tmp2 = "oo";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as single word failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = " f"; tmp2 = "oo";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as single word, leading space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = "fo"; tmp2 = "o ";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as single word, trailing space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = "foo "; tmp2 = "bar";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as multiple word failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = " foo"; tmp2 = " bar";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as multiple word, leading space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = "foo"; tmp2 = " bar ";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "as multiple word, trailing space failed ", 2*bOb, n,"");

    n = 0;
    tmp1 = ""; tmp2 = "";
    bOb = -1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "undefined failed ", 2*bOb, n,"");

    //----------------------------------------------------------------------------
    apInitScenario("11. constants");

    n = 0;
    tmp1 = true; tmp2 = false;
    bOb = -1;
    if (tmp1 * tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "constants--true failed ", 2*bOb, n,"");


    n = 0;
    tmp1 = false; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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
    tmp1 = null; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
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
    
    var obUndef;
    n = 0;
    tmp1 = obUndef; tmp2 = "";
    bOb = 1;
    if (tmp1 + tmp2) {
        n++;
        n++;
    }
    else {
        n--;
        n--;
    }

    if (n != 2*bOb) apLogFailInfo( "undefined failed ", 2*bOb, n,"");


    apEndTest();

}


if109();


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
