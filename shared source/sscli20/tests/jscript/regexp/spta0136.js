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


var iTestID = 53509;

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





function spta0136() {
	

apInitTest("spta0136");
	m_scen = ('Test 17 MiXeD Case /(a)(bc)/gi');

	sCat = "/(a)(bc)/gi ";
	apInitScenario(m_scen);
	regPat = /(a)(bc)/gi;
	regExp = new Array('a','Bc');

	objExp = new Array();
	strTest = 'aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ');
	strTest = '      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ');
	strTest = 'aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       |');
	strTest = 'aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ');
	strTest = '          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       |');
	strTest = '          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ');
	strTest = 'ab aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ');
	strTest = 'ab      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab' , '       ');
	strTest = 'abaBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , '       |');
	strTest = 'ab	aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       |');
	strTest = 'ab          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 18 Slightly more complex strings');

	objExp = new Array(' ab c abracadabra');
	strTest = 'aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab c abracadabra');
	strTest = '              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             ');
	strTest = 'aBc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra|');
	strTest = '|              aBc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ab c abracadabra             |');
	strTest = '|aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra');
	strTest = '|              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             |');
	strTest = 'aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 19 MiXeD Case tests /(a)(bc)/gi multiples');

	regExp = new Array('a','bc');
	objExp = new Array(' ');
	strTest = 'aBc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a','Bc');
	objExp = new Array('     ' , ' ');
	strTest = '     abc aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a','bc');
	objExp = new Array(' ' , '      ');
	strTest = 'aBc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , '      |');
	strTest = 'aBc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         aBc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       |');
	strTest = '         aBc abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' ' , ' abracadabra');
	strTest = 'aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             ');
	strTest = 'aBc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra|');
	strTest = '|              aBc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' abracadabra             |');
	strTest = '|aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra');
	strTest = '|              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             |');
	strTest = 'aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' abracadabra             |');
	strTest = '|           aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a','Bc');
	objExp = new Array(' ' , ' ' , 'adabra');
	strTest = 'aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a','bc');
	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a','Bc');
	objExp = new Array(' ' , ' ' , 'adabra             ');
	strTest = 'aBc abc aBcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra|');
	strTest = '|              aBc abc aBcadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' ' , 'adabra             |');
	strTest = '|aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra');
	strTest = '|              aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' ' , 'adabra             |');
	strTest = 'aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' ' , 'adabra             |');
	strTest = '|           aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 21 MiXeD Case /((a)(bc))/gi');

	sCat = "/((a)(bc))/gi ";
	apInitScenario(m_scen);
	regPat = /((a)(bc))/gi;
	regExp = new Array('aBc','a','Bc');

	objExp = new Array();
	strTest = 'aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ');
	strTest = '      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ');
	strTest = 'aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       |');
	strTest = 'aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ');
	strTest = '          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       |');
	strTest = '          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ');
	strTest = 'ab aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ');
	strTest = 'ab      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab' , '       ');
	strTest = 'abaBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , '       |');
	strTest = 'ab	aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       |');
	strTest = 'ab          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 22 Slightly more complex strings');

	objExp = new Array(' ab c abracadabra');
	strTest = 'aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab c abracadabra');
	strTest = '              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             ');
	strTest = 'aBc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra|');
	strTest = '|              aBc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ab c abracadabra             |');
	strTest = '|aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra');
	strTest = '|              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             |');
	strTest = 'aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 23 MiXeD Case tests /((a)(bc))/gi multiples');

	regExp = new Array('abc','a','bc');
	objExp = new Array(' ');
	strTest = 'aBc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','Bc');
	objExp = new Array('     ' , ' ');
	strTest = '     abc aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('abc','a','bc');
	objExp = new Array(' ' , '      ');
	strTest = 'aBc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , '      |');
	strTest = 'aBc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         aBc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       |');
	strTest = '         aBc abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 24 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' ' , ' abracadabra');
	strTest = 'aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             ');
	strTest = 'aBc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra|');
	strTest = '|              aBc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' abracadabra             |');
	strTest = '|aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra');
	strTest = '|              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             |');
	strTest = 'aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' abracadabra             |');
	strTest = '|           aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','Bc');
	objExp = new Array(' ' , ' ' , 'adabra');
	strTest = 'aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('abc','a','bc');
	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','Bc');
	objExp = new Array(' ' , ' ' , 'adabra             ');
	strTest = 'aBc abc aBcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra|');
	strTest = '|              aBc abc aBcadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' ' , 'adabra             |');
	strTest = '|aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra');
	strTest = '|              aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' ' , 'adabra             |');
	strTest = 'aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' ' , 'adabra             |');
	strTest = '|           aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 25 MiXeD Case /(a)(b)(c)/gi');

	sCat = "/(a)(b)(c)/gi ";
	apInitScenario(m_scen);
	regPat = /(a)(b)(c)/gi;
	regExp = new Array('a', 'B', 'c');

	objExp = new Array();
	strTest = 'aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ');
	strTest = '      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ');
	strTest = 'aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       |');
	strTest = 'aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ');
	strTest = '          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       |');
	strTest = '          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ');
	strTest = 'ab aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ');
	strTest = 'ab      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab' , '       ');
	strTest = 'abaBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , '       |');
	strTest = 'ab	aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       |');
	strTest = 'ab          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 26 Slightly more complex strings');

	objExp = new Array(' ab c abracadabra');
	strTest = 'aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab c abracadabra');
	strTest = '              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             ');
	strTest = 'aBc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra|');
	strTest = '|              aBc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ab c abracadabra             |');
	strTest = '|aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra');
	strTest = '|              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             |');
	strTest = 'aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 27 MiXeD Case tests /(a)(b)(c)/gi multiples');

	regExp = new Array('a', 'b', 'c');
	objExp = new Array(' ');
	strTest = 'aBc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a', 'B', 'c');
	objExp = new Array('     ' , ' ');
	strTest = '     abc aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a', 'b', 'c');
	objExp = new Array(' ' , '      ');
	strTest = 'aBc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , '      |');
	strTest = 'aBc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         aBc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       |');
	strTest = '         aBc abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 28 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' ' , ' abracadabra');
	strTest = 'aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             ');
	strTest = 'aBc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra|');
	strTest = '|              aBc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' abracadabra             |');
	strTest = '|aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra');
	strTest = '|              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             |');
	strTest = 'aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' abracadabra             |');
	strTest = '|           aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a', 'B', 'c');
	objExp = new Array(' ' , ' ' , 'adabra');
	strTest = 'aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a', 'b', 'c');
	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('a', 'B', 'c');
	objExp = new Array(' ' , ' ' , 'adabra             ');
	strTest = 'aBc abc aBcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra|');
	strTest = '|              aBc abc aBcadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' ' , 'adabra             |');
	strTest = '|aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra');
	strTest = '|              aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' ' , 'adabra             |');
	strTest = 'aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' ' , 'adabra             |');
	strTest = '|           aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 29 MiXeD Case /((a)(b)(c))/gi');

	sCat = "/((a)(b)(c))/gi ";
	apInitScenario(m_scen);
	regPat = /((a)(b)(c))/gi;
	regExp = new Array('aBc','a','B','c');

	objExp = new Array();
	strTest = 'aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ');
	strTest = '      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ');
	strTest = 'aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       |');
	strTest = 'aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ');
	strTest = '          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       |');
	strTest = '          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab ');
	strTest = 'ab aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      ');
	strTest = 'ab      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab' , '       ');
	strTest = 'abaBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , '       |');
	strTest = 'ab	aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       ');
	strTest = 'ab          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          ' , '       |');
	strTest = 'ab          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 30 Slightly more complex strings');

	objExp = new Array(' ab c abracadabra');
	strTest = 'aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ab c abracadabra');
	strTest = '              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             ');
	strTest = 'aBc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra|');
	strTest = '|              aBc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ab c abracadabra             |');
	strTest = '|aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ab c abracadabra');
	strTest = '|              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ab c abracadabra             |');
	strTest = 'aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 31 MiXeD Case tests /((a)(b)(c))/gi multiples');

	regExp = new Array('abc','a','b','c');
	objExp = new Array(' ');
	strTest = 'aBc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','B','c');
	objExp = new Array('     ' , ' ');
	strTest = '     abc aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('abc','a','b','c');
	objExp = new Array(' ' , '      ');
	strTest = 'aBc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , '      |');
	strTest = 'aBc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       ');
	strTest = '         aBc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , '       |');
	strTest = '         aBc abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 32 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' ' , ' abracadabra');
	strTest = 'aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             ');
	strTest = 'aBc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra|');
	strTest = '|              aBc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' abracadabra             |');
	strTest = '|aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' abracadabra');
	strTest = '|              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' abracadabra             |');
	strTest = 'aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' abracadabra             |');
	strTest = '|           aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','B','c');
	objExp = new Array(' ' , ' ' , 'adabra');
	strTest = 'aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('abc','a','b','c');
	objExp = new Array('              ' , ' ' , ' abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc','a','B','c');
	objExp = new Array(' ' , ' ' , 'adabra             ');
	strTest = 'aBc abc aBcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra|');
	strTest = '|              aBc abc aBcadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|' , ' ' , ' ' , 'adabra             |');
	strTest = '|aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              ' , ' ' , ' ' , 'adabra');
	strTest = '|              aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , ' ' , 'adabra             |');
	strTest = 'aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           ' , ' ' , ' ' , 'adabra             |');
	strTest = '|           aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 33 Failure MiXeD Case /abcd/gi');

	sCat = "/abcd/gi ";
	apInitScenario(m_scen);
	regPat = /abcd/gi;
	//no new regExp due to spec of RegExp.$1-9

	objExp = new Array('aBc');
	strTest = 'aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      aBc');
	strTest = '      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc       ');
	strTest = 'aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc       |');
	strTest = 'aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          aBc       ');
	strTest = '          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          aBc       |');
	strTest = '          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab aBc');
	strTest = 'ab aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab      aBc');
	strTest = 'ab      aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('abaBc       ');
	strTest = 'abaBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	aBc       |');
	strTest = 'ab	aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          aBc       ');
	strTest = 'ab          aBc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab          aBc       |');
	strTest = 'ab          aBc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 34 Failure-Slightly more complex strings');

	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              aBc ab c abracadabra');
	strTest = '              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc ab c abracadabra             ');
	strTest = 'aBc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc ab c abracadabra|');
	strTest = '|              aBc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|aBc ab c abracadabra             |');
	strTest = '|aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc ab c abracadabra');
	strTest = '|              aBc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc ab c abracadabra             |');
	strTest = 'aBc ab c abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 35 Failure MiXeD Case tests /abcd/gi multiples');

	objExp = new Array('aBc abc');
	strTest = 'aBc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     abc aBc');
	strTest = '     abc aBc';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc      ');
	strTest = 'aBc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc      |');
	strTest = 'aBc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         aBc abc       ');
	strTest = '         aBc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         aBc abc       |');
	strTest = '         aBc abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 36 Failure - Slightly more complex strings w/ multiple finds');

	objExp = new Array('aBc abc abracadabra');
	strTest = 'aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              aBc abc abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc abracadabra             ');
	strTest = 'aBc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc abc abracadabra|');
	strTest = '|              aBc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|aBc abc abracadabra             |');
	strTest = '|aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc abc abracadabra');
	strTest = '|              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc abracadabra             |');
	strTest = 'aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           aBc abc abracadabra             |');
	strTest = '|           aBc abc abracadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc aBcadabra');
	strTest = 'aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              aBc abc abracadabra');
	strTest = '              aBc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc aBcadabra             ');
	strTest = 'aBc abc aBcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc abc aBcadabra|');
	strTest = '|              aBc abc aBcadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|aBc abc aBcadabra             |');
	strTest = '|aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|              aBc abc aBcadabra');
	strTest = '|              aBc abc aBcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc abc aBcadabra             |');
	strTest = 'aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('|           aBc abc aBcadabra             |');
	strTest = '|           aBc abc aBcadabra             |'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
/*****************************************************************************/
	apEndTest();
}


spta0136();


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
