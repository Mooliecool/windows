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


var iTestID = 53940;


function verify( sMsg, sExp, sRes, sBug )
{
	if ( sExp != sRes )
		apLogFailInfo( sMsg, sExp, sRes, sBug );
}
/*
function getProp() {
	return P_BSTR;
}
*/
function passProp( sArg ) {
	verify( "Wrong value", "abcd", sArg, "" );
	return sArg;
}
@if(@aspx)
	expando function newObj() {
		this.str = "abcd";
		this.int = 5;
	}
@else
	function newObj() {
		this.str = "abcd";
		this.int = 5;
	}
@end

function with03(){
	apInitTest( "with03" );

	var caught = false;
	var undef;
	apInitScenario( "2. Use null - invalid" );

@if(@_jscript_version<7)
	try {
		with( null ) {
			apLogFailInfo("No error (still in 'with')", "", "", "");
		}
	}
	catch( err ) {
		verify("Wrong error", "-2146827864", err.number, "");
		caught = true;
	}
	finally {
		verify( "No error caught", true, caught, "");
		caught = false;
	}
@else
	var thisVarIsNull = null;
	try {
		with( thisVarIsNull ) {
			apLogFailInfo("No error (still in 'with')", "", "", "");
		}
	}
	catch( err ) {
		verify("Wrong error", "-2146827864", err.number, 162351);
		caught = true;
	}
	finally {
		verify( "No error caught", true, caught, "");
		caught = false;
	}
@end

	apInitScenario("3. Use user-defined class");
	try {
		var a3 = new newObj();
		with( a3 ) {
			verify( "Wrong value", "abcd", str, "" );
			verify( "Wrong value", 5, int, "" );
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("4. Use a reference to user-defined class");
	try {
		var a4 = a3;
		with( a4 ) {
			verify( "Wrong value", "abcd", str, "" );
			verify( "Wrong value", 5, int, "" );
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("5. Use a string");
	try {
		with("abcd") {
			verify( "Wrong value", 4, length, "" );
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("6. Use an undefined object - invalid - not tested in fast mode");
     @if(!@_fast)
	try {
		with(undefObj) {
			apLogFailInfo("No error", "", "", "");
		}
	}
	catch(err) {
		verify("Wrong error", "-2146823279", err.number, "");
		caught = true;
	}
	finally {
		verify("Didn't catch error", true, caught, "");
		caught = false;
	}
     @end
	apInitScenario("10. Use 'this' object");
	try {
		with(this) {
			var worked = true;
		}
		verify("Wrong value", true, worked, "");
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("11. Use a String object");
	try {
		var strobj = new String("hello");
		with(strobj) {
			verify("Wrong value", 5, length, "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("12. Use a Date object");
	try {
		var datobj = new Date(97, 7, 14);
		with(datobj) {
			verify("Wrong value", 14, getDate(), "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("13. Use an Object object");
	try {
		var objObj = new Object(13);
		with(objObj) {
			verify("Wrong value", "13", toString(), "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("14. Use an Array object");
	try {
		var arrObj = new Array(10, 15, 20);
		with(arrObj) {
			verify("Wrong length", 3, length, "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("15. Use a Boolean object");
	try {
		var boolObj = new Boolean(true);
		
		with( boolObj ) {
			verify("Wrong value", true, valueOf(), "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("16. Use an Enumerator object");
	try {
		var enumObj = new Enumerator();
		with( enumObj ) {
			moveFirst();
			verify( "Wrong value", undef, item(), "");
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("17. Use a Function object");
       @if(!@_fast)
	try {
		var funcObj = new Function( "arg1", "return arg1+1;" );
		Function.prototype.foo1 = 2;
		with( funcObj ) {
			verify("Wrong value", 2, foo1, "");
			Function.prototype.foo2 = 3;
			verify("Wrong value", 3, foo2, "");
		}
		verify( "Wrong value", 3, funcObj.foo2, "");
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}
       @end
	apInitScenario("18. Use a Math object");
	try {
		with( Math ) {
			verify( "Wrong value", 2, ceil(1.1), "" );
		}
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}

	apInitScenario("19. Use a RegExp object"); //305641
	@if (!@_fast)
	try {
		var reObj = new RegExp("d(b+)(d)","ig");
		with( reObj ) {
			var a19 = exec("cdbBdbsbdbdz");
			verify( "Wrong value", "bB", RegExp.$1, "" );
		}
		verify( "Wrong value", 1, RegExp.index, "" );
	}
	catch(err) {
		apLogFailInfo("Error caught", "", err.description, "");
	}
	@end
	apEndTest();
}



with03();


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
