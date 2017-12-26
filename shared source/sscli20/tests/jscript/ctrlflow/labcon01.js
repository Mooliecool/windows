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


var iTestID = 52887;

// Different levels of nesting

function labcon01()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)  
         var a;              
       
   @end       

apInitTest("labcon01");

//---------------------------- Highest level
apInitScenario("1. highest level");
a = 0;
foo: while ( a < 3 ) {
		while ( a < 5 ) {
			while ( a < 7 ) {
				while ( a < 9 ) {
					while ( a < 11 ) {
						a++;
						continue foo;
					}
					a++;
				}
				a++;
			}
			a++;
		}
		a++;
	}
if ( a != 3 ) apLogFailInfo("'a' count is wrong", "3", a, "");


//---------------------------- Middle level
apInitScenario("2. middle level");
a = 0;
while ( a < 3 ) {
	while ( a < 5 ) {
foo: 	while ( a < 7 ) {
			while ( a < 9 ) {
				while ( a < 11 ) {
					a++;
					continue foo;
				}
				a++;
			}
			a++;
		}
		a++;
	}
	a++;
}
if ( a != 9 ) apLogFailInfo("'a' count is wrong", "9", a, "");


//---------------------------- Lowest level
apInitScenario("3. lowest level");
a = 0;
while ( a < 3 ) {
	while ( a < 5 ) {
	 	while ( a < 7 ) {
			while ( a < 9 ) {
foo:			while ( a < 11 ) {
					a++;
					continue foo;
				}
				a++;
			}
			a++;
		}
		a++;
	}
	a++;
}
if ( a != 15 ) apLogFailInfo("'a' count is wrong", "15", a, "");


apEndTest();
}

labcon01();


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
