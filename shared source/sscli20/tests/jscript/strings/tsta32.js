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


var iTestID = 53823;

	//This testcase uses the /ABC/ pattern as the bases for the pattern searching.
	
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
{var i;
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



function tsta32() {
	

apInitTest("tsta32");
	
	
	
	
	m_scen = ('Test 17 mixedCASE /(\u0000)(\001Anmnoz)/');

	
	sCat = "/(\u0000)(\001Anmnoz)/ ";
	apInitScenario(m_scen);
	regPat = /(\u0000)(\001Anmnoz)/;
	objExp = true;
	regExp = new Array('\u0000','\001Anmnoz');
	
	strTest = '\u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A	\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 18 Slightly more complex strings');

	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 19 mixedCASE tests /(\u0000)(\001Anmnoz)/ multiples');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     \u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001Anmnoz\u00001def\u0000\001A\000\u0000';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000            |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|            \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000           | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 21 mixedCASE /((\u0000)(\001Anmnoz))/');

	
	sCat = "/((\u0000)(\001Anmnoz))/ ";
	apInitScenario(m_scen);
	regPat = /((\u0000)(\001Anmnoz))/;
	objExp = true;
	regExp = new Array('\u0000\001Anmnoz','\u0000','\001Anmnoz');
	
	strTest = '\u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A	\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 22 Slightly more complex strings');

	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 23 mixedCASE tests /((\u0000)(\001Anmnoz))/ multiples');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     \u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 24 Slightly more complex strings w/ multiple finds');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001Anmnoz\u00001def\u0000\001A\000\u0000';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000            |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|            \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000           | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 25 mixedCASE /(\u0000)(\001A)(nmnoz)/');

	
	sCat = "/(\u0000)(\001A)(nmnoz)/ ";
	apInitScenario(m_scen);
	regPat = /(\u0000)(\001A)(nmnoz)/;
	objExp = true;
	regExp = new Array('\u0000', '\001A', 'nmnoz');
	
	strTest = '\u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A	\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 26 Slightly more complex strings');

	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 27 mixedCASE tests /(\u0000)(\001A)(nmnoz)/ multiples');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     \u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 28 Slightly more complex strings w/ multiple finds');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001Anmnoz\u00001def\u0000\001A\000\u0000';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000            |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|            \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000           | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 29 mixedCASE /((\u0000)(\001A)(nmnoz))/');

	
	sCat = "/((\u0000)(\001A)(nmnoz))/ ";
	apInitScenario(m_scen);
	regPat = /((\u0000)(\001A)(nmnoz))/;
	objExp = true;
	regExp = new Array('\u0000\001Anmnoz','\u0000','\001A','nmnoz');
	
	strTest = '\u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A      \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A\u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A	\u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 30 Slightly more complex strings');

	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 31 mixedCASE tests /((\u0000)(\001A)(nmnoz))/ multiples');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     \u0000\001Anmnoz \u0000\001Anmnoz';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       |';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 32 Slightly more complex strings w/ multiple finds');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001Anmnoz\u00001def\u0000\001A\000\u0000';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000            |'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|            \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000           | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             | '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 33 Failure UPPERCASE /A\001A\xFF/');

	
	sCat = "/A\001A\xFF/ ";
	apInitScenario(m_scen);
	regPat = /A\001A\xFF/;
	objExp = false;
	//no new regExp expected values due to spec of RegExp.$1-9
	//these tests verify that old values are kept
	
	strTest = '\u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      \u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz       |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          \u0000\001Anmnoz       |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A \u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A      \u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A\u0000\001Anmnoz       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A	\u0000\001Anmnoz       |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001A          \u0000\001Anmnoz       |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 34 Failure-Slightly more complex strings');

	
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001A nmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 35 Failure UPPERCASE tests /A\001A\xFF/ multiples');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     \u0000\001Anmnoz \u0000\001Anmnoz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz      |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         \u0000\001Anmnoz \u0000\001Anmnoz       |';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 36 Failure - Slightly more complex strings w/ multiple finds');

	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001Anmnoz\u00001def\u0000\001A\000\u0000';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|              \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000|'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000            |'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|            \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '\u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000           | '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '|           \u0000\001Anmnoz \u0000\001Anmnoz \u0000\001A\000\u0000nmnoz\u00001def\u0000\001A\000\u0000             | '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
/*****************************************************************************/
	apEndTest();
}


tsta32();


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
