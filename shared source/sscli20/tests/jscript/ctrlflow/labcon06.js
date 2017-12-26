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


var iTestID = 52892;

// test different loop statements with labeled break

function labcon06()
{
// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
      var a, i, x, b;           
    
@end       

apInitTest("labcon06");

//------------------------ for
apInitScenario("1. for");
a = 0;
foo:	for ( i=0; i<5; i++) {
			a++;
			continue foo;
			apLogFailInfo("didn't continue", "", "", "");
		}
if ( a != 5 ) apLogFailInfo("'a' count is wrong", "5", a, "");


//------------------------ for-in
apInitScenario("2. for-in");
a = 0;
x = new Object;
x.t1 = 1;
x.t2 = 2;
x.t3 = 3;
foo:	for ( b in x ) {
			a++;
			continue foo;
			apLogFailInfo("didn't continue", "", "", "");
		}
if ( a != 3 ) apLogFailInfo("'a' count is wrong", "3", a, "");


//------------------------ while
apInitScenario("3. while");
a = 0;
foo:	while ( a < 5 ) {
			a++;
			continue foo;
			apLogFailInfo("didn't continue", "", "", "");
		}
if ( a != 5 ) apLogFailInfo("'a' count is wrong", "5", a, "");


//------------------------ do-while
apInitScenario("4. do-while");
a = 0;
foo:	do {
			a++;
			continue foo;
			apLogFailInfo("didn't continue", "", "", "");
		} while ( a < 5 );
if ( a != 5 ) apLogFailInfo("'a' count is wrong", "5", a, "");


apEndTest();
}


labcon06();


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
