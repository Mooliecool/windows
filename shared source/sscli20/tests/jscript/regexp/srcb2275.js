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


var iTestID = 53621;

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





function srcb2275() {
	

apInitTest("srcb2275");


	sCat = "Pattern: {0} ";
	regPat = /\uC000\uC001\uC002{0}/ig;
	objExp = new Array('\uC000\uC001');
	regExp = new Array();
	

	m_scen = ('Test 18 Slightly more complex strings');
	apInitScenario(m_scen);

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	strTemp = '\uC000\uC001'
	objExp = 0;
	strTest = '\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '\uC000\uC001 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 15;
	strTest = '              \uC501\uC000\uC001 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001'
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end
	
	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001 \uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001 \uC002            '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	m_scen = ('Test 19 unicode   tests w/ multiples');

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 5;
	strTest = '     \uC000\uC001\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002 \uC000\uC001\uC002\uABCD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 9;
	strTest = '         \uC000\uC001\uC002 \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '         \uC000\uC001\uC002\uC501 \uABCD\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 5;
	strTest = '     \uC000\uC001 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001\uC002      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001 \uC000\uC001\uC002\uABCD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 9;
	strTest = '         \uC000\uC001 \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '         \uC000\uC001 \uABCD\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end


	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 5;
	strTest = '     \uC000\uC001\uC002\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001\uC002      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002\uC002 \uC000\uC001\uC002\uABCD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 9;
	strTest = '         \uC000\uC001\uC002\uC002 \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '         \uC000\uC001\uC002\uC002\uC501 \uABCD\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 5;
	strTest = '     \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uC501\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002\uABCD      ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 9;
	strTest = '         \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTest = '         \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC501 \uABCD\uC000\uC001\uC002       ';
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	m_scen = ('Test 20 Slightly more complex strings w/ multiple finds');

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 18;
	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001} \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 4;
	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC000\uC001} \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002\uABCD \uDBCA\uC000\uC001\uC002\u007F \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1977\uC000\uC001\uC002\uB000 \uC501\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 12;
	strTest = '           \uC501\uC000\uC001\uC002\uB000 \u1997\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1997\uC000\uC001\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002 \uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002\uABCA \uC001\uC000\uC001\uC002\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uABCA\uC000\uC001\uC002 \ufAaD\uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC000\uC000\uC001\uC002\uC501 \uC000\uC000\uC001\uC000\uC001\uC002\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 2;
	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC501\u007F \uA000\uC000\uC000\uC001\uC002\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 13;
	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC501\u007F \uC501\uDBCA\uC000\uC001\uC002\uaced\uD000 \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 18;
	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 4;
	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uABCD \uDBCA\uC000\uC001\uC002\u007F \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1977\uC000\uC001\uB000 \uC501\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 12;
	strTest = '           \uC501\uC000\uC001\uB000 \u1997\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1997\uC000\uC001 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001 \uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uABCA \uC001\uC000\uC001\uC002\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uABCA\uC000\uC001 \ufAaD\uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC000\uC000\uC001\uC501 \uC000\uC000\uC001\uC000\uC001\uC002\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 2;
	strTest = '\uDBCA\u007F\uC000\uC001\uC501\u007F \uA000\uC000\uC000\uC001\uC002\uC002\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 13;
	strTest = '           \uA000\u1977\uC000\uC001\uC501\u007F \uC501\uDBCA\uC000\uC001\uC002\uaced\uD000 \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end


	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 18;
	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 4;
	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002\uC002\uABCD \uDBCA\uC000\uC001\uC002\u007F \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1977\uC000\uC001\uC002\uC002\uB000 \uC501\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 12;
	strTest = '           \uC501\uC000\uC001\uC002\uC002\uB000 \u1997\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1997\uC000\uC001\uC002\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002\uC002\uABCA \uC001\uC000\uC001\uC002\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uABCA\uC000\uC001\uC002\uC002 \ufAaD\uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC000\uC000\uC001\uC002\uC002\uC501 \uC000\uC000\uC001\uC000\uC001\uC002\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 2;
	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC002\uC501\u007F \uA000\uC000\uC000\uC001\uC002\uC002\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 13;
	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC002\uC501\u007F \uC501\uDBCA\uC000\uC001\uC002\uaced\uD000 \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	strTemp = '\uC000\uC001';
	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 14;
	strTest = '              \uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 18;
	strTest = '              \uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 4;
	strTest = '\uD000\uABCA\uC000\uB000\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC000\uC001\uD000 \u1997\u9999\uABCD\uB000\uC000\uC001\uC002\uC000\u1977\u00AC\uABCD\uC501\uABCA\uC000\uD151 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uABCD              '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC501\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uABCD \uDBCA\uC000\uC001\uC002\u007F \uABCD\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\u1997'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1977\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uB000 \uC501\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 12;
	strTest = '           \uC501\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uB000 \u1997\uC000\uC001\uC002 \uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC501             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\u1997\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC001\uC000\uC001\uC002 \uABCA\uC000\uC001\uABCA\uC000\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 0;
	strTest = '\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \u1997\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uABCA \uC001\uC000\uC001\uC002\uC000 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 1;
	strTest = '\uABCA\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002 \ufAaD\uC000\uC001\uC002 \uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 15;
	strTest = '              \uC000\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC501 \uC000\uC000\uC001\uC000\uC001\uC002\uC000\uC001 \uC000\uC000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000'; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 2;
	strTest = '\uDBCA\u007F\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC501\u007F \uA000\uC000\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC501\u1997 \uABCA\uFfFf\uC000\uC001\uC002\uabcd\uC000\uC000\uC501\uC000\uC001\uABCA\uC000             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end

	objExp = 13;
	strTest = '           \uA000\u1977\uC000\uC001\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC002\uC501\u007F \uC501\uDBCA\uC000\uC001\uC002\uaced\uD000 \uA000\uD001\uB000\uD000\uC000\uC001\uC002\uC000\uC501\uC000\uC001\uABCA\uC000\uC001\uD151\uD000\uD151             '; 
	ArrayEqual(sCat+strTest, objExp, strTest.search(regPat));
	@if (!@_fast)
		regVerify(sCat+strTest, regExp, strTest.search(regPat));
	@end


/*****************************************************************************/
	apEndTest();
}


srcb2275();


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
