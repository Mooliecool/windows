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


var iTestID = 91748;

//regex017.js - New Exec rules and how they apply to lastIndex property


function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}


function regex017() {
   var i;

	apInitTest ("RegEx017 - New RegExp.exec() rules");

	var re = new RegExp();
	var re2 = new RegExp();


//----------------------------------------------------------------------

	apInitScenario ("1. Baseline RegExp properties cases at initialization");


		verify (RegExp.lastIndex, -1, "init value of lastIndex", "");
		verify (RegExp.input, "", "init value of input", "");
		verify (RegExp.index, -1, "init value of index", "");
		verify (RegExp.length, 2, "init value of length", "");

@if (!@_fast)
		verify (RegExp.$1, "", "init value of RegExp.$1", "")
		verify (RegExp.$2, "", "init value of RegExp.$2", "")
		verify (RegExp.$3, "", "init value of RegExp.$3", "")
		verify (RegExp.$4, "", "init value of RegExp.$4", "")
		verify (RegExp.$5, "", "init value of RegExp.$5", "")
		verify (RegExp.$6, "", "init value of RegExp.$6", "")
		verify (RegExp.$7, "", "init value of RegExp.$7", "")
		verify (RegExp.$8, "", "init value of RegExp.$8", "")
		verify (RegExp.$9, "", "init value of RegExp.$9", "")
@end
		
//----------------------------------------------------------------------

	apInitScenario ("2. The RegExp global properties after a global search with no captures");

		
		re = /\w/g;

		for (i = 0; i < 5; i++) 
		{
			re.exec("abcdef");
@if (!@_fast)
			verify (RegExp.lastIndex, i+1, "lastIndex properties after global search + no captures", "")
			verify (RegExp.input, "abcdef", "input properties after global search + no captures", "")
			verify (RegExp.index, i, "index properties after global search + no captures", "")		
			verify (re.lastIndex, i+1, "lastIndex properties after global search + no captures", "")
@end
		}

//----------------------------------------------------------------------

	apInitScenario ("3. The Regexp global properties after global search with captures");

		
		re = /(((\w)))/g;

		for (i = 0; i < 5; i++) 
		{
			re.exec("abcdef");
@if (!@_fast)
			verify (RegExp.lastIndex, i+1, "lastIndex properties after global search + no captures", "")
			verify (RegExp.input, "abcdef", "input properties after global search + no captures", "")
			verify (RegExp.index, i, "index properties after global search + no captures", "")		
			verify (re.lastIndex, i+1, "lastIndex properties after global search + no captures", "")
			verify (RegExp.$3, "abcdef".split("")[i], "RegExp.$3 contains the word char capture", "");
			verify (RegExp.$4, "", "RegExp.$4 contains no captures after global search without captures", "");
@end
		}


//----------------------------------------------------------------------

	apInitScenario ("4. The Regexp global properties after non-global search without captures");

		
		re = /\w/;

		for (i = 0; i < 5; i++) 
		{
			re.exec("abcdef");
@if (!@_fast)
			verify (RegExp.lastIndex, 1, "lastIndex properties after global search + no captures", "")
			verify (RegExp.input, "abcdef", "input properties after global search + no captures", "")
			verify (RegExp.index, 0, "index properties after global search + no captures", "")		
			verify (re.lastIndex, 1, "lastIndex properties after global search + no captures", "")
@end
		}

//----------------------------------------------------------------------

	apInitScenario ("5. The Regexp global properties after non-global search with captures");

		
		re = /((\w))/g;

		for (i = 0; i < 5; i++) 
		{
			re.exec("abcdef");
@if (!@_fast)
			verify (RegExp.lastIndex, i+1, "lastIndex properties after global search + captures", "")
			verify (RegExp.input, "abcdef", "input properties after global search + captures", "")
			verify (RegExp.index, i, "index properties after global search + captures", "")		
			verify (re.lastIndex, i+1, "lastIndex properties after global search + captures", "")
			verify (RegExp.$2, "abcdef".split("")[i], "RegExp.$2 contains the word char capture", "");
			verify (RegExp.$3, "", "RegExp.$4 contains no captures after global search with captures", "");
@end
		}

//----------------------------------------------------------------------

	apInitScenario ("6. property states after independent RegExp calls");

		re = /[a-z]/;
		re2 = /\d/;

		var str = "h8";

		re.exec(str);
		re2.exec(str);
@if (!@_fast)
		verify (re.lastIndex, 1, "original RE's lastIndex value is unchanged", "");
		verify (re2.lastIndex, 2, "2nd RE's lastIndex value is unchanged", "");
		verify (RegExp.lastIndex, 2, "global RE's lastIndex value is unchanged", "");
		verify (RegExp.index, 1, "global RE's lastIndex value is unchanged", "");
		verify (RegExp.input, str, "global RE's lastIndex value is unchanged", "");

		re.exec(str);
		
		verify (re.lastIndex, 1, "call original RE lastIndex again", "");
		verify (re2.lastIndex, 2, "call 2nd RE's lastIndex value again", "");
		verify (RegExp.lastIndex, 1, "check global RE's lastIndex value again", "");
		verify (RegExp.index, 0, "check global RE's lastIndex value again", "");
		verify (RegExp.input, str, "check global RE's lastIndex value again", "");

		re2.exec(str);
		
		verify (re.lastIndex, 1, "roundtrip test original RE's lastIndex value", "");
		verify (re2.lastIndex, 2, "roundtrip test 2nd RE's lastIndex value", "");
		verify (RegExp.lastIndex, 2, "roundtrip test global RE's lastIndex value", "");
		verify (RegExp.index, 1, "roundtrip test global RE's lastIndex value", "");
		verify (RegExp.input, str, "roundtrip test global RE's lastIndex value", "");		
@end

//----------------------------------------------------------------------

	apInitScenario ("7. property states after independent RegExp calls with captures and global");

		re = /([a-z])/g;
		re2 = /(\d)/g;

		str = "h8";

		re.exec(str);
		re2.exec(str);
@if (!@_fast)
		verify (re.lastIndex, 1, "original RE's lastIndex value is unchanged", "Vs7 #110802");
		verify (re2.lastIndex, 2, "2nd RE's lastIndex value is unchanged", "");
		verify (RegExp.lastIndex, 2, "global RE's lastIndex value is unchanged", "");
		verify (RegExp.index, 1, "global RE's index value is unchanged", "");
		verify (RegExp.input, str, "global RE's input string value is unchanged", "");

		re.exec(str);
		
		verify (re.lastIndex, 0, "check original RE lastIndex again", "");
		verify (re2.lastIndex, 2, "check 2nd RE's lastIndex value again", "");
		verify (RegExp.lastIndex, 2, "check global RE's lastIndex value again", "");
		verify (RegExp.index, 1, "check global RE's index value again", "");
		verify (RegExp.input, str, "check global RE's input string value again", "");

		re2.exec(str);

		verify (re.lastIndex, 0, "roundtrip test original RE's lastIndex value", "");
		verify (re2.lastIndex, 0, "roundtrip test 2nd RE's lastIndex value", "");
		verify (RegExp.lastIndex, 2, "roundtrip test global RE's lastIndex value", "");
		verify (RegExp.index, 1, "roundtrip test global RE's index value", "");
		verify (RegExp.input, str, "roundtrip test global RE's string value", "");	
@end

	apEndTest();

}




regex017();


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
