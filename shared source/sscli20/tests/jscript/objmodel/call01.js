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


var iTestID = 79875;


/* call01.js - test the call() function of JScript
	this test case is similar to call01.js
*/

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

@if(@aspx) expando @end function adder() {
	this.n = 0;
	this.add_x = add_x;
	this.add_xy = add_xy;
	this.assignAddername = assignAddername;
}

@if(@aspx) expando @end function add_x(x) {	
	return x + this.n;
}

@if(@aspx) expando @end function add_xy(x, y) {
	return x + y + this.n;
}

@if(@aspx) expando @end function getAddername() {
	return this.name
}

@if(@aspx) expando @end function assignAddername(name) {
	return (this.name = name);
}

function call01() {

	apInitTest ("Function.call tests");
	
	apInitScenario("1. Use call function with different arguments");
	
		var a = new adder();
		a.n = 10;
		verify (a.add_x(5), 15, "call a.add_x(5) directly", "");
		verify (a.add_x.call(a, (12)), 22, "call a.add_xy via call()", "");

		verify (a.add_xy(3, 5), 18, "call a.add_xy(3, 5) directly", "");
		verify (a.add_xy.call(a, 10, 5), 25, "call a.add_xy via call()", "");
		
		a.name = "Addername";
		a.getAddername = getAddername;
		verify (a.getAddername.call(a, []), "Addername", "calling call with empty array", "");
		verify (a.assignAddername.call(a, ["newAddername"]), "newAddername", "calling call with a string arg in array", "");
		
	apEndTest();
}



call01();


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
