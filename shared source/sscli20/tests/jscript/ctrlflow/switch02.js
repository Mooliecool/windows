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


var iTestID = 53688;

function switch02()
{
apInitTest("switch02");

// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
      var numvar, a, i;                 
    
@end       

//------------------------------------ labeled breaks out of switch
apInitScenario("1. Labeled break out of switch");
numvar = 4321;

a = 0;
foo: for ( i=0; i<5; i++ ) {
		switch( numvar ) {
			case 4321: a++; break foo;
		}
		a++;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ labeled continue out of switch
apInitScenario("2. Labeled continue out of switch");
numvar = 4321;

a = 0;
foo: for ( i=0; i<5; i++ ) {
		switch( numvar ) {
			case 4321: a++; continue foo;
		}
		a++;
}
if ( a != 5 ) apLogFailInfo("wrong count of 'a'", 5, a, "");


//------------------------------------ continue out of switch
apInitScenario("3. Continue out of switch");
numvar = 4321;

a = 0;
for ( i=0; i<5; i++ ) {
		switch( numvar ) {
			case 4321: a++; continue;
		}
		a++;
}
if ( a != 5 ) apLogFailInfo("wrong count of 'a'", 5, a, "");


//------------------------------------ missing break at end
apInitScenario("4. Missing break at end");
a = 0;
switch( 4321 ) {
	case "4321": apLogFailInfo("wrong case", "4321", "'4321'", "");
	case 4321: a++;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ variables as labels
apInitScenario("5. Variables as labels");
a = 0;
switch( 4321 ) {
	case numvar: a++; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ behavior w/o break
apInitScenario("6. Behavior without break");
a = 0;
switch( numvar ) {
	case 4321:
	case "4321": a++;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ behavior with multiple, identical labels
apInitScenario("7. Behavior with multiple, identical labels");
a = 0;
switch( numvar ) {
	case 4321: a++; break;
	case 4321: a++; a++; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ default case with matching case
apInitScenario("8. Default case with matching case");
a = 0;
switch( 4321 ) {
	case 4321: a++; break;
	default: a--; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ default case without matching case
apInitScenario("9. Default case without matching case");
a = 0;
switch( 4321 ) {
	case 4322: a--; break;
	default: a++; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


//------------------------------------ default case before other cases
apInitScenario("10. Default case before other cases");
a = 0;
switch( 4321 ) {
	default: a--; break;
	case 4321: a++; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");

//------------------------------------ default case with null evaluator
apInitScenario("11. Default case with null evaluator");
a = 0;
switch( null ) {
	case 0: a--; break;
	default: a++; break;
}
if ( a != 1 ) apLogFailInfo("wrong count of 'a'", 1, a, "");


apEndTest();
}


switch02();


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
