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


var iTestID = 51734;


function verify(vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

function mkAr(size,val)
{
	var i;
	var ar = new Array(size);

	for(i=0; i<size; i++)
		ar[i]=val;

	return ar;
}

function mkSt(size,val,del)
{
	var i;
	var st = "";

	for(i=0; i<size-1; i++)
		st=st+val+del;
	if(size != 0)
		st=st+val;
	
	return st;
}

function arjoin05(){
@if(@_fast)
    var sz, el, del;
@end
    apInitTest("arJoin05");
    
    //----------------------------------------------------------------------------
    apInitScenario("test join on large array: chars");
	sz=40000,el="a",del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on large array: ints");
	sz=40000,el=1,del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on large array: strings");
	sz=40000,el="hey",del="";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

	//----------------------------------------------------------------------------
    apInitScenario("test join on medium size array: chars");
	sz=640,el="a",del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on medium size array: ints");
	sz=640,el=1,del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on medium size array: strings");
	sz=640,el="hey",del="";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on zero size array: chars");
	sz=0,el="a",del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on zero size array: ints");
	sz=0,el=1,del="|";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);

    //----------------------------------------------------------------------------
    apInitScenario("test join on zero size array: strings");
	sz=0,el="hey",del="";
    verify(mkAr(sz,el).join(del),mkSt(sz,el,del), null);
	apEndTest();
}


arjoin05();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 0;

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
