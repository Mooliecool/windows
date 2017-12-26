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


var iTestID = 78520;


/* object04.js - Object.prototype.isPrototypeOf(V)
	1. Let O be this object.
	2. if V is not an object, return FALSE
	3. Let V be the value of the [[Prototype]] property of V.
	4. if V is null, return false
	5. If O and V refer to the same object or if they refer to objects joined to each other, return TRUE.
	6. Goto step 3.
*/

//added for unfixed bug 314108
@if(@aspx) expando @end function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

function object04() {
@if(@_fast)
	var a,s,n,o,b,d,r1,f1,f2,f3;
@end
	apInitTest ("object04 - Object.prototype.isPrototypeOf() tests");
	
	apInitScenario ("1. Check some default objects and properties");

		a = new Array(1,2);
		verify (Array.prototype.isPrototypeOf(a), true, "Array tests", "VS");

		s = new String("yoyo");
		verify (String.prototype.isPrototypeOf(s), true, "String tests", "");

              
		n = Number(9);
		verify (Number.prototype.isPrototypeOf(n), false, "Number Tests", "");

		f1 = new Function();
		verify (Function.prototype.isPrototypeOf(f1), true, "Function Test 1", "");

		f2 = new Function("a", "b", "c", "return a+b+c");
		verify (Function.prototype.isPrototypeOf(f2), true, "Function Test 2", "");

		f3 = verify;
		verify (Function.prototype.isPrototypeOf(f3), true, "Function Test 3", "");

		o = new Object();
		verify (Object.prototype.isPrototypeOf(o), true, "Object Test 1", "");

		var e = {x: "", y: 10};
		verify (Object.prototype.isPrototypeOf(e), true, "Object Test 2", "");

		b = new Boolean();
		verify (Boolean.prototype.isPrototypeOf(b), true, "Boolean Test", "");

		d = new Date();
		verify (Date.prototype.isPrototypeOf(d), true, "Date Test", "");

		r1 = new RegExp();
		verify (RegExp.prototype.isPrototypeOf(r1), true, "RegExp Test 1", "");

		var r2 = /\w/;
		verify (RegExp.prototype.isPrototypeOf(r2), true, "RegExp Test 2", "");

      apEndTest();
}


object04();


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
