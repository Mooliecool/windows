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


var iTestID = 232271;

// BuildString returns a repeating user-defined string of characters at least x length
// Don't send it wierd values because it's not very well-written :-)

@if (@_jscript_version >= 7)
	import System.Text
@end

function BuildString(x,str) {

	@if (@_jscript_version >= 7)

		var str1 : String;
		var sb = new StringBuilder();
		sb.Insert(0,"abcdefghij",int(x/str.length));
		str1 = sb.ToString();

	@else

		var str1 = str;
		for (var i = 0; str1.length < x; i++) {
			str1 += str1;
		}

	@end 

	return str1;

}

var BigStr = BuildString(10000000, "abcdefghij");

function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function stringsec01() {

    apInitTest("StringSec01: Security testing string constructor"); 


	apInitScenario("1 Generate String Object with huge string");
	try{
		var testStr = new String(BigStr);
	}
	catch(e) {
		apLogFailInfo("Error creating huge string",BigStr.length,testStr.length,"");
		apWriteDebug("Error name = " + e.name);
		apWriteDebug("Error description = " + e.description);
		apWriteDebug("Error number = " + e.number);
	}

	apInitScenario("2 Now add some more text....");

	try{
		testStr += new String(BigStr);
	}
	catch(e) {
		apLogFailInfo("Error creating huge string",BigStr.length,testStr.length,"");
		apWriteDebug("Error name = " + e.name);
		apWriteDebug("Error description = " + e.description);
		apWriteDebug("Error number = " + e.number);
	}


	apEndTest();
}


stringsec01();


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
