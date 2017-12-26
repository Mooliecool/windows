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


var iTestID = 53196;


//This set of tests verifies that \w works in various circumstances with lowercase
var sCat = '';
var regPat = "";
var objExp = "";
var m_scen = '';
var strTest = "";

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



function ArrayEqual(sCat1, arrayExp, arrayAct){
    var i;
	//Makes Sure that Arrays are eq	

	if (!((arrayExp == null) & (arrayAct == null))) {
		if (arrayExp.length != arrayAct.length)
			verify(sCat1 + ' Array length', arrayExp.length, arrayAct.length);
		else
		{
			for (i in arrayExp)
				verify(sCat1 + ' index '+i, arrayExp[i], arrayAct[i]);
		}
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




function exea011() 
{
	apInitTest("exea011");

	m_scen = "Lowercase /((a)(bc))/g";

	sCat = "/((a)(bc))/g ";
	apInitScenario(m_scen);
	regPat = /((a)(bc))/g;
	objExp = new Array('abc','abc','a','bc');
	var regExp = new Array('abc','a','bc');

	var strTest = 'abc';
	regPat.lastIndex = 0;
	var result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ababc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab	abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Slightly more complex strings";

	strTest = 'abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Lowercase /((a)(bc))/g multiples"; 

	strTest = 'abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '     abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = "Slightly more complex strings w/ multiple finds";
	
	strTest = 'abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	strTest = 'abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Lowercase /(a)(b)(c)/g";
	sCat = "/(a)(b)(c)/g ";
	apInitScenario(m_scen);
	regPat = /(a)(b)(c)/g;
	objExp = new Array('abc','a','b','c');
	regExp = new Array('a', 'b', 'c');

	strTest = 'abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ababc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab	abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Slightly more complex strings";

	strTest = 'abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Lowercase tests /(a)(b)(c)/g multiples";

	strTest = 'abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '     abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Slightly more complex strings w/ multiple finds";

	strTest = 'abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	strTest = 'abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Lowercase /((a)(b)(c))/g";

	sCat = "/((a)(b)(c))/g ";
	apInitScenario(m_scen);
	regPat = /((a)(b)(c))/g;
	objExp = new Array('abc','abc','a','b','c');
	regExp = new Array('abc','a','b','c');

	strTest = 'abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ababc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab	abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Slightly more complex strings";

	strTest = 'abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Lowercase tests /((a)(b)(c))/g multiples";

	strTest = 'abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '     abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Slightly more complex strings w/ multiple finds";

	strTest = 'abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	strTest = 'abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	ArrayEqual(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = "Failure Lowercase /aBc/g";

	sCat = "/aBc/g ";
	apInitScenario(m_scen);
	regPat = /aBc/g;
	objExp = null;
	//no new regExp due to spec of RegExp.$1-9
	//this set of tests smakes sure that RegExp.$ keep original values


	strTest = 'abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab      abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ababc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab	abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'ab          abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen =  "Failure-Slightly more complex strings";
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc ab c abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc ab c abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = "Failure Lowercase tests /aBc/g multiples";

	strTest = 'abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '     abc abc';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc      |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       ';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '         abc abc       |';
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = "Failure - Slightly more complex strings w/ multiple finds";
	
	strTest = 'abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abracadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '              abc abc abracadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             '; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra|'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|              abc abc abcadabra'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = 'abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	strTest = '|           abc abc abcadabra             |'; 
	regPat.lastIndex = 0;
	result = regPat.exec(strTest);
	verify(sCat+strTest, objExp, result);
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

/*****************************************************************************/

    apEndTest();

}


exea011();


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
