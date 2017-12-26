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


var iTestID = 53089;

	var sCat = '';
	var regPat = "";
	var objExp = "";
       var regExp = "";
       var strTest = "";
       var m_scen = '';
var strTemp= "";

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





function mtc_0270() {
	
  @if(@_fast)
    var i, strTest, regExp;
  @end

apInitTest("mtc_0270");


	m_scen = ('Test 1 Lowercase /./');

	apInitScenario(m_scen);
	regPat = /./;
	objExp = new Array('A');
	regExp = new Array();

	strTest = 'A';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	regPat = / ./;
	objExp = new Array('A');
	regExp = new Array();

	objExp = new Array(' A');
	strTest = ' A';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = ' ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 2 Lowercase /ABC*/');   // comment

	apInitScenario(m_scen);
	regPat = /ABC*/;     // comment
	objExp = new Array('ABC');
	regExp = new Array();
	
	strTest = 'ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '       ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '       ABC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCC';
	ArrayEqual(sCat+strTest, new Array('ABCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'ABCC              ';
	ArrayEqual(sCat+strTest, new Array('ABCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCC';
	ArrayEqual(sCat+strTest, new Array('ABCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCC              ';
	ArrayEqual(sCat+strTest, new Array('ABCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCC                         ';
	ArrayEqual(sCat+strTest, new Array('ABCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCCCCCCCCCCCCCCCCCCCC';
	ArrayEqual(sCat+strTest, new Array('ABCCCCCCCCCCCCCCCCCCCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTest = 'ABCCCCCCCCCCCCCCCCCCCC              ';
	ArrayEqual(sCat+strTest, new Array('ABCCCCCCCCCCCCCCCCCCCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCCCCCCCCCCCCCCCCCCCC';
	ArrayEqual(sCat+strTest, new Array('ABCCCCCCCCCCCCCCCCCCCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCCCCCCCCCCCCCCCCCCCC              ';
	ArrayEqual(sCat+strTest, new Array('ABCCCCCCCCCCCCCCCCCCCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCCCCCCCCCCCCCCCCCCCC                         ';
	ArrayEqual(sCat+strTest, new Array('ABCCCCCCCCCCCCCCCCCCCC'), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 3 Slightly more complex strings');

	strTest = 'ABC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = 'ABC AB C ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = '              DABC AB C ABRACADABRAB'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABC AB C ABRACADABRAB C            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'AB'
	strTest = 'AB'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'AB AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AB AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = 'AB AB C ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = '              DAB AB C ABRACADABRAB'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DAB AB C ABRACADABRAB C            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	
	strTemp = 'ABCC'
	strTest = 'ABCC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = 'ABCC AB C ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = '              DABCC AB C ABRACADABRAB'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABCC AB C ABRACADABRAB C            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'ABCCCCCCCCCCCCCCCCCCCC'
	strTest = 'ABCCCCCCCCCCCCCCCCCCCC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCCCCCCCCCCCCCCCCCCCC AB C ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = 'ABCCCCCCCCCCCCCCCCCCCC AB C ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end
	
	strTest = '              DABCCCCCCCCCCCCCCCCCCCC AB C ABRACADABRAB'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABCCCCCCCCCCCCCCCCCCCC AB C ABRACADABRAB C            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 4 Lowercase tests w/ multiples');

	strTest = 'ABC ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     ABC ABC';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABC ABC      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABC ABCE      ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABC ABC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABCD EABC       ';
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'AB'
	strTest = 'AB ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     AB ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'AB ABC      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DAB ABCE      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         AB ABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABD EABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTemp = 'ABCC'
	strTest = 'ABCC ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     ABCC ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCC ABC      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABCC ABCE      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABCC ABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABCCD EABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'ABCCCCCCCCCCCCCCCCCCCC'
	strTest = 'ABCCCCCCCCCCCCCCCCCCCC ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '     ABCCCCCCCCCCCCCCCCCCCC ABC';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCCCCCCCCCCCCCCCCCCCC ABC      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'DABCCCCCCCCCCCCCCCCCCCC ABCE      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABCCCCCCCCCCCCCCCCCCCC ABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '         ABCCCCCCCCCCCCCCCCCCCCD EABC       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	m_scen = ('Test 5 Slightly more complex strings w/ multiple finds');

	strTest = 'ABC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABC ABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ORANABCABO SIENABCATHEDRAL ABRACADABRAE'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ORANABCABO SIENABCATHEDRAL ABRACADABRAE              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              DABCE FABCG EABRACADABRAS'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'TABCN DABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           DABCN SABC ABRACADABRAD             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'SABC BABC RABCADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABC BABC RABRACADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABC ABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABCR BABCA ABCADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'RABC VABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AABCD AABABCAB AAABCADABRA'; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'FGABCDG MAABCDS RZABCXAADABRA             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           MTABCDG DFABCWO MKNOABCADABRABLOL             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'AB'
	strTest = 'AB ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AB ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'AB ABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ORANABABO SIENABCATHEDRAL ABRACADABRAE'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ORANABABO SIENABCATHEDRAL ABRACADABRAE              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              DABE FABCG EABRACADABRAS'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'TABN DABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           DABN SABC ABRACADABRAD             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'SAB BABC RABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SAB BABC RABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'AB ABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABR BABCA ABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'RAB VABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AABD AABABCAB AAABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'FGABDG MAABCCDS RZABCXAADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           MTABDG DFABCWO MKNOABCADABRABLOL             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end


	strTemp = 'ABCC'
	strTest = 'ABCC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCC ABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ORANABCCABO SIENABCATHEDRAL ABRACADABRAE'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ORANABCCABO SIENABCATHEDRAL ABRACADABRAE              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              DABCCE FABCG EABRACADABRAS'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'TABCCN DABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           DABCCN SABC ABRACADABRAD             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'SABCC BABC RABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABCC BABC RABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCC ABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABCCR BABCA ABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'RABCC VABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AABCCD AABABCAB AAABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'FGABCCDG MAABCCDS RZABCXAADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           MTABCCDG DFABCWO MKNOABCADABRABLOL             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTemp = 'ABCCCCCCCCCCCCCCCCCCCC'
	strTest = 'ABCCCCCCCCCCCCCCCCCCCC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ABCCCCCCCCCCCCCCCCCCCC ABC ABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCCCCCCCCCCCCCCCCCCCC ABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              ORANABCCCCCCCCCCCCCCCCCCCCABO SIENABCATHEDRAL ABRACADABRAE'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ORANABCCCCCCCCCCCCCCCCCCCCABO SIENABCATHEDRAL ABRACADABRAE              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              DABCCCCCCCCCCCCCCCCCCCCE FABCG EABRACADABRAS'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'TABCCCCCCCCCCCCCCCCCCCCN DABC ABRACADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           DABCCCCCCCCCCCCCCCCCCCCN SABC ABRACADABRAD             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'SABCCCCCCCCCCCCCCCCCCCC BABC RABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABCCCCCCCCCCCCCCCCCCCC BABC RABRACADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'ABCCCCCCCCCCCCCCCCCCCC ABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              SABCCCCCCCCCCCCCCCCCCCCR BABCA ABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'RABCCCCCCCCCCCCCCCCCCCC VABC ABCADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '              AABCCCCCCCCCCCCCCCCCCCCD AABABCAB AAABCADABRA'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = 'FGABCCCCCCCCCCCCCCCCCCCCDG MAABCCCCCCCCCCCCCCCCCCCCDS RZABCXAADABRA             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

	strTest = '           MTABCCCCCCCCCCCCCCCCCCCCDG DFABCWO MKNOABCADABRABLOL             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), strTest.match(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.match(regPat));
	@end

/*****************************************************************************/
	apEndTest();
}


mtc_0270();


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
