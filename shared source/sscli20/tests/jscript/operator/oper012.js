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


var iTestID = 65332;

var undef;
var nullvar = null;

function compare(avar, num)
{
	if ( avar == num )
		return 1;
	if ( avar > num)
		return 2;
	if ( avar < num)
		return 3;
	return 0;
}

function oper012()
{
	var res;

	apInitTest( "oper012");

//--------------------------------------------------------------------
	apInitScenario( "verify (null,0): '==' ->false, '<' ->false, '>' ->false");
	if((res=compare(null, 0)) != 0)
		apLogFailInfo( "Wrong comparison result:",0,res,"");

//--------------------------------------------------------------------
	apInitScenario( "verify (null,-5): '==' ->false, '<' ->false, '>' ->true");
	if((res=compare(null, -5)) != 2)
		apLogFailInfo( "Wrong comparison result:",2,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null,5): '==' ->false, '<' ->true, '>' ->false");
	if((res=compare(null, 5)) != 3)
		apLogFailInfo( "Wrong comparison result:",3,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null,'5.5'): '==' ->false, '<' ->true, '>' ->false");
	if((res=compare(null, "5.5")) != 3)
		apLogFailInfo( "Wrong comparison result:",3,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (undef,0): '==' ->false, '<' ->false, '>' ->false");
	if((res=compare(undef, 0)) != 0)
		apLogFailInfo( "Wrong comparison result:",res,0,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (undef,-5): '==' ->false, '<' ->false, '>' ->false");
	if((res=compare(undef, -5)) != 0)
		apLogFailInfo( "Wrong comparison result:",res,0,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (undef,5): '==' ->false, '<' ->false, '>' ->false");
	if((res=compare(undef, 5)) != 0)
		apLogFailInfo( "Wrong comparison result:",res,0,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (undef,null): '==' ->true, '<' ->false, '>' ->false");
	if((res=compare(undef, null)) != 1)
		apLogFailInfo( "Wrong comparison result:",res,0,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (nullvar+1) == 1");
	if((res= nullvar + 1) != 1)
		apLogFailInfo( "Wrong result:",1,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (nullvar-1) == -1");
	if((res= nullvar - 1) != -1)
		apLogFailInfo( "Wrong result:",-1,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (nullvar*1) == 0");
	if((res= nullvar * 1) != 0)
		apLogFailInfo( "Wrong result:",0,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (nullvar/1) == 0");
	if((res= nullvar / 1) != 0)
		apLogFailInfo( "Wrong result:",0,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null+1) == 1");
	if((res= null + 1) != 1)
		apLogFailInfo( "Wrong result:",1,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null-1) == -1");
	if((res= null - 1) != -1)
		apLogFailInfo( "Wrong result:",-1,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null*1) == 0");
	if((res= null * 1) != 0)
		apLogFailInfo( "Wrong result:",0,res,"");
//--------------------------------------------------------------------
	apInitScenario( "verify (null/1) == 0");
	if((res= null / 1) != 0)
		apLogFailInfo( "Wrong result:",0,res,"");
//--------------------------------------------------------------------
	apInitScenario( );

	var obj, x;

	obj = new Object;
	x = 1;
	obj[x] = ++x;
	if(res=obj[1] != 2)
		apLogFailInfo( "Wrong result:",2,res,"");

	obj = new Object;
	x = 1;
	with (obj)
		x = obj.x = 2;
	if(x != 2)
		apLogFailInfo( "Wrong result:",2,x,"");

	apEndTest();
}


oper012();


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
