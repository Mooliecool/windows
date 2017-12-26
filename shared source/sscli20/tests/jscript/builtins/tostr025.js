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


var iTestID = 78362;


function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) {
        apLogFailInfo (sMsg, sExp, sRes, sBug);
    }
}

function tostr025() {
 @if(@_fast)
    var sExp, sAct, sRes;
 @end

	apInitTest ("toStr024.js - check that object.prototype.toString returns [object <name>] - ECMA 3 compliance");

	apInitScenario ("1. Check Object.prototype.toString.apply(new Object)");

		sExp = "[object String]"
		sRes = Object.prototype.toString.apply(new String());
		verify (sRes, sExp, "String Object", "");

		sExp = "[object Number]"
		sRes = Object.prototype.toString.apply(new Number());
		verify (sRes, sExp, "Number Object", "");

		sExp = "[object Array]"
		sRes = Object.prototype.toString.apply(new Array());
		verify (sRes, sExp, "Array Object", "")

		sExp = "[object Math]"
		sRes = Object.prototype.toString.apply(Math);
		verify (sRes, sExp, "Math Object", "");

		sExp = "[object Date]"
		sRes = Object.prototype.toString.apply(new Date());
		verify (sRes, sExp, "Date Object", "");

		sExp = "[object RegExp]"
		sRes = Object.prototype.toString.apply(new RegExp());
		verify (sRes, sExp, "RegExp Object", "");

		sExp = "[object Object]"
		sRes = Object.prototype.toString.apply(new Object());
		verify (sRes, sExp, "Object Object", "");

		sExp = "[object Boolean]"
		sRes = Object.prototype.toString.apply(new Boolean());
		verify (sRes, sExp, "Boolean Object", "");

		sExp = "[object Function]"
		sRes = Object.prototype.toString.apply(new Function());
		verify (sRes, sExp, "Function Object", "");

	apInitScenario("2. adding to the object prototype chain... not in fast mode");
 @if(!@_fast)
		sExp = "[object String]"
		String.prototype.foo = Object.prototype.toString;
		sAct = new String("yaba");
		verify (sAct.foo(), sExp, "String Object 1", "");
		verify ("test".foo(), sExp, "String Object 2", "");

		sExp = "[object Number]"
		Number.prototype.foo = Object.prototype.toString;
		sAct = new Number(15);
		verify (sAct.foo(), sExp, "Number Object 1", "");
		verify (Number(5).foo(), sExp, "Number Object 2", "");

		sExp = "[object Array]"
		Array.prototype.foo = Object.prototype.toString;
		sAct = new Array(1,2,3, "A");
		verify (sAct.foo(), sExp, "Array Object 1", "");
		verify (Array(5,3).foo(), sExp, "Array Object 2", "");

/*
		sExp = "[object Math]"
		Math.prototype.foo = Object.prototype.toString;
		sAct = Math();
		verify (sAct.foo(), sExp, "MathObject", "");
*/

		sExp = "[object Date]"
		Date.prototype.foo = Object.prototype.toString;
		sAct = new Date();
		verify (sAct.foo(), sExp, "DateObject", "");

		sExp = "[object RegExp]"
		RegExp.prototype.foo = Object.prototype.toString;
		sAct = new RegExp("\w");
		verify (sAct.foo(), sExp, "RegExpObject", "");

		sExp = "[object Object]"
		Object.prototype.foo = Object.prototype.toString;
		sAct = new Object();
		verify (sAct.foo(), sExp, "Object Object 1", "");

		sExp = "[object Function]";
		verify (Object.foo(), sExp, "Object Object 2", "");

		sExp = "[object Boolean]"
		Boolean.prototype.foo = Object.prototype.toString;
		sAct = new Boolean();
		verify (sAct.foo(), sExp, "Boolean Object 1", "");

		sExp = "[object Function]";
		verify (Boolean.foo(), sExp, "Boolean Object 2", "");

		sExp = "[object Function]"
		Function.prototype.foo = Object.prototype.toString;
		sAct = new Function();
		verify (sAct.foo(), sExp, "Function Object 1", "");
		verify (Function.foo(), sExp, "Function Object 2", "");
 @end

	apEndTest();
}


tostr025();


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
