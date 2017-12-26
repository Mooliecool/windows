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


var iTestID = 170667;

/* -------------------------------------------------------------------------
  Test: 	number02
   
  Product:	JScript
 
  Area:		Number
 
  Purpose:	bug regression
 
  Scenarios:    133071, 158327, 165675, 226638
		
  Notes: 	
 ---------------------------------------------------------------------------
  
 
	[00]	29-Mar-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/

function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Failed: "+sMes+" ", sExp+" ", sAct, sBug);
}

function number02(){
	
	apInitTest("number02");
	
	apInitScenario("Regression - Number gives a type mismatch for new Number");
	var n: Number = new Number(42);
	verify(n, 42, "Number gives a type mismatch for new Number", "133071");
	
	apInitScenario("Regression - assigning large values wraps value to negative");
	var y = 17615012534505900000;
	verify(y, 17615012534505900000, "assigning large values wraps value to negative", "158327");	
	
	apInitScenario("Regression - hard integer doesn't compare against literal with >= ");
	var x : int;
	for (x=8;x>=0;x--) {
		if ( x == -1) {
			apLogFailInfo("Failed: ", 0, x, "165675");
			break;
		}
	}
	
	apInitScenario("Regression - JS7 Static initializers incorrect for hex literals");
	var z = { 0x10 : "hex", 017 : "oct" };
	verify( z[15], "oct", "Static initializers incorrect for hex literals", "226638");
	verify( z["017"], undefined, "Static initializers incorrect for hex literals", "226638");
	verify( z[16], "hex", "Static initializers incorrect for hex literals", "226638");
	verify( z["0x10"],undefined, "Static initializers incorrect for hex literals", "226638");

	apEndTest();
}



number02();


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
