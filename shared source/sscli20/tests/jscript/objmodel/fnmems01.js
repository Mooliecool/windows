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


var iTestID = 52710;


function Foo1() { return Bar1(); }
function Bar1() { 
@if(!@_fast)
	return Foo1.arguments;
@end
}

function TestScope(x){
	var x;
	return x;
}

function verify(vAct, vExp, sMsg, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo(sMsg, vExp, vAct, bugNum);
}

// note, try an error run to validate the mem without the func qualifier is a
// reference to an undefined variable and a fatal error.  We're spec'd 
// different from nav in this
function foo ()
{
@if(!@_fast)
    return foo.mem;
@end
}

function fnmems01 (){
 
    apInitTest("fnMems01 - not run in fast mode");
    
    //----------------------------------------------------------------------------
    apInitScenario("test non existant member is undefined");
@if(!@_fast)
    verify( foo.mem1+"", "undefined", "", null );
@end
    //----------------------------------------------------------------------------
    apInitScenario("verify access to mem from outside func");
@if(!@_fast)
    // add mem
    foo.mem = "hi";

    verify( foo.mem+"", "hi", "", null );
@end
    //----------------------------------------------------------------------------
    apInitScenario("verify access to mem from inside func");
@if(!@_fast)
    verify( foo()+"", "hi", null );
@end
    //----------------------------------------------------------------------------
    apInitScenario("fnc args updated correctly");
@if(!@_fast)
    verify(Foo1(),"[object Object]", "", null);
    verify(Bar1(),null, "", null);
@end
    //----------------------------------------------------------------------------
    apInitScenario("Function scoping");

	verify(TestScope(5), 5, "int", "");
	var x;
	x = 3;
	verify(TestScope(x), x, "declared var", "");
@if(!@_fast)
	verify(TestScope.x+"", "undefined", "shouldnt be defined outside of function"), "";
@end
    //----------------------------------------------------------------------------
    apInitScenario("undefined members");
try{
	var scoob = { a : 123 };
	var doob;
	doob.blah();
}catch(e){
	verify(""+e.Description, "undefined", "check undefined fn, assert found here", "JS7 343192");
}
    apEndTest();
}


fnmems01();


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
