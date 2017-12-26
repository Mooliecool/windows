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


var iTestID = 52921;

//four scenarios
//double check precision of mod with standard numbers
//generate infinities with mod to verify +/- 0
//generate -infinities with mod to verify +/- 0
//generate NaN's to verify that mod conforms with the spec


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

var m_scen = "";

// Added to handle strict mode in JScript 7.0
@cc_on 
	@if (@_fast)  
       var p_infinity, n_infinity;
   	 
@end       


	//computes mod for fx%fy
	function modtest(fx, fy, ExpectedAnswer)
	{	
		// Added to handle strict mode in JScript 7.0
		@cc_on 
		    @if (@_fast)  
    			var sCat;
		   	 
		@end       

		var nAct = fx%fy;
		var nExp = ExpectedAnswer;

		sCat = "x =" + fx + " y = " + fy + " x % y";
		if (nAct != nExp)
			apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp, nAct, "");
	}


	//computes the answer to ftop/(fx%fy)
	function infinityModtest(ftop, fx, fy)
	{	
		// Added to handle strict mode in JScript 7.0
		@cc_on 
		    @if (@_fast)  
    			var strTop, strFx, strFy, nExp;
		   	 
		@end       
		

		if ((typeof ftop) == "string")
			strTop = "'" + ftop + "'";
		else
			strTop = ftop;
		
		if ((typeof fx) == "string")
			strFx = "'" + fx+ "'";
		else
			strFx = fx;

		if ((typeof fy) == "string")
			strFy = "'" + fy + "'";
		else
			strFy = fy;
			 
		var nAct = ftop/(fx%fy);
								//cases when the answer should be infinity
		if (((fx > 0) && (ftop > 0)) || ((fx < 0) && (ftop < 0)))
			nExp = p_infinity;
		else
			nExp = n_infinity;;

		var sCat = "top = " + strTop + " x =" + strFx + " y = " + strFy + " top/(x % y)";
		if (nAct != nExp)
			apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp, nAct, "");
	}

	function nanModtest(fx, fy)
	{	//computes mod for fx%fy: it is assumed the answer should be NaN

		var nAct = fx%fy;
		var nExp = Number.NaN;
		var sCat = "x =" + fx + " y = " + fy + " x % y";
		if (!isNaN(nAct))
			apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", nExp, nAct, "");
	}




function mod001() 
{
// Added to handle strict mode in JScript 7.0
@cc_on 
	@if (@_fast)  
    	var x, y;          
   	 
@end       


    apInitTest("mod001 ");

	p_infinity = Number.POSITIVE_INFINITY;
	n_infinity = Number.NEGATIVE_INFINITY;
	

apInitScenario("1. Mod tests that produce numeric answers");
	m_scen = "Mod tests that produce numeric answers";

	//Integer Mod
	x = 5;
	y = 2;
	modtest(x, y, 1);
	modtest(y, x, y);
	modtest(y, y, 0);
	modtest(0, y, 0);	
	modtest(0, p_infinity, 0);	
	modtest(0, n_infinity, 0);	
	modtest(-x, n_infinity, -x);	
	modtest(x, n_infinity, x);
	modtest(-x,p_infinity, -x);
	modtest(x,p_infinity, x);

	x = -5;
	y = 2;
	modtest(x, y, -1);
	modtest(y, x, y);
	modtest(x, x, -0);
	modtest(0, x, 0);	

	x = -5;
	y = -2;
	modtest(x, y, -1);
	modtest(y, x, -2);
	modtest(x, x, 0);
	modtest(0, x, 0);

// Float % Int
	x = 5.625;
	y = 2;	

	modtest(x, y, 1.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(0, x, 0);
	modtest(0, y, 0);
	modtest(-x, n_infinity, -x);	
	modtest(x, n_infinity, x);
	modtest(-x,p_infinity, -x);
	modtest(x,p_infinity, x);

	x = -5.625;
	y = 2;
	modtest(x, y, -1.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(0, x, 0);

	x = 5.625;
	y = -2;	
	modtest(x, y, 1.625);
	modtest(y, x, y);
	modtest(y, y, 0);
	modtest(0, y, 0);

	x = -5.625;
	y = -2;	
	modtest(x, y, -1.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(y, y, 0);
	modtest(0, x, 0);

// Float % Float
	x = 5.625;
	y = 2.5;

	modtest(x, y, 0.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(0, x, 0);
	modtest(0, y, 0);
	modtest(-x, n_infinity, -x);	
	modtest(x, n_infinity, x);
	modtest(-x,p_infinity, -x);
	modtest(x,p_infinity, x);

	x = -5.625;
	y = 2.5;	
	modtest(x, y, -0.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(0, x, 0);

	x = 5.625;
	y = -2.5;	
	modtest(x, y, 0.625);
	modtest(y, x, y);
	modtest(y, y, 0);
	modtest(0, y, 0);

	x = -5.625;
	y = -2.5;	
	modtest(x, y, -0.625);
	modtest(y, x, y);
	modtest(x, x, 0);
	modtest(0, x, 0);


	//the Prefix S is used used in variable names to denote special values

apInitScenario("2. Mod tests that produce +Infinity answers");
	
    m_scen = "Mod tests that produce +Infinity answers";
	
	x = 5;
	y = 5;
	infinityModtest(2, x, y);
	infinityModtest(-2, -x, y);
	infinityModtest('2', x, y);
	infinityModtest(-2, '-5', '5');
	infinityModtest(-2, -5, '5');
	infinityModtest(-2, '-5', 5);
	infinityModtest(2, '5', '5');
	infinityModtest(2, 5, '5');
	infinityModtest(2, '5', 5);

	//float
	infinityModtest(2.0, 2.5, 2.5);
	infinityModtest(-2.0, -2.5, 2.5);
	infinityModtest('2.0', 2.5, 2.5);
	infinityModtest(2, '2.5', '2.5');
	infinityModtest(2, 2.5, '2.5');
	infinityModtest(2, '2.5', 2.5);
	infinityModtest('-2.0', -2.5, 2.5);
	infinityModtest(-2, '-2.5', '2.5');
	infinityModtest(-2, -2.5, '2.5');
	infinityModtest(-2, '-2.5', 2.5);

	infinityModtest('2', '2.5', '2.5');
	infinityModtest(2, 2.5, 2.5);	

apInitScenario("3. Mod tests that produce -Infinity answers");
	
    m_scen = "Mod tests that produce -Infinity answers";
	
	infinityModtest(2, -5, 5);
	infinityModtest(-2, 5, 5);
	infinityModtest('-2', 5,5);
	infinityModtest('2', -5,5);
	infinityModtest(2, '-5', '5');
	infinityModtest(-2, '5', '5');
	infinityModtest('2', '-5', '5');
	infinityModtest('2', '-' + '5', '5');
	infinityModtest('-2', '5', '5');
	infinityModtest("-" + "2", 5, 5);

	//float
	infinityModtest(2, -2.5, 2.5);
	infinityModtest(-2.0, 2.5, 2.5);
	infinityModtest(2.0, -2.5, 2.5);
	infinityModtest('-2.0', 5, 5);
	infinityModtest('2.0', -5, 5);
	infinityModtest('2', -2.5, 2.5);
	infinityModtest(-2, '2.5', '2.5');
	infinityModtest('-2.0', '2.5', '2.5');
	infinityModtest('2.0', '-2.5', '2.5');
	infinityModtest("-" + "2" + ".0", 5, 5);
	infinityModtest(2, '-'+'2'+'.5', '2'+'.5');

apInitScenario("4. Mod tests that produce NaN answers");
	
    m_scen = "Mod tests that produce NaN answers";
	
//number % NaN
	nanModtest(2, Number.NaN);
	nanModtest(-2, Number.NaN);
	nanModtest(2, 'asdkjhaskd');
	nanModtest(-2, 'asdkjhaskd');
	nanModtest(2, new Array(1,2,3));
	nanModtest(-2, new Array(1,2,3));

//Nan%number

	nanModtest(Number.NaN, 2);
	nanModtest(Number.NaN, -2);
	nanModtest('asdkjhaskd', 2);
	nanModtest('asdkjhaskd', -2);
	nanModtest(new Array(1,2,3), 2);
	nanModtest(new Array(1,2,3), -2);

//Nan%NaN

	nanModtest(Number.NaN, Number.NaN);
	nanModtest('asdkjhaskd', Number.NaN);
	nanModtest(new Array(1,2,3), Number.NaN);

//Infinty%number
	nanModtest(p_infinity, 3);
	nanModtest(n_infinity, 3);
	nanModtest(p_infinity, -3);
	nanModtest(n_infinity, -3);
	nanModtest(p_infinity, 0);
	nanModtest(n_infinity, 0);
	nanModtest(p_infinity, -0);
	nanModtest(n_infinity, -0);

//number%0
	nanModtest(3, 0);
	nanModtest(-3, 0);
	nanModtest(3, -0);
	nanModtest(-3, -0);
	nanModtest(0, 0);
	nanModtest(0, -0);
	nanModtest(-0, 0);
	nanModtest(-0, -0);

/*****************************************************************************/


    apEndTest();

}



mod001();


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
