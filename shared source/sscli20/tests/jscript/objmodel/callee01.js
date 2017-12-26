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


var iTestID = 78691;


var sv=ScriptEngineMajorVersion()+(.1*ScriptEngineMinorVersion());

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}

@if(!@_fast)	//arguments object throws error in fast mode
function foo1() {
	var x;
	verify (foo1.arguments.callee, foo1, "callee property with no arguments", "");
}

function foo2(a, b, c) {
	verify (foo2.arguments.callee, foo2, "callee property with 3 arguments", "");
}

function foo3(m, n) {
	//one line comment
	verify (foo3.arguments.callee, foo3, "callee property with 1 line comment", "");
}

function foo4(i, j, k) {
/* multiline 
line
comment
*/
	verify (foo4.arguments.callee, foo4, "callee property with multiline comment", "");
}

function foo5(i, j, k) {
/* multiline 
line
// EMBEDDED COMMENT 1
comment
*/
	verify (foo5.arguments.callee, foo5, "callee property with multiline comment", "");
}

function foo6(i, j, k) {
/* multiline 
line
// EMBEDDED COMMENT 1
comment
*/
	verify (foo6.arguments.callee, foo6, "callee property with multiline comment", "");

// /* EMBEDDED COMMENT 2 */ this doesnt recognize the /* tag since we are on the same line

}

function bar1() {
	bar2();
	bar4();
}

function bar2() {
	verify (bar1.arguments.callee, bar1, "embedded function can see parent 1 level deep", "");
	bar3();
}

function bar3() {
	verify (bar1.arguments.callee, bar1, "embedded function can see parent 2 levels deep", "");
	verify (bar2.arguments.callee, bar2, "embedded function can see parent 1 levels deep", "");
}

function bar4() {
	verify (bar4.arguments.callee, bar4, "embedded function can still see itself", "");
		if (apGetLocale() == 1033)
		{	
			try 
			{	
				var y = bar3.arguments.callee;
			}
			catch(e) 
			{	
				if(sv<7.0) verify (e.description, "'bar3.arguments.callee' is null or not an object", "random function can't see another's callee property", "");		
				else verify (e.description, "Object required", "random function can't see another's callee property", "");
			}	
		}
}//end bar4

function randomfoo() {
	if (apGetLocale() == 1033)
	{
		var x;
	
		//try to call a function that exists outside of my Function scope
		try 
		{
			x = foo1.arguments.callee;
		}
		catch(e) 
		{
		   if(sv<7.0) verify (e.description, "'foo1.arguments.callee' is null or not an object", "random function can't see another's callee property", "");
		   else verify (e.description, "Object required", "random function can't see another's callee property", "");
		}	

		//try to call a function that doesnt exist
		try 
		{
			x = foo.arguments.callee;
		}
		catch(e) 
		{
			verify (e.description, "'foo' is undefined", "random function can't see another's callee property", "");
		}
	}
}

function baz() {
	verify (arguments.callee, baz, "make sure arguments is on the right function", "");
	baz2();
}

function baz2() {
	verify (baz2.arguments.callee, baz2, "make sure arguments is on the right NESTED function", "");
}

var anon = new Function("a", "return(arguments.callee);");
var square = new Function("x", "var y = (arguments.callee);return x*x;");
var anon2 = function(a) {return(anon2.arguments.callee);}
@end

function callee01() {

	apInitTest ("callee01: property tests - not run in fast mode");
	
	apInitScenario ("1. Callee property check with no arguments");
@if(!@_fast)
		foo1();
@end

	apInitScenario ("2. Callee property check with 3 arguments");
@if(!@_fast)
		foo2(1,2,3);
@end

	apInitScenario ("3. Callee property check with oneline comment");
@if(!@_fast)
		foo3(1,2);
@end

	apInitScenario ("4. Callee property check with multiline comment");
@if(!@_fast)
		foo4(1,2,3);
@end

	apInitScenario ("5. Callee property check with embedded comment in multiline");
@if(!@_fast)
		foo5(1,2,3);
@end

	apInitScenario ("6. Callee property check with embedded comment in single-line comment");
@if(!@_fast)
		foo6(1,2,3);
@end

	apInitScenario ("7. Nested function calls can/can not see each other's property");
@if(!@_fast)
		bar1();
@end

	apInitScenario ("8. Random function calls dont see each other's callee property");
@if(!@_fast)
		randomfoo();
@end

	apInitScenario ("9. arguments.callee testing");
@if(!@_fast)
		baz();
@end

       apInitScenario ("10. check anonymous functions");
@if(!@_fast)
	       verify (anon(), anon, "check anonymous function", "");
	       verify (anon2(), anon2, "check anonymous functions", "");
@end

	apEndTest();

}


callee01();


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
