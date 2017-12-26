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


var iTestID = 53133;


//This files purpose is to verify that constants can be assigned to
//without actually changing their value [or causing an error message]

//two scenarios
//verify Math object's constants
//verify Number objects's constants

function cstmod01() {

	apInitTest("constMod001  -  does not run in fast mode");

	var nExp, m_scen, sCat;
	var nAct;
	var y;
	
	apInitScenario("1. Math Constant Modification");
@if(!@_fast)
	m_scen = 'Math Constant Modification';
	
	sCat = 'Math.E';
	nExp = Math.E;
	delete Math.E;
	Math.E = 0;
	y = Math.E = new Function("return 1");
	Math.E = 2.54;
	nAct  = Math.E;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");

	sCat = 'Math.LN10';
	nExp = Math.LN10;
	delete Math.LN10;
	Math.LN10 = 0;
	y = Math.LN10 = new Function("return 1");
	Math.LN10 = 2.54;
	nAct  = Math. LN10;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");
	
	sCat = 'Math.LN2';
	nExp = Math.LN2;
	delete Math.LN2;
	Math.LN2 = 0;
	y = Math.LN2 = new Function("return 1");
	Math.LN2 = 2.54;
	nAct = Math.LN2;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");


	sCat = 'Math.LOG10E';
	nExp = Math.LOG10E;
	delete Math.LOG10E;
	Math.LOG10E = 0;
	y = Math.LOG10E = new Function("return 1");
	Math.LOG10E = 2.54;
	nAct = Math. LOG10E;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");
	
	sCat = 'Math.LOG2E';
	nExp = Math.LOG2E;
	delete Math.LOG2E;
	Math.LOG2E = 0;
	y = Math.LOG2E = new Function("return 1");
	Math.LOG2E = 2.54;
	nAct = Math.LOG2E;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");
	
	sCat = 'Math.PI';
	nExp = Math.PI;
	delete Math.PI;
	Math.PI = 0;
	y = Math.PI = new Function("return 1");
	Math.PI = 2.54;
	nAct = Math.PI;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");

	
	sCat = 'Math.SQRT1_2';
	nExp = Math.SQRT1_2;
	delete Math.SQRT1_2;
	Math.SQRT1_2 = 0;
	y = Math.SQRT1_2 = new Function("return 1");
	Math.SQRT1_2 = 2.54;
	nAct = Math.SQRT1_2;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");
	
	sCat = 'Math.SQRT2';
	nExp = Math.SQRT2;
	delete Math.SQRT2;
	Math.SQRT2 = 0;
	y = Math.SQRT2 = new Function("return 1");
	Math.SQRT2 = 2.54;
	nAct = Math.SQRT2;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");

@end

	apInitScenario("2. Number Constant Modification");
@if(!@_fast)
	m_scen = 'Number Constant Modification';

	sCat = 'Number.MAX_VALUE';
	nExp = Number.MAX_VALUE;
	delete Number.MAX_VALUE;
	Number.MAX_VALUE = 0;
	y = Number.MAX_VALUE = new Function("return 1");
	Number.MAX_VALUE = 2.54;

	nAct = Number.MAX_VALUE;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");


	sCat = 'Number.MIN_VALUE';
	nExp = Number.MIN_VALUE;
	delete Number.MIN_VALUE;
	Number.MIN_VALUE = 0;
	y = Number.MIN_VALUE = new Function("return 1");
	Number.MIN_VALUE = 2.54;
	nAct = Number.MIN_VALUE;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");
	
	sCat = 'Number.NEGATIVE_INFINITY';
	nExp = Number.NEGATIVE_INFINITY;
	delete Number.NEGATIVE_INFINITY;
	Number.NEGATIVE_INFINITY = 0;
	y = Number.NEGATIVE_INFINITY = new Function("return 1");
	Number.NEGATIVE_INFINITY = 2.54;
	nAct = Number.NEGATIVE_INFINITY;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");

	sCat = 'Number.POSITIVE_INFINITY';	
	nExp = Number.POSITIVE_INFINITY;
	delete Number.POSITIVE_INFINITY;
	Number.POSITIVE_INFINITY = 0;
	y = Number.POSITIVE_INFINITY = new Function("return 1");
	Number.POSITIVE_INFINITY = 2.54;
	nAct = Number.POSITIVE_INFINITY;
	if (nExp != nAct)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");

	sCat = 'Number.NaN';
	nExp = Number.NaN;
	delete Number.NaN;
	Number.NaN = 0;
	y = Number.NaN = new Function("return 1");
	Number.NaN = 2.54;
	nAct = Number. NaN;
	if (!isNaN(nAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp,nAct, "");


/*****************************************************************************/
@else
  //    apInitScenario("skiping scenario")
   //   apInitScenario("skiping scenario")
@end
    apEndTest();

}


cstmod01();


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
