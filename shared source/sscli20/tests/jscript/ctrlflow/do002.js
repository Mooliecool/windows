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


var iTestID = 52239;


function do002() {

    apInitTest ("do002 ");

    var CS;
    var Result;
    var Cntr;
    var AvarName;

    apInitScenario( "1) do { .. } w/ Empty");
    Cntr = 0;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while (AvarName);
    if ( Cntr != 1 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "2) do { .. } w/ null");
    Cntr = 0;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var AVarName;
     
@end       

    AVarName = null;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );
    if ( Cntr != 1 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "3) do { .. } w/ Integer");
    Cntr = 0;
    AVarName = 199;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );
    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "4) do { .. } w/ Long");
    Cntr = 0;
    AVarName = 12341234;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName);

    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "5) do { .. } w/ Single");
    Cntr = 0;
    AVarName = 1234.123;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );
    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "6) do { .. } w/ double");
    Cntr = 0;
    AVarName = 1.234E20;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );
    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "8) do { .. } w/ Date");
    Cntr = 0;
    AVarName = new Date("1993-10-18");
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );
    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "9) do { .. } w/ String");
    Cntr = 0;
    AVarName = "true"
    Result = 0;
    do { 
       Cntr = Cntr + 1;
       break;
    } while ( AvarName);


    if (Cntr != 1 ) {
        apLogFailInfo( "do { } w/string bool", "","", "");
    }

    apInitScenario( "11) do { .. } w/ Boolean");
    Cntr = 0;
    AVarName = true;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while ( AVarName );

    if ( Cntr != 10 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "");
    }

    apInitScenario( "12) do { .. } w/ Object w/ Value Prop");
@if(!@rotor)
    Cntr = 0;
    AVarName = new ActiveXObject("IPLangSvr.CallObj");
    AVarName.defaultmemberid = 8;
    AVarName.P_R4 = 0;
    do { 
       Cntr = Cntr + 1;
       if ( Cntr >= 10 ) {
            break;
       }
    } while (AVarName.P_R4);

    if ( Cntr != 1 ) {
        apLogFailInfo( "do { } executed incorrect number of times", "", "", "VEGAS 4027");
    }
@end
    apEndTest();


} // end function do002




do002();


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
