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


var iTestID = 78042;

//regex013.js - Regular Expressions Non-Greedy Matches
//Find the smallest number of matches rather than the largest

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}


function regex013() {

	apInitTest ("RegEx013 - Non-Greedy matching patterns");

//-----------------------------------------------------------
	apInitScenario ("1. mix and match patterns *?,+?,??");

	var sExp = new Array("<b>", "b");
	var re = /<(.)+?>/;
	var ms =  re.exec("<b>vbscript roks!</b>");
	verify (ms.join(), sExp.join(), "Non-greedy 1", "");

	sExp = new Array("<b>", "b");
	re = new RegExp("<(.)*?>");
	ms =  "<b>vbscript roks!</b>".match(re);
	verify (ms.join(), sExp.join(), "Non-greedy 2", "");

	sExp = new Array("<b>", "b");
	re = /<(.)??>/;
	ms =  re.exec("<b>vbscript roks!</b>");
	verify (ms.join(), sExp.join(), "Non-greedy 3", "");

	sExp = "<>";
	re = /<(.)??>/;
    ms =  re.exec("<>");
	verify (ms[0], sExp, "Non-greedy 4", "");

//----------------------------------------------------------
    apInitScenario ("2. stress using replace()");

	sExp = "abc";
	re = /(.){3,3}?/;
	verify ("123".replace(re, "abc"), sExp, "Non-greedy 1", "");

	sExp = "abc4567";
	re = new RegExp("(.){3,6}?", "i");
	verify ("1234567".replace(re, "abc"), sExp, "Non-greedy 2", "");

	sExp = "abcabc7";
	re = new RegExp("(.){3,6}?", "ig");
	verify ("1234567".replace(re, "abc"), sExp, "Non-greedy 3", "");

	sExp = "xc"; 
	re= /(.){2}?/ig;
	verify ("abc".replace(re, "x"), sExp, "Non-greedy 4", "");


//----------------------------------------------------------
    apInitScenario ("3. testing using {m,n}?");

	sExp = "<The> <challenge>";
	re = /((\w+)+?)/g;
	verify ("The challenge".replace(re, "<$1>"), sExp, "Non-greedy 1", "");

	sExp = "The"
	re = new RegExp("((\\w{1,2}){2,9}?)", "gm");
	ms = "The Challenge".match(re)
	verify (ms[0], sExp, "Non-greedy 2", "");

	sExp = new Array("The", "Chal", "leng");
	re = new RegExp("((\\w{1,2}){2,9}?)", "gm");
	ms = "The Challenge".match(re)
	verify (ms.join(), sExp, "Non-greedy 3", "");

	sExp = "e"
	re = new RegExp("((\\w{1,2}){2,9}?)", "gm");
	ms = re.exec("The Challenge")
	verify (ms[2], sExp, "Non-greedy 4", "");

	sExp = "Chal" 
	re = new RegExp("((\\w{1,2}){2,9}?)", "g");
	ms = re.exec("The Challenge");
	ms = re.exec("The Challenge");
    verify (ms[0], sExp, "Non-greedy 5", "");
    verify (ms[1], sExp, "Non-greedy 6", "");
	sExp = "al"
	verify (ms[2], sExp, "Non-greedy 7", "");

	sExp = "leng"
	ms = re.exec("The Challenge");
	verify (ms[0], sExp, "Non-greedy 8", "");
	verify (ms[1], sExp, "Non-greedy 9", "");
	sExp = "ng"
	verify (ms[2], sExp, "Non-greedy 10", "");

	apEndTest();

}


regex013();


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
