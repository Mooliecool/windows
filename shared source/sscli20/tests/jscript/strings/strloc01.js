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


var iTestID = 72440;

//////////
//
//  Test Case:
//
//     strloc01: String.prototype.localeCompare
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, sBug);
}



//////////
//
//  Global variables
//


var str1;



function strloc01() {


    apInitTest("strloc01: String.prototype.localeCompare"); 


	apInitScenario("1 String == '', No stringObj argument");
	str1 = "";
	verify (str1.localeCompare(), -1, "1 Wrong index returned", "VS");	


	apInitScenario("2 String == '', stringObj == null");
	str1 = "";
	verify (str1.localeCompare(null), -1, "2 Wrong index returned", "");	


	apInitScenario("3 String == '', stringObj == ''");
	str1 = "";
	verify (str1.localeCompare(""), 0, "3 Wrong index returned", "");	


	apInitScenario("4 String == '', stringObj is single space char");
	str1 = "";
	verify (str1.localeCompare(" "), -1, "4 Wrong index returned", "");	


	apInitScenario("5 String == '', stringObj is line feed");
	str1 = "";
	verify (str1.localeCompare(String.fromCharCode(10)), -1, "5 Wrong index returned", "");	


	apInitScenario("6 String == '', stringObj is single char");
	str1 = "";
	verify (str1.localeCompare("c"), -1, "6 Wrong index returned", "");	


	apInitScenario("7 String is single char, No stringObj argument");
	str1 = "c";
	verify (str1.localeCompare(), -1, "7 Wrong index returned", "");	


	apInitScenario("8 String is single char, stringObj == null");
	str1 = "c";
	verify (str1.localeCompare(null), -1, "8 Wrong index returned", "");	


	apInitScenario("9 String is single char, stringObj == ''");
	str1 = "c";
	verify (str1.localeCompare(""), 1, "9 Wrong index returned", "");	


	apInitScenario("10 String is single char, stringObj is single space char");
	str1 = "c";
	verify (str1.localeCompare(" "), 1, "10 Wrong index returned", "");	


	apInitScenario("11 String is single char, stringObj is line feed");
	str1 = "c";
	verify (str1.localeCompare(String.fromCharCode(10)), 1, "11 Wrong index returned", "");	


	apInitScenario("12 String is single char, stringObj is non-locale-specific char lower in value than string");
	str1 = "c";
	verify (str1.localeCompare("a"), 1, "12 Wrong index returned", "");	


	apInitScenario("13 String is single char, stringObj is non-locale-specific char higher in value than string");
	str1 = "c";
	verify (str1.localeCompare("q"), -1, "13 Wrong index returned", "");	


	apInitScenario("14 String is single char, stringObj is non-locale-specific char equal in value than string");
	str1 = "c";
	verify (str1.localeCompare("c"), 0, "14 Wrong index returned", "");	


	apInitScenario("15 String == 's', stringObj is 'ﬂ' (after 's')");
	str1 = "s";
	verify (str1.localeCompare("ﬂ"), -1, "15 Wrong index returned", "VS7 85813");	


	apInitScenario("16 String == 't', stringObj is 'ﬂ' (before 't')");
	str1 = "t";
	verify (str1.localeCompare("ﬂ"), 1, "16 Wrong index returned", "VS7 85813");	


	apInitScenario("17 String == 'ﬂ', stringObj is 'ﬂ'");
	str1 = "ﬂ";
	verify (str1.localeCompare("ﬂ"), 0, "17 Wrong index returned", "VS7 85813");	


	apInitScenario("18 String is <long string> + 's', stringObj is equal <long string> + 'ﬂ' (after 's')");
	str1 = "My dog has fleass";
	verify (str1.localeCompare("My dog has fleasﬂ"), -1, "18 Wrong index returned", "VS7 85813");	


	apInitScenario("19 String is <long string> + 't', stringObj is equal <long string> + 'ﬂ' (before 't')");
	str1 = "My dog has fleast";
	verify (str1.localeCompare("My dog has fleasﬂ"), 1, "19 Wrong index returned", "VS7 85813");	


	apInitScenario("20 String is <long string> + 'ﬂ', stringObj is equal <long string> + 'ﬂ'");
	str1 = "My dog has fleasﬂ";
	verify (str1.localeCompare("My dog has fleasﬂ"), 0, "20 Wrong index returned", "VS7 85813");	


	apInitScenario("21 String is 's' + <long string>, stringObj is 'ﬂ' + equal <long string> (after 's')");
	str1 = "sMy dog has fleas";
	verify (str1.localeCompare("ﬂMy dog has fleas"), -1, "21 Wrong index returned", "VS7 120295");	


	apInitScenario("22 String is 't' + <long string>, stringObj is 'ﬂ' + equal <long string> (before 't')");
	str1 = "tMy dog has fleas";
	verify (str1.localeCompare("ﬂMy dog has fleas"), 1, "22 Wrong index returned", "VS7 85813");	


	apInitScenario("23 String is 'ﬂ' + <long string>, stringObj is 'ﬂ' + equal <long string>");
	str1 = "ﬂMy dog has fleas";
	verify (str1.localeCompare("ﬂMy dog has fleas"), 0, "23 Wrong index returned", "VS7 85813");	


	apInitScenario("24 String is <long string> + 's' + <long string>, stringObj is equal <long string> + 'ﬂ' + equal <long string> (after 's')");
	str1 = "My dog has fleas s Dogs hate cats";
	verify (str1.localeCompare("My dog has fleas ﬂ Dogs hate cats"), -1, "24 Wrong index returned", "VS7 85813");	


	apInitScenario("25 String is <long string> + 't' + <long string>, stringObj is equal <long string> + 'ﬂ' + equal <long string> (before 't')");
	str1 = "My dog has fleas t Dogs hate cats";
	verify (str1.localeCompare("My dog has fleas ﬂ Dogs hate cats"), 1, "25 Wrong index returned", "VS7 85813");	


	apInitScenario("26 String is <long string> + 'ﬂ' + <long string>, stringObj is equal <long string> + 'ﬂ' + equal <long string>");
	str1 = "My dog has fleas ﬂ Dogs hate cats";
	verify (str1.localeCompare("My dog has fleas ﬂ Dogs hate cats"), 0, "26 Wrong index returned", "VS7 85813");	


	apEndTest();
}

strloc01();


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
