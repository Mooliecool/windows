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


var iTestID = 52654;

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





function exea111() {
	

apInitTest("Exea111");
	m_scen = ('Test 17 numbers   /(2)(34)/g');

	apInitScenario(m_scen);
regPat = /(2)(34)/g;
objExp = new Array('234', '2','34');
var regExp = new Array('2','34');

var strTest = '234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23	234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 18 Slightly more complex strings');

strTest = '234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 19 numbers   tests /(2)(34)/g multiples');

strTest = '234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '     234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

strTest = '234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


strTest = '234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = ('Test 21 numbers   /((2)(34))/g');

	apInitScenario(m_scen);
regPat = /((2)(34))/g;
objExp = new Array('234','234','2','34');
regExp = new Array('234','2','34');

strTest = '234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23	234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 22 Slightly more complex strings');

strTest = '234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 23 numbers   tests /((2)(34))/g multiples');

strTest = '234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '     234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 24 Slightly more complex strings w/ multiple finds');

strTest = '234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


strTest = '234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 25 numbers   /(2)(3)(4)/g');

	apInitScenario(m_scen);
regPat = /(2)(3)(4)/g;
objExp = new Array('234','2','3','4');
regExp = new Array('2', '3', '4');

strTest = '234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23	234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 26 Slightly more complex strings');

strTest = '234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 27 numbers   tests /(2)(3)(4)/g multiples');

strTest = '234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '     234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 28 Slightly more complex strings w/ multiple finds');

strTest = '234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


strTest = '234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 29 numbers   /((2)(3)(4))/g');

	apInitScenario(m_scen);
regPat = /((2)(3)(4))/g;
objExp = new Array('234','234','2','3','4');
regExp = new Array('234','2','3','4');

strTest = '234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23	234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 30 Slightly more complex strings');

strTest = '234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 31 numbers   tests /((2)(3)(4))/g multiples');

strTest = '234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '     234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 32 Slightly more complex strings w/ multiple finds');

strTest = '234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


strTest = '234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
ArrayEqual(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = ('Test 33 Failure Lowercase /2345/g');

	apInitScenario(m_scen);
regPat = /2345/g;
objExp = null;
//no new regExp due to spec of RegExp.$1-9
//this set of tests smakes sure that RegExp.$ keep original values


strTest = '234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23      234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23	234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '23          234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = ('Test 34 Failure-Slightly more complex strings');


strTest = '              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 23 4 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 23 4 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end


	m_scen = ('Test 35 Failure Lowercase tests /234/g multiples');

strTest = '234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '     234 234';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234      |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       ';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '         234 234       |';
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

	m_scen = ('Test 36 Failure - Slightly more complex strings w/ multiple finds');

strTest = '234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 23424272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '              234 234 23424272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             '; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342|'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|              234 234 234272342'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end

strTest = '|           234 234 234272342             |'; 
regPat.lastIndex = 0;
result = regPat.exec(strTest);
verify(sCat+strTest, objExp, result);
@if (!@_fast)
		regVerify(sCat+strTest, regExp, result);
	@end
/*****************************************************************************/
	apEndTest();
}

exea111();


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
