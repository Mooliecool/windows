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


var iTestID = 53564;

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
{
	//redirects function call to verify
	verify(sCat1, arrayExp, arrayAct);
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





function srca0171() {
	

apInitTest("srca0171");



	m_scen = ('Test 13 Lowercase /abc{1,}/');

	sCat = "Pattern: {1,} ";
	apInitScenario(m_scen);
	regPat = /abc{1,}/;
	objExp = new Array('abc');
	regExp = new Array();
	
	objExp = 0;
	strTest = 'abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = 'abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 7;
	strTest = '       abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = '       abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end


	strTest = 'abcc              ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = '              abcc              ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcc                         ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = 'abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end


	strTest = 'abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcccccccccccccccccccc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = '              abcccccccccccccccccccc              ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcccccccccccccccccccc                         ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	m_scen = ('Test 14 Slightly more complex strings');

	strTest = 'abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 0;
	strTest = 'abc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 15;
	strTest = '              dabc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	strTemp = 'abcc'
	objExp = 0;
	strTest = 'abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 0;
	strTest = 'abcc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 15;
	strTest = '              dabcc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabcc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTemp = 'abcccccccccccccccccccc'
	objExp = 0;
	strTest = 'abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcccccccccccccccccccc ab c abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 0;
	strTest = 'abcccccccccccccccccccc ab c abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end
	
	objExp = 15;
	strTest = '              dabcccccccccccccccccccc ab c abracadabrab'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabcccccccccccccccccccc ab c abracadabrab c            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	m_scen = ('Test 15 Lowercase tests w/ multiples');

	objExp = 0;
	strTest = 'abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 5;
	strTest = '     abc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabc abce      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 9;
	strTest = '         abc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 9;
	strTest = '         abcd eabc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTemp = 'abcc'
	objExp = 0;
	strTest = 'abcc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 5;
	strTest = '     abcc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabcc abce      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 9;
	strTest = '         abcc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = '         abccd eabc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTemp = 'abcccccccccccccccccccc'
	objExp = 0;
	strTest = 'abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 5;
	strTest = '     abcccccccccccccccccccc abc';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcccccccccccccccccccc abc      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'dabcccccccccccccccccccc abce      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 9;
	strTest = '         abcccccccccccccccccccc abc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTest = '         abccccccccccccccccccccd eabc       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

	objExp = 0;
	strTest = 'abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 18;
	strTest = '              oranabcabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 4;
	strTest = 'oranabcabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              dabce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'tabcn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 12;
	strTest = '           dabcn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'sabc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabcr babca abcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'rabc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              aabcd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 2;
	strTest = 'fgabcdg maabcds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 13;
	strTest = '           mtabcdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTemp = 'abcc'
	objExp = 0;
	strTest = 'abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 18;
	strTest = '              oranabccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 4;
	strTest = 'oranabccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              dabcce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'tabccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 12;
	strTest = '           dabccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'sabcc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabcc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'rabcc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              aabccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 2;
	strTest = 'fgabccdg maabccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 13;
	strTest = '           mtabccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	strTemp = 'abcccccccccccccccccccc'
	objExp = 0;
	strTest = 'abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 14;
	strTest = '              abcccccccccccccccccccc abc abracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcccccccccccccccccccc abc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 18;
	strTest = '              oranabccccccccccccccccccccabo sienabcathedral abracadabrae'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 4;
	strTest = 'oranabccccccccccccccccccccabo sienabcathedral abracadabrae              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              dabcccccccccccccccccccce fabcg eabracadabras'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'tabccccccccccccccccccccn dabc abracadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 12;
	strTest = '           dabccccccccccccccccccccn sabc abracadabrad             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'sabcccccccccccccccccccc babc rabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabcccccccccccccccccccc babc rabracadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 0;
	strTest = 'abcccccccccccccccccccc abc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              sabccccccccccccccccccccr babca abcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 1;
	strTest = 'rabcccccccccccccccccccc vabc abcadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 15;
	strTest = '              aabccccccccccccccccccccd aababcab aaabcadabra'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 2;
	strTest = 'fgabccccccccccccccccccccdg maabccccccccccccccccccccds rzabcxaadabra             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end

	objExp = 13;
	strTest = '           mtabccccccccccccccccccccdg dfabcwo mknoabcadabrablol             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	@end


/*****************************************************************************/
	apEndTest();
}


srca0171();


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
