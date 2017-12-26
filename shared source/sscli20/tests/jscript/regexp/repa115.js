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


var iTestID = 53333;

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





function repa115() {
	
  @if(@_fast)
    var i, strTest, regExp;
  @end

apInitTest("repa115");
	m_scen = ('Test 13 numbers   /176|234/');

	apInitScenario(m_scen);
	regPat = /176|234/;
	regExp = new Array();

	objExp = 'a176';
	strTest = '234176';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a176';
	strTest = '7234176';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a51767';
	strTest = '723451767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a51767';
	strTest = '23451767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      a1767';
	strTest = '      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a       1767';
	strTest = '234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a       1767';
	strTest = '7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          a       1767';
	strTest = '          234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          7a       1767';
	strTest = '          7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 a1767';
	strTest = '23 2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23      a1767';
	strTest = '23      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 1767      ';
	strTest = '23234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	a5 1767      ';
	strTest = '23	2345 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          a   1767    ';
	strTest = '23          234   1767    ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '235          7a5  1767     ';
	strTest = '235          72345  1767     ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	objExp = 'a1767 23 4 23924272392'; 
	strTest = '2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a1767 23 4 23924272392'; 
	strTest = '              2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 4 7a01767  23924272392             '; 
	strTest = '23 4 723401767  23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a1767 23 4 239242723923'; 
	strTest = '              72341767 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a1767 23 4 239242723923 4            '; 
	strTest = '72341767 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5a 23 4 23924272392'; 
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 1767 23 4 239242723923 7            '; 
	strTest = '7234 1767 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 15 numbers   tests w/ multiples');

	objExp = 'a 1767';
	strTest = '234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     a 1767';
	strTest = '     234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767      ';
	strTest = '234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 1767      ';
	strTest = '7234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a 1767       ';
	strTest = '         234 1767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a7 51767       ';
	strTest = '         2347 51767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	objExp = 'a 1767 23924272392'; 
	strTest = '234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 1767 23924272392'; 
	strTest = '              234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767 23924272392             '; 
	strTest = '234 1767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5927a235 885717671957920 239242723925'; 
	strTest = '              5927234235 885717671957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '5927a235 885717671957920 239242723925              '; 
	strTest = '5927234235 885717671957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a5 61767 5239242723928'; 
	strTest = '              72345 61767 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1a7 71767 23924272392             '; 
	strTest = '12347 71767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           7a7 81767 239242723927             '; 
	strTest = '           72347 81767 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8a 31767 9234272392'; 
	strTest = '8234 31767 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a 31767 923924272392'; 
	strTest = '              8234 31767 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767 234272392             '; 
	strTest = '234 1767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a9 317672 234272392'; 
	strTest = '              82349 317672 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9a 01767 234272392             '; 
	strTest = '9234 01767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              2a7 223176723 22234272392'; 
	strTest = '              22347 223176723 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '60a70 1217678 9623472272392             '; 
	strTest = '6023470 1217678 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           11a70 761767 15752342723923050             '; 
	strTest = '           1123470 761767 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 17 numbers   /234|234/');

	apInitScenario(m_scen);
	regPat = /234|234/;
	regExp = new Array();

	objExp = 'a176';
	strTest = '234176';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a176';
	strTest = '7234176';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a51767';
	strTest = '723451767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a51767';
	strTest = '23451767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      a1767';
	strTest = '      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a       1767';
	strTest = '234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a       1767';
	strTest = '7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          a       1767';
	strTest = '          234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          7a       1767';
	strTest = '          7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 a1767';
	strTest = '23 2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23      a1767';
	strTest = '23      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 1767      ';
	strTest = '23234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	a5 1767      ';
	strTest = '23	2345 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          a   1767    ';
	strTest = '23          234   1767    ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '235          7a5  1767     ';
	strTest = '235          72345  1767     ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 18 Slightly more complex strings');

	objExp = 'a1767 23 4 23924272392'; 
	strTest = '2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a1767 23 4 23924272392'; 
	strTest = '              2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 4 7a01767  23924272392             '; 
	strTest = '23 4 723401767  23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a1767 23 4 239242723923'; 
	strTest = '              72341767 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a1767 23 4 239242723923 4            '; 
	strTest = '72341767 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5a 23 4 23924272392'; 
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 1767 23 4 239242723923 7            '; 
	strTest = '7234 1767 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 19 numbers   tests w/ multiples');

	objExp = 'a 1767';
	strTest = '234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     a 1767';
	strTest = '     234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767      ';
	strTest = '234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 1767      ';
	strTest = '7234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a 1767       ';
	strTest = '         234 1767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a7 51767       ';
	strTest = '         2347 51767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	objExp = 'a 1767 23924272392'; 
	strTest = '234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 1767 23924272392'; 
	strTest = '              234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767 23924272392             '; 
	strTest = '234 1767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5927a235 885717671957920 239242723925'; 
	strTest = '              5927234235 885717671957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '5927a235 885717671957920 239242723925              '; 
	strTest = '5927234235 885717671957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a5 61767 5239242723928'; 
	strTest = '              72345 61767 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1a7 71767 23924272392             '; 
	strTest = '12347 71767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           7a7 81767 239242723927             '; 
	strTest = '           72347 81767 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8a 31767 9234272392'; 
	strTest = '8234 31767 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a 31767 923924272392'; 
	strTest = '              8234 31767 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 1767 234272392             '; 
	strTest = '234 1767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a9 317672 234272392'; 
	strTest = '              82349 317672 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9a 01767 234272392             '; 
	strTest = '9234 01767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              2a7 223176723 22234272392'; 
	strTest = '              22347 223176723 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '60a70 1217678 9623472272392             '; 
	strTest = '6023470 1217678 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           11a70 761767 15752342723923050             '; 
	strTest = '           1123470 761767 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 21 Failure - Lowercase /234|234/');

	apInitScenario(m_scen);
	regPat = /23407|17657/;
	regExp = new Array();

	objExp = '234176';
	strTest = '234176';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7234176';
	strTest = '7234176';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '723451767';
	strTest = '723451767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23451767';
	strTest = '23451767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      2341767';
	strTest = '      2341767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '234       1767';
	strTest = '234       1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7234       1767';
	strTest = '7234       1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          234       1767';
	strTest = '          234       1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          7234       1767';
	strTest = '          7234       1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 2341767';
	strTest = '23 2341767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23      2341767';
	strTest = '23      2341767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23234 1767      ';
	strTest = '23234 1767      ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	2345 1767      ';
	strTest = '23	2345 1767      ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          234   1767    ';
	strTest = '23          234   1767    ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '235          72345  1767     ';
	strTest = '235          72345  1767     ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 22 Failure - Slightly more complex strings');

	objExp = '2341767 23 4 23924272392'; 
	strTest = '2341767 23 4 23924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              2341767 23 4 23924272392'; 
	strTest = '              2341767 23 4 23924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 4 723401767  23924272392             '; 
	strTest = '23 4 723401767  23924272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              72341767 23 4 239242723923'; 
	strTest = '              72341767 23 4 239242723923'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '72341767 23 4 239242723923 4            '; 
	strTest = '72341767 23 4 239242723923 4            '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5234 23 4 23924272392'; 
	strTest = '              5234 23 4 23924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7234 1767 23 4 239242723923 7            '; 
	strTest = '7234 1767 23 4 239242723923 7            '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 23 Failure - Lowercase tests w/ multiples');

	objExp = '234 1767';
	strTest = '234 1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     234 1767';
	strTest = '     234 1767';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '234 1767      ';
	strTest = '234 1767      ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7234 1767      ';
	strTest = '7234 1767      ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         234 1767       ';
	strTest = '         234 1767       ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         2347 51767       ';
	strTest = '         2347 51767       ';
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 24 Failure - Slightly more complex strings w/ multiple finds');

	objExp = '234 1767 23924272392'; 
	strTest = '234 1767 23924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              234 1767 23924272392'; 
	strTest = '              234 1767 23924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '234 1767 23924272392             '; 
	strTest = '234 1767 23924272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5927234235 885717671957920 239242723925'; 
	strTest = '              5927234235 885717671957920 239242723925'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '5927234235 885717671957920 239242723925              '; 
	strTest = '5927234235 885717671957920 239242723925              '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              72345 61767 5239242723928'; 
	strTest = '              72345 61767 5239242723928'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '12347 71767 23924272392             '; 
	strTest = '12347 71767 23924272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           72347 81767 239242723927             '; 
	strTest = '           72347 81767 239242723927             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8234 31767 9234272392'; 
	strTest = '8234 31767 9234272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8234 31767 923924272392'; 
	strTest = '              8234 31767 923924272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '234 1767 234272392             '; 
	strTest = '234 1767 234272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              82349 317672 234272392'; 
	strTest = '              82349 317672 234272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9234 01767 234272392             '; 
	strTest = '9234 01767 234272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              22347 223176723 22234272392'; 
	strTest = '              22347 223176723 22234272392'; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '6023470 1217678 9623472272392             '; 
	strTest = '6023470 1217678 9623472272392             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           1123470 761767 15752342723923050             '; 
	strTest = '           1123470 761767 15752342723923050             '; 
	verify(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
/*****************************************************************************/
	apEndTest();
}


repa115();


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
