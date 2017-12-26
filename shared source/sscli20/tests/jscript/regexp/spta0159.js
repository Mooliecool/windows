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


var iTestID = 53510;

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





function spta0159() {
	

apInitTest("spta0159");
	m_scen = ('Test 13 MiXed Case /((mnz)|(abc))/ig');

	sCat = "Pattern: exist | exist found ";
	apInitScenario(m_scen);
	regPat = /((mnz)|(abc))/ig;

	regExp = new Array('MnZ', 'MnZ');

	objExp = new Array();
	strTest = 'aBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D');
	strTest = 'DaBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('D' , 'E' , 'd');
	strTest = 'DaBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('E' , 'd');
	strTest = 'aBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ' , 'd');
	strTest = '      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       ' , 'd');
	strTest = 'aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , '       ' , 'd');
	strTest = 'DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       ' , 'd');
	strTest = '          aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          D' , '       ' , 'd');
	strTest = '          DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab ' , 'd');
	strTest = 'Ab aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      ' , 'd');
	strTest = 'Ab      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aB' , ' ' , 'd      ');
	strTest = 'aBaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , 'E ' , 'd      ');
	strTest = 'ab	aBcE MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab          ' , '   ' , 'd    ');
	strTest = 'Ab          aBc   MnZd    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('abE          D' , 'E  ' , 'd     ');
	strTest = 'abE          DaBcE  MnZd     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	objExp = new Array('d Ab c aBRacaDabRa');
	strTest = 'aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'd Ab c aBRacaDabRa');
	strTest = '              aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab c D' , 'v' , 'd  abRacaDabRa             ');
	strTest = 'Ab c DaBcvMnZd  abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'd Ab c aBRacaDabRaB');
	strTest = '              DaBcMnZd Ab c aBRacaDabRaB'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'd Ab c aBRacaDabrAb c            ');
	strTest = 'DaBcMnZd Ab c aBRacaDabrAb c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('n' , ' ' , 'd Ab c aBRacaDabrAb d            ');
	strTest = 'naBc MnZd Ab c aBRacaDabrAb d            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 15 MiXed Case tests w/ multiples');

	objExp = new Array(' ' , 'd');
	strTest = 'aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     ' , ' ' , 'd');
	strTest = '     aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , 'd      ');
	strTest = 'aBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' ' , 'd      ');
	strTest = 'DaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' ' , 'd       ');
	strTest = '         aBc MnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , 'D e' , 'd       ');
	strTest = '         aBcD eMnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	regExp = new Array('MnZ', 'MnZ');
	objExp = new Array(' ' , 'd abRacaDabRa');
	strTest = 'aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' ' , 'd abRacaDabRa');
	strTest = '              aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , 'd abRacaDabRa             ');
	strTest = 'aBc MnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              oRAn' , 'abo sien' , 'dthedral abRacaDabRae');
	strTest = '              oRAnaBcabo sienMnZdthedral abRacaDabRae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('oRAn' , 'abo sien' , 'dthedral abRacaDabRae              ');
	strTest = 'oRAnaBcabo sienMnZdthedral abRacaDabRae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'E f' , 'd eabRacaDabRas');
	strTest = '              DaBcE fMnZd eabRacaDabRas'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('t' , 'n d' , 'd abRacaDabRa             ');
	strTest = 'taBcn dMnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , 'n s' , 'd abRacaDabRad             ');
	strTest = '           DaBcn sMnZd abRacaDabRad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	regExp = new Array('aBc', '','aBc');
	objExp = new Array('s' , ' b' , 'd r' , 'aDabRa');
	strTest = 'saBc bMnZd raBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' ' , 'd ' , 'aDabRa             ');
	strTest = 'aBc MnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              s' , 'r b' , 'Da ' , 'aDabRa');
	strTest = '              saBcr bMnZDa aBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('r' , ' v' , 'd ' , 'aDabRa             ');
	strTest = 'raBc vMnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              A' , 'D Aab' , 'DAb Aa' , 'aDabRa');
	strTest = '              AaBcD AabMnZDAb AaaBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('fg' , 'Dg ma' , 'ds rz' , 'xAaDabRa             ');
	strTest = 'fgaBcDg maMnZds rzaBcxAaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           mT' , 'Dg df' , 'd mkno' , 'aDabRaBlol             ');
	strTest = '           mTaBcDg dfMnZd mknoaBcaDabRaBlol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 17 MiXed Case /((abc)|(abc))/ig');

	sCat = "Both Patterns same ";
	apInitScenario(m_scen);
	regPat = /((abc)|(abc))/ig;
	regExp = new Array('aBc','aBc');

	objExp = new Array('MnZ');
	strTest = 'aBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('D' , 'MnZ');
	strTest = 'DaBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('D' , 'EMnZd');
	strTest = 'DaBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('EMnZd');
	strTest = 'aBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      ' , 'MnZd');
	strTest = '      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('       MnZd');
	strTest = 'aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , '       MnZd');
	strTest = 'DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          ' , '       MnZd');
	strTest = '          aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          D' , '       MnZd');
	strTest = '          DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab ' , 'MnZd');
	strTest = 'Ab aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      ' , 'MnZd');
	strTest = 'Ab      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aB' , ' MnZd      ');
	strTest = 'aBaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	' , 'E MnZd      ');
	strTest = 'ab	aBcE MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab          ' , '   MnZd    ');
	strTest = 'Ab          aBc   MnZd    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('abE          D' , 'E  MnZd     ');
	strTest = 'abE          DaBcE  MnZd     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 18 Slightly more complex strings');

	objExp = new Array('MnZd Ab c aBRacaDabRa');
	strTest = 'aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , 'MnZd Ab c aBRacaDabRa');
	strTest = '              aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab c D' , 'vMnZd  abRacaDabRa             ');
	strTest = 'Ab c DaBcvMnZd  abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'MnZd Ab c aBRacaDabRaB');
	strTest = '              DaBcMnZd Ab c aBRacaDabRaB'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , 'MnZd Ab c aBRacaDabrAb c            ');
	strTest = 'DaBcMnZd Ab c aBRacaDabrAb c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              E' , ' Ab c aBRacaDabRa');
	strTest = '              EaBc Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('n' , ' MnZd Ab c aBRacaDabrAb d            ');
	strTest = 'naBc MnZd Ab c aBRacaDabrAb d            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 19 MiXed Case tests w/ multiples');

	objExp = new Array(' MnZd');
	strTest = 'aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     ' , ' MnZd');
	strTest = '     aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' MnZd      ');
	strTest = 'aBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('D' , ' MnZd      ');
	strTest = 'DaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , ' MnZd       ');
	strTest = '         aBc MnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         ' , 'D eMnZd       ');
	strTest = '         aBcD eMnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	objExp = new Array(' MnZd abRacaDabRa');
	strTest = 'aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              ' , ' MnZd abRacaDabRa');
	strTest = '              aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' MnZd abRacaDabRa             ');
	strTest = 'aBc MnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              oRAn' , 'abo sienMnZdthedral abRacaDabRae');
	strTest = '              oRAnaBcabo sienMnZdthedral abRacaDabRae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('oRAn' , 'abo sienMnZdthedral abRacaDabRae              ');
	strTest = 'oRAnaBcabo sienMnZdthedral abRacaDabRae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              D' , 'E fMnZd eabRacaDabRas');
	strTest = '              DaBcE fMnZd eabRacaDabRas'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('t' , 'n dMnZd abRacaDabRa             ');
	strTest = 'taBcn dMnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           D' , 'n sMnZd abRacaDabRad             ');
	strTest = '           DaBcn sMnZd abRacaDabRad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('s' , ' bMnZd r' , 'aDabRa');
	strTest = 'saBc bMnZd raBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              s' , ' bMnZd rabRacaDabRa');
	strTest = '              saBc bMnZd rabRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array(' MnZd ' , 'aDabRa             ');
	strTest = 'aBc MnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              s' , 'r bMnZDa ' , 'aDabRa');
	strTest = '              saBcr bMnZDa aBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('r' , ' vMnZd ' , 'aDabRa             ');
	strTest = 'raBc vMnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              A' , 'D AabMnZDAb Aa' , 'aDabRa');
	strTest = '              AaBcD AabMnZDAb AaaBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('fg' , 'Dg maMnZds rz' , 'xAaDabRa             ');
	strTest = 'fgaBcDg maMnZds rzaBcxAaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           mT' , 'Dg dfMnZd mkno' , 'aDabRaBlol             ');
	strTest = '           mTaBcDg dfMnZd mknoaBcaDabRaBlol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 21 Failure - MiXed Case /((abcg)|(mnzod))/ig');

	sCat = "Both Patterns do not exist ";
	apInitScenario(m_scen);
	regPat = /((abcg)|(mnzod))/ig;

	objExp = new Array('aBcMnZ');
	strTest = 'aBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('DaBcMnZ');
	strTest = 'DaBcMnZ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	objExp = new Array('DaBcEMnZd');
	strTest = 'DaBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBcEMnZd');
	strTest = 'aBcEMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('      aBcMnZd');
	strTest = '      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc       MnZd');
	strTest = 'aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('DaBc       MnZd');
	strTest = 'DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          aBc       MnZd');
	strTest = '          aBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('          DaBc       MnZd');
	strTest = '          DaBc       MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab aBcMnZd');
	strTest = 'Ab aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab      aBcMnZd');
	strTest = 'Ab      aBcMnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBaBc MnZd      ');
	strTest = 'aBaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('ab	aBcE MnZd      ');
	strTest = 'ab	aBcE MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab          aBc   MnZd    ');
	strTest = 'Ab          aBc   MnZd    ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('abE          DaBcE  MnZd     ');
	strTest = 'abE          DaBcE  MnZd     ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 22 Failure - Slightly more complex strings');

	objExp = new Array('aBcMnZd Ab c aBRacaDabRa');
	strTest = 'aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              aBcMnZd Ab c aBRacaDabRa');
	strTest = '              aBcMnZd Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('Ab c DaBcvMnZd  abRacaDabRa             ');
	strTest = 'Ab c DaBcvMnZd  abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              DaBcMnZd Ab c aBRacaDabRaB');
	strTest = '              DaBcMnZd Ab c aBRacaDabRaB'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('DaBcMnZd Ab c aBRacaDabrAb c            ');
	strTest = 'DaBcMnZd Ab c aBRacaDabrAb c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              EaBc Ab c aBRacaDabRa');
	strTest = '              EaBc Ab c aBRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('naBc MnZd Ab c aBRacaDabrAb d            ');
	strTest = 'naBc MnZd Ab c aBRacaDabrAb d            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end


	m_scen = ('Test 23 Failure - MiXed Case tests w/ multiples');

	objExp = new Array('aBc MnZd');
	strTest = 'aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('     aBc MnZd');
	strTest = '     aBc MnZd';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc MnZd      ');
	strTest = 'aBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('DaBc MnZd      ');
	strTest = 'DaBc MnZd      ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         aBc MnZd       ');
	strTest = '         aBc MnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('         aBcD eMnZd       ');
	strTest = '         aBcD eMnZd       ';
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	m_scen = ('Test 24 Failure - Slightly more complex strings w/ multiple finds');

	objExp = new Array('aBc MnZd abRacaDabRa');
	strTest = 'aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              aBc MnZd abRacaDabRa');
	strTest = '              aBc MnZd abRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc MnZd abRacaDabRa             ');
	strTest = 'aBc MnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              oRAnaBcabo sienMnZdthedral abRacaDabRae');
	strTest = '              oRAnaBcabo sienMnZdthedral abRacaDabRae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('oRAnaBcabo sienMnZdthedral abRacaDabRae              ');
	strTest = 'oRAnaBcabo sienMnZdthedral abRacaDabRae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              DaBcE fMnZd eabRacaDabRas');
	strTest = '              DaBcE fMnZd eabRacaDabRas'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('taBcn dMnZd abRacaDabRa             ');
	strTest = 'taBcn dMnZd abRacaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           DaBcn sMnZd abRacaDabRad             ');
	strTest = '           DaBcn sMnZd abRacaDabRad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('saBc bMnZd raBcaDabRa');
	strTest = 'saBc bMnZd raBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              saBc bMnZd rabRacaDabRa');
	strTest = '              saBc bMnZd rabRacaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('aBc MnZd aBcaDabRa             ');
	strTest = 'aBc MnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              saBcr bMnZDa aBcaDabRa');
	strTest = '              saBcr bMnZDa aBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('raBc vMnZd aBcaDabRa             ');
	strTest = 'raBc vMnZd aBcaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('              AaBcD AabMnZDAb AaaBcaDabRa');
	strTest = '              AaBcD AabMnZDAb AaaBcaDabRa'; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('fgaBcDg maMnZds rzaBcxAaDabRa             ');
	strTest = 'fgaBcDg maMnZds rzaBcxAaDabRa             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end

	objExp = new Array('           mTaBcDg dfMnZd mknoaBcaDabRaBlol             ');
	strTest = '           mTaBcDg dfMnZd mknoaBcaDabRaBlol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.split(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.split(regPat));
	@end
/*****************************************************************************/
	apEndTest();
}


spta0159();


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
