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


var iTestID = 53248;

var sCat = '';
var regPat = "";
var objExp = "";
var m_scen = '';
var strTest = "";
var strTemp = "";
var result = "";


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
{  var i;
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



function exec036() {
	

apInitTest("exec036");


	m_scen = ('Test 1 Mixed Case /^AbC/');

	sCat = "Pattern: begin ";
	apInitScenario(m_scen);
	regPat = /^AbC/;
	objExp = new Array('AbC');
	var regExp = new Array();
	
	var strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'AbCd';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'AbCd                         ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'AbC       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	

	m_scen = ('Test 2 Slightly more complex strings');

	strTest = 'AbC aB c aBracAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	
	strTest = 'AbC aB c aBracAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	

	strTest = 'AbCd aB c aBracAdAbRaB c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	
	m_scen = ('Test 3 Mixed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC AbC      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbCd AbCe      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

	strTest = 'AbC AbC aBracAdAbRa'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC AbC aBracAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbCabo sienAbCathedral aBracAdAbRae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'AbCn dAbC aBracAdAbRa             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC AbC AbCadaBra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbCd aAbAbCAb aaAbCadaBra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 5 Mixed Case /AbC$/');

	sCat = "Pattern: end ";
	apInitScenario(m_scen);
	regPat = /AbC$/;
	objExp = new Array('AbC');
	regExp = new Array();

	strTest = 'AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab      AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 6 Slightly more complex strings');

	strTest = 'AnC aB c aBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              AnC aB c aBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 7 Mixed Case tests w/ multiples');

	strTest = 'AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     AbC AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '  Ab             c       AbC                  AbC';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

	strTest = 'Ab AbC ad ase hh aBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '			AbC ad ase hh aBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              oranAbCabo sienAbCathedral aBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              dAbCe fAbCg eaBracaDaBrAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'aAbCd aAbAbCAb aaAbCadaBraaaaaaaaaaaaaaaaAbC'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 9 Failure - Mixed Case /^AbC /');

	sCat = "Pattern dows not exist: ";
	apInitScenario(m_scen);
	regPat = /^AbC/;
	objExp = null;

	strTest = ' AbCmnz';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbCmnz';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbCemnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '                                       AbCemnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = '          AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '          dAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'abAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'ab	AbCe mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab          AbC   mnzd    ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'abe          dAbCe  mnzd     ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 10 Failure - Slightly more complex strings');


	strTest = '              AbCmnzd aB c aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'aB c dAbCvmnzd  aBracAdAbRa             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              dAbCmnzd aB c aBracAdAbRab'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'dAbCmnzd aB c aBracAdAbRaB c            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              eAbC aB c aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'nAbC mnzd aB c aBracAdAbRAb d            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 11 Failure - Mixed Case tests w/ multiples');


	strTest = '     AbC mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         AbC mnzd       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         AbCd emnzd       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 12 Failure - Slightly more complex strings w/ multiple finds');


	strTest = '              AbC mnzd aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = '              oranAbCabo sienmnzdthedral aBracAdAbRae'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'oranAbCabo sienmnzdthedral aBracAdAbRae              '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              dAbCe fmnzd eaBracAdAbRas'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'tAbCn dmnzd aBracAdAbRa             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           dAbCn smnzd aBracAdAbRad             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'sAbC bmnzd rAbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              sAbC bmnzd raBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = '              sAbCr bmnzda AbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'rAbC vmnzd AbCadaBra             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              aAbCd aAbmnzdAb aaAbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'fgAbCdg mamnzds rzAbCxaadaBra             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           mtAbCdg dfmnzd mknoAbCadaBrablol             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 13 Failure - Mixed Case /AbC$/');

	sCat = " Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /AbC$/;
	objExp = null;
	
	strTest = 'AbCmnz';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbCmnz';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = 'dAbCemnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbCemnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'dAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '          AbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '          dAbC       mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab      AbCmnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'abAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'ab	AbCe mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'Ab          AbC   mnzd    ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'abe          dAbCe  mnzd     ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 14 Failure - Slightly more complex strings');

	strTest = 'AbCmnzd aB c aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              AbCmnzd aB c aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'aB c dAbCvmnzd  aBracAdAbRa             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              dAbCmnzd aB c aBracAdAbRab'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'dAbCmnzd aB c aBracAdAbRaB c            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              eAbC aB c aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'nAbC mnzd aB c aBracAdAbRAb d            '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 15 Failure - Mixed Case tests w/ multiples');

	strTest = 'AbC mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     AbC mnzd';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'dAbC mnzd      ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         AbC mnzd       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         AbCd emnzd       ';
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 16 Failure - Slightly more complex strings w/ multiple finds');

	strTest = 'AbC mnzd aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              AbC mnzd aBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC mnzd aBracAdAbRa             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              oranAbCabo sienmnzdthedral aBracAdAbRae'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'oranAbCabo sienmnzdthedral aBracAdAbRae              '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              dAbCe fmnzd eaBracAdAbRas'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'tAbCn dmnzd aBracAdAbRa             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           dAbCn smnzd aBracAdAbRad             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'sAbC bmnzd rAbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              sAbC bmnzd raBracAdAbRa'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'AbC mnzd AbCadaBra             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              sAbCr bmnzda AbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'rAbC vmnzd AbCadaBra             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              aAbCd aAbmnzdAb aaAbCadaBra'; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = 'fgAbCdg mamnzds rzAbCxaadaBra             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           mtAbCdg dfmnzd mknoAbCadaBrablol             '; 
	verify(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

/*****************************************************************************/
	apEndTest();
}


exec036();


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
