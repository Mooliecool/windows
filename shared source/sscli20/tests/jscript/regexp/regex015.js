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


var iTestID = 78046;

//RegEx015 - Regular Expressions Non-Capturing parentheses

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) {
        apLogFailInfo (sMsg, sExp, sRes, sBug);
    }
}

function regex015() {

	apInitTest ("RegEx015 - Non-Capturing parentheses");

//-------------------------------------------------------------
	apInitScenario ("1. Non-Capturing parentheses - Basic patterns");

	var sExp = true	
	var re = new RegExp("(?:)");
	verify (re.test(""), sExp, "non-capturing 1", "");

	sExp = true
	re = /(?:)/;
	verify (re.test("abc"), sExp, "non-capturing 2", "");


//-------------------------------------------------------------
	apInitScenario ("2. Nested non-capturing parens");

	sExp = true
	re = new RegExp("((?:)(?:())(?:(?:())))", "g");
	verify (re.test(""), sExp, "nested 1", "");

	sExp = true
	re = /((?:)(?:())(?:(?:())))/;
	verify (sExp, re.test("xyz"), "nested 2", "");

	sExp = "abc"
	re = new RegExp("((?:bc)|(?:abc))");
	var ms = re.exec("123abcx");
	verify (ms[0], sExp, "nested 3", "");

@if (!@_fast)
//Properties of the RegExp object are not avail in fast mode
	sExp = 3
	verify (RegExp.index, sExp, "nested 4", "");
	sExp = 6
	verify (RegExp.lastIndex, sExp, "nested 5", "");
	sExp = "123abcx";
	verify (RegExp.input, sExp, "nested 6", "");
@end


//-----------------------------------------------------------------
	apInitScenario ("3. ? to confuse parens")

	sExp = true;
	re = /(?:capture?)/;
	verify (re.test("capture"), sExp, "confusing ?'s 1", "");

	sExp = "capture";
	re = /(?:capture?)/;
	ms = "capture".match(re);
	verify (ms[0], sExp, "confusing ?'s 2", "");

	sExp = "captur";
	re = /(?:capture??)/;
	ms = "capture".match(re);
	verify (ms[0], sExp, "confusing ?'s 3", "");
	
	sExp = "captur";
	re = /(?:capture??)?/;
	ms = "capture".match(re);
	verify (ms[0], sExp, "confusing ?'s 4", "");

	sExp = "";
	re = /(?:capture??)??/;
	ms = "capture".match(re);
	verify (ms[0], sExp, "confusing ?'s 5", "");

	sExp = "captur";
	re = /\w??(?:capture??)?/;
	ms = "capture".match(re);
	verify (ms[0], sExp, "confusing ?'s 4", "");


//-----------------------------------------------------------------

	apInitScenario ("4. Nested capturing within non-capturing.");

	sExp = new Array("a!b!c!", "c!", "!");
	re = /([abc](!))+/;
	verify ("a!b!c!".match(re).join(), sExp.join(), "nested capturing 1", "");

	sExp = new Array("a!b!c!", "!");
	re = /(?:[abc](!))+/;
	verify ("a!b!c!".match(re).join(), sExp.join(), "nested capturing 2", "");

	sExp = "getthis"
	re = new RegExp("(?:get(this))");
	ms = re.exec("getthis")
	verify (ms[0], sExp, "nested capturing 3", "");
	sExp = "this"
	verify (ms[1], sExp, "nested capturing 4", "");

	sExp = "get"
	re = /(?:(get)this)/;
	ms = "getthis".match(re);
	verify (ms[1], sExp, "nested capturing 5", "");

	sExp = "tt"
	re = /(?:ge(t(t))his)/;
	ms = "getthis".match(re);
	verify (ms[1], sExp, "nested capturing 6", "");
	sExp = "t";
	verify (ms[2], sExp, "nested capturing 7", "");

	sExp = "tt"
	re = /(?:ge(t(?:t))his)/;
	ms = "getthis".match(re);
	verify (ms[1], sExp, "nested capturing 6", "");



    apEndTest();

}	


regex015();


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
