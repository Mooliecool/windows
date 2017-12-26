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


var iTestID = 65334;

// !== Tests

function test( a, b, desc, succeed )
{
	if ( succeed ) {
		if ( a !== b )
			;
		else
			apLogFailInfo( desc, succeed, a !== b, "" );
	}
	else {
		if ( a !== b )
			apLogFailInfo( desc, succeed, a !== b, "" );
	}
}


// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
      var numvar, numvar2, numvar0, numvar1;
      var infvar, ninfvar, nanvar, maxvar, minvar;
      var floatvar, floatvar2, numobjvar, posnumvar, negnumvar;
      var strvar, strvar2, strvar0, emptstrvar, spstrvar;
      var strobjvar, truestrvar, falsestrvar;
      var nullvar, truevar, falsevar;
      var objvar, cpobjvar, dupobjvar, funcvar, dupfuncvar, cpfuncvar;                 
    
@end 


numvar = 4321;
numvar2 = 432;
numvar0 = 0;
numvar1 = 0;
infvar = Number.POSITIVE_INFINITY;
ninfvar = Number.NEGATIVE_INFINITY;
nanvar = Number.NaN;
maxvar = Number.MAX_VALUE;
minvar = Number.MIN_VALUE;
floatvar = 43.21
floatvar2 = 43.2
numobjvar = new Number(4321);
posnumvar = +4321;
negnumvar = -4321;

strvar = "4321";
strvar2 = "432";
strvar0 = "0";
emptstrvar = "";
spstrvar = " 4321";
strobjvar = new String(4321);
truestrvar = "true";
falsestrvar = "false";

nullvar = null;

truevar = true;
falsevar = false;

objvar = new Object();
objvar.test = 42;
cpobjvar = objvar
dupobjvar = new Object();
dupobjvar.test = 42;
funcvar = new Function( "var x = 6;" );
dupfuncvar = funcvar;
cpfuncvar = new Function( "var x = 6;" );


function tripeq02()
{
apInitTest("tripeq02 - !==");

//------------------------------------------------------ Number tests
apInitScenario("1. Number tests");
test( numvar, numvar, "numvar !== numvar", false );

test( numvar, numvar2, "numvar !== numvar2", true );

test( numvar, floatvar, "numvar !== floatvar", true );

test( floatvar, floatvar, "floatvar !== floatvar", false );

test( floatvar, floatvar2, "floatvar !== floatvar2", true );

test( infvar, infvar, "infvar !== infvar", false );

test( infvar, numvar0, "infvar !== numvar0", true );

test( infvar, maxvar, "infvar !== maxvar", true );

test( ninfvar, ninfvar, "ninfvar !== ninfvar", false );

test( ninfvar, numvar0, "ninfvar !== numvar0", true );

test( ninfvar, minvar, "ninfvar !== minvar", true );

test( infvar, ninfvar, "infvar !== ninfvar", true );

test( numvar0, nanvar, "numvar0 !== nanvar", true );

test( infvar, nanvar, "infvar !== nanvar", true );

test( ninfvar, nanvar, "ninfvar !== nanvar", true );

test( numobjvar, numvar, "numobjvar !== numvar", true );

test( posnumvar, numvar, "posnumvar !== numvar", false );

test( negnumvar, numvar, "negnumvar !== numvar", true );

test( numvar, numvar.toString(), "numvar !== numvar.toString()", true );

test( numvar, strvar, "numvar !== strvar", true );


//------------------------------------------------------ String tests
apInitScenario("2. String tests");
test( strvar, numvar, "strvar !== numvar", true );

test( spstrvar, numvar, "spstrvar !== numvar", true );

test( strvar, strvar, "strvar !== strvar", false );

test( strvar, strvar2, "strvar !== strvar2", true );

test( emptstrvar, strvar, "emptstrvar !== strvar", true );

test( emptstrvar, numvar0, "emptstrvar !== numvar0", true );

test( emptstrvar, nullvar, "emptstrvar !== nullvar", true );

test( strobjvar, strvar, "strobjvar !== strvar", true );

test( strobjvar, numvar, "strobjvar !== numvar", true );

test( strobjvar, numobjvar, "strobjvar !== numobjvar", true );


//------------------------------------------------------ Null tests
apInitScenario("3. Null tests");
test( nullvar, nullvar, "nullvar !== nullvar", false );

test( nullvar, numvar0, "nullvar !== numvar0", true );

test( nullvar, strvar0, "nullvar !== strvar0", true );

test( nullvar, nanvar, "nullvar !== nanvar", true );

test( nullvar, infvar, "nullvar !== infvar", true );

test( nullvar, ninfvar, "nullvar !== ninfvar", true );

test( nullvar, maxvar, "nullvar !== maxvar", true );

test( nullvar, minvar, "nullvar !== minvar", true );


//------------------------------------------------------ Boolean tests
apInitScenario("4. Boolean tests");
test( truevar, truevar, "truevar !== truevar", false );

test( falsevar, falsevar, "falsevar !== falsevar", false );

test( truevar, falsevar, "truevar !== falsevar", true );

test( truevar, numvar1, "truevar !== numvar1", true );

test( falsevar, numvar0, "falsevar !== numvar0", true );

test( falsevar, nullvar, "falsevar !== nullvar", true );

test( falsevar, nanvar, "falsevar !== nanvar", true );

test( truevar, numvar, "truevar !== numvar", true );

test( truevar, truestrvar, "truevar !== truestrvar", true );

test( falsevar, falsestrvar, "falsevar !== falsestrvar", true );


//------------------------------------------------------ Object tests
apInitScenario("5. Object tests");
test( objvar, cpobjvar, "objvar !== cpobjvar", false );

test( objvar, dupobjvar, "objvar !== dupobjvar", true );

test( funcvar, funcvar, "funcvar !== funcvar", false );

test( funcvar, cpfuncvar, "funcvar !== cpfuncvar", true );

test( funcvar, dupfuncvar, "funcvar !== dupfuncvar", false );


apEndTest();
}


tripeq02();


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
