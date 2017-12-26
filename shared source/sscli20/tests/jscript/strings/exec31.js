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


var iTestID = 52608;

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



function exec31() {
	

apInitTest("Exec31");


	m_scen = ('Test 1 mixedcase /AB1\xFF/');

sCat = "/AB1\xFF/ ";
	apInitScenario(m_scen);
regPat = /AB1\xFF/;
objExp = new Array('AB1\xFF');
var regExp = new Array();

var strTest = 'AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


strTest = '      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1 AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1	AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 2 Slightly more complex strings');

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 3 mixedcase tests /AB1\xFF/ multiples');

strTest = 'AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '     AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 4 Slightly more complex strings w/ multiple finds');

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 5 mixedcase /(AB1\xFF)/');

sCat = "/(AB1\xFF)/ ";
	apInitScenario(m_scen);
regPat = /(AB1\xFF)/;
objExp = new Array('AB1\xFF', 'AB1\xFF');
regExp = new Array('AB1\xFF');

strTest = 'AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1 AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1	AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 6 Slightly more complex strings');

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 7 mixedcase tests /(AB1\xFF)/ multiples');

strTest = 'AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '     AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 8 Slightly more complex strings w/ multiple finds');

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 9 mixedcase /(AB1)(\xFF)/');

sCat = "/(AB1)(\xFF)/ ";
	apInitScenario(m_scen);
regPat = /(AB1)(\xFF)/;
objExp = new Array('AB1\xFF','AB1','\xFF');
regExp = new Array('AB1','\xFF');

strTest = 'AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1 AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1	AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 10 Slightly more complex strings');

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 11 mixedcase tests /(AB1)(\xFF)/ multiples');

strTest = 'AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '     AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 12 Slightly more complex strings w/ multiple finds');

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 13 mixedcase /((AB1)(\xFF))/');

sCat = "/((AB1)(\xFF))/ ";
	apInitScenario(m_scen);
regPat = /((AB1)(\xFF))/;
objExp = new Array('AB1\xFF', 'AB1\xFF','AB1','\xFF');
regExp = new Array('AB1\xFF','AB1','\xFF');

strTest = 'AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1 AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1      AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1	AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1          AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 14 Slightly more complex strings');

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1 \xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 15 mixedcase tests //((ab)(c))/ multiples');

strTest = 'AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '     AB1\xFF AB1\xFF';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF      |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       ';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '         AB1\xFF AB1\xFF       |';
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	m_scen = ('Test 16 Slightly more complex strings w/ multiple finds');

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '              AB1\xFF AB1\xFF AB1mA\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             '; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA|'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|              AB1\xFF AB1\xFF AB1\xFFA\\AB1mA'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = 'AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

strTest = '|           AB1\xFF AB1\xFF AB1\xFFA\\AB1mA             |'; 
ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

/*****************************************************************************/
	apEndTest();
}


exec31();


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
