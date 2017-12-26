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


var iTestID = 53355;

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



function rep_119() {
	
  @if(@_fast)
    var i, strTest, regExp;
  @end

apInitTest("rep_119");


	m_scen = ('Test 1 numbers   /[2-4][2-4][2-4]/');

	apInitScenario(m_scen);
	regPat = /[2-4][2-4][2-4]/;
	regExp = new Array();
	
	objExp = 'a';
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a';
	strTest = '7234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a5';
	strTest = '72345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a5';
	strTest = '2345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      a';
	strTest = '      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a       ';
	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a       ';
	strTest = '7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '          a       ';
	strTest = '          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '          7a       ';
	strTest = '          7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '23 a';
	strTest = '23 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '23      a';
	strTest = '23      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '1 7a       ';
	strTest = '1 7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	a5       ';
	strTest = '23	2345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          a       ';
	strTest = '23          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '235          7a5       ';
	strTest = '235          72345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 2 Slightly more complex strings');

	objExp = 'a 23 4 23924272392'; 
	strTest = '234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 23 4 23924272392'; 
	strTest = '              234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = 'a 23 4 23924272392             '; 
	strTest = '234 23 4 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end
	
	objExp = '              7a 23 4 239242723923'; 
	strTest = '              7234 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 23 4 239242723923 4            '; 
	strTest = '7234 23 4 239242723923 4            '; 
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
	
	objExp = '7a 23 4 239242723923 7            '; 
	strTest = '7234 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 3 numbers   tests w/ multiples');

	objExp = 'a 234';
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     a 234';
	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234      ';
	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 2345      ';
	strTest = '7234 2345      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a 234       ';
	strTest = '         234 234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a7 5234       ';
	strTest = '         2347 5234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	objExp = 'a 234 23924272392'; 
	strTest = '234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 234 23924272392'; 
	strTest = '              234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234 23924272392             '; 
	strTest = '234 234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5927a235 885723421957920 239242723925'; 
	strTest = '              5927234235 885723421957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '5927a235 885723421957920 239242723925              '; 
	strTest = '5927234235 885723421957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a5 62340 5239242723928'; 
	strTest = '              72345 62340 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1a7 7234 23924272392             '; 
	strTest = '12347 7234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           7a7 8234 239242723927             '; 
	strTest = '           72347 8234 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8a 3234 9234272392'; 
	strTest = '8234 3234 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a 3234 923924272392'; 
	strTest = '              8234 3234 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234 234272392             '; 
	strTest = '234 234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a9 32342 234272392'; 
	strTest = '              82349 32342 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9a 0234 234272392             '; 
	strTest = '9234 0234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              1a7 22323423 22234272392'; 
	strTest = '              12347 22323423 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '60a70 1223478 9623472272392             '; 
	strTest = '6023470 1223478 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           11a70 7623415 15752342723923050             '; 
	strTest = '           1123470 7623415 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 5 numbers   /[2-4][2-6][2-7]/');

	apInitScenario(m_scen);
	regPat = /[2-4][2-6][2-7]/;
	regExp = new Array();

	objExp = 'a';
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a';
	strTest = '7234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a5';
	strTest = '72345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a5';
	strTest = '2345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      a';
	strTest = '      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a       ';
	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a       ';
	strTest = '7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          a       ';
	strTest = '          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          7a       ';
	strTest = '          7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 a';
	strTest = '23 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23      a';
	strTest = '23      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1 7a       ';
	strTest = '1 7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	a5       ';
	strTest = '23	2345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          a       ';
	strTest = '23          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '239          7a5       ';
	strTest = '239          72345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 6 Slightly more complex strings');

	objExp = 'a 23 4 23924272392'; 
	strTest = '234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 23 4 23924272392'; 
	strTest = '              234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 23 4 23924272392             '; 
	strTest = '234 23 4 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a 23 4 239242723923'; 
	strTest = '              7234 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 23 4 239242723923 4            '; 
	strTest = '7234 23 4 239242723923 4            '; 
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

	objExp = '7a 23 4 239242723923 7            '; 
	strTest = '7234 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 7 numbers   tests w/ multiples');

	objExp = 'a 234';
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     a 234';
	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234      ';
	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a 2345      ';
	strTest = '7234 2345      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a 234       ';
	strTest = '         234 234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a7 5234       ';
	strTest = '         2347 5234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	objExp = 'a 234 23924272392'; 
	strTest = '234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a 234 23924272392'; 
	strTest = '              234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234 23924272392             '; 
	strTest = '234 234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5967a235 885723421957920 239242723925'; 
	strTest = '              5967234235 885723421957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '5967a235 885723421957920 239242723925              '; 
	strTest = '5967234235 885723421957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a5 62340 5239242723928'; 
	strTest = '              72345 62340 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1a7 7234 23924272392             '; 
	strTest = '12347 7234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           7a7 8234 239242723927             '; 
	strTest = '           72347 8234 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8a 3234 9234272392'; 
	strTest = '8234 3234 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a 3234 923924272392'; 
	strTest = '              8234 3234 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a 234 234272392             '; 
	strTest = '234 234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a9 32342 234272392'; 
	strTest = '              82349 32342 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9a 0234 234272392             '; 
	strTest = '9234 0234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              6a7 22323423 22234272392'; 
	strTest = '              62347 22323423 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '60a70 1223478 9623472272392             '; 
	strTest = '6023470 1223478 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           11a70 7623415 15752342723923050             '; 
	strTest = '           1123470 7623415 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 9 numbers   /[4]/');

	apInitScenario(m_scen);
	regPat = /[4]/;
	regExp = new Array();

	objExp = '23a';
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '723a';
	strTest = '7234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '723a5';
	strTest = '72345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a5';
	strTest = '2345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      23a';
	strTest = '      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a       ';
	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '723a       ';
	strTest = '7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          23a       ';
	strTest = '          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          723a       ';
	strTest = '          7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23 23a';
	strTest = '23 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23      23a';
	strTest = '23      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '2323a       ';
	strTest = '23234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23	23a5       ';
	strTest = '23	2345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23          23a       ';
	strTest = '23          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '235          723a5       ';
	strTest = '235          72345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 10 Slightly more complex strings');

	objExp = '23a 23 4 23924272392'; 
	strTest = '234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              23a 23 4 23924272392'; 
	strTest = '              234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 23 4 23924272392             '; 
	strTest = '234 23 4 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              723a 23 4 239242723923'; 
	strTest = '              7234 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '723a 23 4 239242723923 4            '; 
	strTest = '7234 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              523a 23 4 23924272392'; 
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '723a 23 4 239242723923 7            '; 
	strTest = '7234 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 11 numbers   tests w/ multiples');

	objExp = '23a 234';
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     23a 234';
	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 234      ';
	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '723a 2345      ';
	strTest = '7234 2345      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         23a 234       ';
	strTest = '         234 234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         23a7 5234       ';
	strTest = '         2347 5234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 12 Slightly more complex strings w/ multiple finds');

	objExp = '23a 234 23924272392'; 
	strTest = '234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              23a 234 23924272392'; 
	strTest = '              234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 234 23924272392             '; 
	strTest = '234 234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              592723a235 885723421957920 239242723925'; 
	strTest = '              5927234235 885723421957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '592723a235 885723421957920 239242723925              '; 
	strTest = '5927234235 885723421957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              723a5 62340 5239242723928'; 
	strTest = '              72345 62340 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '123a7 7234 23924272392             '; 
	strTest = '12347 7234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           723a7 8234 239242723927             '; 
	strTest = '           72347 8234 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '823a 3234 9234272392'; 
	strTest = '8234 3234 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              823a 3234 923924272392'; 
	strTest = '              8234 3234 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '23a 234 234272392             '; 
	strTest = '234 234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              823a9 32342 234272392'; 
	strTest = '              82349 32342 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '923a 0234 234272392             '; 
	strTest = '9234 0234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              223a7 22323423 22234272392'; 
	strTest = '              22347 22323423 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '6023a70 1223478 9623472272392             '; 
	strTest = '6023470 1223478 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           1123a70 7623415 15752342723923050             '; 
	strTest = '           1123470 7623415 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 13 numbers   /[2-2]/');

	apInitScenario(m_scen);
	regPat = /[2-2]/;
	regExp = new Array();

	objExp = 'a34';
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a34';
	strTest = '7234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	objExp = '7a345';
	strTest = '72345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a345';
	strTest = '2345';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '      a34';
	strTest = '      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a34       ';
	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a34       ';
	strTest = '7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          a34       ';
	strTest = '          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '          7a34       ';
	strTest = '          7234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a3 234';
	strTest = '23 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a3      234';
	strTest = '23      234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a3234       ';
	strTest = '23234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a3	2345       ';
	strTest = '23	2345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a3          234       ';
	strTest = '23          234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a35          72345       ';
	strTest = '235          72345       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	objExp = 'a34 23 4 23924272392'; 
	strTest = '234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a34 23 4 23924272392'; 
	strTest = '              234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a34 23 4 23924272392             '; 
	strTest = '234 23 4 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a34 23 4 239242723923'; 
	strTest = '              7234 23 4 239242723923'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a34 23 4 239242723923 4            '; 
	strTest = '7234 23 4 239242723923 4            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              5a34 23 4 23924272392'; 
	strTest = '              5234 23 4 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a34 23 4 239242723923 7            '; 
	strTest = '7234 23 4 239242723923 7            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


	m_scen = ('Test 15 numbers   tests w/ multiples');

	objExp = 'a34 234';
	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '     a34 234';
	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a34 234      ';
	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '7a34 2345      ';
	strTest = '7234 2345      ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a34 234       ';
	strTest = '         234 234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '         a347 5234       ';
	strTest = '         2347 5234       ';
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	objExp = 'a34 234 23924272392'; 
	strTest = '234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a34 234 23924272392'; 
	strTest = '              234 234 23924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a34 234 23924272392             '; 
	strTest = '234 234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              59a7234235 885723421957920 239242723925'; 
	strTest = '              5927234235 885723421957920 239242723925'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '59a7234235 885723421957920 239242723925              '; 
	strTest = '5927234235 885723421957920 239242723925              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              7a345 62340 5239242723928'; 
	strTest = '              72345 62340 5239242723928'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '1a347 7234 23924272392             '; 
	strTest = '12347 7234 23924272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           7a347 8234 239242723927             '; 
	strTest = '           72347 8234 239242723927             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '8a34 3234 9234272392'; 
	strTest = '8234 3234 9234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a34 3234 923924272392'; 
	strTest = '              8234 3234 923924272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = 'a34 234 234272392             '; 
	strTest = '234 234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              8a349 32342 234272392'; 
	strTest = '              82349 32342 234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '9a34 0234 234272392             '; 
	strTest = '9234 0234 234272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '              a2347 22323423 22234272392'; 
	strTest = '              22347 22323423 22234272392'; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '60a3470 1223478 9623472272392             '; 
	strTest = '6023470 1223478 9623472272392             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end

	objExp = '           11a3470 7623415 15752342723923050             '; 
	strTest = '           1123470 7623415 15752342723923050             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.replace(regPat, "a"));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.replace(regPat, "a"));
	@end


/*****************************************************************************/
	apEndTest();
}


rep_119();


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
