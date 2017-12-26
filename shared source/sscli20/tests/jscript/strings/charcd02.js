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


var iTestID = 53675;

// String.prototype.charCodeAt( pos )
@cc_on
@if(@_fast)
  var a, b, c, d = "";
  var foo, foo2 = "";
  var numobjvar = "";
@end


foo = "abcde";
foo2 = String.fromCharCode(0, 97, 65535 );
function bar() {
	return 2;
}

function test( res, exp, bug )
{
	if ( res != exp )
		apLogFailInfo( "wrong number", exp, res, bug );
}


function charcd02()
{
apInitTest("charcd02");

//----------------------------------- negative number
apInitScenario("1. negative number");
if ( foo.charCodeAt(-1) )
	apLogFailInfo( "wrong number", "undefined", foo.charCodeAt(-1), "" );


//----------------------------------- string
apInitScenario("2. string");
test( foo.charCodeAt("2"), 99 );


//----------------------------------- number past String.length-1
apInitScenario("3. number past String.length-1");
if ( foo.charCodeAt(5) )
	apLogFailInfo( "wrong number", "undefined", foo.charCodeAt(5), "" );


//----------------------------------- float
apInitScenario("4. float");
test( foo.charCodeAt(2.1), 99, "" );
test( foo.charCodeAt(2.8), 99, "" );


//----------------------------------- boolean
apInitScenario("5. boolean");
test( foo.charCodeAt(true), 98, "" );
test( foo.charCodeAt(false), 97, "" );


//----------------------------------- Number
apInitScenario("6. Number");
numobjvar = new Number(2);
test( foo.charCodeAt(numobjvar), 99, "" );


//----------------------------------- variable
apInitScenario("7. variable");
a = 2;
test( foo.charCodeAt(a), 99, "" )


//----------------------------------- expression
apInitScenario("8. expression");
test( foo.charCodeAt( 10/5 ), 99, "" );


//----------------------------------- eval
apInitScenario("9. eval");
test( foo.charCodeAt( eval(1+1) ), 99, "" );


//----------------------------------- function
apInitScenario("10. function");
test( foo.charCodeAt( bar() ), 99, "" );


//----------------------------------- first position
apInitScenario("11. first position");
test( foo.charCodeAt(0), 97, "" );


//----------------------------------- last position
apInitScenario("12. last position");
test( foo.charCodeAt(4), 101, "" );


//----------------------------------- no argument
apInitScenario("13. no argument");
test( foo.charCodeAt(), 97, "" );


//----------------------------------- return 0
apInitScenario("14. return 0");
test( foo2.charCodeAt(0), 0, "" );


//----------------------------------- return 2^16
apInitScenario("15. return 2^16");
@if (@_win16)
test( foo2.charCodeAt(2), 63, "" );
@else
test( foo2.charCodeAt(2), 65535, "" );
@end


apEndTest();
}


charcd02();


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
