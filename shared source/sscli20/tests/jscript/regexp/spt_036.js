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


var iTestID = 53524;

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



function spt_036() {
	

apInitTest("spt_036");


	m_scen = ('Test 1 Lowercase /^AbC/');

	apInitScenario(m_scen);
	regPat = /^AbC/;
	regExp = new Array();
	
	objExp = new Array();
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('d');
	strTest = 'AbCd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('d                         ');
	strTest = 'AbCd                         ';
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
	

	m_scen = ('Test 2 Slightly more complex strings');

	objExp = new Array(' Ab C AbRACAdAbRA');
	strTest = 'AbC Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	
	objExp = new Array(' Ab C AbRACAdAbRA             ');
	strTest = 'AbC Ab C AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
	

	objExp = new Array('d Ab C AbRACAdAbRAb C            ');
	strTest = 'AbCd Ab C AbRACAdAbRAb C            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
	
	
	m_scen = ('Test 3 Lowercase tests w/ multiples');

	objExp = new Array(' AbC');
	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' AbC      ');
	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d AbCe      ');
	strTest = 'AbCd AbCe      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' AbC AbRACAdAbRA');
	strTest = 'AbC AbC AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' AbC AbRACAdAbRA             ');
	strTest = 'AbC AbC AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Abo sienAbCAthedRAl AbRACAdAbRAe              ');
	strTest = 'AbCAbo sienAbCAthedRAl AbRACAdAbRAe              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('n dAbC AbRACAdAbRA             ');
	strTest = 'AbCn dAbC AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' AbC AbCAdAbRA             ');
	strTest = 'AbC AbC AbCAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('d AAbAbCAb AAAbCAdAbRA');
	strTest = 'AbCd AAbAbCAb AAAbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 5 Lowercase /AbC$/');

	apInitScenario(m_scen);
	regPat = /AbC$/;
	regExp = new Array();

	objExp = new Array();
	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('d');
	strTest = 'dAbC';
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

	objExp = new Array('Ab ');
	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      ');
	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 6 Slightly more complex strings');

	objExp = new Array('AnC Ab C AbRACAdAbR');
	strTest = 'AnC Ab C AbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AnC Ab C AbRACAdAbR');
	strTest = '              AnC Ab C AbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 7 Lowercase tests w/ multiples');

	objExp = new Array('AbC ');
	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     AbC ');
	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('  Ab             C       AbC                  ');
	strTest = '  Ab             C       AbC                  AbC';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	objExp = new Array('Ab AbC Ad Ase hh AbRACAdAbR');
	strTest = 'Ab AbC Ad Ase hh AbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('			AbC Ad Ase hh AbRACAdAbR');
	strTest = '			AbC Ad Ase hh AbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              oRAnAbCAbo sienAbCAthedRAl AbRACAdAbR');
	strTest = '              oRAnAbCAbo sienAbCAthedRAl AbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              dAbCe fAbCg eAbRACAdAbR');
	strTest = '              dAbCe fAbCg eAbRACAdAbRAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AAbCd AAbAbCAb AAAbCAdAbRAAAAAAAAAAAAAAAA');
	strTest = 'AAbCd AAbAbCAb AAAbCAdAbRAAAAAAAAAAAAAAAAAbC'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 9 Failure - Lowercase /^AbC /');

	apInitScenario(m_scen);
	regPat = /^AbC/;

	objExp = new Array(' AbCmnz');
	strTest = ' AbCmnz';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('dAbCmnz');
	strTest = 'dAbCmnz';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('dAbCemnzd');
	strTest = 'dAbCemnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('                                       AbCemnzd');
	strTest = '                                       AbCemnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      AbCmnzd');
	strTest = '      AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('          AbC       mnzd');
	strTest = '          AbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          dAbC       mnzd');
	strTest = '          dAbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab AbCmnzd');
	strTest = 'Ab AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      AbCmnzd');
	strTest = 'Ab      AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbAbC mnzd      ');
	strTest = 'AbAbC mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab	AbCe mnzd      ');
	strTest = 'Ab	AbCe mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab          AbC   mnzd    ');
	strTest = 'Ab          AbC   mnzd    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Abe          dAbCe  mnzd     ');
	strTest = 'Abe          dAbCe  mnzd     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 10 Failure - Slightly more complex strings');


	objExp = new Array('              AbCmnzd Ab C AbRACAdAbRA');
	strTest = '              AbCmnzd Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab C dAbCvmnzd  AbRACAdAbRA             ');
	strTest = 'Ab C dAbCvmnzd  AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              dAbCmnzd Ab C AbRACAdAbRAb');
	strTest = '              dAbCmnzd Ab C AbRACAdAbRAb'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('dAbCmnzd Ab C AbRACAdAbRAb C            ');
	strTest = 'dAbCmnzd Ab C AbRACAdAbRAb C            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              eAbC Ab C AbRACAdAbRA');
	strTest = '              eAbC Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('nAbC mnzd Ab C AbRACAdAbRAb d            ');
	strTest = 'nAbC mnzd Ab C AbRACAdAbRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 11 Failure - Lowercase tests w/ multiples');


	objExp = new Array('     AbC mnzd');
	strTest = '     AbC mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('dAbC mnzd      ');
	strTest = 'dAbC mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         AbC mnzd       ');
	strTest = '         AbC mnzd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         AbCd emnzd       ');
	strTest = '         AbCd emnzd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 12 Failure - Slightly more complex strings w/ multiple finds');


	objExp = new Array('              AbC mnzd AbRACAdAbRA');
	strTest = '              AbC mnzd AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('              oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe');
	strTest = '              oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe              ');
	strTest = 'oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              dAbCe fmnzd eAbRACAdAbRAs');
	strTest = '              dAbCe fmnzd eAbRACAdAbRAs'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('tAbCn dmnzd AbRACAdAbRA             ');
	strTest = 'tAbCn dmnzd AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           dAbCn smnzd AbRACAdAbRAd             ');
	strTest = '           dAbCn smnzd AbRACAdAbRAd             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('sAbC bmnzd RAbCAdAbRA');
	strTest = 'sAbC bmnzd RAbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              sAbC bmnzd RAbRACAdAbRA');
	strTest = '              sAbC bmnzd RAbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('              sAbCR bmnzdA AbCAdAbRA');
	strTest = '              sAbCR bmnzdA AbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('RAbC vmnzd AbCAdAbRA             ');
	strTest = 'RAbC vmnzd AbCAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AAbCd AAbmnzdAb AAAbCAdAbRA');
	strTest = '              AAbCd AAbmnzdAb AAAbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('fgAbCdg mAmnzds RzAbCxAAdAbRA             ');
	strTest = 'fgAbCdg mAmnzds RzAbCxAAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           mtAbCdg dfmnzd mknoAbCAdAbRAblol             ');
	strTest = '           mtAbCdg dfmnzd mknoAbCAdAbRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 13 Failure - Lowercase /AbC$/');

	apInitScenario(m_scen);
	regPat = /AbC$/;
	
	objExp = new Array('AbCmnz');
	strTest = 'AbCmnz';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('dAbCmnz');
	strTest = 'dAbCmnz';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('dAbCemnzd');
	strTest = 'dAbCemnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbCemnzd');
	strTest = 'AbCemnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      AbCmnzd');
	strTest = '      AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbC       mnzd');
	strTest = 'AbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('dAbC       mnzd');
	strTest = 'dAbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          AbC       mnzd');
	strTest = '          AbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          dAbC       mnzd');
	strTest = '          dAbC       mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab AbCmnzd');
	strTest = 'Ab AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      AbCmnzd');
	strTest = 'Ab      AbCmnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbAbC mnzd      ');
	strTest = 'AbAbC mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab	AbCe mnzd      ');
	strTest = 'Ab	AbCe mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab          AbC   mnzd    ');
	strTest = 'Ab          AbC   mnzd    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Abe          dAbCe  mnzd     ');
	strTest = 'Abe          dAbCe  mnzd     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 14 Failure - Slightly more complex strings');

	objExp = new Array('AbCmnzd Ab C AbRACAdAbRA');
	strTest = 'AbCmnzd Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AbCmnzd Ab C AbRACAdAbRA');
	strTest = '              AbCmnzd Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab C dAbCvmnzd  AbRACAdAbRA             ');
	strTest = 'Ab C dAbCvmnzd  AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              dAbCmnzd Ab C AbRACAdAbRAb');
	strTest = '              dAbCmnzd Ab C AbRACAdAbRAb'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('dAbCmnzd Ab C AbRACAdAbRAb C            ');
	strTest = 'dAbCmnzd Ab C AbRACAdAbRAb C            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              eAbC Ab C AbRACAdAbRA');
	strTest = '              eAbC Ab C AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('nAbC mnzd Ab C AbRACAdAbRAb d            ');
	strTest = 'nAbC mnzd Ab C AbRACAdAbRAb d            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 15 Failure - Lowercase tests w/ multiples');

	objExp = new Array('AbC mnzd');
	strTest = 'AbC mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     AbC mnzd');
	strTest = '     AbC mnzd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbC mnzd      ');
	strTest = 'AbC mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('dAbC mnzd      ');
	strTest = 'dAbC mnzd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         AbC mnzd       ');
	strTest = '         AbC mnzd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         AbCd emnzd       ');
	strTest = '         AbCd emnzd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 16 Failure - Slightly more complex strings w/ multiple finds');

	objExp = new Array('AbC mnzd AbRACAdAbRA');
	strTest = 'AbC mnzd AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AbC mnzd AbRACAdAbRA');
	strTest = '              AbC mnzd AbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbC mnzd AbRACAdAbRA             ');
	strTest = 'AbC mnzd AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe');
	strTest = '              oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe              ');
	strTest = 'oRAnAbCAbo sienmnzdthedRAl AbRACAdAbRAe              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              dAbCe fmnzd eAbRACAdAbRAs');
	strTest = '              dAbCe fmnzd eAbRACAdAbRAs'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('tAbCn dmnzd AbRACAdAbRA             ');
	strTest = 'tAbCn dmnzd AbRACAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           dAbCn smnzd AbRACAdAbRAd             ');
	strTest = '           dAbCn smnzd AbRACAdAbRAd             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('sAbC bmnzd RAbCAdAbRA');
	strTest = 'sAbC bmnzd RAbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              sAbC bmnzd RAbRACAdAbRA');
	strTest = '              sAbC bmnzd RAbRACAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('AbC mnzd AbCAdAbRA             ');
	strTest = 'AbC mnzd AbCAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              sAbCR bmnzdA AbCAdAbRA');
	strTest = '              sAbCR bmnzdA AbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('RAbC vmnzd AbCAdAbRA             ');
	strTest = 'RAbC vmnzd AbCAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AAbCd AAbmnzdAb AAAbCAdAbRA');
	strTest = '              AAbCd AAbmnzdAb AAAbCAdAbRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('fgAbCdg mAmnzds RzAbCxAAdAbRA             ');
	strTest = 'fgAbCdg mAmnzds RzAbCxAAdAbRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           mtAbCdg dfmnzd mknoAbCAdAbRAblol             ');
	strTest = '           mtAbCdg dfmnzd mknoAbCAdAbRAblol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

/*****************************************************************************/
	apEndTest();
}


spt_036();


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
