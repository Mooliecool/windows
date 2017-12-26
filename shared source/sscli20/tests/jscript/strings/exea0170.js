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


var iTestID = 53202;

var sCat = '';
var regPat = "";
var objExp = "";
var m_scen = '';
var strTest = "";
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





function exea0170() {
	

apInitTest("exea0170");
	m_scen = ('Test 6 Lowercase /abc+/');

	sCat = "Pattern: + ";
	apInitScenario(m_scen);
	regPat = /abc+/;
	objExp = new Array('abc');
	var regExp = new Array();
	
	var strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '       abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '       abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc';
	ArrayEqual(sCat+strTest, new Array('abcc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = 'abcc              ';
	ArrayEqual(sCat+strTest, new Array('abcc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc';
	ArrayEqual(sCat+strTest, new Array('abcc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc              ';
	ArrayEqual(sCat+strTest, new Array('abcc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc                         ';
	ArrayEqual(sCat+strTest, new Array('abcc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, new Array('abcccccccccccccccccccc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = 'abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, new Array('abcccccccccccccccccccc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, new Array('abcccccccccccccccccccc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, new Array('abcccccccccccccccccccc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc                         ';
	ArrayEqual(sCat+strTest, new Array('abcccccccccccccccccccc'), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 7 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTemp = 'abcc'
	strTest = 'abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abcc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabcc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abcccccccccccccccccccc'
	strTest = 'abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abcccccccccccccccccccc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabcccccccccccccccccccc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcccccccccccccccccccc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 8 Lowercase tests w/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabc abce      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abc abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcd eabc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abcc'
	strTest = 'abcc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abcc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcc abce      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcc abc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abccd eabc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abcccccccccccccccccccc'
	strTest = 'abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcccccccccccccccccccc abce      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcccccccccccccccccccc abc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abccccccccccccccccccccd eabc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 9 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabcabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabcabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabcn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabcn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcr babca abcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabcd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabcdg maabcds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabcdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abcc'
	strTest = 'abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabcce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabcc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabcc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabccdg maabccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abcccccccccccccccccccc'
	strTest = 'abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabccccccccccccccccccccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabccccccccccccccccccccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabcccccccccccccccccccce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabccccccccccccccccccccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabccccccccccccccccccccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabcccccccccccccccccccc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcccccccccccccccccccc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabccccccccccccccccccccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabcccccccccccccccccccc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabccccccccccccccccccccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabccccccccccccccccccccdg maabccccccccccccccccccccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabccccccccccccccccccccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 10 Lowercase /abc?/');

	sCat = "Pattern: ? ";
	apInitScenario(m_scen);
	regPat = /abc?/;
	objExp = new Array('abc');
	regExp = new Array();
	
	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '       abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '       abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = 'abcc              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc                         ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTest = 'abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc                         ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 11 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTemp = 'ab'
	strTest = 'ab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'ab ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              ab ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'ab ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dab ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dab ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abc'
	strTest = 'abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abcc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabcc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abc';
	strTest = 'abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = 'abcccccccccccccccccccc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	
	strTest = '              dabcccccccccccccccccccc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcccccccccccccccccccc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 12 Lowercase tests w/ multiples');

	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabc abce      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abc abc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcd eabc       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'ab';
	strTest = 'ab abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     ab abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'ab abc      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dab abce      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         ab abc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         ab eabc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTemp = 'abc';
	strTest = 'abcc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abcc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcc abce      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcc abc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abccd eabc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abc';
	strTest = 'abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '     abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'dabcccccccccccccccccccc abce      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abcccccccccccccccccccc abc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '         abccccccccccccccccccccd eabc       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	m_scen = ('Test 13 Slightly more complex strings w/ multiple finds');

	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabcabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabcabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabcn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabcn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcr babca abcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabcd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabcdg maabcds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabcdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'ab';
	strTest = 'ab abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              ab abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'ab abc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranababo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranababo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabe fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sab babc rabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sab babc rabracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'ab abc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabr babca abcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rab vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabdg maabccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end


	strTemp = 'abc';
	strTest = 'abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabcce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabcc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabcc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabccdg maabccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTemp = 'abc';
	strTest = 'abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              oranabccccccccccccccccccccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'oranabccccccccccccccccccccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              dabcccccccccccccccccccce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'tabccccccccccccccccccccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           dabccccccccccccccccccccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'sabcccccccccccccccccccc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabcccccccccccccccccccc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'abcccccccccccccccccccc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              sabccccccccccccccccccccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'rabcccccccccccccccccccc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '              aabccccccccccccccccccccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = 'fgabccccccccccccccccccccdg maabccccccccccccccccccccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end

	strTest = '           mtabccccccccccccccccccccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end/*****************************************************************************/
	apEndTest();
}


exea0170();


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
