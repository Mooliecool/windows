// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/////////////////////////////////////////////////////////////////////////
import System

function WriteLine(str)
{
	print(str);
}

//*****************************************************************************

function Right(str, len)
{
	return str.substr(str.length - len);
}

//*****************************************************************************

function RegExpTest(regExp, srcStr, replStr, count, matchExp, searchExp, replExp)
{
	var good = true;

	if (count != null || matchExp != null) {
		var matches = srcStr.match(regExp);
		var matchCount = matches == null ? 0 : matches.length;
		if (count != null && matchCount != count) {
			if (good) {
				WriteLine(regExp + ":");
				good = false;
			}
			WriteLine("Count=" + matchCount);
		}
		if (matchExp != null) {
			var matchRes = matches.toString();
			if (matchRes != matchExp) {
				if (good) {
					WriteLine(regExp + ":");
					good = false;
				}
				WriteLine(matchRes);
			}
		}
	}

	if (searchExp != null) {
		var searchRes = srcStr.search(regExp);
		if (searchRes != searchExp) {
			if (good) {
				WriteLine(regExp + ":");
				good = false;
			}
			WriteLine("Search=" + searchRes);
		}
	}

	if (replExp != null) {
		var replRes = srcStr.replace(regExp, replStr);
		if (replRes != replExp) {
			if (good) {
				WriteLine(regExp + ":");
				good = false;
			}
			WriteLine("{" + replRes + "}");
		}
	}
}

//*****************************************************************************

function RegExpTest1()
{
	WriteLine("***** RegExp Test #1: Basic Functionality *****");

	var regExp = /is/ig;
	var msg = regExp.toString();
	if (msg != "/is/ig")
		WriteLine("RegExp=" + msg);

	RegExpTest(regExp, "THIS is it", "***",
		2, "IS,is", 2, "TH*** *** it");
	RegExpTest(regExp, "THAT was it", "***",
		0, null, -1, "THAT was it");
}

//*****************************************************************************

function RegExpTest2()
{
	WriteLine("***** RegExp Test #2: Long Source Strings *****");

	var str1 = "fix fox ";
	var str2 = "fox fix ";

	for (var i = 0; i < 10; i++) {
		str1 += str1;
		str2 += str2;
	}

	var matches = str1.match(/fox/g);
	var matchCount = matches == null ? 0 : matches.length;
	if (matchCount != 1024)
		WriteLine("Count=" + matchCount);

	str1 = str1.replace(/fox/g,  "temp");
	str1 = str1.replace(/fix/g,  "fox");
	str1 = str1.replace(/temp/g, "fix");
	if (str1 != str2)
		WriteLine("MISMATCH");
}

//*****************************************************************************

function RegExpTest3()
{
	WriteLine("***** RegExp Test #3: IgnoreCase and Global Flags *****");

	var src  = "xXxX";
	var repl = "^";

	RegExpTest(/X/,   src, repl, 1, "X",       1, "x^xX");
	RegExpTest(/X/g,  src, repl, 2, "X,X",     1, "x^x^");
	RegExpTest(/X/i,  src, repl, 1, "x",       0, "^XxX");
	RegExpTest(/X/ig, src, repl, 4, "x,X,x,X", 0, "^^^^");
}

//*****************************************************************************

function RegExpTest4()
{
	WriteLine("***** RegExp Test #4: Beginning, End, and Newline *****");

	var src  = "foo\nbar";
	var repl = "X";

	RegExpTest(/\n/g, src, repl, 1, null, null, "fooXbar");
	RegExpTest(/^/g,  src, repl, 1, null, null, "X" + src);
	RegExpTest(/$/g,  src, repl, 1, null, null, src + "X");
}

//*****************************************************************************

function RegExpTest5()
{
	WriteLine("***** RegExp Test #5: Shorthands and Ranges *****");

	var src  = "ABCabc:: :::01::::  :::234:   ::De5    d5E:";
	var repl = "[$1]";

	RegExpTest(/(\w+)/ig,    src, repl, 5, null, null,
		"[ABCabc]:: :::[01]::::  :::[234]:   ::[De5]    [d5E]:");
	RegExpTest(/(\W+)/ig,    src, repl, 5, null, null,
		"ABCabc[:: :::]01[::::  :::]234[:   ::]De5[    ]d5E[:]");
	RegExpTest(/(\d+)/ig,    src, repl, 4, null, null,
		"ABCabc:: :::[01]::::  :::[234]:   ::De[5]    d[5]E:");
	RegExpTest(/(\D+)/ig,    src, repl, 5, null, null,
		"[ABCabc:: :::]01[::::  :::]234[:   ::De]5[    d]5[E:]");
	RegExpTest(/(\s+)/ig,    src, repl, 4, null, null,
		"ABCabc::[ ]:::01::::[  ]:::234:[   ]::De5[    ]d5E:");
	RegExpTest(/(\S+)/ig,    src, repl, 5, null, null,
		"[ABCabc::] [:::01::::]  [:::234:]   [::De5]    [d5E:]");
	RegExpTest(/\b/ig,       src, "^", 10, null, null,
		"^ABCabc^:: :::^01^::::  :::^234^:   ::^De5^    ^d5E^:");
	RegExpTest(/\B/ig,       src, "^", 34, null, null,
		"A^B^C^a^b^c:^:^ ^:^:^:0^1:^:^:^:^ ^ ^:^:^:2^3^4:^ ^ ^ ^:^:D^e^5 ^ ^ ^ d^5^E:^");
	RegExpTest(/([A-Z]+)/g,  src, repl, 3, null, null,
		"[ABC]abc:: :::01::::  :::234:   ::[D]e5    d5[E]:");
	RegExpTest(/([^A-Z]+)/g, src, repl, 3, null, null,
		"ABC[abc:: :::01::::  :::234:   ::]D[e5    d5]E[:]");
	RegExpTest(/([a-z]+)/g,  src, repl, 3, null, null,
		"ABC[abc]:: :::01::::  :::234:   ::D[e]5    [d]5E:");
	RegExpTest(/([^a-z]+)/g, src, repl, 4, null, null,
		"[ABC]abc[:: :::01::::  :::234:   ::D]e[5    ]d[5E:]");
}

//*****************************************************************************

function RegExpTest6()
{
	WriteLine("***** RegExp Test #6: ASCII Escape Values *****");

	var src = "";
	for (var i = 0; i < 128; i++)
		src += String.fromCharCode(i);
	for (i = 0; i < 4; i++)
		src += src;

	for (i = 0; i < 128; i++) {
		var patrn1  = "00" + i.toString(8);
		patrn1  = "\\" + Right(patrn1, 3);
		var regExp  = new RegExp(patrn1, "g");
		var matches = src.match(regExp);
		if (matches == null) {
			var count1 = 0;
			var msg1   = "0 {}";
		} else {
			count1 = matches.length;
			msg1   = matches.toString();
		}

		var patrn2  = "0" + i.toString(16);
		patrn2  = "\\x" + Right(patrn2, 2);
		regExp  = new RegExp(patrn2, "g");
		matches = src.match(regExp);
		if (matches == null) {
			var count2 = 0;
			var msg2   = "0 {}";
		} else {
			count2 = matches.length;
			msg2   = matches.toString();
		}

		var patrn3  = "000" + i.toString(16);
		patrn3  = "\\u" + Right(patrn3, 4);
		regExp  = new RegExp(patrn3, "g");
		matches = src.match(regExp);
		if (matches == null) {
			var count3 = 0;
			var msg3   = "0 {}";
		} else {
			count3 = matches.length;
			msg3   = matches.toString();
		}

		if (count1 != 16 || count2 != 16 || count3 != 16
		 || msg1 != msg2 || msg1 != msg3) {
			WriteLine(patrn1 + ": " + count1);
			WriteLine(patrn2 + ": " + count2);
			WriteLine(patrn3 + ": " + count3);
		}
	}

	var chs = ["b","B","d","D","f","n","r","s","S","t","v","w","W"];
	var cts = [128,1921,160,1888,16,16,16,96,1952,16,16,1008,1040];

	for (i = 0; i < 13; i++) {
		patrn1  = "\\" + chs[i];
		regExp  = new RegExp(patrn1, "g");
		matches = src.match(regExp);
		count1  = matches == null ? 0 : matches.length;
		if (count1 != cts[i])
			WriteLine(patrn1 + ": " + count1);
	}

	for (i = 0; i < 26; i++) {
		patrn1 = "\\c" + String.fromCharCode(i+65);
		regExp = new RegExp(patrn1, "g");
		matches = src.match(regExp);
		if (matches == null) {
			count1 = 0;
			msg1   = "0 {}";
		} else {
			count1 = matches.length;
			msg1   = matches.toString();
		}

		patrn2 = "\\c" + String.fromCharCode(i+97);
		regExp = new RegExp(patrn2, "g");
		matches = src.match(regExp);
		if (matches == null) {
			count2 = 0;
			msg2   = "0 {}";
		} else {
			count2 = matches.length;
			msg2   = matches.toString();
		}

		if (count1 != 16 || count2 != 16 || msg1 != msg2) {
			WriteLine(patrn1 + ": " + count1);
			WriteLine(patrn2 + ": " + count2);
		}
	}
}

//*****************************************************************************

function RegExpTest7()
{
	WriteLine("***** RegExp Test #7: Bar *****");

	RegExpTest(/([A-Z]+)|([a-z]+)|(\d+)/g,
		"The 61 QUICK brOwn f0x",
		"[$1$2$3]", 10, null, null,
		"[T][he] [61] [QUICK] [br][O][wn] [f][0][x]")
}

//*****************************************************************************

function RepeatTest(src, min, max, exp)
{
	if (min > 0) {
		var regExp  = new RegExp("\\b\\w{" + min + "}\\b", "g");
		var matches = src.match(regExp);
		var msg     = matches == null ? 0 : matches.length;
	} else
		msg     = "-";

	regExp  = new RegExp("\\b\\w{" + min + ",}\\b", "g");
	matches = src.match(regExp);
	msg    += "," + (matches == null ? 0 : matches.length);

	for (var i = min; i <= max; i++) {
		regExp  = new RegExp("\\b\\w{" + min + "," + i + "}\\b", "g");
		matches = src.match(regExp);
		msg    += "," + (matches == null ? 0 : matches.length);
	}

	if (msg != exp)
		WriteLine("(" + min + "," + max + "): " + msg);
}

function CompareTest(patrn1, patrn2, src)
{
	var regExp  = new RegExp(patrn1, "g");
	var matches = src.match(regExp);
	var msg1    = matches == null ? "0 {}" : matches.toString();
	var msg2    = src.replace(regExp, "[$1]");

	regExp  = new RegExp(patrn2, "g");
	matches = src.match(regExp);
	var msg3    = matches == null ? "0 {}" : matches.toString();
	var msg4    = src.replace(regExp, "[$1]");

	if (msg1 != msg3) {
		WriteLine(patrn1 + ": " + msg1);
		WriteLine(patrn2 + ": " + msg3);
		System.Environment.ExitCode = 1;
	}
	if (msg2 != msg4) {
		WriteLine(patrn1 + ": " + msg2);
		WriteLine(patrn2 + ": " + msg4);
		System.Environment.ExitCode = 1;
	}
}

function RegExpTest8()
{
	WriteLine("***** RegExp Test #8: Repetition *****");

	var src = "";
	for (var i = 1; i <= 15; i++) {
		var ch = i.toString(16);
		for (var j = 0; j < i; j++)
			src += ch;
		src += " ";
	}

	RepeatTest(src,  0, 16, "-,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30");
	RepeatTest(src,  1, 16, "1,15,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15");
	RepeatTest(src,  2, 16, "1,14,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14");
	RepeatTest(src,  3, 16, "1,13,1,2,3,4,5,6,7,8,9,10,11,12,13,13");
	RepeatTest(src,  4, 16, "1,12,1,2,3,4,5,6,7,8,9,10,11,12,12");
	RepeatTest(src,  5, 16, "1,11,1,2,3,4,5,6,7,8,9,10,11,11");
	RepeatTest(src,  6, 16, "1,10,1,2,3,4,5,6,7,8,9,10,10");
	RepeatTest(src,  7, 16, "1,9,1,2,3,4,5,6,7,8,9,9");
	RepeatTest(src,  8, 16, "1,8,1,2,3,4,5,6,7,8,8");
	RepeatTest(src,  9, 16, "1,7,1,2,3,4,5,6,7,7");
	RepeatTest(src, 10, 16, "1,6,1,2,3,4,5,6,6");
	RepeatTest(src, 11, 16, "1,5,1,2,3,4,5,5");
	RepeatTest(src, 12, 16, "1,4,1,2,3,4,4");
	RepeatTest(src, 13, 16, "1,3,1,2,3,3");
	RepeatTest(src, 14, 16, "1,2,1,2,2");
	RepeatTest(src, 15, 16, "1,1,1,1");
	RepeatTest(src, 16, 16, "0,0,0");

	CompareTest("(\\b\\w{0,1}\\b)", "(\\b\\w?\\b)", src);
	CompareTest("(\\b\\w{0,}\\b)",  "(\\b\\w*\\b)", src);
	CompareTest("(\\b\\w{1,}\\b)",  "(\\b\\w+\\b)", src);
}

//*****************************************************************************

function RegExpTest9()
{
	WriteLine("***** RegExp Test #9: Backreferences *****");

	var f1  = 1;
	var f2  = 1;
	var src = "11";

	for (var i = 2; i <= 40; i++) {
		var f    = f1 + f2;
		f2   = f1;
		f1   = f;
		src += f.toString(2);
	}

	RegExpTest(/(.)\1/g,        src, null, 191, null, null, null);
	RegExpTest(/(.{2})\1/g,     src, null,  69, null, null, null);
	RegExpTest(/(.{3})\1/g,     src, null,  34, null, null, null);
	RegExpTest(/(.{4})\1/g,     src, null,  16, null, null, null);
	RegExpTest(/(.{5})\1/g,     src, null,   8, null, null, null);
	RegExpTest(/(.{6})\1/g,     src, null,   5, null, null, null);
	RegExpTest(/(.{7})\1/g,     src, null,   2, null, null, null);
	RegExpTest(/(.{8})\1/g,     src, null,   1, null, null, null);
	RegExpTest(/(.{9})\1/g,     src, null,   1, null, null, null);
	RegExpTest(/(.{10})\1/g,    src, null,   0, null, null, null);
	RegExpTest(/(.+)\1\1/g,     src, null,  83, null, null, null);
	RegExpTest(/(.+)(.+)\2\1/g, src, null,  62, null, null, null); 
}

//*****************************************************************************

System.Environment.ExitCode = 0;

RegExpTest1();
RegExpTest2();
RegExpTest3();
RegExpTest4();
RegExpTest5();
RegExpTest6();
RegExpTest7();
RegExpTest8();
RegExpTest9();
