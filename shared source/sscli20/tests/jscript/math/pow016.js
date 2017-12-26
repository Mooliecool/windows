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


var iTestID = 53124;

var inf = 1 / 0;
var nan = 0 / 0;

function p(a)
{
	var res;

	apInitScenario(a);
	with (Math)
	{
		if(!(res = eval(a)))
			apLogFailInfo( "wrong return value",res, "true","");
	}
}

function equal(a, b)
{
	var fT = (a == b) && (1 / a == 1 / b);
	if (fT)
		return 'true';
	return 'false (' + a + ', ' + b + ', ' + (1/a) + ', ' + (1/b) + ')';
}

function pow016()
{
	apInitTest("pow016");

	// pow (nan, *)
	p("isNaN(pow(nan, nan))");
	p("isNaN(pow(nan, +inf))");
	p("isNaN(pow(nan, -inf))");
	p("isNaN(pow(nan, +1))");
	p("isNaN(pow(nan, -1))");
	p("equal(pow(nan, +0), +1)");
	p("equal(pow(nan, -0), +1)");

	// pow (*, nan)
	p("isNaN(pow(+inf, nan))");
	p("isNaN(pow(-inf, nan))");
	p("isNaN(pow(+1, nan))");
	p("isNaN(pow(-1, nan))");
	p("isNaN(pow(+0, nan))");
	p("isNaN(pow(-0, nan))");

	// pow (+inf, *)
	p("equal(pow(+inf, +inf), +inf)");
	p("equal(pow(+inf, -inf), +0)");
	p("equal(pow(+inf, +1), +inf)");
	p("equal(pow(+inf, -1), +0)");
	p("equal(pow(+inf, +0), +1)");
	p("equal(pow(+inf, -0), +1)");

	// pow (-inf, *)
	p("equal(pow(-inf, +inf), +inf)");
	p("equal(pow(-inf, -inf), +0)");
	p("equal(pow(-inf, +3), -inf)");
	p("equal(pow(-inf, -3), -0)");
	p("equal(pow(-inf, +2), +inf)");
	p("equal(pow(-inf, -2), +0)");
	p("equal(pow(-inf, +1.1), +inf)");
	p("equal(pow(-inf, -1.1), +0)");
	p("equal(pow(-inf, +0), +1)");
	p("equal(pow(-inf, -0), +1)");

	// pow (+2, *)
	p("equal(pow(+2, +inf), +inf)");
	p("equal(pow(+2, -inf), +0)");
	p("equal(pow(+2, +3), +8)");
	p("equal(pow(+2, -3), +0.125)");
	p("equal(pow(+2, +2), +4)");
	p("equal(pow(+2, -2), +0.25)");
	p("equal(pow(+2, +0), +1)");
	p("equal(pow(+2, -0), +1)");

	// pow (-2, *)
	p("equal(pow(-2, +inf), +inf)");
	p("equal(pow(-2, -inf), +0)");
	p("equal(pow(-2, +3), -8)");
	p("equal(pow(-2, -3), -0.125)");
	p("equal(pow(-2, +2), +4)");
	p("equal(pow(-2, -2), +0.25)");
	p("isNaN(pow(-2, +1.1))");
	p("isNaN(pow(-2, -1.1))");
	p("equal(pow(-2, +0), +1)");
	p("equal(pow(-2, -0), +1)");

	// pow (+1, *)
	p("isNaN(pow(+1, +inf))");
	p("isNaN(pow(+1, -inf))");
	p("equal(pow(+1, +3), +1)");
	p("equal(pow(+1, -3), +1)");
	p("equal(pow(+1, +2), +1)");
	p("equal(pow(+1, -2), +1)");
	p("equal(pow(+1, +0), +1)");
	p("equal(pow(+1, -0), +1)");

	// pow (-1, *)
	p("isNaN(pow(-1, +inf))");
	p("isNaN(pow(-1, -inf))");
	p("equal(pow(-1, +3), -1)");
	p("equal(pow(-1, -3), -1)");
	p("equal(pow(-1, +2), +1)");
	p("equal(pow(-1, -2), +1)");
	p("isNaN(pow(-1, +1.1))");
	p("isNaN(pow(-1, -1.1))");
	p("equal(pow(-1, +0), +1)");
	p("equal(pow(-1, -0), +1)");

	// pow (+0.5, *)
	p("equal(pow(+0.5, +inf), +0)");
	p("equal(pow(+0.5, -inf), +inf)");
	p("equal(pow(+0.5, +3), +0.125)");
	p("equal(pow(+0.5, -3), +8)");
	p("equal(pow(+0.5, +2), +0.25)");
	p("equal(pow(+0.5, -2), +4)");
	p("equal(pow(+0.5, +0), +1)");
	p("equal(pow(+0.5, -0), +1)");

	// pow (-0.5, *)
	p("equal(pow(-0.5, +inf), +0)");
	p("equal(pow(-0.5, -inf), +inf)");
	p("equal(pow(-0.5, +3), -0.125)");
	p("equal(pow(-0.5, -3), -8)");
	p("equal(pow(-0.5, +2), +0.25)");
	p("equal(pow(-0.5, -2), +4)");
	p("isNaN(pow(-0.5, +1.1))");
	p("isNaN(pow(-0.5, -1.1))");
	p("equal(pow(-0.5, +0), +1)");
	p("equal(pow(-0.5, -0), +1)");

	// pow (+0, *)
	p("equal(pow(+0, +inf), +0)");
	p("equal(pow(+0, -inf), +inf)");
	p("equal(pow(+0, +1), +0)");
	p("equal(pow(+0, -1), +inf)");
	p("equal(pow(+0, +0), +1)");
	p("equal(pow(+0, -0), +1)");

	// pow (-0, *)
	p("equal(pow(-0, +inf), +0)");
	p("equal(pow(-0, -inf), +inf)");
	p("equal(pow(-0, +3), -0)");
	p("equal(pow(-0, -3), -inf)");
	p("equal(pow(-0, +2), +0)");
	p("equal(pow(-0, -2), +inf)");
	p("equal(pow(-0, +1.1), +0)");
	p("equal(pow(-0, -1.1), +inf)");

	apEndTest();
}



pow016();


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
