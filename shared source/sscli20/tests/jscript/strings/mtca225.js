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


var iTestID = 53074;

	var sCat = '';
	var regPat = "";
	var objExp = "";
       var regExp = "";
       var strTest = "";
       var m_scen = '';
       var strTemp = "";

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





function mtca225() {
	
  @if(@_fast)
    var i, strTest, regExp;
  @end

apInitTest("mtca225");
	m_scen = ('Test 13 nonalpha  /\uA000\uB000\uFfFf|\uC000\uC001\uC002/');

	apInitScenario(m_scen);
	regPat = /\uA000\uB000\uFfFf|\uC000\uC001\uC002/;
	objExp = new Array('\uC000\uC001\uC002');
	regExp = new Array();

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFf';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          \uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002   \uA000\uB000\uFfFfD    ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uABCD          D\uC000\uC001\uC002\uABCD  \uA000\uB000\uFfFfD     ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC002 D\uC000\uC001\uC002\ufAaD\uA000\uB000\uFfFfD  \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 D            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	m_scen = ('Test 15 nonalpha  tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     \uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002 \uA000\uB000\uFfFfD       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002D \uABCD\uA000\uB000\uFfFfD       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uABCD \uDBCA\uA000\uB000\uFfFfD \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 D\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           D\uC000\uC001\uC002\uB000 \u1997\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000D             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uA000\uB000\uFfFfD\uC000 \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC000\uC001\uC002D \uC000\uC000\uC001\uA000\uB000\uFfFfD\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002D\u007F \uA000\uC000\uA000\uB000\uFfFfD\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002D\u007F D\uDBCA\uA000\uB000\uFfFfD \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	m_scen = ('Test 17 nonalpha  /\uC000\uC001\uC002|\uC000\uC001\uC002/');

	apInitScenario(m_scen);
	regPat = /\uC000\uC001\uC002|\uC000\uC001\uC002/;
	objExp = new Array('\uC000\uC001\uC002');
	regExp = new Array();

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFf';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          \uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002   \uA000\uB000\uFfFfD    ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uABCD          D\uC000\uC001\uC002\uABCD  \uA000\uB000\uFfFfD     ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 18 Slightly more complex strings');

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC002 D\uC000\uC001\uC002\ufAaD\uA000\uB000\uFfFfD  \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 D            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	m_scen = ('Test 19 nonalpha  tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     \uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002 \uA000\uB000\uFfFfD       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002D \uABCD\uA000\uB000\uFfFfD       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uABCD \uDBCA\uA000\uB000\uFfFfD \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 D\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           D\uC000\uC001\uC002\uB000 \u1997\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000D             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uA000\uB000\uFfFfD\uC000 \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC000\uC001\uC002D \uC000\uC000\uC001\uA000\uB000\uFfFfD\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002D\u007F \uA000\uC000\uA000\uB000\uFfFfD\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000D\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002D\u007F D\uDBCA\uA000\uB000\uFfFfD \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 21 Failure - Lowercase /\uC000\uC001\uC002|\uC000\uC001\uC002/');

	apInitScenario(m_scen);
	regPat = /\uC000\uC001\uC002\u007F|\uA000\uB000\uFfFf\uD000D/;
	objExp = null;
	regExp = new Array();

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFf';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'D\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002\uABCD\uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          \uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '          D\uC000\uC001\uC002       \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001      \uC000\uC001\uC002\uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001	\uC000\uC001\uC002\uABCD \uA000\uB000\uFfFfD      ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001          \uC000\uC001\uC002   \uA000\uB000\uFfFfD    ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uABCD          D\uC000\uC001\uC002\uABCD  \uA000\uB000\uFfFfD     ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 22 Failure - Slightly more complex strings');

	strTest = '\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001 \uC002 D\uC000\uC001\uC002\ufAaD\uA000\uB000\uFfFfD  \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002\uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uABCD\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uB000\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001 D            '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	m_scen = ('Test 23 Failure - Lowercase tests w/ multiples');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     \uC000\uC001\uC002 \uA000\uB000\uFfFfD';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'D\uC000\uC001\uC002 \uA000\uB000\uFfFfD      ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002 \uA000\uB000\uFfFfD       ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         \uC000\uC001\uC002D \uABCD\uA000\uB000\uFfFfD       ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 24 Failure - Slightly more complex strings w/ multiple finds');

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uA000\uB000\uFfFfD\u1977\u00AC\uABCDD\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\uABCD              '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              D\uC000\uC001\uC002\uABCD \uDBCA\uA000\uB000\uFfFfD \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000\u1997'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1977\uC000\uC001\uC002\uB000 D\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           D\uC000\uC001\uC002\uB000 \u1997\uA000\uB000\uFfFfD \uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000D             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uA000\uB000\uFfFfD \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uC000\uC001\uC002 \uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uA000\uB000\uFfFfD\uC000 \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uA000\uB000\uFfFfD \uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              \uC000\uC000\uC001\uC002D \uC000\uC000\uC001\uA000\uB000\uFfFfD\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '\uDBCA\u007F\uC000\uC001\uC002D\u007F \uA000\uC000\uA000\uB000\uFfFfD\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000D\uC000\uC001\uABCA\uC000             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           \uA000\u1977\uC000\uC001\uC002D\u007F D\uDBCA\uA000\uB000\uFfFfD \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000D\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
/*****************************************************************************/
	apEndTest();
}


mtca225();


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
