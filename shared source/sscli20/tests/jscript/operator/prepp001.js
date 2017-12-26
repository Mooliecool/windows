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


var iTestID = 52924;


//one scenario
//make sure all circumstances of the pre ++ operator produce a typeof "number"


// Added to handle strict mode in JScript 7.0
@cc_on 
   	@if (@_fast)  
		var m_scen;                 
	 
@end       


function obFoo() 
{
}
	

function preIncrementTest (token)
{
	/*	this function takes a parameter and tries to push
		it into a number using the pre ++ operator*/

	// Added to handle strict mode in JScript 7.0
	@cc_on 
   		@if (@_fast)  
    		var sCat;             
   		 
	@end       

	var prevTokenVal = token;
	var strTmp;

	if (typeof token == "string")
		strTmp = "'" + token+ "'";
	else
		strTmp = token;

	var stExp = "number";
	++token;
	var stAct = typeof token;
		
	sCat =   "++" +strTmp;
	if (stAct != stExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", stExp,stAct, "");
}


function prepp001() 
{
	var p_infinity = Number.POSITIVE_INFINITY;
	var n_infinity = Number.NEGATIVE_INFINITY;

    apInitTest("prepp001 ");

	//integers

	apInitScenario("1. Pre ++ with int conversion");
	m_scen = "Pre ++ with int conversion";

	preIncrementTest(+3);
	preIncrementTest(+0);
	preIncrementTest(3);
	preIncrementTest(0);
	preIncrementTest(-0);
	preIncrementTest(-3);

	apInitScenario("2. Pre ++ with string int conversion");
	m_scen = "Pre ++ with string int conversion";

	preIncrementTest("+3");
	preIncrementTest("+0");
	preIncrementTest("3");
	preIncrementTest("0");
	preIncrementTest("-0");
	preIncrementTest("-3");

	preIncrementTest("   	 	 	 	 			  +3");
	preIncrementTest("   	 	 	 	 			  +0");
	preIncrementTest("   	 	 	 	 			  3");
	preIncrementTest("   	 	 	 	 			  0");
	preIncrementTest("   	 	 	 	 			  -0");
	preIncrementTest("   	 	 	 	 			  -3");

	preIncrementTest("-000000000000000000000000000000");
	preIncrementTest("-000000000000000000000000000003");
	preIncrementTest("+000000000000000000000000000000");
	preIncrementTest("+000000000000000000000000000003");
	preIncrementTest("000000000000000000000000000000");
	preIncrementTest("000000000000000000000000000003");

	//float

	apInitScenario("3. Pre ++ with float conversion");
	m_scen = "Pre ++ with float conversion";

	preIncrementTest(3.727);
	preIncrementTest(.727);
	preIncrementTest(0.727);	
	preIncrementTest(+3.727);
	preIncrementTest(+.727);
	preIncrementTest(+0.727);	
	preIncrementTest(-.727);
	preIncrementTest(-0.727);	
	preIncrementTest(-3.727);	

	apInitScenario("4. Pre ++ with string float conversion");
	m_scen = "Pre ++ with string float conversion";

	preIncrementTest("3.727");
	preIncrementTest(".727");
	preIncrementTest("0.727");	
	preIncrementTest("+3.727");
	preIncrementTest("+.727");
	preIncrementTest("+0.727");	
	preIncrementTest("-.727");
	preIncrementTest("-0.727");	
	preIncrementTest("-3.727");

	preIncrementTest("   	 	 	 	 			  3.727");
	preIncrementTest("   	 	 	 	 			  .727");
	preIncrementTest("   	 	 	 	 			  0.727");	
	preIncrementTest("   	 	 	 	 			  +3.727");
	preIncrementTest("   	 	 	 	 			  +.727");
	preIncrementTest("   	 	 	 	 			  +0.727");	
	preIncrementTest("   	 	 	 	 			  -.727");
	preIncrementTest("   	 	 	 	 			  -0.727");	
	preIncrementTest("   	 	 	 	 			  -3.727");


	preIncrementTest("00000000000000000000000000003.727");
	preIncrementTest("00000000000000000000000000000.727");
	preIncrementTest("-0000000000000000000000000000.727");
	preIncrementTest("-0000000000000000000000000003.727");
	preIncrementTest("+0000000000000000000000000000.727");
	preIncrementTest("+0000000000000000000000000003.727");

//infinities
	apInitScenario("5. Pre ++ with Infinty conversion");
	m_scen = "Pre ++ with Infinity conversion";

	preIncrementTest(Number.POSITIVE_INFINITY);
	preIncrementTest(Number.NEGATIVE_INFINITY);

	apInitScenario("6. Pre ++ with string Infinity conversion");
	m_scen = "Pre ++ with string Infinity conversion";

	preIncrementTest(String(Number.POSITIVE_INFINITY));
	preIncrementTest(String(Number.NEGATIVE_INFINITY));

	preIncrementTest("   	 	 	 	 "+String(Number.POSITIVE_INFINITY));
	preIncrementTest("   	 	 	 	 "+String(Number.NEGATIVE_INFINITY));
	
//NaN

	apInitScenario("7. Pre ++ with NaN conversion");
	m_scen = "Pre ++ with NaN conversion";

	preIncrementTest('2n');
	preIncrementTest('2.054r');
	preIncrementTest('-2.054r');
	preIncrementTest('-2n');
	preIncrementTest('new arr');


	preIncrementTest('000000000000000000000002n');
	preIncrementTest('000000000000000000000002.054r');
	preIncrementTest('-00000000000000000000002.054r');
	preIncrementTest('-00000000000000000000002n');
	preIncrementTest('00000000000000000000000new arr');

	preIncrementTest(new Array(1,2,3));


/*****************************************************************************/


    apEndTest();

}




prepp001();


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
