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


var iTestID = 52666;

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



function exec1171() {
	

apInitTest("Exec1171");


	m_scen = ('Test 1 numbers   /234{4,4}/');

	apInitScenario(m_scen);
	regPat = /234{5,5}/;

	objExp = new Array('2344444');
	var regExp = new Array();
	
	var strTest = '2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 2 numbers   /234{4,4+1}/');

	apInitScenario(m_scen);
	regPat = /234{5,6}/;

	objExp = new Array('2344444');
	regExp = new Array();
	
	strTest = '2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	objExp = new Array('23444444');
	regExp = new Array();
	
	strTest = '23444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       23444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       23444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 3 numbers   /234{4,4+20}/');

	apInitScenario(m_scen);
	regPat = /234{5,25}/;

	objExp = new Array('2344444');
	regExp = new Array();
	
	strTest = '2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	objExp = new Array('23444444');
	regExp = new Array();
	
	strTest = '23444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       23444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       23444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	objExp = new Array('234444444444444444444444444');
	regExp = new Array();

	strTest = '234444444444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234444444444444444444444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234444444444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234444444444444444444444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444444444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       2344444444444444444444444444       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 4 numbers   /234{0,1}/');

	apInitScenario(m_scen);
	regPat = /234{0,1}/;
	objExp = new Array('234');
	regExp = new Array();
	
	strTest = '234';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '       234       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = '2344              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344                         ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTest = '2344444444444444444444              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344444444444444444444';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344444444444444444444              ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444                         ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 5 Slightly more complex strings');

	strTest = '234 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              234 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '234 23 4 23424272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '              7234 23 4 234242723423'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '7234 23 4 234242723423 4            '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTemp = '23'
	strTest = '23'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              23 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '23 23 4 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '              723 23 4 234242723423'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '723 23 4 234242723423 4            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '234'
	strTest = '2344 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '2344 23 4 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '              72344 23 4 234242723423'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '72344 23 4 234242723423 4            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '234';
	strTest = '2344444444444444444444 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344444444444444444444 23 4 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '2344444444444444444444 23 4 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end
	
	strTest = '              72344444444444444444444 23 4 234242723423'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '72344444444444444444444 23 4 234242723423 4            '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 6 numbers   tests w/ multiples');

	strTest = '234 234';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     234 234';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234 234      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '7234 2345      ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         234 234       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         2347 5234       ';
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '23';
	strTest = '23 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     23 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23 234      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '723 2345      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         23 234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         23 5234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTemp = '234';
	strTest = '2344 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     2344 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344 234      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '72344 2345      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         2344 234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         23447 5234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '234';
	strTest = '2344444444444444444444 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '     2344444444444444444444 234';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444 234      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '72344444444444444444444 2345      ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         2344444444444444444444 234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '         23444444444444444444447 5234       ';
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	m_scen = ('Test 7 Slightly more complex strings w/ multiple finds');

	strTest = '234 234 23424272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              234 234 23424272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234 234 23424272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              5427234235 885723421457423 234242723425'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '5427234235 885723421457423 234242723425              '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              72345 62340 5234242723428'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '12347 7234 23424272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           72347 8234 234242723427             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '8234 3234 4234272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              8234 3234 423424272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '234 234 234272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              82344 32342 234272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '4234 3234 234272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              22347 22323423 22234272342'; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '6023470 4223478 4623472272342             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           4123470 7623445 42752342723423353             '; 
	ArrayEqual(sCat+strTest, objExp, regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '23';
	strTest = '23 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              23 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23 234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              542723235 885723421457423 234242723425'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '542723235 885723421457423 234242723425              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              7235 62340 5234242723428'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '1237 7234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           7237 8234 234242723427             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '823 3234 4234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              823 3234 423424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '23 234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              8235 32342 234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '423 3234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2237 22323423 22234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '602370 42234478 4623472272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           412370 7623445 42752342723423353             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


	strTemp = '234';
	strTest = '2344 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344 234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              54272344235 885723421457423 234242723425'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '54272344235 885723421457423 234242723425              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              723445 62340 5234242723428'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '123447 7234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           723447 8234 234242723427             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '82344 3234 4234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              82344 3234 423424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344 234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              823444 32342 234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '42344 3234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              223447 22323423 22234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '60234470 42234478 4623472272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           41234470 7623445 42752342723423353             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTemp = '234';
	strTest = '2344444444444444444444 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              2344444444444444444444 234 23424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444 234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              54272344444444444444444444235 885723421457423 234242723425'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '54272344444444444444444444235 885723421457423 234242723425              '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              723444444444444444444445 62340 5234242723428'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '123444444444444444444447 7234 23424272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           723444444444444444444447 8234 234242723427             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '82344444444444444444444 3234 4234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              82344444444444444444444 3234 423424272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '2344444444444444444444 234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              823444444444444444444444 32342 234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '42344444444444444444444 3234 234272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '              223444444444444444444447 22323423 22234272342'; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '60234444444444444444444470 42234444444444444444444478 4623472272342             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end

	strTest = '           41234444444444444444444470 7623445 42752342723423353             '; 
	ArrayEqual(sCat+strTest, new Array(strTemp), regPat.exec(strTest));
	@if (!@_fast)
		@if (!@_fast)
		regVerify(sCat+strTest, regExp, regPat.exec(strTest));
	@end
	@end


/*****************************************************************************/
	apEndTest();
}


exec1171();


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
