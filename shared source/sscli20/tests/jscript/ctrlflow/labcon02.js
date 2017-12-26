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


var iTestID = 52888;

// Using multiple labels in the same block

function labcon02()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)  
         var a, i;              
       
   @end       

apInitTest("labcon02");

//----------------------------------- same line
apInitScenario("1. same line");
a = 0;
while ( a < 1 ) {
foo: bar:	while ( a < 3 ) {
				if ( a == 0 ) {
					a++;
					continue foo;
				} else {
					a++;
					continue bar;
				}
				apLogFailInfo("didn't continue", "", "", "");
			}
			a++;
}
if ( a != 4 ) apLogFailInfo("'a' count is wrong", "4", a, "");


//---------------------------------- different lines
apInitScenario("2. different lines");
a = 0;
aaa: while ( a < 1 ) {
bbb:	for ( i=0; i < 3; i++ ) {
ccc:		do {
				if ( a == 2 ) {
					a++;
					continue aaa;
					apLogFailInfo("didn't continue", "", "", "");
				}
				if ( a == 1 ) {
					a++;
					continue bbb;
					apLogFailInfo("didn't continue", "", "", "");
				}
				if ( a == 0 ) {
					a++;
					continue ccc;
					apLogFailInfo("didn't continue", "", "", "");
				}
				a++;
			} while ( a < 5 );
			a++;
		}
		a++;
	}
if ( a != 3 ) apLogFailInfo("'a' count is wrong", "3", a, "");


apEndTest();
}

labcon02();


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
