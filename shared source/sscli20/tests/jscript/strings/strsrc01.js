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


var iTestID = 72448;


//////////
//
//  Test Case:
//
//     strsrc01: String.prototype.search -  can take a string or a RegExp as a parameter
//
//  Author:
//




//////////
//
//  Helper functions


function verify(sAct, sExp, sMes, SBUG){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}



//////////
//
//  Global variables
//


var str1;
var reg1;
var iSearch;



reg1 = new RegExp();



function strsrc01() {


    apInitTest("strsrc01: String.prototype.search -  can take a string or a RegExp as a parameter"); 


	str1 = new String();
	str1 = "My dog has fleas.";


	apInitScenario("1 Matching argument is null");
	iSearch = str1.search(null);
	verify (iSearch, -1, "1.1 Wrong result returned", "");
	verify (iSearch, -1, "1.2 Wrong result returned", "");


	apInitScenario("2 Matching argument is undefined");
	iSearch = str1.search(undefined);
	verify (iSearch, 0, "2.1 Wrong result returned", "VS7 #111013");
	verify (iSearch, 0, "2.2 Wrong result returned", "");


	apInitScenario("3 Matching argument is empty string");
	iSearch = str1.search("");
	verify (iSearch, 0, "3.1 Wrong result returned", "");
	verify (iSearch, 0, "3.2 Wrong result returned", "");


	apInitScenario("4 Matching argument is exactly equal to String");
	iSearch = str1.search("My dog has fleas.");
	verify (iSearch, 0, "4.1 Wrong result returned", "");
	verify (iSearch, 0, "4.2 Wrong result returned", "");


	apInitScenario("5 Matching argument is not found in String");
	iSearch = str1.search("cat");
	verify (iSearch, -1, "5.1 Wrong result returned", "");
	verify (iSearch, -1, "5.2 Wrong result returned", "");


	apInitScenario("6 Matching argument matches beginning of String");
	iSearch = str1.search("My");
	verify (iSearch, 0, "6.1 Wrong result returned", "");
	verify (iSearch, 0, "6.2 Wrong result returned", "");


	apInitScenario("7 Matching argument matches end of String");
	iSearch = str1.search("fleas.");
	verify (iSearch, str1.length - 6, "7.1 Wrong result returned", "");
	verify (iSearch, str1.length - 6, "7.2 Wrong result returned", "");


	apInitScenario("8 Matching argument is longer String which starts with calling string");
	iSearch = str1.search("My dog has fleas.  My dog hates cats.");
	verify (iSearch, -1, "8.1 Wrong result returned", "");
	verify (iSearch, -1, "8.2 Wrong result returned", "");


	apInitScenario("9 Matching argument is empty RegExp");
	reg1 = //gi;
	iSearch = str1.search(reg1);
	verify (iSearch, 0, "9.1 Wrong result returned", "");
	verify (iSearch, 0, "9.2 Wrong result returned", "");


	apInitScenario("10 Matching argument is RegExp exactly equal to String");
	reg1 = /My dog has fleas./gi;
	iSearch = str1.search(reg1);
	verify (iSearch, 0, "10.1 Wrong result returned", "");
	verify (iSearch, 0, "10.2 Wrong result returned", "");


	apInitScenario("11 Matching argument is RegExp which will match once in String");
	reg1 = /dog/gi;
	iSearch = str1.search(reg1);
	verify (iSearch, 3, "11.1 Wrong result returned", "");
	verify (iSearch, 3, "11.2 Wrong result returned", "");


	apInitScenario("12 Matching argument is RegExp which will match multiple times in String");
	reg1 = /as/gi;
	iSearch = str1.search(reg1);
	verify (iSearch, 8, "12.1 Wrong result returned", "");
	verify (iSearch, 8, "12.2 Wrong result returned", "");


	apInitScenario("13 Matching argument is RegExp which won't match String");
	reg1 = /[0-9]/gi;
	iSearch = str1.search(reg1);
	verify (iSearch, -1, "13.1 Wrong result returned", "");
	verify (iSearch, -1, "13.2 Wrong result returned", "");


	apEndTest();
}

strsrc01();


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
