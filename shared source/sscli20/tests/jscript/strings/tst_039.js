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


var iTestID = 53814;

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



function tst_039() {
	

apInitTest("tst_039");
	
	
	m_scen = ('Test 1 MiXed Case /[a-cA-C][a-cA-C][a-cA-C]/');

	sCat = "Precise Sets ";
	apInitScenario(m_scen);
	regPat = /[a-cA-C][a-cA-C][a-cA-C]/;
	objExp = true;
	regExp = new Array();
	
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'm nAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 2 Slightly more complex strings');

	strTest = 'AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC Ab c aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbC Ab c aBRaCaDaBRAb'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC Ab c aBRaCaDaBRAb c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC Ab c aBRaCaDaBRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 3 MiXed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC AbCe      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd eAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	strTest = 'AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              oranAbCAbo sienAbCathedral aBRaCaDaBRae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'oranAbCAbo sienAbCathedral aBRaCaDaBRae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fAbCg eaBRaCaDaBRas'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn DAbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn sAbC aBRaCaDaBRad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bAbC rAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bAbC raBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bAbCa AbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vAbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              mAbCd aAbAbCAb aaAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg maAbCds rzAbCxaadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfAbCwo mknoAbCadaBRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 5 MiXed Case /[a-cA-C][A-z][a-dA-D]/');

	sCat = "Less Precise Sets ";
	apInitScenario(m_scen);
	regPat = /[a-cA-C][A-z][a-dA-D]/;
	objExp = true;
	regExp = new Array();
	
	strTest = 'AbC';
	
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'm nAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 6 Slightly more complex strings');

	strTest = 'AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC Ab c aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbC Ab c aBRaCaDaBRAb'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC Ab c aBRaCaDaBRAb c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC Ab c aBRaCaDaBRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 7 MiXed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC AbCe      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd eAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	strTest = 'AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              orfnAbCAbo sienAbCathedral aBRaCaDaBRae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'orfnAbCAbo sienAbCathedral aBRaCaDaBRae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fAbCg eaBRaCaDaBRas'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn DAbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn sAbC aBRaCaDaBRad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bAbC rAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bAbC raBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bAbCa AbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vAbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              fAbCd aAbAbCAb aaAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg maAbCds rzAbCxaadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfAbCwo mknoAbCadaBRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 9 MiXed Case /[C]/');

	sCat = "One Character Sets ";
	apInitScenario(m_scen);
	regPat = /[C]/;
	objExp = true;
	regExp = new Array();
	
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 10 Slightly more complex strings');

	strTest = 'AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC Ab c aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbC Ab c aBRaCaDaBRAb'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC Ab c aBRaCaDaBRAb c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC Ab c aBRaCaDaBRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 11 MiXed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC AbCe      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd eAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 12 Slightly more complex strings w/ multiple finds');

	strTest = 'AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              oranAbCAbo sienAbCathedral aBRaCaDaBRae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'oranAbCAbo sienAbCathedral aBRaCaDaBRae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fAbCg eaBRaCaDaBRas'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn DAbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn sAbC aBRaCaDaBRad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bAbC rAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bAbC raBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bAbCa AbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vAbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              aAbCd aAbAbCAb aaAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg maAbCds rzAbCxaadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfAbCwo mknoAbCadaBRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 13 MiXed Case /[A-A]/');

	sCat = "one character ranges ";
	apInitScenario(m_scen);
	regPat = /[A-A]/;
	objExp = true;
	regExp = new Array();
	
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCe';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 14 Slightly more complex strings');

	strTest = 'AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC Ab c aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbC Ab c aBRaCaDaBRAb'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC Ab c aBRaCaDaBRAb c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC Ab c aBRaCaDaBRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 15 MiXed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC AbCe      ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd eAbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	strTest = 'AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC AbC aBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              oranAbCAbo sienAbCathedral aBRaCaDaBRae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'oranAbCAbo sienAbCathedral aBRaCaDaBRae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fAbCg eaBRaCaDaBRas'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn DAbC aBRaCaDaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn sAbC aBRaCaDaBRad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bAbC rAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bAbC raBRaCaDaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC AbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bAbCa AbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vAbC AbCadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              aAbCd aAbAbCAb aaAbCadaBRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg maAbCds rzAbCxaadaBRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfAbCwo mknoAbCadaBRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 17 Failure - MiXed Case /[q-q]/');

	sCat = "Both Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /[q-q]/;
	
	objExp = false;
	regExp = new Array();
	
	strTest = 'AbCmnz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCmnz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCemnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCemnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC   mnzd    ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe  mnzd     ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 18 Failure - Slightly more complex strings');

	strTest = 'AbCmnzd Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbCmnzd Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab c DAbCvmnzd  aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCmnzd Ab c aBRaCaDaBRAb'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbCmnzd Ab c aBRaCaDaBRAb c            '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC mnzd Ab c aBRaCaDaBRAb d            '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 19 Failure - MiXed Case tests w/ multiples');

	strTest = 'AbC mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC mnzd       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd emnzd       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 20 Failure - Slightly more complex strings w/ multiple finds');

	strTest = 'AbC mnzd aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC mnzd aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              oranAbCAbo sienmnzdthedral aBRaCaDaBRae'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'oranAbCAbo sienmnzdthedral aBRaCaDaBRae              '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fmnzd eaBRaCaDaBRas'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn dmnzd aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn smnzd aBRaCaDaBRad             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bmnzd rAbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bmnzd raBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd AbCadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bmnzda AbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vmnzd AbCadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              aAbCd aAbmnzdAb aaAbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg mamnzds rzAbCxaadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfmnzd mknoAbCadaBRAblol             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 21 Failure - MiXed Case /[^A-z 	]/');

	sCat = "Both Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /[^A-z 	]/;
	
	objExp = false;
	regExp = new Array();
	
	strTest = 'AbCmnz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCmnz';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	strTest = 'DAbCemnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbCemnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '          DAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab	AbCe mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab          AbC   mnzd    ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Abe          DAbCe  mnzd     ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 22 Failure - Slightly more complex strings');

	strTest = 'AbCmnzd Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbCmnzd Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'Ab c DAbCvmnzd  aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCmnzd Ab c aBRaCaDaBRAb'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbCmnzd Ab c aBRaCaDaBRAb c            '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              eAbC Ab c aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'nAbC mnzd Ab c aBRaCaDaBRAb d            '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	
	m_scen = ('Test 23 Failure - MiXed Case tests w/ multiples');

	strTest = 'AbC mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '     AbC mnzd';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'DAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbC mnzd       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '         AbCd emnzd       ';
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	m_scen = ('Test 24 Failure - Slightly more complex strings w/ multiple finds');

	strTest = 'AbC mnzd aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              AbC mnzd aBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              oranAbCAbo sienmnzdthedral aBRaCaDaBRae'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'oranAbCAbo sienmnzdthedral aBRaCaDaBRae              '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              DAbCe fmnzd eaBRaCaDaBRas'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'tAbCn dmnzd aBRaCaDaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           DAbCn smnzd aBRaCaDaBRad             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'sAbC bmnzd rAbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbC bmnzd raBRaCaDaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'AbC mnzd AbCadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              sAbCr bmnzda AbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'rAbC vmnzd AbCadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '              aAbCd aAbmnzdAb aaAbCadaBRa'; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = 'fgAbCdg mamnzds rzAbCxaadaBRa             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
	
	strTest = '           mtAbCdg dfmnzd mknoAbCadaBRAblol             '; 
	verify(sCat+strTest, objExp, regPat.test(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.test(strTest));
	@end
/*****************************************************************************/
	apEndTest();
}


tst_039();


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
