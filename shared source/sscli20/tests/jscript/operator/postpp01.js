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


var iTestID = 52923;

//one scenario
//make sure all circumstances of the post ++ operator produce a typeof "number"


// Added to handle strict mode in JScript 7.0
@cc_on 
   	@if (@_fast)  
		var m_scen;                 
	 
@end       


function obFoo() 
{
}


function postIncrementTest (token)
{
	 /*	this function takes a parameter and tries to push
		it into a number using the post ++ operator*/

	var prevTokenVal = token;
	var strTmp;

	// Added to handle strict mode in JScript 7.0
	@cc_on 
	   	@if (@_fast)  
     		var sCat;         
	   	 
	@end       

	if (typeof token == "string")
		strTmp = "'" + token+ "'";
	else
		strTmp = token;

	var stExp = "number";
	token++;
	var	stAct = typeof token;
		
	sCat = strTmp + "++";
	if (stAct != stExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", stExp,stAct, "");
}


function postpp01() 
{
	var p_infinity = Number.POSITIVE_INFINITY;
	var n_infinity = Number.NEGATIVE_INFINITY;

    apInitTest("postpp001 ");

	//integers

	apInitScenario("1. Post ++ with int conversion");
	m_scen = "Post ++ with int conversion";
	
	postIncrementTest(+3);
	postIncrementTest(+0);
	postIncrementTest(3);
	postIncrementTest(0);
	postIncrementTest(-0);
	postIncrementTest(-3);

	apInitScenario("2. Post ++ with string int conversion");
	m_scen = "Post ++ with string int conversion";

	postIncrementTest("+3");
	postIncrementTest("+0");
	postIncrementTest("3");
	postIncrementTest("0");
	postIncrementTest("-0");
	postIncrementTest("-3");

	postIncrementTest("   	 	 	 	 			  +3");
	postIncrementTest("   	 	 	 	 			  +0");
	postIncrementTest("   	 	 	 	 			  3");
	postIncrementTest("   	 	 	 	 			  0");
	postIncrementTest("   	 	 	 	 			  -0");
	postIncrementTest("   	 	 	 	 			  -3");

	postIncrementTest("-000000000000000000000000000000");
	postIncrementTest("-000000000000000000000000000003");
	postIncrementTest("+000000000000000000000000000000");
	postIncrementTest("+000000000000000000000000000003");
	postIncrementTest("000000000000000000000000000000");
	postIncrementTest("000000000000000000000000000003");

	//float
	apInitScenario("3. Post ++ with float conversion");
	m_scen = "Post ++ with float conversion";

	postIncrementTest(3.727);
	postIncrementTest(.727);
	postIncrementTest(0.727);	
	postIncrementTest(+3.727);
	postIncrementTest(+.727);
	postIncrementTest(+0.727);	
	postIncrementTest(-.727);
	postIncrementTest(-0.727);	
	postIncrementTest(-3.727);	

	apInitScenario("4. Post ++ with string float conversion");
	m_scen = "Post ++ with string float conversion";

	postIncrementTest("3.727");
	postIncrementTest(".727");
	postIncrementTest("0.727");	
	postIncrementTest("+3.727");
	postIncrementTest("+.727");
	postIncrementTest("+0.727");	
	postIncrementTest("-.727");
	postIncrementTest("-0.727");	
	postIncrementTest("-3.727");

	postIncrementTest("   	 	 	 	 			  3.727");
	postIncrementTest("   	 	 	 	 			  .727");
	postIncrementTest("   	 	 	 	 			  0.727");	
	postIncrementTest("   	 	 	 	 			  +3.727");
	postIncrementTest("   	 	 	 	 			  +.727");
	postIncrementTest("   	 	 	 	 			  +0.727");	
	postIncrementTest("   	 	 	 	 			  -.727");
	postIncrementTest("   	 	 	 	 			  -0.727");	
	postIncrementTest("   	 	 	 	 			  -3.727");


	postIncrementTest("00000000000000000000000000003.727");
	postIncrementTest("00000000000000000000000000000.727");
	postIncrementTest("-0000000000000000000000000000.727");
	postIncrementTest("-0000000000000000000000000003.727");
	postIncrementTest("+0000000000000000000000000000.727");
	postIncrementTest("+0000000000000000000000000003.727");

//infinities
	apInitScenario("5. Post ++ with Infinty conversion");
	m_scen = "Post ++ with Infinity conversion";

	postIncrementTest(Number.POSITIVE_INFINITY);
	postIncrementTest(Number.NEGATIVE_INFINITY);

	apInitScenario("6. Post ++ with string Infinity conversion");
	m_scen = "Post ++ with string Infinity conversion";

	postIncrementTest(String(Number.POSITIVE_INFINITY));
	postIncrementTest(String(Number.NEGATIVE_INFINITY));

	postIncrementTest("   	 	 	 	 "+String(Number.POSITIVE_INFINITY));
	postIncrementTest("   	 	 	 	 "+String(Number.NEGATIVE_INFINITY));
	
//NaN
	apInitScenario("7. Post ++ with NaN conversion");
	m_scen = "Post ++ with NaN conversion";

	postIncrementTest('2n');
	postIncrementTest('2.054r');
	postIncrementTest('-2.054r');
	postIncrementTest('-2n');
	postIncrementTest('new arr');


	postIncrementTest('000000000000000000000002n');
	postIncrementTest('000000000000000000000002.054r');
	postIncrementTest('-00000000000000000000002.054r');
	postIncrementTest('-00000000000000000000002n');
	postIncrementTest('00000000000000000000000new arr');

	postIncrementTest(new Array(1,2,3));
/*****************************************************************************/


    apEndTest();

}





postpp01();


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
