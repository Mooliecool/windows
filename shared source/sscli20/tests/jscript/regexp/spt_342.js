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


var iTestID = 53532;


var sCat = '';
var regPat = "";
var objExp = "";
var strTest = "";
var regExp = ""
var m_scen = '';
var strTemp = "";

function verify(sCat1, sExp, sAct){
	if (sExp != sAct)
		apLogFailInfo( m_scen+(sCat1.length?"--"+sCat1:"")+" failed", sExp,sAct, "");
}

function verifyStringObj(sCat1, sExp, sAct){
	verify(sCat1, sExp.valueOf(), sAct.valueOf());
}

function ArrayEqual(sCat1, arrayExp, arrayAct){
	var i;

	if (arrayAct == null)
		verify(sCat1 + ' NULL Err', arrayExp, arrayAct);
	else if (arrayExp.length != arrayAct.length)
		verify(sCat1 + ' Array length', arrayExp.length, arrayAct.length);
	else{
		for (i in arrayExp)
			verify(sCat1 + ' index '+i, arrayExp[i], arrayAct[i]);
	}
}

function regVerify(sCat1, arrayReg, arrayAct){
	var i;
	var expArray = new Array('','','','','','','','','');

	for (i in arrayReg)
		if (i < 10)
			expArray[i] = arrayReg[i];
		else
			break;

	for(i =0; i<9;i++)
		verify(sCat1 + ' RegExp.$'+ (i+1) +' ', expArray[i], eval('RegExp.$'+(i+1)));
}

function spt_342() {
	apInitTest("spt_342");

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 1 Mixed Case /(a)(\\u0000)(C)(911)(\+)(\^)(\*)(\\t)(\\005)(\\uFFFF)/');
	apInitScenario(m_scen);

	sCat = "/(a)(\\u0000)(C)(911)(\+)(\^)(\*)(\\t)(\\005)(\\uFFFF)/ ";
	regPat = /(a)(\u0000)(C)(911)(\+)(\^)(\*)(\t)(\005)(\uFFFF)/;
	regExp = new Array('a','\u0000', 'C', '911', '+', '^', '*','\t', '\005', '\uFFFF');

	objExp = new Array();
	strTest = 'a\u0000C911+^*\t\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ');
	strTest = 'a\u0000C911+^*\t\005\uFFFF          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ');
	strTest = '             a\u0000C911+^*\t\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '           ');
	strTest = '        a\u0000C911+^*\t\005\uFFFF           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 2 Mixed Case /(\\w)(\\u0000)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)/');
	apInitScenario(m_scen);

	sCat = "/(\\w)(\\u0000)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)/ ";
	regPat = /(\w)(\u0000)(\w)(\d\d\d)(\W)(\^)(\*)(\s)(\005)(\uFFFF)/;
	regExp = new Array('a','\u0000', 'C', '911', '+', '^', '*','\t', '\005', '\uFFFF');

	objExp = new Array();
	strTest = 'a\u0000C911+^*\t\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ');
	strTest = 'a\u0000C911+^*\t\005\uFFFF          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ');
	strTest = '             a\u0000C911+^*\t\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '           ');
	strTest = '        a\u0000C911+^*\t\005\uFFFF           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 3 Mixed Case /(\\w)(\\u0000)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)/');
	apInitScenario(m_scen);

	sCat = "/(\\w)(\\u0000)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)/ ";
	regPat = /(\w)(\u0000)(\w)(\d\d\d)(\W)(\^)(\*)(\s)(\005)(\uFFFF)/;
	regExp = new Array('b','\u0000', 'C', '921', '\xFF', '^', '*','\f', '\005', '\uFFFF');

	objExp = new Array();
	strTest = 'b\u0000C921\xFF^*\f\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ');
	strTest = 'b\u0000C921\xFF^*\f\005\uFFFF          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ');
	strTest = '             b\u0000C921\xFF^*\f\005\uFFFF';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '           ');
	strTest = '        b\u0000C921\xFF^*\f\005\uFFFF           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 4 Mixed Case /(\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+)/');
	apInitScenario(m_scen);

	sCat = "/(\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+)/ ";
	regPat = /(\w)(\u0001)(\w)(\d\d\d)(\W)(\^)(\*)(\s)(\005)(\uFFFF)(d+)/;
	regExp = new Array('b','\u0001', 'C', '921', '\xFF', '^', '*','\f', '\005', '\uFFFF');

	objExp = new Array();
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ');
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFd          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ');
	strTest = '             b\u0001C921\xFF^*\f\005\uFFFFd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '           ');
	strTest = '        b\u0001C921\xFF^*\f\005\uFFFFd           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 5 Mixed Case /((\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+))/');
	apInitScenario(m_scen);

	sCat = "/((\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+))/ ";
	regPat = /((\w)(\u0001)(\w)(\d\d\d)(\W)(\^)(\*)(\s)(\005)(\uFFFF)(d+))/;
	regExp = new Array('b\u0001C921\xFF^*\f\005\uFFFFd','b','\u0001', 'C', '921', '\xFF', '^', '*','\f', '\005');

	objExp = new Array();
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ');
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFd          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ');
	strTest = '             b\u0001C921\xFF^*\f\005\uFFFFd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '           ');
	strTest = '        b\u0001C921\xFF^*\f\005\uFFFFd           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 6 Mixed Case /((\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+))\2\3\4\5\6\7\8\9');
	apInitScenario(m_scen);

	sCat = "/((\\w)(\\u0001)(\\w)(\\d\\d\\d)(\\W)(\\^)(\\*)(\\s)(\\005)(\\uFFFF)(d+))\2\3\4\5\6\7\8\9/ ";
	regPat = /((\w)(\u0001)(\w)(\d\d\d)(\W)(\^)(\*)(\s)(\005)(\uFFFF)(d+))\2\3\4\5\6\7\8\9/;
	regExp = new Array('b\u0001C921\xFF^*\f\005\uFFFFd','b','\u0001', 'C', '921', '\xFF', '^', '*','\f', '\005');

	objExp = new Array();
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFdb\u0001C921\xFF^*\f';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('\005\uFFFFd          ');
	strTest = 'b\u0001C921\xFF^*\f\005\uFFFFdb\u0001C921\xFF^*\f\005\uFFFFd          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ' , '\005\uFFFFd');
	strTest = '             b\u0001C921\xFF^*\f\005\uFFFFdb\u0001C921\xFF^*\f\005\uFFFFd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('        ' , '\005\uFFFFd           ');
	strTest = '        b\u0001C921\xFF^*\f\005\uFFFFdb\u0001C921\xFF^*\f\005\uFFFFd           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

//-----------------------------------------------------------------------------------------
	m_scen = ('Test 7 Mixed Case /(-)(\1\1)(\1\2)(\1\3)(\2\3)(\2\4)/ ');
	apInitScenario(m_scen);

	sCat = "/(-)(\1\1)(\1\2)(\1\3)(\2\3)(\2\4)/ ";
	regPat = /(-)(\1\1)(\1\2)(\1\3)(\2\3)(\2\4)/;
	regExp = new Array('-','--','---', '----', '-----', '------');

	objExp = new Array('-');
	strTest = '----------------------';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('-          ');
	strTest = '----------------------          ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('             ' , '-');
	strTest = '             ----------------------';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ' , '-           ');
	strTest = '       ----------------------           ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
//-----------------------------------------------------------------------------------------
	apInitScenario("return control from last scenario");
	apEndTest();
}


spt_342();


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
