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


var iTestID = 52882;

// Multiple labels

function labbrk02()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)  
         var i;
         var a;		             
       
   @end  

apInitTest("labbrk02");

//------------------------------------------- same construct
apInitScenario("1. same construct");
a = 0;
while ( a <= 3 ) {
foo: bar:	if ( a == 0 ) {
				a++;
				break foo;
				apLogFailInfo( "didn't break: 1", "", "", "" );
			}
			else if ( a == 2 ){
				a++;
				break bar;
				apLogFailInfo( "didn't break: 2", "", "", "" );
			}
			a++;
}
if ( a != 4 ) apLogFailInfo( "'a' wrong count", "4", a, "" );


//------------------------------------------- different constructs
apInitScenario("2. different constructs");
a = 0;
aaa: while ( a <= 10 ) {
bbb:	for ( i=0; i < 5; i++ ) {
ccc:		do {
				if ( a == 6 ) {
					a++;
					break aaa;
					apLogFailInfo( "didn't break: 1", "", "", "" );
				}
				if ( a == 4 ) {
					a++;
					break bbb;
					apLogFailInfo( "didn't break: 2", "", "", "" );
				}
				if ( a == 2 ) {
					a++;
					break ccc;
					apLogFailInfo( "didn't break: 3", "", "", "" );
				}
eee:			if ( a == 0 ) {
					a++;
					break eee;
					apLogFailInfo( "didn't break: 4", "", "", "" );
				}
				a++;
			} while ( a < 10 );
			a++;
		}
		a++;
	}
if ( a != 7 ) apLogFailInfo( "'a' wrong count", "7", a, "" );


apEndTest();
}

labbrk02();


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
