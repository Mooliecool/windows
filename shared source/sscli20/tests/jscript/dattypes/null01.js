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
//@set @rotor=true;

import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;
var lInScenario;



function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "Scenario failed: "+sMes, sExp, sAct, "");
}

function null01() {
 @if(@_fast)
    var func1, func2;
 @end
    apInitTest("null01");

    
    apInitScenario("Verify null == null");
	verify(null==null, true);
	verify(null, null);
	
    apInitScenario("Verify Null var == null");
	var Null = null;
	verify(Null== null, true);
	verify(Null, Null);
	
    apInitScenario("VT_NULL == null");
	verify(NULL_DISPATCH, null);
	verify(NULL_DISPATCH == null, true);
	verify(null == NULL_DISPATCH, true);
	verify(NULL_DISPATCH, NULL_DISPATCH);
	
    apInitScenario("Verify function returns VT_NULL == null");
	func1 = new Function("return NULL_DISPATCH");
	verify(func1(),null);
	verify(func1() == NULL_DISPATCH, true);
	verify(func1() == null,true);
	
    apInitScenario("Verify VT_NULL as param returns correctly");
	func2 = new Function("x","return x");
	verify(func2(NULL_DISPATCH),null);
	verify(func2(NULL_DISPATCH) == NULL_DISPATCH, true);
	verify(func2(NULL_DISPATCH) == null,true);
	
    apInitScenario("Verify NULL_DISPATCH prop assignment");
	var x = NULL_DISPATCH;
	verify(x,null);
	var y = new Array(NULL_DISPATCH);
	verify(y[0],null);



    apEndTest();

}





null01();
if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  
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
