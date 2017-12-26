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


var iTestID = 53401;

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



function rxp_01() {
	
  @if(@_fast)
    var i, strTest, regExp;
  @end

apInitTest("rxp_01");


	m_scen = ('Test 1 Lowercase /abc/');

	sCat = "/abc/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat.compile("abc","");
	objExp = new Array('abc');
	regExp = new Array();

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end









	m_scen = ('Test 2 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 3 Lowercase tests /abc/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end



	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end













	m_scen = ('Test 5 Lowercase /(abc)/');

	sCat = "/(abc)/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("(abc)","");
	objExp = new Array('abc', 'abc');
	regExp = new Array('abc');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end









	m_scen = ('Test 6 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 7 Lowercase tests /(abc)/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end













	m_scen = ('Test 9 Lowercase /(ab)(c)/');

	sCat = "/(ab)(c)/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("(ab)(c)","");
	objExp = new Array('abc','ab','c');
	regExp = new Array('ab','c');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 10 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 11 Lowercase tests /(ab)(c)/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end



	m_scen = ('Test 12 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end














	m_scen = ('Test 13 Lowercase /((ab)(c))/');

	sCat = "/((ab)(c))/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat.compile("((ab)(c))","");
	objExp = new Array('abc', 'abc','ab','c');
	regExp = new Array('abc','ab','c');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 14 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 15 Lowercase tests //((ab)(c))/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end















	m_scen = ('Test 17 Lowercase /(a)(bc)/');

	sCat = "/(a)(bc)/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("(a)(bc)","");
	objExp = new Array('abc', 'a','bc');
	regExp = new Array('a','bc');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 18 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 19 Lowercase tests /(a)(bc)/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end














	m_scen = ('Test 21 Lowercase /((a)(bc))/');

	sCat = "/((a)(bc))/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("((a)(bc))","");
	objExp = new Array('abc','abc','a','bc');
	regExp = new Array('abc','a','bc');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 22 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 23 Lowercase tests /((a)(bc))/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 24 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end















	m_scen = ('Test 25 Lowercase /(a)(b)(c)/');

	sCat = "/(a)(b)(c)/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat.compile("(a)(b)(c)","");
	objExp = new Array('abc','a','b','c');
	regExp = new Array('a', 'b', 'c');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 26 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 27 Lowercase tests /(a)(b)(c)/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 28 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end















	m_scen = ('Test 29 Lowercase /((a)(b)(c))/');

	sCat = "/((a)(b)(c))/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("((a)(b)(c))","");
	objExp = new Array('abc','abc','a','b','c');
	regExp = new Array('abc','a','b','c');

	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end










	m_scen = ('Test 30 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 31 Lowercase tests /((a)(b)(c))/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end




	m_scen = ('Test 32 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end














	m_scen = ('Test 33 Failure Lowercase /aBc/');

	sCat = "/aBc/ ";
	apInitScenario(m_scen);
	regPat = /a/ig;
	regPat = new RegExp("aBc","");
	objExp = null;
	//no new regExp due to spec of RegExp.$1-9
	//this set of tests smakes sure that RegExp.$ keep original values


	strTest = 'abc';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '      abc';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc       |';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end









	m_scen = ('Test 34 Failure-Slightly more complex strings');

	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc ab c abracadabra'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc ab c abracadabra             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc ab c abracadabra|'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end





	m_scen = ('Test 35 Failure Lowercase tests /aBc/ multiples');

	strTest = 'abc abc';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     abc abc';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      ';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc      |';
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end



	m_scen = ('Test 36 Failure - Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              abc abc abracadabra'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'abc abc abracadabra             '; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '|              abc abc abracadabra|'; 
	verify(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end












/*****************************************************************************/
	apEndTest();
}


rxp_01();


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
