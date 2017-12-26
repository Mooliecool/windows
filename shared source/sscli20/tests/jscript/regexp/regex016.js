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


var iTestID = 78047;

//RegEx016 - Regular Expressions Backreferences and Replacement Strings

@if (@_fast)
	var sExp = "";
	var ms = "";
	var sAct = "";
@end

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) {
        apLogFailInfo (sMsg, sExp, sRes, sBug);
    }
}

function regex016() {
  @if(@_fast)
    var sExp, sAct, re, ms;
  @end

	apInitTest ("RegEx016 - Backreferences and Replacement strings");

//-------------------------------------------------------------------

	apInitScenario ("1. Basic BackReference sets");

	sExp = false;
	re = new RegExp ("(\d) \1");
	verify (re.test("5 6"), sExp, "BackReferences 1", "");

	sExp = false
	re = new RegExp ("(?=\\w)\\1");
	verify (re.test("aa"), sExp, "Backreferences 2", "VS7 #110896");

	sExp = "n n"
	re = /(n) \01/;
	verify ("n n".replace(re, "x"), sExp, "BackReferences 3", "");

	sExp = "t t&t";
	re = /\1 (\w)&\1/;	//pattern with backrefence in the beginning
	verify ("t t&t".replace(re, "xxx"), sExp, "Backreferences 4", "");

	sExp = "xxx";
	re = /\1 (\w)&\1/;	
	verify ("\001 t&t".replace(re, "xxx"), sExp, "Backreferences 5", "");

	sExp = "\001 xx x \003"	
	re = new RegExp("\1 ((\\w))\\1 \\2 \3", "m");
	ms = re.exec("\001 xx x \003");
	verify (ms[0], sExp, "Backreferences 4", "");
	sExp = 3
	verify (ms.length, sExp, "backreferences 5", "");
	sExp = "x"

	verify (ms[1], sExp, "Backreferences 6", "");
	verify (ms[2], sExp, "Backreferences 7", "");

	sExp = true
	re = /\1 (((\w))\1) \2 \3 \02/;
	sAct = re.test("\1 x x x \2");
	verify (sAct, sExp, "Backreferences 8", "");

	if (sAct) {
		sExp = "x"
		verify (ms[1], sExp, "Backreferences 9", "");
	}



	apInitScenario ("2. Replacement Strings");
	
	re = /(y)/;
	sExp = "x<y>z";
	verify ("xyz".replace(re, "<$1>"), sExp, "Replacement Strings 1", "");
	verify ("xyz".replace(re, "<$01>"), sExp, "Replacement Strings 2", "");
	verify ("xyz".replace(re, "<$&>"), sExp, "Replacement Strings 3", "");
	verify ("xyz".replace(re, "<$+>"), sExp, "Replacement Strings 4", "");
	sExp = "x<$>z";
	verify ("xyz".replace(re, "<$$>"), sExp, "Replacement Strings 5", "");
	sExp = "x<x>z"
	verify ("xyz".replace(re, "<$`>"), sExp, "Replacement Strings 6", "");
	sExp = "x<z>z";
	verify ("xyz".replace(re, "<$'>"), sExp, "Replacement Strings 7", "");
	sExp = "x<xyz>z";
	verify ("xyz".replace(re, "<$_>"), sExp, "Replacement Strings 8", "");
		
	sExp = "123\012x$z"
	re = new RegExp("(\\$)z", "m");
	verify ("123\nx$z".replace(re, "$1z"), sExp, "Replacement Strings 9", "");
	verify ("123\nx$z".replace(re, "$01z"), sExp, "Replacement Strings 10", "");
	verify ("123\nx$z".replace(re, "$+z"), sExp, "Replacement Strings 11", "");
	verify ("123\nx$z".replace(re, "$&"), sExp, "Replacement Strings 12", "");
	verify ("123\nx$z".replace(re, "$$z"), sExp, "Replacement Strings 13", "");

	sExp = "123\012x123\012x";
	verify ("123\nx$z".replace(re, "$`"), sExp, "Replacement Strings 14", "");
	sExp = "123\012x";
	verify ("123\nx$z".replace(re, "$'"), sExp, "Replacement Strings 15", "");
	sExp = "123\nx123\nx$z";
	verify ("123\nx$z".replace(re, "$_"), sExp, "Replacement Strings 16", "");

//----------------------------------------------------------------------------

	apInitScenario ("3. $n substitutions");

	re = /((.)(\d))/;
	sExp = "ad1"
	verify ("ad1".replace(re, "$1"), sExp, "$n sub 1", "");
	sExp = "ad"
	verify ("ad1".replace(re, "$2"), sExp, "$n sub 2", "");
	sExp = "a1"
	verify ("ad1".replace(re, "$3"), sExp, "$n sub 3", "");
	sExp = "a$4"
	verify ("ad1".replace(re, "$4"), sExp, "$n sub 4", "");
	sExp = "a$1"
	verify ("ad1".replace(re, "$$1"), sExp, "$n sub 5", "");

//----------------------------------------------------------------------------

	apInitScenario ("4. basic backslash character expressions")

	@if (@_jscript_version >= 7)
	re = /\a/;
	sExp = "\u0007";
	verify ("\u0007".match(re), sExp, "character expression \\a", "")
	@end

	re = /[\b]/;
	sExp = "\u0008";
	verify ("\u0008".match(re), sExp, "character expression \\b", "");

	re = /\t/;
	sExp = "\u0009";
	verify ("\u0009".match(re), sExp, "character expression \\t", "");

	re = /\r/;
	sExp = "\u000D";
	verify ("\u000D".match(re), sExp, "character expression \\r", "");

	re = /\v/;
	sExp = "\u000B";
	verify ("\u000B".match(re), sExp, "character expression \\v", "");

	re = /\f/;
	sExp = "\u000C";
	verify ("\u000C".match(re), sExp, "character expression \\f", "");

	re = /\n/;
	sExp = "\u000A";
	verify ("\u000A".match(re), sExp, "character expression \\n", "");

	@if (@_jscript_version >= 7)
	re = /\e/;
	sExp = "\u001B";
	verify ("\u001B".match(re), sExp, "character expression \\e" ,"");
	@end

	re = /\cC/;	//control-C
	sExp = "\u0003";
	verify ("\u0003".match(re), sExp, "character expression \cC", "");

	re = /\041/;
	sExp = "\u0021";
	verify ("\u0021".match(re), sExp, "character expression - octal test", "");

	re = /\x21/;
	sExp = "\u0021";
	verify ("\u0021".match(re), sExp, "character expression - hexadecimal test", "");

	re = /\u0021/;
	sExp = "\u0021";
	verify ("\u0021".match(re), sExp, "character expression - unicode test", "");

//----------------------------------------------------------------------------

	apInitScenario ("5. named substitutions")

	@if (@_jscript_version >= 7)
	var re = /(?<proto>\w+):\/\/[^:]+:(?<port>\d+)?/;
	sExp = "http";
	verify ("http://www.foobar.com:8080".replace(re, "${proto}"), sExp, "named sub 1", "");

	sExp = "http8080";
	verify ("http://www.foobar.com:8080".replace(re, "${proto}${port}"), sExp, "named sub 2", "");

	re = /(?<p>\w+?)\1/g;
	sExp = "1x";
	verify ("xxx".replace(re, "1"), sExp, "named sub 3", "");

	re = /(?<p>\w+?)\1/g;
	sExp = "2x2x";
	verify ("xxx".replace(re, "2${p}2"), sExp, "named sub 4", "");

	re = /(?<p>\w)(?<p>\d)/;
	sExp = "1";
	verify ("a1".replace(re, "${p}"), sExp, "named sub 5", "");

	re = /(?<p>\w)(?<p>\d)/g;
	sExp = "123";
	verify ("x1y2z3".replace(re, "${p}"), sExp, "named sub 6", "");

	re = /(?<char>\w?)\k<char>/
	sExp = "aa";
	verify ("aa".match(re), sExp, "named sub 7", "");

	re = /(?<a1>\w)\k<a1>/;
	sExp = "38";
	verify ("338".replace(re, "${a1}"), sExp, "named sub 8", "");

	re = /(?<a>\w)/;
	sExp = "${foo}38";
	verify ("338".replace(re, "${foo}"), sExp, "named sub 9", "");

	re = /(?<nothing>(nonmatch)?)abc/;
	sExp = "123foobar"
	verify ("123abcbar".replace(re, "fo${nothing}o"), sExp, "named sub 10", "");



	@end


	apEndTest();
}



regex016();


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
