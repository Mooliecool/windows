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


var iTestID = 78514;


/* object05.js - Object.prototype.propertyIsEnumerable(V)
	1. Let O be this object.
	2. Call ToString(V)
	3. If O doesn't have a property with the name given by Result(2), return FALSE
	4. If the property has the DontEnum attribute, return FALSE
       5. Return TRUE
*/

@cc_on

//added for unfixed bug 314108
@if(@aspx) expando @end function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

function object05() {
	apInitTest ("object05 - Object.prototype.propertyIsEnumerable() tests");
	
	apInitScenario ("1. Check objects for enumeration");

		var a = ["a", 2, 3];
		verify (a.propertyIsEnumerable(0), true, "check Array", "");

		var s = new String();
		s.foo = 33;
		s.bar = verify;
		verify (s.propertyIsEnumerable("foo"), true, "add enum variables to object", "");
		verify (s.propertyIsEnumerable("bar"), true, "add enum function to object", "");

@if (@_fast)
	var o;
@else
		String.foobar = 10;
		verify (String.propertyIsEnumerable("foobar"), true, "add enum to String object", "");
		verify (String.prototype.propertyIsEnumerable("foobar"), false, "check to see we didnt add enum to String.prototype", "");
		String.prototype.yo = "yo";
		verify (String.prototype.propertyIsEnumerable("yo"), true, "check that we add enum to String.prototype", "");
@end

		var myobj = { a: "apple", 101: 1 }
		verify (myobj.propertyIsEnumerable('a'), true, "add enum property to custom object", "");
		verify (myobj.propertyIsEnumerable(101), true, "add enum numeric property to custom object 1", "");
		verify (myobj.propertyIsEnumerable("101"), true, "add enum numeric property to custom object 2" ,"");

		for (o in myobj) {
			verify (myobj.propertyIsEnumerable(o), true, "for...in loop propertyIsEnumerable enum testing", "");
		}

	apEndTest();	
}


object05();


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
