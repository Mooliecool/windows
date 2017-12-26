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


var iTestID = 53511;

	var sCat = '';
	var regPat;
	var objExp;
var strTest;
var regExp;
var m_scen = '';
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





function spta0188() {
	

apInitTest("spta0188");
	m_scen = ('Test 17 MiXed Case /\babc\b/ig');

	sCat = "/\babc\b/ig ";
	apInitScenario(m_scen);
	regPat = /\babc\b/ig;
	regExp = new Array();


	objExp = new Array();
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('      ');
	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ');
	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ');
	strTest = '          AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ');
	strTest = 'ab AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ');
	strTest = 'ab      AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('az ' , '       ');
	strTest = 'az AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , '       ');
	strTest = 'ab	AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 18 Slightly more complex strings');

	objExp = new Array(' ab C AbRaCAdAbRa');
	strTest = 'AbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab C AbRaCAdAbRa');
	strTest = '              AbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab C AbRaCAdAbRa             ');
	strTest = 'AbC ab C AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ab ' , ' C AbRaCAdAbRa');
	strTest = '              ab AbC C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab C AbRaaCAdAbRa             ');
	strTest = 'AbC ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab C AbRaCAdAbRa');
	strTest = '              AbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab C AbRaaCAdAbRa             ');
	strTest = 'AbC ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 19 MiXed Case tests w/ multiples');

	objExp = new Array(' ');
	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     ' , ' ');
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , '      ');
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' ' , '      ');
	strTest = ' AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' ' , ' AbRaCAdAbRa');
	strTest = 'AbC AbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbRaCAdAbRa');
	strTest = '              AbC AbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbRaCAdAbRa             ');
	strTest = 'AbC AbC AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbRaaCAdAbRa');
	strTest = '              AbC AbC AbRaaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbRaaCAdAbRa             ');
	strTest = 'AbC AbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbRaCAdAbRa');
	strTest = '              AbC AbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbRaaCAdAbRa             ');
	strTest = 'AbC AbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           ' , ' ' , ' AbRaaCAdAbRa             ');
	strTest = '           AbC AbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbCadAbRa');
	strTest = 'AbC AbC AbCadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbRaCAdAbRa');
	strTest = '              AbC AbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbCadAbRa             ');
	strTest = 'AbC AbC AbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbCadAbRa|');
	strTest = '              AbC AbC AbCadAbRa|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbCadAbRa             ');
	strTest = 'AbC AbC AbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' AbCadAbRa');
	strTest = '              AbC AbC AbCadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' AbCadAbRa             ');
	strTest = 'AbC AbC AbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           ' , ' ' , ' AbCadAbRa             ');
	strTest = '           AbC AbC AbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end



	m_scen = ('Test 21 MiXed Case /\babc\B/ig');

	sCat = "/\babc\B/ig ";
	apInitScenario(m_scen);
	regPat = /\babc\B/ig;
	regExp = new Array();


	objExp = new Array('d');
	strTest = 'AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('      ' , 'd');
	strTest = '      AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d       ');
	strTest = 'AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , 'd       ');
	strTest = '          AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ' , 'd');
	strTest = 'ab AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ' , 'd');
	strTest = 'ab      AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('az ' , 'd       ');
	strTest = 'az AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , 'd       ');
	strTest = 'ab	AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , 'd       ');
	strTest = 'ab          AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , 'd       ');
	strTest = 'ab          AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 22 Slightly more complex strings');

	objExp = new Array('d ab C AbRaCAdAbRa');
	strTest = 'AbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ab C AbRaCAdAbRa');
	strTest = '              AbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ab C AbRaCAdAbRa             ');
	strTest = 'AbCd ab C AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ab ' , 'd C AbRaCAdAbRa');
	strTest = '              ab AbCd C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ab C AbRaaCAdAbRa             ');
	strTest = 'AbCd ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ab C AbRaCAdAbRa');
	strTest = '              AbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ab C AbRaaCAdAbRa             ');
	strTest = 'AbCd ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 23 MiXed Case tests w/ multiples');

	objExp = new Array('d ' , 'd');
	strTest = 'AbCd AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     ' , 'd ' , 'd');
	strTest = '     AbCd AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd      ');
	strTest = 'AbCd AbCd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , 'd ' , 'd      ');
	strTest = ' AbCd AbCd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , 'd ' , 'd       ');
	strTest = '         AbCd AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , 'd ' , 'd       ');
	strTest = '         AbCd AbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 24 Slightly more complex strings w/ multiple finds');

	objExp = new Array('d ' , 'd AbRaCAdAbRa');
	strTest = 'AbCd AbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd AbRaCAdAbRa');
	strTest = '              AbCd AbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd AbRaCAdAbRa             ');
	strTest = 'AbCd AbCd AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd AbRaaCAdAbRa');
	strTest = '              AbCd AbCd AbRaaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd AbRaaCAdAbRa             ');
	strTest = 'AbCd AbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd AbRaCAdAbRa');
	strTest = '              AbCd AbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd AbRaaCAdAbRa             ');
	strTest = 'AbCd AbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           ' , 'd ' , 'd AbRaaCAdAbRa             ');
	strTest = '           AbCd AbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd ' , 'dadAbRa');
	strTest = 'AbCd AbCd AbCdadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd AbRaCAdAbRa');
	strTest = '              AbCd AbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd ' , 'dadAbRa             ');
	strTest = 'AbCd AbCd AbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd ' , 'dadAbRa|');
	strTest = '              AbCd AbCd AbCdadAbRa|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd ' , 'dadAbRa             ');
	strTest = 'AbCd AbCd AbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd ' , 'd ' , 'dadAbRa');
	strTest = '              AbCd AbCd AbCdadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d ' , 'd ' , 'dadAbRa             ');
	strTest = 'AbCd AbCd AbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           ' , 'd ' , 'd ' , 'dadAbRa             ');
	strTest = '           AbCd AbCd AbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 25 MiXed Case /\Babc\b/ig');

	sCat = "/\Babc\b/ig ";
	apInitScenario(m_scen);
	regPat = /\Babc\b/ig;
	regExp = new Array();


	objExp = new Array('D');
	strTest = 'DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('      D');
	strTest = '      DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , '       ');
	strTest = 'DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          D' , '       ');
	strTest = '          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab D');
	strTest = 'ab DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      D');
	strTest = 'ab      DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('az D' , '       ');
	strTest = 'az DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	D' , '       ');
	strTest = 'ab	DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          D' , '       ');
	strTest = 'ab          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          D' , '       ');
	strTest = 'ab          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 26 Slightly more complex strings');

	objExp = new Array('D' , ' ab C AbRaCAdAbRa');
	strTest = 'DAbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' ab C AbRaCAdAbRa');
	strTest = '              DAbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' ab C AbRaCAdAbRa             ');
	strTest = 'DAbC ab C AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ab D' , ' C AbRaCAdAbRa');
	strTest = '              ab DAbC C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' ab C AbRaaCAdAbRa             ');
	strTest = 'DAbC ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' ab C AbRaCAdAbRa');
	strTest = '              DAbC ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' ab C AbRaaCAdAbRa             ');
	strTest = 'DAbC ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 27 MiXed Case tests w/ multiples');

	objExp = new Array('D' , ' D');
	strTest = 'DAbC DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     D' , ' D');
	strTest = '     DAbC DAbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , '      ');
	strTest = 'DAbC DAbC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' D' , ' D' , '      ');
	strTest = ' DAbC DAbC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         D' , ' D' , '       ');
	strTest = '         DAbC DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         D' , ' D' , '       ');
	strTest = '         DAbC DAbC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 28 Slightly more complex strings w/ multiple finds');

	objExp = new Array('D' , ' D' , ' AbRaCAdAbRa');
	strTest = 'DAbC DAbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' AbRaCAdAbRa');
	strTest = '              DAbC DAbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' AbRaCAdAbRa             ');
	strTest = 'DAbC DAbC AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' AbRaaCAdAbRa');
	strTest = '              DAbC DAbC AbRaaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' AbRaaCAdAbRa             ');
	strTest = 'DAbC DAbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' AbRaCAdAbRa');
	strTest = '              DAbC DAbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' AbRaaCAdAbRa             ');
	strTest = 'DAbC DAbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , ' D' , ' AbRaaCAdAbRa             ');
	strTest = '           DAbC DAbC AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' DAbCadAbRa');
	strTest = 'DAbC DAbC DAbCadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' AbRaCAdAbRa');
	strTest = '              DAbC DAbC AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' DAbCadAbRa             ');
	strTest = 'DAbC DAbC DAbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' DAbCadAbRa|');
	strTest = '              DAbC DAbC DAbCadAbRa|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' DAbCadAbRa             ');
	strTest = 'DAbC DAbC DAbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , ' D' , ' DAbCadAbRa');
	strTest = '              DAbC DAbC DAbCadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' D' , ' DAbCadAbRa             ');
	strTest = 'DAbC DAbC DAbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , ' D' , ' DAbCadAbRa             ');
	strTest = '           DAbC DAbC DAbCadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 29 MiXed Case /\Babc\B/ig');

	sCat = "/\Babc\B/ig ";
	apInitScenario(m_scen);
	regPat = /\Babc\B/ig;
	regExp = new Array();


	objExp = new Array('D' , 'd');
	strTest = 'DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('      D' , 'd');
	strTest = '      DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd       ');
	strTest = 'DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          D' , 'd       ');
	strTest = '          DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab D' , 'd');
	strTest = 'ab DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      D' , 'd');
	strTest = 'ab      DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('az D' , 'd       ');
	strTest = 'az DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	D' , 'd       ');
	strTest = 'ab	DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          D' , 'd       ');
	strTest = 'ab          DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          D' , 'd       ');
	strTest = 'ab          DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 30 Slightly more complex strings');

	objExp = new Array('D' , 'd ab C AbRaCAdAbRa');
	strTest = 'DAbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd ab C AbRaCAdAbRa');
	strTest = '              DAbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd ab C AbRaCAdAbRa             ');
	strTest = 'DAbCd ab C AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ab D' , 'd C AbRaCAdAbRa');
	strTest = '              ab DAbCd C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd ab C AbRaaCAdAbRa             ');
	strTest = 'DAbCd ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd ab C AbRaCAdAbRa');
	strTest = '              DAbCd ab C AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd ab C AbRaaCAdAbRa             ');
	strTest = 'DAbCd ab C AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 31 MiXed Case tests w/ multiples');

	objExp = new Array('D' , 'd D' , 'd');
	strTest = 'DAbCd DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     D' , 'd D' , 'd');
	strTest = '     DAbCd DAbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd      ');
	strTest = 'DAbCd DAbCd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' D' , 'd D' , 'd      ');
	strTest = ' DAbCd DAbCd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         D' , 'd D' , 'd       ');
	strTest = '         DAbCd DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         D' , 'd D' , 'd       ');
	strTest = '         DAbCd DAbCd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 32 Slightly more complex strings w/ multiple finds');

	objExp = new Array('D' , 'd D' , 'd AbRaCAdAbRa');
	strTest = 'DAbCd DAbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd AbRaCAdAbRa');
	strTest = '              DAbCd DAbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd AbRaCAdAbRa             ');
	strTest = 'DAbCd DAbCd AbRaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd AbRaaCAdAbRa');
	strTest = '              DAbCd DAbCd AbRaaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd AbRaaCAdAbRa             ');
	strTest = 'DAbCd DAbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd AbRaCAdAbRa');
	strTest = '              DAbCd DAbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd AbRaaCAdAbRa             ');
	strTest = 'DAbCd DAbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , 'd D' , 'd AbRaaCAdAbRa             ');
	strTest = '           DAbCd DAbCd AbRaaCAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd D' , 'dadAbRa');
	strTest = 'DAbCd DAbCd DAbCdadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd AbRaCAdAbRa');
	strTest = '              DAbCd DAbCd AbRaCAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd D' , 'dadAbRa             ');
	strTest = 'DAbCd DAbCd DAbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd D' , 'dadAbRa|');
	strTest = '              DAbCd DAbCd DAbCdadAbRa|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd D' , 'dadAbRa             ');
	strTest = 'DAbCd DAbCd DAbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd D' , 'd D' , 'dadAbRa');
	strTest = '              DAbCd DAbCd DAbCdadAbRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd D' , 'd D' , 'dadAbRa             ');
	strTest = 'DAbCd DAbCd DAbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , 'd D' , 'd D' , 'dadAbRa             ');
	strTest = '           DAbCd DAbCd DAbCdadAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


/*****************************************************************************/
	apEndTest();
}


spta0188();


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
