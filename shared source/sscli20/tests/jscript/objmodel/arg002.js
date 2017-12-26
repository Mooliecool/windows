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


var iTestID = 52713;

//This is tests whether functions correctly report that
//unassigned parameters are trerated as undefined values

function verifyNaN(sCat)
{
	//this function verifies that all answers
	//are either undefined or NaN
	//contingent on the fact that isNaN's spec doesn't change

	var x = 1;

	var nExp = Number.NaN;
	var nAct = eval(sCat);

	if (!isNaN(nAct))
		apLogFailInfo(m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp, nAct, "");
}

function isTrue(sCat)
{
	//verifies that boolean functions operate properly

	var nExp = true;
	var nAct = eval(sCat);

	if (!nAct)
		apLogFailInfo(m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp, nAct, "");
}

function myFunction1(p1)
{
	return(p1*1)
}

function myFunction2(p1, p2)
{
	return(p2*1)
}

function myFunction3(p1, p2, p3)
{
	return(p3*1)
}

function myFunction4(p1, p2, p3, p4)
{
	return(p4*1)
}

function myFunction5(p1, p2, p3, p4, p5)
{
	return(p5*1)
}

function myFunction20(p1, p2, p3, p4, p5,p6, p7, p8, p9, p10,p11, p12, p13, p14, p15, p16, p17, p18, p19, p20)
{
	return(p20*1)
}

var varFunc1 = new Function('p1', 'return(p1*1)');
var varFunc2 = new Function('p1','p2', 'return(p2*1)');
var varFunc3 = new Function('p1','p2','p3', 'return(p3*1)');
var varFunc4 = new Function('p1','p2','p3','p4', 'return(p4*1)');
var varFunc5 = new Function('p1','p2','p3','p4','p5', 'return(p5*1)');
var varFunc20 = new Function('p1, p2, p3, p4, p5,p6, p7, p8, p9, p10,p11, p12, p13, p14, p15, p16, p17, p18, p19, p20', 'return(p20*1)');

var m_scen = '';

function arg002()
{
	apInitTest("arg002 ");

	//All Math Functions
	apInitScenario("1. Math Functions");
	m_scen = 'Math Functions ';
	verifyNaN('Math.abs()');
	verifyNaN('Math.acos()');
	verifyNaN('Math.asin()');
	verifyNaN('Math.atan()');
	verifyNaN('Math.atan2()');	
	verifyNaN('Math.atan2(x)');
	verifyNaN('Math.ceil()');
	verifyNaN('Math.cos()');
	verifyNaN('Math.exp()');
	verifyNaN('Math.floor()');
	verifyNaN('Math.log()');

       @cc_on 
       @if (@_jscript_version < 5.5)
	     verifyNaN('Math.max()');
	     verifyNaN('Math.max(x)');
	     verifyNaN('Math.min()');
	     verifyNaN('Math.min(x)');
       @end 
	
	verifyNaN('Math.pow()');
	verifyNaN('Math.pow(x)');
	verifyNaN('Math.round()');
	verifyNaN('Math.sin()');
	verifyNaN('Math.sqrt()');
	verifyNaN('Math.tan()');

	//BuiltIns
	apInitScenario("2. Built In Functions");
	m_scen = 'Built In Functions ';

	verifyNaN('eval()');
	verifyNaN('parseInt()');
	verifyNaN('parseFloat()');
	verifyNaN('escape()');
	verifyNaN('unescape()');
	isTrue('isNaN()');
	isTrue('!isFinite()');
	
	//Custom made functions
	apInitScenario("3. Custom Built Functions");
	m_scen = 'Custom Built Functions ';

	verifyNaN('myFunction1()');
	verifyNaN('myFunction2()');
	verifyNaN('myFunction2(x)');
	verifyNaN('myFunction3()');
	verifyNaN('myFunction3(x)');
	verifyNaN('myFunction3(x, x)');
	verifyNaN('myFunction4()');
	verifyNaN('myFunction4(x)');
	verifyNaN('myFunction4(x, x)');
	verifyNaN('myFunction4(x, x, x)');
	verifyNaN('myFunction5()');
	verifyNaN('myFunction5(x)');
	verifyNaN('myFunction5(x, x)');
	verifyNaN('myFunction5(x, x, x)');
	verifyNaN('myFunction5(x, x, x, x)');
	verifyNaN('myFunction20(x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('myFunction20(x,x,x,x,x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('myFunction20(x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('myFunction20(x,x,x,x,x,x,x)');
	verifyNaN('myFunction20(x,x,x)');
	verifyNaN('myFunction20(x)');
	verifyNaN('myFunction20()');
	
	//Function Objects
	apInitScenario("4. Custom Built Function Objects");
	m_scen = 'Custom Built Function Objects ';

	verifyNaN('varFunc1()');
	verifyNaN('varFunc2()');
	verifyNaN('varFunc2(x)');
	verifyNaN('varFunc3()');
	verifyNaN('varFunc3(x)');
	verifyNaN('varFunc3(x, x)');
	verifyNaN('varFunc4()');
	verifyNaN('varFunc4(x)');
	verifyNaN('varFunc4(x, x)');
	verifyNaN('varFunc4(x, x, x)');
	verifyNaN('varFunc5()');
	verifyNaN('varFunc5(x)');
	verifyNaN('varFunc5(x, x)');
	verifyNaN('varFunc5(x, x, x)');
	verifyNaN('varFunc5(x, x, x, x)');
	verifyNaN('varFunc20(x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('varFunc20(x,x,x,x,x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('varFunc20(x,x,x,x,x,x,x,x,x,x,x)');
	verifyNaN('varFunc20(x,x,x,x,x,x,x)');
	verifyNaN('varFunc20(x,x,x)');
	verifyNaN('varFunc20(x)');
	verifyNaN('varFunc20()');
/*****************************************************************************/


    apEndTest();

}

arg002();


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
