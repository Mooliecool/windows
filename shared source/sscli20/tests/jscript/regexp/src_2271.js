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


var iTestID = 53630;

var sCat = '';
var regPat = "";
var objExp = "";
var m_scen = '';
var strTest = "";
var strTemp = "";
var regExp = "";


function verify(sCat1, sExp, sAct)
{
	//this function makes sure sAct and sExp are equal

	if (sExp != sAct)
		apLogFailInfo( m_scen+(sCat1.length?"--"+sCat1:"")+" failed", sExp,sAct, "");
}



function verifyStringObj(sCat1, sExp, sAct)
{
	//this function simply calls verify with the values of the string
	verify(sCat1, sExp.valueOf(), sAct.valueOf());
}



function ArrayEqual(sCat1, arrayExp, arrayAct)
{
	//redirects function call to verify
	verify(sCat1, arrayExp, arrayAct);
}



function regVerify(sCat1, arrayReg, arrayAct)
{
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





function src_2271() {
	

apInitTest("src_2271");


	m_scen = ('Test 1 unicode   /\uC000\uC001\uC002{{,{}/');

	sCat = "m == n ";
	apInitScenario(m_scen);
	regPat = /\uC000\uC001\uC002{5,5}/;

	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	m_scen = ('Test 2 unicode   /\uC000\uC001\uC002{{,{+1}/');

	sCat = "m == n - 1 ";
	apInitScenario(m_scen);
	regPat = /\uC000\uC001\uC002{5,6}/;

	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	m_scen = ('Test 3 unicode   /\uC000\uC001\uC002{{,{+20}/');

	sCat = "m == n - 20 ";
	apInitScenario(m_scen);
	regPat = /\uC000\uC001\uC002{5,25}/;

	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end


	objExp = new Array('\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002');
	regExp = new Array();

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 7;
	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '       \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

/*****************************************************************************/
	apEndTest();
}


src_2271();


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
