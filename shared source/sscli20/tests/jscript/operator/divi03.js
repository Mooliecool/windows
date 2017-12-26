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


var iTestID = 52920;

//this file tests for various divide by 0 conditions
//the first set of tests creates infinity conditions
//the second set creates -Infinity conditions
//the third set creates NaN conditions like [0/0]

//the answers as stored in psuedo constants shown below


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

var m_scen = "";

function divi03() 
{
    apInitTest("divi03 ");


// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var infinit;
        var n_infinit;
     
@end       

    //expected answer constants
    infinit = Number.POSITIVE_INFINITY;
    n_infinit= Number.NEGATIVE_INFINITY;

    
//----------------------------------------------------------------------------
   
//this section of code generates (+)Infinities
apInitScenario("1. Positive Infinity Testing");
	
    m_scen = "Positive Infinity Testing";

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var sExp;
        var sCat;
        var sAct;
     
@end       

	sExp = infinit;

	sCat = "2/0";
	sAct = 2/0;
	if (sAct != sExp)
	    apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-2/-0";
	sAct = -2/-0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "120108");
	
	sCat = "\"2\"/0";
	sAct = "2"/0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");
	
	sCat = "2/\"0\"";
	sAct = 2/"0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2\"/\"0\"";
	sAct = "2"/"0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var x;
        var y;
     
@end       

	x =2;
	y= 0;
	sCat = "x = " + x + " y = " + y + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	y= -0;
	x =-2;
	sCat = "x = " + x + " y = -0" + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "120108");
	
//float
	sCat = "2.0/0.0";
	sAct = 2.0/0.0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-2.0/-0.0";
	sAct = -2.0/-0.0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2.0\"/0";
	sAct = "2.0"/0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "2/\"0.0\"";
	sAct = 2/"0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2\"/\"0.0\"";
	sAct = "2"/"0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x =2;
	y = 0.0;
	sCat = "x = " + x + " y = " + y + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	y = -0.0;
	x = -2.0;

	sCat = "x = " + x + " y = -0.0" + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "120108");

	sCat = "1/Math.round(0)";
	sAct = 1/Math.round(0);
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");
	
	sCat = "-1/Math.round(-0)";
	sAct = -1/Math.round(-0);
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");



//this section of code generates -Infinities
apInitScenario("2. Negative Infinity Testing");
	
	m_scen = "Negative Infinity Testing";
	sExp = n_infinit;

	sCat = "2/-0";
	sAct = 2/-0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");
	
	sCat = "-2/0";
	sAct = -2/0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"-2\"/0";
	sAct = "-2"/0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2\"/-0";
	sAct = "2"/-0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "2/\"-0\"";
	sAct = 2/"-0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-2/\"0\"";
	sAct = -2/"0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"-2\"/\"0\"";
	sAct = "-2"/"0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2\"/\"-0\"";
	sAct = "2"/"-0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x =-2;
	y = 0;
	sCat = "x = " + x + " y = " + y + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x =2;
	y = -0;
	sCat = "x = " + x + " y = -0" + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x = "-" + "2";
	y= 0;
	sCat = "x = " + x + " y = " + y + " x/y string added -2";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	y= "-" + "0";
	x =2;
	sCat = "x = " + x + " y = " + y + " x/y string added -0";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

//float
	sCat = "2/-0.0";
	sAct = 2/-0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-2.0/0.0";
	sAct = -2.0/0.0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"-2.0\"/0";
	sAct = "-2.0"/0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2.0\"/-0";
	sAct = "2.0"/-0;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "2/\"-0.0\"";
	sAct = 2/"-0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-2/\"0.0\"";
	sAct = -2/"0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"-2.0\"/\"0.0\"";
	sAct = "-2.0"/"0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"2.0\"/\"-0.0\"";
	sAct = "2.0"/"-0.0";
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x =-2.0;
	y= 0.0;
	sCat = "x = " + x + " y = 0.0" + " x/y";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	x = "-" + "2" + ".0";
	y= 0;
	sCat = "x = " + x + " y = " + y + " x/y string added -2.0";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	y= "-" + "0" + ".0";
	x =2;
	sCat = "x = " + x + " y = " + y + " x/y string added -0.0";
	sAct = x/y;
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-1/Math.round(0)";
	sAct = -1/Math.round(0);
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "1/Math.round(-0)";
	sAct = 1/Math.round(-0);
	if (sAct != sExp)
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

//This section of code tests various not a number conditions with divide by 0's
apInitScenario("3. Not a Number Testing");
	
    m_scen = "Not a Number Testing";
	
	sCat = "0/0";
	sAct = 0/0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-0/0";
	sAct = -0/0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "0/-0";
	sAct = 0/-0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-0/-0";
	sAct = -0/-0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"0\"/0";
	sAct = "0"/0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"0\"/\"0\"";
	sAct = "0"/"0";
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "null / \"0\"";
	sAct = null / "0";
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "null / 0";
	sAct = null / 0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "\"script\" / \"0\"";
	sAct = "script" / "0";
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");
	
	sCat = "\"script\" / 0";
	sAct = "script" / 0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "new Array(1,2,3) / \"0\"";
	sAct = new Array(1,2,3) / "0";
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "new Array(1,2,3) / 0";
	sAct = new Array(1,2,3) / 0;
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-0/Math.round(-0)";
	sAct = -0/Math.round(-0);
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "0/Math.round(0)";
	sAct = 0/Math.round(0);
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "-0/Math.round(0)";
	sAct = -0/Math.round(0);
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");

	sCat = "0/Math.round(-0)";
	sAct = 0/Math.round(-0);
	if (!isNaN(sAct))
		apLogFailInfo( m_scen+(sCat.length?"--"+sCat:"")+" failed", sExp, sAct, "");


/*****************************************************************************/


    apEndTest();

}




divi03();


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
