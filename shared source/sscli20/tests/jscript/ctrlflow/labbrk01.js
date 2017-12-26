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


var iTestID = 52881;

// test label at various levels

function labbrk01()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)  
         var i;
         var a;		             
       
   @end  

apInitTest("labbrk01");

//------------------------------ test label at highest level
apInitScenario("1. highest level");
i = 0;
a = 0;
foo: if ( i != 2 ) {
	a++;
	if ( i != 2 ) {
		a++;
		if ( i != 2 ) {
			a++;
			if ( i != 2 ) {
				a++;
				if ( i != 2 ) {
					a++;
					i++
					break foo;
					apLogFailInfo( "didn't break: 1", "", "", "" );
				}
			apLogFailInfo( "didn't break: 2", "", "", "" );
			}
		apLogFailInfo( "didn't break: 3", "", "", "" );
		}
	apLogFailInfo( "didn't break: 4", "", "", "" );
	}
} else apLogFailInfo( "didn't break: 5", "", "", "" );
if ( a != 5 ) apLogFailInfo( "final count of levels wrong", "5", a, "" );


//----------------------------- test label in between levels
apInitScenario("2. middle level");
a = 0;
if ( i != 2 ) {
	a++;
	if ( i != 2 ) {
		a++;
foo:	if ( i != 2 ) {
			a++;
			if ( i != 2 ) {
				a++;
				if ( i != 2 ) {
					a++;
					break foo;
					apLogFailInfo( "didn't break: 1", "", "", "" );
				}
				apLogFailInfo( "didn't break: 2", "", "", "" );
			}
			apLogFailInfo( "didn't break: 3", "", "", "" );
		}
		a++;
	}
	a++;
}
if ( a != 7 ) apLogFailInfo( "final count of levels wrong", "7", a, "" );


//------------------------------ test label at lowest level
apInitScenario("3. lowest level");

a = 0;
if ( i != 2 ) {
	a++;
	if ( i != 2 ) {
		a++;
		if ( i != 2 ) {
			a++;
			if ( i != 2 ) {
				a++;
foo:			if ( i != 2 ) {
					a++;
					i++
					break foo;
					apLogFailInfo( "didn't break: 1", "", "", "" );
				}
				a++;
			}
			a++;
		}
		a++;
	}
	a++;
}
if ( a != 9 ) apLogFailInfo( "final count of levels wrong", "9", a, "" );


apEndTest();
}

labbrk01();


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
