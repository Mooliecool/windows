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


var iTestID = 53543;

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



function spt_116() {
	

apInitTest("spt_116");


	m_scen = ('Test 1 numbers   /^234/');

	apInitScenario(m_scen);
	regPat = /^234/;
	regExp = new Array();
	
	objExp = new Array();
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7');
	strTest = '2347';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7                         ');
	strTest = '2347                         ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('       ');
	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
	

	m_scen = ('Test 2 Slightly more complex strings');

	objExp = new Array(' 23 4 23924272392');
	strTest = '234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	
	objExp = new Array(' 23 4 23924272392             ');
	strTest = '234 23 4 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
	

	objExp = new Array('7 23 4 239242723923 4            ');
	strTest = '2347 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
	
	
	m_scen = ('Test 3 numbers   tests w/ multiples');

	objExp = new Array(' 234');
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' 234      ');
	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7 2345      ');
	strTest = '2347 2345      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' 234 23924272392');
	strTest = '234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' 234 23924272392             ');
	strTest = '234 234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('235 885723421957920 239242723925              ');
	strTest = '234235 885723421957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7 7234 23924272392             ');
	strTest = '2347 7234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' 234 234272392             ');
	strTest = '234 234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7 22323423 22234272392');
	strTest = '2347 22323423 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 5 numbers   /234$/');

	apInitScenario(m_scen);
	regPat = /234$/;
	regExp = new Array();

	objExp = new Array();
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7');
	strTest = '7234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ');
	strTest = '      234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23 ');
	strTest = '23 234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23      ');
	strTest = '23      234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 6 Slightly more complex strings');

	objExp = new Array('274 23 4 2392427239');
	strTest = '274 23 4 2392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              274 23 4 2392427239');
	strTest = '              274 23 4 2392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 7 numbers   tests w/ multiples');

	objExp = new Array('234 ');
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     234 ');
	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('  23             4       234                  ');
	strTest = '  23             4       234                  234';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	objExp = new Array('23 234 27 285 99 2392427239');
	strTest = '23 234 27 285 99 2392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('			234 27 285 99 2392427239');
	strTest = '			234 27 285 99 2392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              5927234235 885723421957920 2392427239');
	strTest = '              5927234235 885723421957920 2392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              72345 62340 52392427239');
	strTest = '              72345 62340 52392427239234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('22347 22323423 22234272392222222222222222');
	strTest = '22347 22323423 22234272392222222222222222234'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 9 Failure - Lowercase /^234 /');

	apInitScenario(m_scen);
	regPat = /^234/;

	objExp = new Array(' 234176');
	strTest = ' 234176';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7234176');
	strTest = '7234176';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('723451767');
	strTest = '723451767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('                                       23451767');
	strTest = '                                       23451767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      2341767');
	strTest = '      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('          234       1767');
	strTest = '          234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          7234       1767');
	strTest = '          7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23 2341767');
	strTest = '23 2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23      2341767');
	strTest = '23      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23234 1767      ');
	strTest = '23234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23	2345 1767      ');
	strTest = '23	2345 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23          234   1767    ');
	strTest = '23          234   1767    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('235          72345  1767     ');
	strTest = '235          72345  1767     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 10 Failure - Slightly more complex strings');


	objExp = new Array('              2341767 23 4 23924272392');
	strTest = '              2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23 4 723401767  23924272392             ');
	strTest = '23 4 723401767  23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              72341767 23 4 239242723923');
	strTest = '              72341767 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('72341767 23 4 239242723923 4            ');
	strTest = '72341767 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              5234 23 4 23924272392');
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7234 1767 23 4 239242723923 7            ');
	strTest = '7234 1767 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 11 Failure - Lowercase tests w/ multiples');


	objExp = new Array('     234 1767');
	strTest = '     234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7234 1767      ');
	strTest = '7234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         234 1767       ');
	strTest = '         234 1767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         2347 51767       ');
	strTest = '         2347 51767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 12 Failure - Slightly more complex strings w/ multiple finds');


	objExp = new Array('              234 1767 23924272392');
	strTest = '              234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('              5927234235 885717671957920 239242723925');
	strTest = '              5927234235 885717671957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('5927234235 885717671957920 239242723925              ');
	strTest = '5927234235 885717671957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              72345 61767 5239242723928');
	strTest = '              72345 61767 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('12347 71767 23924272392             ');
	strTest = '12347 71767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           72347 81767 239242723927             ');
	strTest = '           72347 81767 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('8234 31767 9234272392');
	strTest = '8234 31767 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              8234 31767 923924272392');
	strTest = '              8234 31767 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('              82349 317672 234272392');
	strTest = '              82349 317672 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('9234 01767 234272392             ');
	strTest = '9234 01767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              22347 223176723 22234272392');
	strTest = '              22347 223176723 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('6023470 1217678 9623472272392             ');
	strTest = '6023470 1217678 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           1123470 761767 15752342723923050             ');
	strTest = '           1123470 761767 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 13 Failure - Lowercase /234$/');

	apInitScenario(m_scen);
	regPat = /234$/;
	
	objExp = new Array('234176');
	strTest = '234176';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('7234176');
	strTest = '7234176';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('723451767');
	strTest = '723451767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23451767');
	strTest = '23451767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      2341767');
	strTest = '      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('234       1767');
	strTest = '234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7234       1767');
	strTest = '7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          234       1767');
	strTest = '          234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          7234       1767');
	strTest = '          7234       1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23 2341767');
	strTest = '23 2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23      2341767');
	strTest = '23      2341767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23234 1767      ');
	strTest = '23234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23	2345 1767      ');
	strTest = '23	2345 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23          234   1767    ');
	strTest = '23          234   1767    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('235          72345  1767     ');
	strTest = '235          72345  1767     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 14 Failure - Slightly more complex strings');

	objExp = new Array('2341767 23 4 23924272392');
	strTest = '2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              2341767 23 4 23924272392');
	strTest = '              2341767 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('23 4 723401767  23924272392             ');
	strTest = '23 4 723401767  23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              72341767 23 4 239242723923');
	strTest = '              72341767 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('72341767 23 4 239242723923 4            ');
	strTest = '72341767 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              5234 23 4 23924272392');
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7234 1767 23 4 239242723923 7            ');
	strTest = '7234 1767 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 15 Failure - Lowercase tests w/ multiples');

	objExp = new Array('234 1767');
	strTest = '234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     234 1767');
	strTest = '     234 1767';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('234 1767      ');
	strTest = '234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('7234 1767      ');
	strTest = '7234 1767      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         234 1767       ');
	strTest = '         234 1767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         2347 51767       ');
	strTest = '         2347 51767       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 16 Failure - Slightly more complex strings w/ multiple finds');

	objExp = new Array('234 1767 23924272392');
	strTest = '234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              234 1767 23924272392');
	strTest = '              234 1767 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('234 1767 23924272392             ');
	strTest = '234 1767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              5927234235 885717671957920 239242723925');
	strTest = '              5927234235 885717671957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('5927234235 885717671957920 239242723925              ');
	strTest = '5927234235 885717671957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              72345 61767 5239242723928');
	strTest = '              72345 61767 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('12347 71767 23924272392             ');
	strTest = '12347 71767 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           72347 81767 239242723927             ');
	strTest = '           72347 81767 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('8234 31767 9234272392');
	strTest = '8234 31767 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              8234 31767 923924272392');
	strTest = '              8234 31767 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('234 1767 234272392             ');
	strTest = '234 1767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              82349 317672 234272392');
	strTest = '              82349 317672 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('9234 01767 234272392             ');
	strTest = '9234 01767 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              22347 223176723 22234272392');
	strTest = '              22347 223176723 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('6023470 1217678 9623472272392             ');
	strTest = '6023470 1217678 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           1123470 761767 15752342723923050             ');
	strTest = '           1123470 761767 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

/*****************************************************************************/
	apEndTest();
}


spt_116();


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
