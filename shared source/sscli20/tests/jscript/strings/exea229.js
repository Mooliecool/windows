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


var iTestID = 52630;

var sCat = '';
var regPat;
var objExp;

var m_scen = '';


function verify(sCat1, sExp, sAct)
{
	//this function makes sure sAct and sExp are equal

	if (sExp != sAct)
		apLogFailInfo( m_scen+(sCat1.length?"--"+sCat1:"")+" failed", sExp,sAct, "");
}





function verifyStringObj(sCat1, sExp, sAct)
{
	//this function SImply calls verify with the values of the string
	verify(sCat1, sExp.valueOf(), sAct.valueOf());
}





function ArrayEqual(sCat1, arrayExp, arrayAct)
{  var i;
	//Makes Sure that Arrays are equal
	if (arrayAct == null)
		verify(sCat1 + ' NULL Err', arrayExp, arrayAct);
	else if (arrayExp.length != arrayAct.length)
		verify(sCat1 + ' Array length', arrayExp.length, arrayAct.length);
	else
	{
		for (i in arrayExp)
			verify(sCat1 + ' index '+i, arrayExp[i], arrayAct[i]);
	}
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



function exea229() {
	

apInitTest("Exea229");



	m_scen = ('Test 13 unicode  e /[\uC000-\uC000]/');

	sCat = "one character ranges ";
	apInitScenario(m_scen);
	regPat = /[\uC000-\uC000]/;
	objExp = new Array('\uC000');
	var regExp = new Array();

	var strTest = '\uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002\uABCD';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002\uABCD';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '      \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC501\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uABCD          \uC501\uC000\uC001\uC002\uABCD       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	strTest = '\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC501            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	m_scen = ('Test 15 unicode  e tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     \uC000\uC001\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002 \uC000\uC001\uC002\uABCD      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002 \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002\uC501 \uABCD\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002\uABCD \uDBCA\uC000\uC001\uC002\u007F \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 \uC501\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uC501\uC000\uC001\uC002\uB000 \u1997\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uC000\uC001\uC002\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC000\uC001\uC002\uC501 \uC000\uC001\uC000\uC001\uC002\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC501\u007F \uA000\uC000\uC000\uC001\uC002\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC501\u007F \uC501\uDBCA\uC000\uC001\uC002\uaced\uD000 {*\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001.}.             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	m_scen = ('Test 17 Failure - Upper Case /[\uF000-\uF000]/');

	sCat = "Both Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /[\uF000-\uF000]/;

	objExp = null;
	regExp = new Array();

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '      \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC501\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002   \uA000\uB000\uFfFf\uC501    ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uABCD          \uC501\uC000\uC001\uC002\uABCD  \uA000\uB000\uFfFf\uC501     ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 18 Failure - Slightly more complex strings');

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001 \uC002 \uC501\uC000\uC001\uC002\ufAaD\uA000\uB000\uFfFf\uC501  \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC501            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	m_scen = ('Test 19 Failure - Upper Case tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002\uC501 \uABCD\uA000\uB000\uFfFf\uC501       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 20 Failure - Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFf\uC501\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFf\uC501\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002\uABCD \uDBCA\uA000\uB000\uFfFf\uC501 \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 \uC501\uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uC501\uC000\uC001\uC002\uB000 \u1997\uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFf\uC501 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFf\uC501 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uA000\uB000\uFfFf\uC501\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uA000\uB000\uFfFf\uC501 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC000\uC001\uC002\uC501 \uC000\uC001\uA000\uB000\uFfFf\uC501\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC501\u007F \uA000\uC000\uA000\uB000\uFfFf\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC501\u007F \uC501\uDBCA\uA000\uB000\uFfFf\uC501 {*\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001.}.             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 21 Failure - Upper Case /[^\uC000-\uFfFf 	]/');

	sCat = "Both Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /[^\u007E-\uFfFf 	]/;

	objExp = null;
	regExp = new Array();

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = '\uC501\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '      \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '          \uC501\uC000\uC001\uC002       \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002   \uA000\uB000\uFfFf\uC501    ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uABCD          \uC501\uC000\uC001\uC002\uABCD  \uA000\uB000\uFfFf\uC501     ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 22 Failure - Slightly more complex strings');

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001 \uC002 \uC501\uC000\uC001\uC002\ufAaD\uA000\uB000\uFfFf\uC501  \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002\uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC501            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	m_scen = ('Test 23 Failure - Upper Case tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC501\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         \uC000\uC001\uC002\uC501 \uABCD\uA000\uB000\uFfFf\uC501       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 24 Failure - Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFf\uC501\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFf\uC501\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC501\uC000\uC001\uC002\uABCD \uDBCA\uA000\uB000\uFfFf\uC501 \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 \uC501\uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uC501\uC000\uC001\uC002\uB000 \u1997\uA000\uB000\uFfFf\uC501 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFf\uC501 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFf\uC501 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFf\uC501 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uA000\uB000\uFfFf\uC501\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uA000\uB000\uFfFf\uC501 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              \uC000\uC000\uC001\uC002\uC501 \uC000\uC001\uA000\uB000\uFfFf\uC501\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC501\u007F \uA000\uC000\uA000\uB000\uFfFf\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC501\u007F \uC501\uDBCA\uA000\uB000\uFfFf\uC501 \uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

/*****************************************************************************/
	apEndTest();
}


exea229();


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
