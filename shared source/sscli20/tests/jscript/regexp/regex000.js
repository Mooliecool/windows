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


var iTestID = 88498;


//regex000.js - Regular Expression constructor/function tests
//Testing for new constructor

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}


function regex000() {
  @if(@_fast)
    var re, pat, e, illegalPattern;
  @end

	var loc_id = apGetLocale();
	apInitTest ("RegEx000 - RegExp contructor/function tests");

//-----------------------------------------------------------
	apInitScenario ("1. Test cases for testing JS-regular expression constructor/function");

		re = RegExp ("abc123");	
		verify (re, "/abc123/", "simple valid test case on RegExp function", "");

		re = new RegExp ("abc123");
		verify (re, "/abc123/", "simple valid test case on RegExp constructor", "");


		re = RegExp ("\\S\\w\\B");
		verify (re, "/\\S\\w\\B/", "valid test case with character class on RegExp function", "");

		re = new RegExp ("\\S\\w\\B");
		verify (re, "/\\S\\w\\B/", "valid test case with character class on RegExp constructor", "");


		
		try {
		re = RegExp("\w", "gmi");
		verify (re, "/\w/igm", "check for order of our flags when calling RegExp as function", "");
		}
		catch(e) {
			verify (re, "/\w/igm", "check for order of our flags when calling RegExp as function", "");
		}

		
		try {
			re = new RegExp("\w", "gmi");
			verify (re, "/\w/igm", "check for order of our flags when calling RegExp as function", "");
		}
		catch(e) {
			verify (re, "/\w/igm", "check for order of our flags when calling RegExp as function", "");
		}

		re = RegExp ("\\d{3}[a-z]*?\\w\\B", "i");
		verify (re, "/\\d{3}[a-z]*?\\w\\B/i", "complex declarations (in quotes) for function constructor", "");

		re = new RegExp ("\\d{3}[a-z]*?\\w\\B", "i");
		verify (re, "/\\d{3}[a-z]*?\\w\\B/i", "complex declarations (in quotes) for RegExp constructor", "");


		re = RegExp ("(?:[t-z\\w\\D]*?)??jack", "mg");
		verify (re, "/(?:[t-z\\w\\D]*?)??jack/gm", "more complex declarations (in quotes) for RegExp function ", "");

		re = new RegExp ("(?:[t-z\\w\\D]*?)??jack", "mg");
		verify (re, "/(?:[t-z\\w\\D]*?)??jack/gm", "more complex declarations (in quotes) for RegExp constructor", "");


		pat = "[\\d_]+?";

		re = RegExp (pat);
		verify (re, "/[\\d_]+?/", "passing a string as parameter to RegExp function", "");

		re = new RegExp (pat);
		verify (re, "/[\\d_]+?/", "passing a string as parameter to RegExp constructor", "");


		pat = /(?:[1224ab-z]?)/m;
	
		re = new RegExp (pat);
		verify (re, "/(?:[1224ab-z]?)/m", "passing a RegExp as parameter to RegExp constructor", "");
		if (re == new RegExp(re))
			apLogFailInfo ("test that RegExp returns a clone via the RegExp constructor", re, new RegExp(re), "");


		pat = new RegExp("(?![4-6_y-z]+)??a*", "gmi");

		re = RegExp (pat);
		verify (re, "/(?![4-6_y-z]+)??a*/igm", "passing a constructed RegExp as parameter to RegExp function", "");  // comment

		re = new RegExp (pat);
		verify (re, "/(?![4-6_y-z]+)??a*/igm", "passing a constructed RegExp as parameter to RegExp constructor", ""); // comment


		var undefpat;

		re = RegExp (undefpat, "m");
		verify (re, "//m", "undefined pattern as parameter to RegExp function is legal", "");

		re = new RegExp(undefpat, "m");
		verify (re, "//m", "undefined pattern as parameter to RegExp constructor is legal", "");


		pat = /(?=\w)?/g;

		re = RegExp (pat, undefpat);
		verify (re, "/(?=\\w)?/g", "undefined flags to RegExp function are legal", "");

		re = new RegExp (pat, undefpat);
		verify (re, "/(?=\\w)?/g", "undefined flags to RegExp constructor are legal", "");



	apInitScenario ("2. Invalid arguments passed to the RegExp function or constructor");

		try {
			re = RegExp("\w", "a");
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}


		try {
			re = new RegExp("\w", "a");
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}


/*
 **	The following is compile time error, that we can't check in automation
 *
		try {
			re = RegExp(/\w/a);
			verify (e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "")
		}
		catch(e) {
			verify (e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "")
		}


		try {
			re = new RegExp(/\w/a);
			verify (e.description, "Syntax error in regular expression", "invalid flags in Regexp constructor", "")
		}
		catch(e) {
			verify (e.description, "Syntax error in regular expression", "invalid flags in Regexp constructor", "")
		}
 *
 *
*/


	
		try {
			illegalPattern = "\\";
			re = RegExp(illegalPattern);
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}

		try {
			illegalPattern = "\\";
			re = RegExp(illegalPattern);
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}


		pat = /\w\d/ig;
		try {
			re = RegExp (pat, "m");
			verify (re, "Wrong value", "Illegal parameter passed to RegExp function - can't pass flags if the first param is a regexp", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}

		try {
			re = new RegExp (pat, "m");
			verify (re, "Wrong value", "Illegal parameter pased to RegExp constructor - can't pass flags if the first param is a regexp", "");
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}



		try {
		    re = RegExp("\w", "ggmimi");
		    verify (re, "/\w/igm", "check for multiple occurances of flags when calling RegExp constructor", "");	
		}
		catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
		}

        try {
		    re = new RegExp("\w", "ggmimi");
		    verify (re, "/\w/igm", "check for multiple occurances of our flags when calling RegExp constructor", "");
       	}
        catch(e) {
			verify(e.description, "Syntax error in regular expression", "invalid flags in Regexp as a function", "");
       	}

	apEndTest();
}


regex000();


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
