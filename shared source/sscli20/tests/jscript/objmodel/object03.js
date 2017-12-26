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


var iTestID = 78510;


/* object03.js - Object.prototype.hasOwnProperty(V)
	1. Let O be this object.
	2. Call ToString(V)
	3. if O doesn't have a property with the name given by Result(2), return FALSE
	4. Return TRUE
*/

//added for unfixed bug 314108
@if(@aspx) expando @end function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

function object03() {
@if(@_fast)
	var sAct;
@end
	apInitTest ("object03 - Object.prototype.hasOwnProperty() tests");

	apInitScenario ("1. Check some default objects and properties");

		sAct = new Array("a", "b", 3)
		verify (sAct.hasOwnProperty("length"), true, "check Arrays have property .length", "VS");
		verify (sAct.hasOwnProperty("dummy"), false, "check Array dont have property .dummy", "");
		verify (sAct.hasOwnProperty("join"), false, "check Arrays dont inherit the prototype chain explicitly", "");
		verify (Array.prototype.hasOwnProperty("join"), true, "check that Array.prototype contains the methods of the prototype chain", "");

		sAct = String.hasOwnProperty("fromCharCode");
		verify (sAct, true, "check String has .fromCharCode", "");
		sAct = String.prototype.lastIndexOf.hasOwnProperty("length");
		verify (sAct, true, "check String.prototype.lastIndexOf has .length", "VS");	
		sAct = String.hasOwnProperty("prototype");
		verify (sAct, true, "check String has method .prototype", "VS");

		verify (String("the fat cat").split(" ").join().hasOwnProperty("length"), true, "calling hasOwnProperty on the result of a operation", "VS");

	apInitScenario ("2. custom objects");

		sAct = new Object();
		sAct.x = "add x as property";
		verify (sAct.hasOwnProperty("none"), false, "No property is added to Object()", "");
		verify (sAct.hasOwnProperty("x"), true, "add a property to Object()", "");

		var a = new Object();
		verify (a.hasOwnProperty("verify"), false, "havent added verify method to object a", "");
		a.verify = verify;
		verify (a.hasOwnProperty("verify"), true, "added verify method to object a", "");
		verify (a.hasOwnProperty("length"), false, "havent added length property", "");
		a.length = 0;
		verify (a.hasOwnProperty("length"), true, "havent added length property", "");

      apEndTest();
}


object03();


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
