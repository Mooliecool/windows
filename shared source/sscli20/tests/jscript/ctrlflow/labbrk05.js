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


var iTestID = 52885;

// Different levels of nesting

function labbrk05()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)   
         var a;
         var i, j, k, l, m, n, o, p, q, r;
         var x, y, z;
       
   @end  

apInitTest("labbrk05");

//---------------------------------- 1 level
apInitScenario("1. 1 level");
a = 0;
foo:	for ( i=0; i<5; i++ ) {
			a++;
			break foo;
			apLogFailInfo("didn't break", "", "", "");
		}
if ( a != 1 ) apLogFailInfo("'a' count wrong", "1", a, "");


//---------------------------------- many identical levels
apInitScenario("2. many identical levels");
a = 0;
foo:	for ( i=0; i<5; i++ ) {
			for ( j=0; j<5; j++ ) {
				for ( k=0; k<5; k++ ) {
					for ( l=0; l<5; l++ ) {
						for ( m=0; m<5; m++ ) {
							for ( n=0; n<5; n++ ) {
								for ( o=0; o<5; o++ ) {
									for ( p=0; p<5; p++ ) {
										for ( q=0; q<5; q++ ) {
											for ( r=0; r<5; r++ ) {
												a++;
												break foo;
												a++;
											}
											a++;
										}
										a++;
									}
									a++;
								}
								a++;
							} while ( a < 5 );
							a++;
						}
						a++;
					}
					a++;
				}
				a++;
			}
			a++;
		}
if ( a != 1 ) apLogFailInfo("'a' count wrong", "1", a, "");


//---------------------------------- many different levels
apInitScenario("3. many different levels");
a = 0;
y = new Object();
y.test = 1;
foo:	for ( i=0; i<5; i++ ) {
			while ( a < 10 ) {
				for ( k=0; k<5; k++ ) {
					for ( x in y ) {
						for ( m=0; m<5; m++ ) {
							do {
								for ( o=0; o<5; o++ ) {
									if ( true ) {
										for ( q=0; q<5; q++ ) {
											for ( z in y ) {
												a++;
												break foo;
												a++;
											}
											a++;
										}
										a++;
									}
									a++;
								}
								a++;
							} while ( a < 5 );
							a++;
						}
						a++;
					}
					a++;
				}
				a++;
			}
			a++;
		}
if ( a != 1 ) apLogFailInfo("'a' count wrong", "1", a, "");


apEndTest();
}


labbrk05();


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
