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


var iTestID = 51829;


// Not totally accurate, but a quick fix
function isDaylightSavings ()
{
    var curDate = new Date();
    var nCurYear = curDate.getYear();

    // get the first Sunday of Apr.
    var strDate = new Date(nCurYear,3,1);
    for (var i=1; i<=7; i++)
    {
        strDate.setDate(i);
        if ( strDate.getDay()==0 ) break;
    }

    // get the last Sunday of Oct.
    var endDate = new Date(nCurYear,9,22);
    for (i=31; i>=25; i--)
    {
        endDate.setDate(i);
        if ( endDate.getDay()==0 ) break;
    }


    if ( Date.parse(curDate) >= Date.parse(strDate) && 
         Date.parse(curDate) <  Date.parse(endDate) ) return true;

    return false;
}
    function datmth01() {
    var expected;
    var res;

    var myD;

    apInitTest("datmth01");

    myD = new Date();
    myD.setTime(0);
    myD.setTime(Date.UTC(70,0,1,0) + myD.getTimezoneOffset() * 60000 - 28800000);

    apInitScenario("basic date method functionality - getDate");
    expected = 31;
    res = myD.getDate();
    if (expected != res) {
	apLogFailInfo("getDate returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - getDay");
    expected = 3 ;
    res = myD.getDay();
    if (expected != res) {
	apLogFailInfo("getDay returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - getHours");
    expected = 16;
    res = myD.getHours();
    if (expected != res) {
	apLogFailInfo("getHours returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - getMinutes");
    expected = 0;
    res = myD.getMinutes();
    if (expected != res) {
	apLogFailInfo("getMinutes returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - getMonth");
    expected = 11;
    res = myD.getMonth();
    if (expected != res) {
	apLogFailInfo("getMonth returned wrong result",expected,res,"");
    }

    apInitScenario("basic date method functionality - getSeconds");
    expected = 00;
    res = myD.getSeconds();
    if (expected != res) {
	apLogFailInfo("getSeconds returned wrong result",expected,res,"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("basic date method functionality - getTime");
    expected = Date.UTC(70,0,1,0) + myD.getTimezoneOffset() * 60000 - 28800000;
    res = myD.getTime();
    if (expected != res) {
	apLogFailInfo("getTime returned wrong result",expected,res,"");
    }
    }

    apInitScenario("basic date method functionality - getYear");
    expected = 69;
    res = myD.getYear();
    if (expected != res) {
	apLogFailInfo("getYear returned wrong result",expected,res,"");
    }

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    apInitScenario("basic date method functionality - getTimezoneOffset");

 if (apGlobalObj.LangHost() == 1033) {
//    expected = isDaylightSavings() ? 60 * -7 : 60 * -8; // Seattle is GMT - 8h
 //   expected = isDaylightSavings() ? 60 * 7 : 60 * 8; // Seattle is GMT - 8h
// The time of year of myD is during regular time, not DST, so it'll always be
// GMT - 8
    expected = 480;
    res = myD.getTimezoneOffset();
    if (expected != res) {
	apLogFailInfo("getTimezoneOffset returned wrong result!",expected,res,"Vegas 393");
    }
    }
 } // end if

    apEndTest();
}


datmth01();


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
