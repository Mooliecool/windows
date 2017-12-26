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


var iTestID = 91749;

//regex014.js - Zero-Width look ahead assertions
 
function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}


function regex014() {
  @if(@_fast)
    var i, a, sExp, sAct, re, ms;
  @end


	apInitTest ("RegEx014 - Zero-width look ahead patterns");

//----------------------------------------------------------------------
	apInitScenario ("1. Positive zero-width look ahead patterns");


	sExp = "abc"
	re = /\w+(?=z)/;
	ms = re.exec("abcz");
	sAct = ms[0]
	verify (sAct, sExp, "pos zero-width look ahead 1", "");

	sExp = "12"
	re = /(?=1)12/;
	ms = re.exec("12");
	sAct = ms[0];
	verify (sAct, sExp, "pos zero-width look ahead 2", "");

	sExp = "\001";
	re = new RegExp("\\1(?=ecma3)", "i");
	ms = re.exec("\001ECMA3");
	sAct = ms[0];
 	verify (sAct, sExp, "pos zero-width look ahead 3", "VS7 #110896");

	sExp = "ECMA3ECMA3";
	//build an expression with 10 subexpressions to confuse the engine
	re = /(((((((((((?=ECMA)ECMA(?=3)3))))))))))\10/m;
	ms = re.exec("ECMA3ECMA3\n");
	verify (ms[0], sExp, "pos zero-width look ahead 4", "");
	verify (ms[0].length, 10, "pos zero-width look ahead 5", "");
	verify (ms.index, 0, "pos zero-width look ahead 6", "");

	sExp = "ECMA3"
	re = /(((((((((((?=ECMA)ECMA(?=3)3))))))))))\10/m;
	ms = re.exec("ECMA3ECMA3\n");
	//just checking the submatches
	for (i = 1; i<ms.length; i++) {
		verify (ms[i], sExp, "pos zero-width look ahead 7; i= " + i, "");
	}


//-----------------------------------------------------------------------
	apInitScenario ("2. deeply nested zero width look ahead");

	sExp = ("NSCP");
	//build an expression with 10 subexpressions to confuse the engine
	re = new RegExp("((((((((((\\w{2,}))))))))))\\10\012\\w", "m");
	verify ("IEIE\nM".replace(re, "NSCP"), sExp, "deeply nested 1", "");

	sExp = ("NSCPIEIE");
	re = /(((?=IE)))\1/
	verify ("IEIE".replace(re, "NSCP"), sExp, "deeply nested with no consumption + backrefence", "");


//-----------------------------------------------------------------------
	apInitScenario ("3. Character classes and pos zero-width look ahead");

	sExp = true;
	re = new RegExp("((?=[A-z]*)1)");
	sAct = re.test(1)
	verify (sAct, sExp, "char class 1", "");

	sExp = "1";
	re = /((1(?=[A-z]*)))/;
	ms = re.exec("1")
	verify (ms[0], sExp, "char class 2", "");

	sExp = true;
	re = new RegExp("((?=[A-z0-9]+)1)");
	sAct = re.test("yaDIdAdA34BiBOP" + 1)
	verify (sAct, sExp, "char class 3", "");

	sExp = "1";
	re = /((1(?=[A-z]+)))/;
	ms = re.exec("1xyz")
	verify (ms[0], sExp, "char class 4", "");

	sExp = new Array(65 + "\1");
	re = /(?=[\w0-5]+?)65\1/;   
	verify ("565\1".match(re), sExp.join(), "char class 5", "");

//-----------------------------------------------------------------------
	apInitScenario ("4. pos assertion(zero-width look ahead) after a greedy algorithm");

	sExp = true
	re = new RegExp("((a+)(?=a)a)");
	verify (re.test("aaa"), sExp, "assertion after greedy 1", "");
	
	sExp = "AAA"
	re = new RegExp("((a+)(?=a)a)", "i");
	ms = re.exec("AAA");
	verify (ms[0], sExp, "assertion after greedy 2", "bug ");
	verify (ms[1], sExp, "assertion after greedy 3", "");
	sExp = "AA"
	verify (ms[2], sExp, "assertion after greedy 4", "");

	sExp = "Testing_"
	re = new RegExp("([a-z_]+)(?=\\d)", "img");
	ms = re.exec("Testing_1\nTesting_2")
	verify (ms[0], sExp, "assertion after greedy 5", "");

//------------------------------------------------------------------------
	apInitScenario("5. pos assertion(zero-width look ahead) with {m,n}");

	sExp = "Xabcccc"
	re = /(?=abc{1,})/m;
	verify ("abcccc".replace(re, "X"), sExp, "zero-width look ahead {m,n} constructs 1", "");

	sExp = "Xabcccc"
	re = new RegExp("(?=abc{1,})(?=a)");
	verify ("abcccc".replace(re, "X"), sExp, "zero-width look ahead {m,n} constructs 2", "");

	sExp = true
	re = /(?=abc{1,})(?=a)(?=\w)/;
	verify (re.test ("abca"), sExp, "zero-width look ahead {m,n} constructs 3", "");


//-------------------------------------------------------------------------
	apInitScenario("6. Placement in pattern: by itself, w/ backrefence, in the middle w/ optional ?, and nested");

	sExp = "";
	re = /(?=\w)/;
	ms = "a".match(re);		//assertion, without a capture
	verify (ms, sExp, "assertion without capture" ,"");

	sExp = new Array("", "");
	re = /((?=\w))/;
	ms = "a".match(re);		//assertion, without a capture
	verify (ms.join(), sExp.join(), "assertion with a capture" ,"")

	sExp = false;
	re = /(?=\w)\1/;             //this will look for a word character assertion, then octal 1, whic hcan never occur since the assertion doesnt eat space
	ms = re.test("b");
	verify (ms, sExp, "assertion followed by impossible octal 1", "VS");

	sExp = new Array("", "");
	re = /((?=\w)\1)/;
	a = "x".match(re);

	for (i = 0; i < a.length; i++);
	{
		if (a[i] != sExp[i])
			apLogFailInfo ("Check value of array", sExp[i], a[i], "");
	}

	sExp = ""
	re = new RegExp("test(?=(ing)?)(ing)?", "g");
	verify ("testingtest".replace(re, ""), sExp, "placement 4", "")

	sExp = "nested zero-width look aheadsefg"
	re = /a?b+c{2,4}(?=d(?=e(?=f)(?=\w)))d/;
	verify ("bccdefg".replace(re, "nested zero-width look aheads"), sExp, "placement 5", "");


//----------------------------------------------------------------------------
	apInitScenario ("7. neg zero-width look ahead");

	sExp = true
	re = /(?!foo)bar/;	//this pattern specifies to match that the next 3 characters are not foo, then search for bar
	verify (re.test("foobar"), sExp, "neg zero-width look ahead 1", "");
	
	sExp = false
	re = /(?!foo)foo/;	//this pattern specifies to match that the next 3 characters are not foo, then search for bar
	verify (re.test("foobar"), sExp, "neg zero-width look ahead 2", "");

	sExp = "foobar"
	re = new RegExp("foo(?!bar)");
	verify ("foobar".replace(re, "XXX"), sExp, "neg zero-width look ahead 3", "");

	sExp = 2;			//lastIndex should point to the second b in abba.
	re = /(?!a)b/g;
	ms = re.exec("abba");
	verify (ms.lastIndex, sExp, "neg zero-width look ahead 4", "");
	sExp = 3;
	ms = re.exec("abba");
	verify (ms.lastIndex, sExp, "neg zero-width look ahead 5", "");

	sExp = false
	re = /a(?!\w?)/;	//\w? always suceeds, so the neg pattern always fails.
	verify (re.test("a"), sExp, "neg zero-width look ahead 6", "");

	sExp = null;
	re = /(?!(\w)??)/;
	ms = "x".match(re);
	verify (ms, sExp, "neg zero-width look ahead 7", "");	

	sExp = null;
	re = /(?!(\w)*?)/;
	ms = "x".match(re);
	verify (ms, sExp, "neg zero-width look ahead 8", "");	

	sExp = new Array("", "");
	re = /(?!(\w)+?)/;
	ms = "x".match(re);
	verify (ms.join(), sExp.join(), "neg zero-width look ahead 8", "");	


	apEndTest();

}


regex014();


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
